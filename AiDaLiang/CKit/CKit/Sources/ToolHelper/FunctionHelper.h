/** 全局辅助函数
*   @FileName  : FunctionHelper.h
*   @Author    : Double Sword(doublesword.jack@gmail.com)
*   @date      : 2016-4-7
*   @Copyright : Copyright belong to Author and 9u2,CO.LTD
*   @RefDoc    : --
*/

#ifndef __FunctionHelper_H
#define __FunctionHelper_H

//-------------------------------------------------------------------------------
/*
* !brief 提升权限(SE_DEBUG)
*/
bool SetPrivilege(PCTSTR szPrivilege=SE_DEBUG_NAME, BOOL fEnable=TRUE);

/*
* !brief 是否支持64位
*/
BOOL GetWOW64PlatformSupport();

/*
* !brief 系统是否是64位0S
*/
BOOL IsWindows64();

/*
* !brief 判断是否是x64进程
*/
BOOL IsWow64ProcessEx(HANDLE hProcess);
BOOL IsWow64ProcessEx(DWORD dwPid);
BOOL IsWow64ProcessEx(LPCTSTR szExeName);

//文件签名是否可信-微软验证
//BOOL CheckFileTrust( LPCWSTR lpFileName);

int GetSysVersion();

/*
* !brief 整数IP转字符串
*/
std::string NIp2str(DWORD dwIp);

/*
* !brief 字符串转整数IP
*/
DWORD Str2NIp(std::string szIp);
/*
* !brief 设置重定向
*/
DWORD SetDisableOrEnable(BOOL IsEnable,PVOID *OldValue);
/*
* !brief 时间戳转换到FileTime
*/
void UnixTimeToFileTime(DWORD tmUnixTime, FILETIME& fileTime) ;
/*
* !brief FileTime转换到时间戳
*/
void FileTimeToUnixTime(DWORD& tmUnixTime, const FILETIME& fileTime) ;
/*
* !brief 获取开机时间(unix time stamp)
*/
DWORD GetBootTime();
/*
* !brief 获取关机时间(unix time stamp)
*/
DWORD GetShutTime();
//-------------------------------------------------------------------------------
/*
当前模块句柄
*/
HMODULE ThisModuleHandle();
/*
* !brief 解压文件
*/
void CheckAndExportFile(const TCHAR* pResName, const TCHAR * path,DWORD dwResource);

/*
* !brief 获取pe文件的内存数据
		 nResId:资源ID
		 szResType:资源类型
*/
std::vector<char> GetPEFile(int nResId,const char* szResType);


//-------------------------------------------------------------------------------
/*
* !brief Dos路径转化为NT路径
*/
BOOL DosPath2NtPath(LPTSTR pszDosPath, LPTSTR pszNtPath); 

/*
* !brief 获取进程路径名称
*/
std::string GetProcessName(HANDLE hProcess);

/*
* !brief 获得win7进程路径
*/
std::string GetWin7ProcessName(HANDLE handle);

/*
* !brief 获得winXP进程路径
*/
std::string GetXPProcessName(HANDLE handle);

/*
* !brief 翻译驱动路径
*/
std::string change_dirver_path(std::string path);

/*
* !brief 获取进程id通过名字
*/
DWORD GetProcessIdByName(LPCTSTR procname);

/*
* !brief 进程是否包含此dll
*/
BOOL hasDll(DWORD processId,LPCTSTR szDllName) ;

//-------------------------------------------------------------------------------
/*
* !brief 查找字串不区分大小写
*/
char* stristr (const char * str1,const char * str2);
WCHAR* wcsistr (const WCHAR * str1,const WCHAR * str2);

std::string	W2AString(LPCWSTR szSrc);
std::wstring A2WString(LPCSTR szSrc);

//-------------------------------------------------------------------------------
/*
* !brief 获取文件MD5
*/
std::string GetFileMd5(const char * filename);
/*
* !brief 获取文件CRC
*/
unsigned int GetFileCrc(const char * filename);
/*
* !brief 获取文件CRC和MD5
*/
std::string GetFileMd5OrCRC(const char * filename,unsigned int & unCRC);

////获取操作Key
//unsigned int GetActionKey(LPCSTR pParantPath,LPCSTR pChildPath,WORD operate);
//
////获取操作Key
//unsigned int GetActionKeyWithResult(LPCSTR pParantPath,LPCSTR pChildPath,WORD operate,int nResult);

//-------------------------------------------------------------------------------

#ifdef UNICODE
#	define TSTD_STING std::wstring
#	define T2AString(szSrc)  W2AString(szSrc)
#	define T2WString(szSrc)  std::wstring(szSrc)
#	define A2TString(szSrc)  A2WString(szSrc)
#	define W2TString(szSrc)  std::wstring(szSrc)

#	define _tcsistr wcsistr
#else
#	define TSTD_STING std::string
#	define T2AString(szSrc)  std::string(szSrc)
#	define T2WString(szSrc)  A2WString(szSrc)
#	define A2TString(szSrc)  std::string(szSrc)
#	define W2TString(szSrc)  W2AString(szSrc)

#	define _tcsistr stristr
#endif

#endif //__FunctionHelper_H