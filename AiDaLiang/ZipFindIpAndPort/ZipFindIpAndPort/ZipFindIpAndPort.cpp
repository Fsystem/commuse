// ZipFindIpAndPort.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "ZipFindIpAndPort.h"
#include <Shellapi.h>
#include <Windowsx.h>
#include <atlconv.h>
#include "SearchIpAndPort.h"
#include <Shlwapi.h>


#pragma comment(lib,"Shlwapi.lib")

//-------------------------------------------------------------------------------
std::string g_sLog;
//-------------------------------------------------------------------------------

BOOL Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	SetWindowLong(hwnd,GWL_EXSTYLE,GetWindowLong(hwnd,GWL_EXSTYLE)|WS_EX_ACCEPTFILES);

	return TRUE;
}

void Cls_OnDropFiles(HWND hwnd, HDROP hdrop)
{
	std::string sDragFilePath="";
	int nFiles = ::DragQueryFile(hdrop,0xFFFFFFFF,NULL,0);
	for (int i=0;i<nFiles;i++)
	{
		TCHAR szDropFile[MAX_PATH]={0};
		::DragQueryFile(hdrop,i,szDropFile,MAX_PATH);
		if (i>0) sDragFilePath += ";";
		sDragFilePath += T2AString(szDropFile);
	}
	

	Edit_SetText(GetDlgItem(hwnd,IDC_EDIT_PATH),A2TString(sDragFilePath.c_str()).c_str());
}

void AnalysisIpAndPortZip(HWND hwnd,LPCSTR pszZipFile,LPCSTR pszKey)
{
	g_sLog += pszZipFile;
	g_sLog += "\r\n";

	std::string sKey = pszKey;
	int nZip = OpenZip(pszZipFile,NULL);
	if (nZip != 0)
	{
		char szFileList[4096]={0},*pFileName;
		GetZipFileList(nZip,szFileList);
		pFileName = szFileList;
		if (szFileList[0]!=0)
		{
			while(*pFileName)
			{
				int nSize = GetZipFileSize(nZip,pFileName);
				char* pData = new char[nSize];
				ReadZipFile(nZip,pFileName,NULL,pData,nSize);
				SearchIpAndPort s;
				auto list = s.GetIpAndport(pszZipFile,pFileName,pData,sKey.c_str());
				int nIndex = ListBox_GetCount(GetDlgItem(hwnd,IDC_LIST_IP_PORT));
				g_sLog += "\t";
				g_sLog += pFileName;
				g_sLog += "\r\n";
				for (auto it = list.begin();it != list.end();it++)
				{
					//ListBox_InsertString(GetDlgItem(hwnd,IDC_LIST_IP_PORT),nIndex++,A2TString(it->c_str()).c_str());
					g_sLog	+= "\t\t";
					g_sLog	+= *it;
					g_sLog	+= "\r\n";
					Edit_SetText(GetDlgItem(hwnd,IDC_EDIT_RESULT),A2TString(g_sLog.c_str()).c_str());
				}
				delete pData;

				pFileName += (strlen(pFileName)+1);
			}
		}

		CloseZip(nZip,false);
	}
}

void AnalysisIpAndPortDir(HWND hwnd,LPCSTR pszPath,LPCSTR pszKey)
{
	char szFind[MAX_PATH];
	char szFile[MAX_PATH];

	WIN32_FIND_DATAA FindFileData;

	strcpy(szFind,pszPath);
	strcat(szFind,"\\*.*" );

	HANDLE hFind=::FindFirstFileA(szFind,&FindFileData);
	if(INVALID_HANDLE_VALUE == hFind)    return;

	while(TRUE)
	{
		if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if(FindFileData.cFileName[0]!='.')
			{
				strcpy(szFile,pszPath);
				strcat(szFile,"\\");
				strcat(szFile,FindFileData.cFileName);
				AnalysisIpAndPortDir(hwnd,szFile,pszKey);
			}
		}
		else
		{
			strcpy(szFile,pszPath);
			strcat(szFile,"\\" );
			strcat(szFile,FindFileData.cFileName);

			AnalysisIpAndPortZip(hwnd,szFile,pszKey);
		}
		if(!FindNextFileA(hFind,&FindFileData))    break;
	}
	FindClose(hFind);
}

void AnalysisIpAndPort(HWND hwnd)
{
	TCHAR szFile[MAX_PATH]={0};
	Edit_GetText(GetDlgItem(hwnd,IDC_EDIT_PATH),szFile,MAX_PATH);
	TCHAR szKey[MAX_PATH]={0};
	Edit_GetText(GetDlgItem(hwnd,IDC_EDIT_KEYWORD),szKey,MAX_PATH);

	if(szFile[0]==0) {
		MessageBoxA(NULL,"请提供zip文件","提示",MB_OK);
		return;
	}
	std::string sPath = T2AString(szFile),sKey = T2AString(szKey);
	size_t s_,snpos = std::string::npos;
	do 
	{
		std::string sFile;
		s_ = sPath.find(";");
		if (s_ != snpos)
		{
			sFile = sPath.substr(0,s_);
			sPath = sPath.substr(s_+1,snpos);
		}
		else
		{
			sFile = sPath;
		}

		if(PathIsDirectoryA(sFile.c_str()))
		{
			AnalysisIpAndPortDir(hwnd,sFile.c_str(),sKey.c_str());
		}
		else
		{
			AnalysisIpAndPortZip(hwnd,sFile.c_str(),sKey.c_str());
		}
		

	} while (s_!=snpos);
	
}

void Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id)
	{
	case IDOK:
		{
			AnalysisIpAndPort(hwnd);
			break;
		}
	case IDCANCEL:
		{
			EndDialog(hwnd,id);
			break;
		}
	case IDC_BN_CLEAR:
		{
			Edit_SetText(GetDlgItem(hwnd,IDC_EDIT_RESULT),TEXT(""));
			break;
		}
	}
}

INT_PTR CALLBACK DialogProc(
	__in  HWND hwndDlg,
	__in  UINT uMsg,
	__in  WPARAM wParam,
	__in  LPARAM lParam
	)
{
	switch(uMsg)
	{
		HANDLE_MSG(hwndDlg,WM_INITDIALOG,Cls_OnInitDialog); return TRUE;
		HANDLE_MSG(hwndDlg,WM_COMMAND,Cls_OnCommand);  return TRUE;
		HANDLE_MSG(hwndDlg,WM_DROPFILES,Cls_OnDropFiles); return TRUE;
	}

	return 0L;
}


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	
	DialogBox(hInstance,MAKEINTRESOURCE(IDD_MAIN_DLG),NULL,DialogProc);
	return 0;
}