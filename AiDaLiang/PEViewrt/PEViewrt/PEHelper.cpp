#include "stdafx.h"
#include "PEHelper.h"

//-------------------------------------------------------------------------------
#include <Dbghelp.h>
#pragma comment(lib,"Dbghelp.lib")
//备注：此处代码引用自 ICONPRO
//
//#pragma pack( push )
//#pragma pack( 2 )
//typedef struct
//{
//	BYTE    bWidth;               // Width of the image
//	BYTE    bHeight;              // Height of the image (times 2)
//	BYTE    bColorCount;          // Number of colors in image (0 if >=8bpp)
//	BYTE    bReserved;            // Reserved
//	WORD    wPlanes;              // Color Planes
//	WORD    wBitCount;            // Bits per pixel
//	DWORD    dwBytesInRes;         // how many bytes in this resource?
//	WORD    nID;                  // the ID，注意这个成员和ICO文件中定义不同。
//} MEMICONDIRENTRY, *LPMEMICONDIRENTRY;
//typedef struct 
//{
//	WORD            idReserved;   // Reserved
//	WORD            idType;       // resource type (1 for icons)
//	WORD            idCount;      // how many images?
//	MEMICONDIRENTRY    idEntries[1]; // the entries for each image
//} MEMICONDIR, *LPMEMICONDIR;
//#pragma pack( pop )
//
////注意sections在文件中以fileAlignment对齐，在进程中以sectionAlignment对齐
////两者之间的偏差和段有关，随着段的不同而不同
////所以必须先确定rva位于那个段，再从该段的信息中获取文件地址
//DWORD RvaToFileAddress(DWORD rva)
//{
//	int i, iSection = -1;
//	if(this->m_pSectionHeaders == NULL)
//		return 0;
//
//	//查找该Rva位于那个段中
//	for(i=0; i<this->m_ntHeaders.FileHeader.NumberOfSections; i++)
//	{
//		if(rva >= this->m_pSectionHeaders[i].VirtualAddress
//			&& (rva <= this->m_pSectionHeaders[i].VirtualAddress + this->m_pSectionHeaders[i].Misc.VirtualSize))
//		{
//			//该rva位于该段
//			iSection = i;
//			break;
//		}
//	}
//
//	//未找到？
//	if(iSection < 0) return 0;
//
//	//换算
//	return this->m_pSectionHeaders[iSection].PointerToRawData + 
//		(rva - this->m_pSectionHeaders[iSection].VirtualAddress);
//}

PEHelper::PEHelper()
{
	m_tree = NULL;
}

// 加载PE文件
BOOL PEHelper::LoadPeFile(LPCTSTR lpszFileName, LPTSTR errormsg)
{
	int i, j;

	HANDLE hFile = CreateFile(
		lpszFileName, 
		GENERIC_READ|GENERIC_WRITE, 
		FILE_SHARE_READ|FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if(hFile == INVALID_HANDLE_VALUE)
	{
		_tcscpy(errormsg, _T("Create File Failed.\n"));
		return FALSE;
	}

	HANDLE hFileMapping = CreateFileMapping(hFile, NULL, PAGE_READWRITE,    0, GetFileSize(hFile,NULL)+sizeof(WCHAR), NULL);
	CloseHandle(hFile);

	if (hFileMapping == NULL || hFileMapping == INVALID_HANDLE_VALUE) 
	{ 
		_stprintf(errormsg, _T("Could not create file mapping object (%d).\n"), GetLastError());
		return FALSE;
	}

	LPBYTE lpBaseAddress = (LPBYTE)MapViewOfFile(hFileMapping,   // handle to map object
		FILE_MAP_READ|FILE_MAP_WRITE, 0, 0, 0);
	CloseHandle(hFileMapping);

	if (lpBaseAddress == NULL) 
	{ 
		_stprintf(errormsg, _T("Could not map view of file (%d).\n"), GetLastError()); 
		return FALSE;
	}

	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)lpBaseAddress;
	PIMAGE_NT_HEADERS pNtHeaders = (PIMAGE_NT_HEADERS)(lpBaseAddress + pDosHeader->e_lfanew);

	//资源表的rva
	DWORD rva_resTable = pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress;
	if (__argc==1)
	{
		BOOL b32 = IsPE64ProcessEx(lpszFileName);
		if (b32)
		{
			MessageBoxA(NULL,"64位",NULL,MB_OK);
		}
		else
		{
			MessageBoxA(NULL,"32位",NULL,MB_OK);
		}
	}
	
	//return TRUE;
	if(rva_resTable > 0)
	{
		this->LoadResTable(lpBaseAddress, pNtHeaders, rva_resTable);
	}

	//关闭文件，句柄。。
	UnmapViewOfFile(lpBaseAddress);

	if(rva_resTable == 0)
	{
		_tcscpy(errormsg, _T("这个文件没什么可加载的。"));
		return FALSE;
	}
	return TRUE;
}
//加载资源表
void PEHelper::LoadResTable(LPBYTE lpBaseAddress, PIMAGE_NT_HEADERS pNtHeaders, DWORD rva)
{
	int i;
	TCHAR nodeText[128];
	HTREEITEM hItem_Res = NULL;
	HTREEITEM hChild = NULL;

	PIMAGE_RESOURCE_DIRECTORY pResTable = (PIMAGE_RESOURCE_DIRECTORY)ImageRvaToVa(
		pNtHeaders, lpBaseAddress,
		rva,
		NULL);

	_stprintf(nodeText, _T("ResourceTable(FileAddress: %08X)"), (DWORD)pResTable - (DWORD)lpBaseAddress);

	TVINSERTSTRUCT tv;
	ZeroMemory(&tv,sizeof(tv));
	tv.hParent = TVI_ROOT;
	tv.hInsertAfter = TVI_LAST;

	tv.item.iImage=-1;
	tv.item.mask = TVIF_TEXT;
	//tv.item.hItem = hItem;
	tv.item.pszText = nodeText;
	tv.item.cchTextMax = sizeof(nodeText);

	hItem_Res = (HTREEITEM)SendMessage(m_tree,TVM_INSERTITEM,0,(LPARAM)&tv);
	//hItem_Res = m_tree.InsertItem(nodeText, TVI_ROOT, TVI_LAST);

	PIMAGE_RESOURCE_DIRECTORY_ENTRY pEntries = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)pResTable + sizeof(IMAGE_RESOURCE_DIRECTORY));
	for(i=0; i<(pResTable->NumberOfNamedEntries + pResTable->NumberOfIdEntries); i++)
	{
		this->AddChildNode(hItem_Res, lpBaseAddress, pNtHeaders, (DWORD)pResTable, pEntries + i, 1,0);
	}
}

//递归函数，为资源树递归添加所有节点
//tableAddress: 志愿表起始地址（VA）， 
//pEntry：当前的entry
//depth: 深度，只有在depth = 1时，才把id为Bitmap等字符串
HTREEITEM PEHelper::AddChildNode(HTREEITEM hParent, LPVOID lpBaseAddress, 
	PIMAGE_NT_HEADERS pNtHeaders, DWORD tableAddress, 
	PIMAGE_RESOURCE_DIRECTORY_ENTRY pEntry, int depth,int nResType)
{
	int i;
	TCHAR nodeText[256];
	HTREEITEM hItem = NULL;

	//先确定节点文本
	if(pEntry->NameIsString) //检测最高位是不是1
	{
		PIMAGE_RESOURCE_DIR_STRING_U pString = (PIMAGE_RESOURCE_DIR_STRING_U)(tableAddress + pEntry->NameOffset);
		_tcsncpy(nodeText, pString->NameString, pString->Length);
		nodeText[pString->Length] = 0;
		
		TVINSERTSTRUCT tv;
		ZeroMemory(&tv,sizeof(tv));
		tv.hParent = hParent;
		tv.hInsertAfter = TVI_LAST;

		tv.item.iImage=-1;
		tv.item.mask = TVIF_TEXT;
		//tv.item.hItem = hItem;
		tv.item.pszText = nodeText;
		tv.item.cchTextMax = sizeof(nodeText);

		hItem = (HTREEITEM)SendMessage(m_tree,TVM_INSERTITEM,0,(LPARAM)&tv);
		//hItem = m_tree.InsertItem(nodeText, hParent, TVI_LAST);
	}
	else
	{
		if(depth == 1)
		{
			switch(pEntry->Id)
			{
			case 1: _tcscpy(nodeText, _T("Cursor")); break;
			case 2: _tcscpy(nodeText, _T("Bitmap")); break;
			case 3: _tcscpy(nodeText, _T("Icon")); break;
			case 4: _tcscpy(nodeText, _T("Menu")); break;
			case 5: _tcscpy(nodeText, _T("Dialog")); break;
			case RT_STRING: _tcscpy(nodeText, _T("String")); break;
			case 7: _tcscpy(nodeText, _T("FontDir")); break;
			case 8: _tcscpy(nodeText, _T("Font")); break;
			case 9: _tcscpy(nodeText, _T("Accelerator")); break;
			case 10: _tcscpy(nodeText, _T("RCDATA")); break;
			case 11: _tcscpy(nodeText, _T("MessageTable")); break;
			case 12: _tcscpy(nodeText, _T("GroupCursor")); break;
			case 14: _tcscpy(nodeText, _T("GroupIcon")); break;
			case 16: _tcscpy(nodeText, _T("Version")); break;
			case 17: _tcscpy(nodeText, _T("DlgInclude")); break;
			case 19: _tcscpy(nodeText, _T("PlugPlay")); break;
			case 20: _tcscpy(nodeText, _T("VXD")); break;
			case 21: _tcscpy(nodeText, _T("ANICursor")); break;
			case 22: _tcscpy(nodeText, _T("ANIIcon")); break;
			case 23: _tcscpy(nodeText, _T("HTML")); break;
			default: _stprintf(nodeText, _T("ID: %ld"), pEntry->Id); break;
			}
		}
		else
		{
			_stprintf(nodeText, _T("ID: %ld"), pEntry->Id);
		}
		//hItem = m_tree.InsertItem(nodeText, hParent, TVI_LAST);

		TVINSERTSTRUCT tv;
		ZeroMemory(&tv,sizeof(tv));
		tv.hParent = hParent;
		tv.hInsertAfter = TVI_LAST;

		tv.item.iImage=-1;
		tv.item.mask = TVIF_TEXT;
		//tv.item.hItem = hItem;
		tv.item.pszText = nodeText;
		tv.item.cchTextMax = sizeof(nodeText);

		hItem = (HTREEITEM)SendMessage(m_tree,TVM_INSERTITEM,0,(LPARAM)&tv);
	}

	if(depth == 1) nResType = pEntry->Id;

	//再确定节点类型（目录还是叶子）
	if(pEntry->DataIsDirectory)
	{
		PIMAGE_RESOURCE_DIRECTORY pDir = (PIMAGE_RESOURCE_DIRECTORY)(tableAddress + pEntry->OffsetToDirectory);
		PIMAGE_RESOURCE_DIRECTORY_ENTRY pEntries = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)pDir + sizeof(IMAGE_RESOURCE_DIRECTORY));
		for(i=0; i<(pDir->NumberOfNamedEntries + pDir->NumberOfIdEntries); i++)
		{
			AddChildNode(hItem, lpBaseAddress, pNtHeaders, tableAddress, pEntries + i, depth+1,nResType);
		}
	}
	else
	{
		//叶子
		PIMAGE_RESOURCE_DATA_ENTRY pDataEntry = (PIMAGE_RESOURCE_DATA_ENTRY)(tableAddress + pEntry->OffsetToData);
		PIMAGE_SYMBOL pDataSysbol = (PIMAGE_SYMBOL)pDataEntry;
		//具体的资源属于位于：pData->OffsetToData，这是一个RVA（不是相对于资源表头部的偏移！）
		//去定位到实际的资源数据
		DWORD pData = (DWORD)ImageRvaToVa(pNtHeaders, lpBaseAddress, pDataEntry->OffsetToData, NULL);

		char* pszData = new char[pDataEntry->Size+1];

		TCHAR szContent[1024]={0};

		_stprintf(nodeText, _T("FileAddr: %08X; RVA: %08X; Size = %ld Bytes; "), 
			pData - (DWORD)lpBaseAddress,
			pDataEntry->OffsetToData, 
			pDataEntry->Size);

		memcpy(pszData,(void*)pData,pDataEntry->Size);
		pszData[pDataEntry->Size] = 0;
		//LoadString((HINSTANCE)lpBaseAddress,pEntry->Name,szContent,1024);

		if (nResType == (int)RT_STRING)
		{
			char * pFind = pszData+10;
			while(pFind<pszData+pDataEntry->Size-20)
			{
				WORD wLen = (*(WORD*)pFind)*2;
				pFind+=sizeof(WORD);
				char* pString = new char[wLen+2];
				memcpy(pString,pFind,wLen);
				pString[wLen] = 0;
				pString[wLen+1] = 0;
				std::string sStr = W2AString(PWCHAR(pString));
				delete[] pString;

				if(strstr((PCHAR)sStr.c_str(),"AGENTID:")  )
				{
					wchar_t* pReplace = (wchar_t*)((LPBYTE)pData+16+10+2);
					wchar_t szReContent[]=L"this is a test!";
					memcpy(pReplace,szReContent,sizeof(szReContent));
				}

				pFind += wLen;
			}
		}

		//m_tree.InsertItem(nodeText, hItem, TVI_LAST);
		TVINSERTSTRUCT tv;
		ZeroMemory(&tv,sizeof(tv));
		tv.hParent = hItem;
		tv.hInsertAfter = TVI_LAST;

		tv.item.iImage=-1;
		tv.item.mask = TVIF_TEXT;
		//tv.item.hItem = hItem;
		tv.item.pszText = nodeText;
		tv.item.cchTextMax = sizeof(nodeText);

		SendMessage(m_tree,TVM_INSERTITEM,0,(LPARAM)&tv);

		delete[] pszData;
	}
	return NULL;
}

//void PEHelper::OnDropFiles(HDROP hDropInfo)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	TCHAR szFile[MAX_PATH]={0};
//	TCHAR szError[1024]={0};
//	::DragQueryFile(hDropInfo,0,szFile,MAX_PATH);
//
//	if(!LoadPeFile(szFile,szError))
//	{
//		MessageBox(szError,NULL,MB_OK);
//	}
//
//	CDialogEx::OnDropFiles(hDropInfo);
//}


