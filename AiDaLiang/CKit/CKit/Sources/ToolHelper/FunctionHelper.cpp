#include <stdafx.h>
#include "FunctionHelper.h"
#include <windows.h>
#include <io.h>
#include <atlconv.h>
#include <Mmsystem.h>
#include <time.h>

#include <WinSock.h>

#include <Psapi.h>
#include <Tlhelp32.h>

#pragma comment(lib,"psapi")
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"Winmm.lib")

//-------------------------------------------------------------------------------
//提升权限
bool SetPrivilege(PCTSTR szPrivilege, BOOL fEnable)
{
	// Enabling the debug privilege allows the application to see
	// information about service applications
	bool fOk = false;    // Assume function fails
	HANDLE hToken;

	// Try to open this process's access token
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, 
		&hToken)) {

			// Attempt to modify the given privilege
			TOKEN_PRIVILEGES tp;
			tp.PrivilegeCount = 1;
			LookupPrivilegeValue(NULL, szPrivilege, &tp.Privileges[0].Luid);
			tp.Privileges[0].Attributes = fEnable ? SE_PRIVILEGE_ENABLED : 0;
			AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
			fOk = (GetLastError() == ERROR_SUCCESS);

			// Don't forget to close the token handle
			CloseHandle(hToken);
	}
	return(fOk);
}

//-------------------------------------------------------------------------------
typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS)(HANDLE,   PBOOL);   
LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(TEXT("kernel32")), "IsWow64Process");   
  
BOOL GetWOW64PlatformSupport()   
{   
	BOOL bIsWow64 = FALSE;

	//IsWow64Process is not available on all supported versions of Windows.
	//Use GetModuleHandle to get a handle to the DLL that contains the function
	//and GetProcAddress to get a pointer to the function if available.

	if(NULL != fnIsWow64Process)
	{
		if (!fnIsWow64Process(GetCurrentProcess(),&bIsWow64))
		{
			//handle error
		}
	}
	return bIsWow64;
 
}  

//系统是否是64位0S
BOOL IsWindows64()
{
	SYSTEM_INFO si={0};
	typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);
	PGNSI fnGetNativeSystemInfo = (PGNSI)GetProcAddress( GetModuleHandleA("kernel32"), "GetNativeSystemInfo");
	if (NULL != fnGetNativeSystemInfo) fnGetNativeSystemInfo(&si);
	else GetSystemInfo(&si);

	if(si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
		si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64) 
	{
		return TRUE;
	}
	else return FALSE;
}

/*判断是否是x64进程 
参  数:进程句柄 
返回值:是x64进程返回TRUE,否则返回FALSE 
*/  
BOOL IsWow64ProcessEx(HANDLE hProcess)  
{  
	if (!IsWindows64()) return FALSE;

	typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);   
	LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(TEXT("kernel32")), "IsWow64Process");  

	//如果没有导出则判定为32位 
	if(!fnIsWow64Process) return FALSE; 

	BOOL bX64=FALSE;  
	if(!fnIsWow64Process(hProcess,&bX64)) return FALSE;  

	return !bX64;  
}  

BOOL IsWow64ProcessEx(DWORD dwPid)  
{  
	BOOL bRet = FALSE;
	if(dwPid == 0) return bRet;

	HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_QUERY_LIMITED_INFORMATION ,FALSE,dwPid);
	if (hProcess)
	{
		bRet = IsWow64ProcessEx(hProcess);
		CloseHandle(hProcess);
	}
	return bRet;
}

BOOL IsWow64ProcessEx(LPCTSTR szExeName)
{
	return IsWow64ProcessEx(GetProcessIdByName(szExeName));
}

int GetSysVersion()
{
	OSVERSIONINFO osvi;//定义OSVERSIONINFO数据结构对象
	memset(&osvi, 0, sizeof(OSVERSIONINFO));//开空间 
	osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);//定义大小 
	GetVersionEx (&osvi);//获得版本信息 

	return osvi.dwMajorVersion;
}

std::string NIp2str(DWORD dwIp)
{
	std::string sIp;
	in_addr addr;
	addr.S_un.S_addr = dwIp;
	sIp = inet_ntoa(addr);

	return sIp;
}

DWORD Str2NIp(std::string szIp)
{
	DWORD dwIp=0;

	dwIp = inet_addr(szIp.c_str());

	return dwIp;
}

DWORD SetDisableOrEnable(BOOL IsEnable,PVOID *OldValue)
{
	typedef int (WINAPI *Wow64DisableWow64FsRedirectionFn)(PVOID *OldValue);
	typedef int (WINAPI *Wow64RevertWow64FsRedirectionFn)(PVOID OldValue);
	HMODULE hkernel = GetModuleHandleA("kernel32.dll");
	Wow64DisableWow64FsRedirectionFn Disable_redirect = NULL;
	Wow64RevertWow64FsRedirectionFn Enable_redirect = NULL;

	if(hkernel == NULL)
	{
		return 0;
	}

	Disable_redirect = (Wow64DisableWow64FsRedirectionFn)GetProcAddress(hkernel, "Wow64DisableWow64FsRedirection");
	Enable_redirect = (Wow64RevertWow64FsRedirectionFn)GetProcAddress(hkernel, "Wow64RevertWow64FsRedirection");

	if (Disable_redirect == NULL || Enable_redirect == NULL)
	{
		return 0;
	}

	if (!IsEnable) Disable_redirect(OldValue);
	else Enable_redirect(*OldValue);

	return 0;
}

void UnixTimeToFileTime(DWORD tmUnixTime, FILETIME& fileTime)  
{  
	//FILETIME starts from 1601-01-01 UTC, epoch from 1970- 01-01
	ULARGE_INTEGER ull = {0};
	ull.QuadPart = tmUnixTime * 10000000ULL + 116444736000000000ULL;  
	fileTime.dwLowDateTime = ull.LowPart;  
	fileTime.dwHighDateTime = ull.HighPart;  
}  

void FileTimeToUnixTime(DWORD& tmUnixTime, const FILETIME& fileTime) 
{
	//FILETIME starts from 1601-01-01 UTC, epoch from 1970- 01-01
	ULARGE_INTEGER ull = { 0 };
	ull.HighPart = fileTime.dwHighDateTime;
	ull.LowPart = fileTime.dwLowDateTime;
	tmUnixTime = (DWORD)((ull.QuadPart - 116444736000000000ULL) / 10000000ULL);
}

DWORD GetBootTime()
{
	DWORD startMSCount;//从开机到现在的毫秒数
	startMSCount = timeGetTime();
	time_t CurSysTime, BootSysTime;
	time(&CurSysTime);
	//将开机到现在的毫秒数转换为秒数，再用当前的时间减去，获得开机时间
	BootSysTime = CurSysTime - startMSCount/1000;

	return BootSysTime;
}

DWORD GetShutTime()
{
	HKEY hKey;
	LONG lResult;
	DWORD ShutdownTime = 0;
	LPTSTR lpSubKey=TEXT("SYSTEM\\CurrentControlSet\\Control\\Windows");
	lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpSubKey, 0, KEY_READ, &hKey);

	if (lResult != ERROR_SUCCESS)
		return ShutdownTime;
	FILETIME FileTime;
	DWORD dwSize;
	lResult = RegQueryValueEx(hKey, TEXT("ShutdownTime"), 
		NULL, NULL, NULL, &dwSize);
	if (lResult == ERROR_SUCCESS && dwSize==sizeof(FileTime))
	{
		lResult = RegQueryValueEx(hKey, TEXT("ShutdownTime"), NULL, 
			NULL, (LPBYTE)&FileTime, &dwSize);
		FileTimeToUnixTime(ShutdownTime,FileTime);
	}
	RegCloseKey(hKey);
	return ShutdownTime;
}

//-------------------------------------------------------------------------------
/*
通过一个地址取模块句柄
*/
HMODULE ModuleHandleByAddr(const void* ptrAddr)
{
	MEMORY_BASIC_INFORMATION info;
	::VirtualQuery(ptrAddr, &info, sizeof(info));
	return (HMODULE)info.AllocationBase;
}
/*
当前模块句柄
*/
HMODULE ThisModuleHandle()
{
	static HMODULE sInstance = ModuleHandleByAddr((void*)&ThisModuleHandle);
	return sInstance;
}

//-------------------------------------------------------------------------------

BOOL hasDll(DWORD processId,LPCTSTR szDllName) 
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processId);
	if (hSnapshot == NULL)
	{
		return FALSE;
	}
	MODULEENTRY32 me;
	me.dwSize = sizeof(MODULEENTRY32);
	Module32First(hSnapshot, &me);
	do {
		if (_tcsncmp(me.szModule, szDllName , MAX_MODULE_NAME32) == 0) 
		{
			CloseHandle(hSnapshot);
			return TRUE;
		}
	} while (Module32Next(hSnapshot, &me));

	CloseHandle(hSnapshot);

	return FALSE;
}
//-------------------------------------------------------------------------------

BOOL ExtractFile(LPCTSTR restype, int resid, LPCTSTR destpath,HMODULE hModule)
{
	HRSRC hRes;
	HGLOBAL hFileData;
	BOOL bResult = FALSE;

	hRes = FindResource(hModule, MAKEINTRESOURCE(resid), restype);
	if(hRes)
	{
		hFileData = LoadResource(hModule, hRes);
		if(hFileData)
		{
			DWORD dwSize = SizeofResource(hModule, hRes);
			if(dwSize)
			{
				HANDLE hFile = CreateFile( destpath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, 0);
				if(hFile != INVALID_HANDLE_VALUE)
				{
					DWORD dwWritten = 0;
					BOOL b = WriteFile(hFile, hFileData, dwSize, &dwWritten, NULL);
					if(b && dwWritten == dwSize)
					{
						CloseHandle(hFile);
						return TRUE;
					}

					CloseHandle(hFile);
				}
			}
		}
	} 
	return bResult;
}


void CheckAndExportFile(const TCHAR* pResName, const TCHAR * path,DWORD dwResource)
{
	if (_taccess(path,0) == -1)
	{
		ExtractFile(pResName, dwResource, path, ThisModuleHandle());
	}

}

std::vector<char> GetPEFile(int nResId,const char* szResType)
{
	std::vector<char> vecRet;

	HMODULE hCurModule = ThisModuleHandle();

	HRSRC hRes = FindResourceA(hCurModule,MAKEINTRESOURCEA(nResId),szResType);
	if (hRes)
	{
		HGLOBAL hFileData = LoadResource(hCurModule,hRes);
		if (hFileData)
		{
			DWORD dwFileSize = SizeofResource(hCurModule,hRes);

			vecRet.assign((char*)hFileData,(char*)hFileData+dwFileSize);
		}
	}

	return vecRet;
}

//-------------------------------------------------------------------------------
//#pragma region 全局函数

BOOL DosPath2NtPath(LPTSTR pszDosPath, LPTSTR pszNtPath)  
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
//  			if(!lstrcmpi(&(szDriveStr[i]), _T("A:\\")) || !lstrcmpi(&(szDriveStr[i]), _T("B:\\")))  
//  				continue;  

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

//获取进程路径名称
std::string GetProcessName(HANDLE hProcess)
{
	OSVERSIONINFO osvi;//定义OSVERSIONINFO数据结构对象
	memset(&osvi, 0, sizeof(OSVERSIONINFO));//开空间 
	osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);//定义大小 
	GetVersionEx (&osvi);//获得版本信息 

	if (osvi.dwMajorVersion >= 6)//os vista above
	{
		return GetWin7ProcessName(hProcess);
	}
	else
	{
		return GetXPProcessName(hProcess);
	}
}

//获得win7进程路径
std::string GetWin7ProcessName(HANDLE handle)
{
	std::string strRet="";

	USES_CONVERSION;

	typedef DWORD (__stdcall *ptr_fun)(HANDLE,LPTSTR,DWORD);
	HINSTANCE hInstance = LoadLibrary(TEXT("Psapi.dll") ); 
	if (hInstance != NULL)
	{
#ifdef UNICODE
		ptr_fun ptr = (ptr_fun)GetProcAddress(hInstance, "GetProcessImageFileNameW");
#else
		ptr_fun ptr = (ptr_fun)GetProcAddress(hInstance, "GetProcessImageFileNameA");
#endif

		if(ptr != NULL)  
		{
			TCHAR procName[MAX_PATH] = {0};
			DWORD dwRet = ptr(handle, procName, MAX_PATH);
			char procName_tmp[MAX_PATH] = {0};
			if( 0 == dwRet)
			{
				DWORD dwLastErr = ::GetLastError();
				dwRet = GetModuleFileNameEx(handle, NULL, procName, MAX_PATH);
				strRet = T2A(procName);
			}
			else
			{
				TCHAR cNtPath[MAX_PATH + 1] = {0};
				DosPath2NtPath(procName, cNtPath);
				strRet = T2A(cNtPath);
			}

			//DosPathToNtPath(procName, procName_tmp);
			FreeLibrary(hInstance);
		}
	}
	return strRet;
}

//获得winXP进程路径
std::string GetXPProcessName(HANDLE handle)
{
	std::string strRet="";

	USES_CONVERSION;

	typedef DWORD (__stdcall *ptr_fun)(HANDLE,LPTSTR,DWORD);
	HINSTANCE hInstance = LoadLibrary(TEXT("Psapi.dll") ); 
	if (hInstance != NULL)
	{
#ifdef UNICODE
		ptr_fun ptr = (ptr_fun)GetProcAddress(hInstance, "GetProcessImageFileNameW"); 
#else
		ptr_fun ptr = (ptr_fun)GetProcAddress(hInstance, "GetProcessImageFileNameA"); 
#endif

		if (ptr != NULL)
		{
			TCHAR procName[MAX_PATH] = {0};
			DWORD dwRet = ptr(handle, procName, MAX_PATH);
			char procName_tmp[MAX_PATH] = {0};
			if( 0 == dwRet)
			{
				dwRet = GetModuleFileNameEx(handle, NULL, procName, MAX_PATH);
				strRet = T2A(procName);
			}
			else
			{
				TCHAR cNtPath[MAX_PATH + 1] = {0};
				DosPath2NtPath(procName, cNtPath);
				strRet = T2A(cNtPath);
			}

			//DosPathToNtPath(procName, procName_tmp);
			FreeLibrary(hInstance);
		}
	}
	return strRet;
}

std::string change_dirver_path(std::string path)
{
	if (path.empty())
		return "";
	char src[MAX_PATH] = {0};
	char dest[MAX_PATH] = {0};
	std::string str_32_u = "system32";
	std::string str_32_l = "System32";

	std::string str_sysroot_ru = "\\SystemRoot\\System32\\";
	std::string str_sysroot_rl = "\\SystemRoot\\system32\\";

	std::string str_sysroot_au = "%SystemRoot%\\System32\\";
	std::string str_sysroot_al = "%SystemRoot%\\system32\\";

	std::string str_sysroot_ll = "%systemroot%\\system32\\";
	if (path.length() > 4)
	{
		if (path.substr(0 , 4).compare("\\??\\") == 0)
			return path.substr(4, path.length() - 4);
		else if(path.substr(0, str_32_u.length()).compare(str_32_u) == 0)
		{
			strcat(dest, "C:\\Windows\\system32");
			strcat(dest, path.substr(str_32_u.length(), path.length() - str_32_u.length()).c_str());
			return dest;
		}
		else if(path.substr(0, str_32_l.length()).compare(str_32_l) == 0)
		{
			strcat(dest, "C:\\Windows\\System32");
			strcat(dest, path.substr(str_32_l.length(), path.length() - str_32_l.length()).c_str());
			return dest;
		}
		else if(path.substr(0, str_sysroot_ru.length()).compare(str_sysroot_ru) == 0)
		{
			strcat(dest, "C:\\Windows\\System32\\");
			strcat(dest, path.substr(str_sysroot_ru.length(), path.length() - str_sysroot_ru.length()).c_str());
			return dest;
		}
		else if(path.substr(0, str_sysroot_rl.length()).compare(str_sysroot_rl) == 0)
		{
			strcat(dest, "C:\\Windows\\System32\\");
			strcat(dest, path.substr(str_sysroot_rl.length(), path.length() - str_sysroot_rl.length()).c_str());
			return dest;
		}
		else if(path.substr(0, str_sysroot_au.length()).compare(str_sysroot_au) == 0)
		{
			strcat(dest, "C:\\Windows\\System32\\");
			strcat(dest, path.substr(str_sysroot_au.length(), path.length() - str_sysroot_au.length()).c_str());
			return dest;
		}
		else if(path.substr(0, str_sysroot_al.length()).compare(str_sysroot_al) == 0)
		{
			strcat(dest, "C:\\Windows\\System32\\");
			strcat(dest, path.substr(str_sysroot_al.length(), path.length() - str_sysroot_al.length()).c_str());
			return dest;
		}
		else if(path.substr(0, str_sysroot_ll.length()).compare(str_sysroot_ll) == 0)
		{
			strcat(dest, "C:\\Windows\\System32\\");
			strcat(dest, path.substr(str_sysroot_al.length(), path.length() - str_sysroot_al.length()).c_str());
			return dest;
		}
		else
			return path;
	}
	else
		return path;
}


//-------------------------------------------------------------------------------
DWORD GetProcessIdByName(LPCTSTR procname)
{
	if(procname == NULL) return 0;

	HANDLE SnapShot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(SnapShot==NULL)
	{ 
		return 0;
	}

	PROCESSENTRY32 ProcessInfo;//声明进程信息变量
	ProcessInfo.dwSize=sizeof(ProcessInfo);//设置ProcessInfo的大小
	//返回系统中第一个进程的信息
	BOOL Status=Process32First(SnapShot,&ProcessInfo); 

	while(Status)
	{    
		if (_tcsnicmp(procname,ProcessInfo.szExeFile,MAX_PATH)==0)
		{
			CloseHandle(SnapShot);
			return ProcessInfo.th32ProcessID;
		}
		
		Status=Process32Next(SnapShot,&ProcessInfo);
	}
	CloseHandle(SnapShot);
	return 0;
}

//-------------------------------------------------------------------------------
/***
*char *stristr(string1, string2) - search for string2 in string1 without regard to case.
*
*Purpose:
*       finds the first occurrence of string2 in string1 without regard to case.
*
*Entry:
*       char *string1 - string to search in
*       char *string2 - string to search for
*
*Exit:
*       returns a pointer to the first occurrence of string2 in
*       string1, or NULL if string2 does not occur in string1
*
*Uses:
*
*Exceptions:
*
*******************************************************************************/

char* stristr (
	const char * str1,
	const char * str2
	)
{
	char *cp = (char *) str1;
	char *s1, *s2;

	if ( !*str2 )
		return((char *)str1);

	while (*cp)
	{
		s1 = cp;
		s2 = (char *) str2;

		while (*s1 && *s2)
		{
			char ch1=*s1,ch2=*s2;
			if (isascii(*s1) && isupper(*s1) ) ch1 = _tolower(*s1);
			if (isascii(*s2) && isupper(*s2) ) ch2 = _tolower(*s2);

			if(ch1-ch2==0) s1++, s2++;
			else break;
		}
			
		if (!*s2)
			return(cp);

		cp++;
	}

	return(NULL);

}

WCHAR* wcsistr (
	const WCHAR * str1,
	const WCHAR * str2
	)
{
	WCHAR *cp = (WCHAR *) str1;
	WCHAR *s1, *s2;

	if ( !*str2 )
		return((WCHAR *)str1);

	while (*cp)
	{
		s1 = cp;
		s2 = (WCHAR *) str2;

		while (*s1 && *s2)
		{
			WCHAR ch1=*s1,ch2=*s2;
			if (iswascii(*s1) && iswupper(*s1) ) ch1 = towlower(*s1);
			if (iswascii(*s2) && iswupper(*s2) ) ch2 = towlower(*s2);

			if(ch1-ch2==0) s1++, s2++;
			else break;
		}

		if (!*s2)
			return(cp);

		cp++;
	}

	return(NULL);

}

std::string W2AString(LPCWSTR szSrc)
{
	std::string sOut = "";
	LPSTR pszOut = NULL;
	if(szSrc==NULL) return sOut;

	int sLen = wcslen(szSrc);
	if(sLen == 0) return sOut;
	int len = ::WideCharToMultiByte(CP_ACP,0,szSrc,sLen,NULL,0,0,0);
	pszOut = new char[len+1];
	if (pszOut)
	{
		::WideCharToMultiByte(CP_ACP,0,szSrc,sLen,pszOut,len,0,0);
		pszOut[len] = 0;
		sOut = std::string(pszOut);
		delete[] pszOut;
	}

	return sOut;
}

std::wstring A2WString(LPCSTR szSrc)
{
	std::wstring sOut = L"";
	LPWSTR pszOut = NULL;

	if(szSrc == NULL) return sOut;

	int sLen = strlen(szSrc);
	if(sLen == 0) return sOut;

	int len = ::MultiByteToWideChar(CP_ACP,0,szSrc,sLen,NULL,0);
	pszOut = new WCHAR[len+1];
	if (pszOut)
	{
		::MultiByteToWideChar(CP_ACP,0,szSrc,sLen,pszOut,len);
		pszOut[len] = 0;
		sOut = std::wstring(pszOut);
		delete[] pszOut;
	}

	return sOut;
}

//-------------------------------------------------------------------------------
std::string GetFileMd5(const char * filename)
{
	std::string strRet;

	PVOID old_value = NULL;
	if(GetWOW64PlatformSupport()) SetDisableOrEnable(FALSE,&old_value);

	{
		if( NULL == filename)
		{
			return strRet;
		}

		FILE* fp = fopen(filename, "rb");
		if( NULL == fp)
		{
			DWORD err = GetLastError();
			return strRet;
		}
		fseek(fp, 0, SEEK_END);
		long nTotalLen = ftell(fp);
		fseek(fp, SEEK_SET, 0);
		char* pBuffer = new char[nTotalLen + 100];
		if( NULL == pBuffer)
		{
			fclose(fp);
			return "";
		}
		int nReadLen = 0;
		int nIndex = 0;
		while( nIndex < nTotalLen)
		{
			nReadLen = fread(&pBuffer[nIndex], 1, 4096, fp);
			nIndex += nReadLen;
			if(nReadLen < 1)
			{
				break;
			}
		}
		pBuffer[nIndex] = 0;
		fclose(fp);
		MD5 md5;
		md5.update(pBuffer, nIndex);
		delete [] pBuffer;
		strRet = md5.toString();
	}

	if(GetWOW64PlatformSupport()) SetDisableOrEnable(TRUE,&old_value);

	return strRet;
}

unsigned int GetFileCrc(const char * filename)
{
	unsigned int unRet = 0;

	PVOID old_value = NULL;
	if(GetWOW64PlatformSupport()) SetDisableOrEnable(FALSE,&old_value);

	{
		if( NULL == filename)
		{
			char szError[512]={0};
			_snprintf(szError,512,"error:文件名空\n");
			OutputDebugStringA(szError);
			return 0;
		}

		FILE* fp = fopen(filename, "rb");
		if( NULL == fp)
		{
			DWORD err = GetLastError();
			char szError[512]={0};
			_snprintf(szError,512,"error:[GetFileCrc]打开文件失败[%s][errno:%u]\n",filename,err);
			OutputDebugStringA(szError);
			return 0;
		}

		fseek(fp, 0, SEEK_END);
		long nTotalLen = ftell(fp);
		rewind(fp);

		char cbBuffer[8192];

		int nReadLen = 0;

		CCRC crc;
		while( (nReadLen=fread(cbBuffer, 1, 8192, fp)) > 0 )
		{
			unRet = crc.GetCrcNumber(cbBuffer,nReadLen);
		}

		fclose(fp);
	}

	if(GetWOW64PlatformSupport()) SetDisableOrEnable(TRUE,&old_value);

	return unRet;
}

std::string GetFileMd5OrCRC(const char * filename,unsigned int & unCRC)
{
	//unsigned int unRet = 0;
	unCRC = 0;
	std::string strRet="";

	PVOID old_value = NULL;
	if(GetWOW64PlatformSupport()) SetDisableOrEnable(FALSE,&old_value);

	{
		if( NULL == filename)
		{
			char szError[512]={0};
			_snprintf(szError,512,"error:文件名空\n");
			OutputDebugStringA(szError);
			return strRet;
		}

		FILE* fp = fopen(filename, "rb");
		if( NULL == fp)
		{
			DWORD err = GetLastError();
			char szError[512]={0};
			_snprintf(szError,512,"error:[GetFileMd5OrCRC]打开文件失败[%s][errno:%u]\n",filename,err);
			OutputDebugStringA(szError);
			return strRet;
		}

		fseek(fp, 0, SEEK_END);
		long nTotalLen = ftell(fp);

		if( 0 >= nTotalLen)
		{
			char szError[512]={0};
			_snprintf(szError,512,"error:[GetFileMd5OrCRC]文件长度错误0[%s]\n",filename);
			OutputDebugStringA(szError);

			fclose(fp);
			return strRet;
		}

		rewind(fp);

		char* pBuffer = new char[nTotalLen];

		if(pBuffer == NULL)
		{
			char szError[512]={0};
			_snprintf(szError,512,"error:[GetFileMd5OrCRC]分配内存错误[%s][nTotalLen]\n",filename,nTotalLen);
			OutputDebugStringA(szError);

			fclose(fp);
			return strRet;
		}

		CCRC crc;
		if( fread(pBuffer, 1, nTotalLen, fp) != nTotalLen )
		{
			char szError[512]={0};
			_snprintf(szError,512,"error:[GetFileMd5OrCRC]读取文件长度错误[%u][%s]\n",nTotalLen,filename);
			OutputDebugStringA(szError);

			fclose(fp);
			delete[] pBuffer;

			return strRet;
		}

		//CRC
		unCRC = crc.GetCrcNumber(pBuffer,nTotalLen);

		//MD5
		static MD5 md5;
		md5.reset();
		md5.update(pBuffer, nTotalLen);
		strRet = md5.toString();

		fclose(fp);
		delete[] pBuffer;
	}

	if(GetWOW64PlatformSupport()) SetDisableOrEnable(TRUE,&old_value);

	return strRet;
}

unsigned int GetFileCrc_Old(const char * filename)
{
	unsigned int unRet = 0;

	HANDLE hand;///句柄
	hand=CreateFileA(filename,GENERIC_READ,///打开方式，可读
		FILE_SHARE_READ,//共享读
		NULL,OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	//没有打开
	if (hand==INVALID_HANDLE_VALUE)
	{
		char szError[512]={0};
		_snprintf(szError,512,"error:[GetFileCrc]打开文件失败[%s]\n",filename);
		OutputDebugStringA(szError);
		return 0;
	}

	LARGE_INTEGER m_file_len;
	BY_HANDLE_FILE_INFORMATION m_file_info;
	::GetFileInformationByHandle(hand,&m_file_info);
	m_file_len.LowPart=m_file_info.nFileSizeLow;
	m_file_len.HighPart=m_file_info.nFileSizeHigh;

	ULONG re;
	char buf[1024*8];
	CCRC crc;
	while(m_file_len.QuadPart)
	{
		memset(buf,0,1024*8);
		if(!ReadFile(hand,buf,1024*8,&re,0))
		{
			///读取文件失败
			return 0;
		}
		m_file_len.QuadPart=m_file_len.QuadPart-re;
		unRet = crc.GetCrcNumber((char*)buf,re);
	}
	CloseHandle(hand);

	if (unRet == 0)
	{
		char szError[512]={0};
		_snprintf(szError,512,"error:[GetFileCrc]打开文件失败[%s]长度[%i64d]\n",filename,m_file_len.QuadPart);
		OutputDebugStringA(szError);
	}

	return unRet;
}

////获取操作Key
//unsigned int GetActionKey(LPCSTR pParantPath,LPCSTR pChildPath,WORD operate)
//{
//	if(operate == enOperateNull) return 0;
//
//	char szRandKey[1024]={0};
//	sprintf(szRandKey,"%s%s%d",pParantPath, pChildPath, operate);
//
//	CCRC crc;
//	return crc.GetStrCrc_Key(szRandKey,strlen(szRandKey));
//}
//
////获取操作Key
//unsigned int GetActionKeyWithResult(LPCSTR pParantPath,LPCSTR pChildPath,WORD operate,int nResult)
//{
//	if(operate == enOperateNull) return 0;
//
//	char szRandKey[1024]={0};
//	sprintf(szRandKey,"%s%s%d%d",pParantPath, pChildPath, operate,nResult);
//
//	CCRC crc;
//	return crc.GetStrCrc_Key(szRandKey,strlen(szRandKey));
//}

//-------------------------------------------------------------------------------
