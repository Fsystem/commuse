// ToolOper.cpp: implementation of the CToolOper class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ToolOper.h"

#include <atlconv.h>

#include <TlHelp32.h>
#include <Psapi.h>

#pragma comment(lib,"Version.lib")
//#pragma comment(lib,"Psapi.lib")
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CToolOper::CToolOper()
{

}

CToolOper::~CToolOper()
{

}


BOOL	CToolOper::ReadFileVersionInfo(std::string filename,VERSION_INFO_LIST &_list)
{
	DWORD	dwLen = 0;
	char	*lpData=NULL;
	BOOL	bSuccess = FALSE;

	//获得文件基础信息
	//--------------------------------------------------------
	dwLen = GetFileVersionInfoSizeA(filename.c_str(), NULL);
	if (0 == dwLen)
	{ 
		return FALSE;
	}

	lpData = (char*) malloc(dwLen+1);

	if (lpData == NULL)
	{
		return FALSE;
	}
	
	do 
	{
		memset(lpData,0,dwLen + 1);
		
		bSuccess = GetFileVersionInfoA(filename.c_str(), NULL , dwLen, lpData);
		if (!bSuccess)
		{
			break;
		}
		
		char* pVerValue = NULL;
		UINT nSize = 0;
		if(!VerQueryValueA(lpData,"\\VarFileInfo\\Translation",
			(LPVOID*)&pVerValue,&nSize))
		{
			break;
		}
		
		
		char		strTranslation[128];
		memset(strTranslation,0,sizeof(strTranslation));
		char		buffer[128];
		int			len = 0;
		
		memset(buffer,0,sizeof(buffer));
		wsprintfA(buffer,"000%x",*((unsigned short int *)pVerValue));
		len = strlen(buffer) - 4;
		strcpy(strTranslation,&buffer[len]);
		
		
		memset(buffer,0,sizeof(buffer));
		wsprintfA(buffer,"000%x",*((unsigned short int *)&pVerValue[2]));
		len = strlen(buffer) - 4;
		strcat(strTranslation,&buffer[len]);		
		
		PVOID	lpBuffer;
		UINT	uLen = 0;
		char		path_code[MAX_PATH];
		memset(path_code,0,sizeof(path_code));
		wsprintfA(path_code,"\\StringFileInfo\\%s\\FileDescription",strTranslation);
		
		bSuccess = VerQueryValueA(lpData,
			path_code,
			&lpBuffer,
			&uLen);
		if (!bSuccess)
		{ 
			break;
		}
		_list[FILEDESCRIPTION] = (PCHAR)lpBuffer;

	


		memset(path_code,0,sizeof(path_code));
		wsprintfA(path_code,"\\StringFileInfo\\%s\\ProductName",strTranslation);
		
		bSuccess = VerQueryValueA(lpData,
			path_code,
			&lpBuffer,
			&uLen);
		if (!bSuccess)
		{ 
			break;
		}
		_list[PRODUCTNAME] = (PCHAR)lpBuffer;
	 
		memset(path_code,0,sizeof(path_code));
		wsprintfA(path_code,"\\StringFileInfo\\%s\\LegalCopyright",strTranslation);
		
		bSuccess = VerQueryValueA(lpData,
			path_code,
			&lpBuffer,
			&uLen);
		if (!bSuccess)
		{ 
			DWORD	err = GetLastError();
			err++;
			break;
		}
		_list[LEGALTRADEMARKS] = (PCHAR)lpBuffer;
		//StringFileInfo//%s//LegalTradeMarks
		
	} while (FALSE);
	
	
	free(lpData); 
	return bSuccess; 
}

BOOL DosPathToNtPath(LPTSTR pszDosPath, LPTSTR pszNtPath)  
{  
	TCHAR           szDriveStr[500];  
	TCHAR           szDrive[3];  
	TCHAR           szDevName[100];  
	INT             cchDevName;  
	INT             i;  

	//检查参数  
	if(!pszDosPath || !pszNtPath )  
		return FALSE;  

	//获取本地磁盘字符串  
	if(GetLogicalDriveStrings(sizeof(szDriveStr), szDriveStr))  
	{  
		for(i = 0; szDriveStr[i]; i += 4)  
		{  
			/*
			if(!lstrcmpi(&(szDriveStr[i]), _T("A:\\")) || !lstrcmpi(&(szDriveStr[i]), _T("B:\\")))  
				continue; 
			*/

			szDrive[0] = szDriveStr[i];  
			szDrive[1] = szDriveStr[i + 1];  
			szDrive[2] = '\0';  
			if(!QueryDosDevice(szDrive, szDevName, 100))//查询 Dos 设备名  
				return FALSE;

			_tcscat(szDevName,_T("\\"));
			cchDevName = lstrlen(szDevName);  
			if(_tcsnicmp(pszDosPath, szDevName, cchDevName) == 0)//命中  
			{  
				lstrcpy(pszNtPath, szDrive);//复制驱动器  
				lstrcat(pszNtPath, pszDosPath + cchDevName - 1);//复制路径  

				return TRUE;  
			}             
		}  
	}  
	lstrcpy(pszNtPath, pszDosPath);  
	return FALSE;  
}  

BOOL CToolOper::GetProcessList(SYSTEM_PROCESS_MAP &process_list)
{
	
	HANDLE SnapShot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(SnapShot==NULL)
	{ 
		return false;
	}
	
	
	PROCESSENTRY32 ProcessInfo;//声明进程信息变量
	ProcessInfo.dwSize=sizeof(ProcessInfo);//设置ProcessInfo的大小
	//返回系统中第一个进程的信息
	BOOL Status=Process32First(SnapShot,&ProcessInfo); 
	
	USES_CONVERSION;
	std::string		key_str;
	PROCESS_INFO_2	info_2;
	while(Status)
	{    
		
		info_2.process_name = "";
		info_2.process_path = "";
		
		HANDLE hProcess=OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE,ProcessInfo.th32ProcessID);
		if (hProcess)
		{ 
			char	buffer[MAX_PATH];
			char    buffer_full[MAX_PATH];
			DWORD	needed = 0;
			
			memset(buffer,0,sizeof(buffer));
			memset(buffer_full,0,sizeof(buffer_full));
// 			EnumProcessModules(hProcess, &hModule, sizeof(hModule), &needed);
// 			GetModuleFileNameEx(hProcess, hModule, buffer, sizeof(buffer));
			
			std::string path = GetProcessName(hProcess);

// 			GetProcessImageFileName(hProcess, buffer, MAX_PATH);
// 			DosPathToNtPath(buffer, buffer_full); 

			key_str = path;
//			key_str = buffer_full;
			info_2.process_path = key_str;	
			CloseHandle(hProcess);
		}
 
		key_str = T2A(ProcessInfo.szExeFile); 
		info_2.process_name = key_str;
		
		process_list[ProcessInfo.th32ProcessID] = info_2;
		Status=Process32Next(SnapShot,&ProcessInfo);
	}
	CloseHandle(SnapShot);
	return TRUE;
}

void CToolOper::GetProcessInfo(DWORD pid, std::string &process_name, std::string &process_path)
{
	SYSTEM_PROCESS_MAP process_list;
	if (GetProcessList(process_list))
	{
		process_name = process_list[pid].process_name;
		process_path = process_list[pid].process_path;
	}
	else
	{
		process_name = "";
		process_path = "";
	}
}

void CToolOper::get_memory_info(DWORD pid, PPROCESS_MEMORY_COUNTERS ptr)
{
	typedef BOOL (WINAPI* func)(HANDLE,PPROCESS_MEMORY_COUNTERS,DWORD);
	HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
	if (hProcess)
	{
		HINSTANCE hInstance = LoadLibraryA("Psapi.dll"); 
		if (hInstance != NULL)
		{
			func ptr_func = (func)GetProcAddress(hInstance, "GetProcessMemoryInfo"); 
			if(ptr_func != NULL) 
			{
				ptr_func(hProcess, ptr, sizeof(PROCESS_MEMORY_COUNTERS));
			}
			FreeLibrary(hInstance);
		}

		CloseHandle(hProcess);
	}
}

size_t CToolOper::get_work_size(HANDLE hProcess, SIZE_T pageSize, HINSTANCE hinstance)
{
	typedef BOOL (WINAPI *Func)(HANDLE,PVOID,DWORD);
	Func ptr;
	ptr = (Func)GetProcAddress(hinstance, "QueryWorkingSet");
	if (ptr == NULL)
		return 0;
	BOOL bRet = TRUE;
	PSAPI_WORKING_SET_INFORMATION workSetInfo;
	PBYTE pByte = NULL;
	PSAPI_WORKING_SET_BLOCK *pWorkSetBlock = workSetInfo.WorkingSetInfo;
	memset(&workSetInfo, 0, sizeof(workSetInfo));
	// 要求操作进程的权限：PROCESS_QUERY_INFORMATION and PROCESS_VM_READ
	// 第一次调用获取实际缓冲区大小
	//bRet = ::QueryWorkingSet(hProcess, &workSetInfo, sizeof(workSetInfo));
	bRet = ptr(hProcess, &workSetInfo, sizeof(workSetInfo));
	//bRet = ptr(hProcess, buff, sizeof(buff));
	DWORD err = GetLastError();
	if(!bRet) // 调用失败
	{
		if(GetLastError() == ERROR_BAD_LENGTH) // 需要重新分配缓冲区
		{
			DWORD realSize = sizeof(workSetInfo.NumberOfEntries) 
				+ workSetInfo.NumberOfEntries*sizeof(PSAPI_WORKING_SET_BLOCK);

			pByte = new BYTE[realSize];
			memset(pByte, 0, realSize);
			pWorkSetBlock = (PSAPI_WORKING_SET_BLOCK *)(pByte + sizeof(workSetInfo.NumberOfEntries));
			// 重新获取
			if(!ptr(hProcess, pByte, realSize)/*!::QueryWorkingSet(hProcess, pByte, realSize)*/)
			{
				delete[] pByte; // 清理内存
				return 0;
			}
		}
		else // 其它错误，认为获取失败
			return 0;
	}
	SIZE_T workSetPrivate = 0;
	for (ULONG_PTR i = 0; i < workSetInfo.NumberOfEntries; ++i)
	{
		if(!pWorkSetBlock[i].Shared) // 如果不是共享页
			workSetPrivate += pageSize;
	}
	if(pByte)
		delete[] pByte;
	return workSetPrivate;
}

size_t CToolOper::get_work_size(DWORD pid)
{
	typedef BOOL (WINAPI * func)(PPERFORMANCE_INFORMATION,DWORD);
	PERFORMANCE_INFORMATION performanceInfo;
	memset(&performanceInfo, 0, sizeof(performanceInfo));

	SIZE_T page_size;
	HINSTANCE hinstace = LoadLibraryA("Psapi.dll");
	if (hinstace)
	{
		func ptr;
		ptr = (func)GetProcAddress(hinstace, "GetPerformanceInfo");
		if (ptr)
		{
			bool ret = ptr(&performanceInfo, sizeof(performanceInfo));
			if (!ret)
			{
				FreeLibrary(hinstace);
				return 0;
			}
			else
				page_size = performanceInfo.PageSize;
		}
	}
	HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
	if (hProcess)
	{
		size_t s = get_work_size(hProcess, page_size, hinstace);
		FreeLibrary(hinstace);
		CloseHandle(hProcess);
		return s;
	}
	else
	{
		FreeLibrary(hinstace);
		return 0;
	}
}