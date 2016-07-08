/** ȫ�ָ�������
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
* !brief ����Ȩ��(SE_DEBUG)
*/
bool SetPrivilege(PCTSTR szPrivilege=SE_DEBUG_NAME, BOOL fEnable=TRUE);

/*
* !brief �Ƿ�֧��64λ
*/
BOOL GetWOW64PlatformSupport();

/*
* !brief ϵͳ�Ƿ���64λ0S
*/
BOOL IsWindows64();

/*
* !brief �ж��Ƿ���x64����
*/
BOOL IsWow64ProcessEx(HANDLE hProcess);
BOOL IsWow64ProcessEx(DWORD dwPid);
BOOL IsWow64ProcessEx(LPCTSTR szExeName);

//�ļ�ǩ���Ƿ����-΢����֤
//BOOL CheckFileTrust( LPCWSTR lpFileName);

int GetSysVersion();

/*
* !brief ����IPת�ַ���
*/
std::string NIp2str(DWORD dwIp);

/*
* !brief �ַ���ת����IP
*/
DWORD Str2NIp(std::string szIp);
/*
* !brief �����ض���
*/
DWORD SetDisableOrEnable(BOOL IsEnable,PVOID *OldValue);
/*
* !brief ʱ���ת����FileTime
*/
void UnixTimeToFileTime(DWORD tmUnixTime, FILETIME& fileTime) ;
/*
* !brief FileTimeת����ʱ���
*/
void FileTimeToUnixTime(DWORD& tmUnixTime, const FILETIME& fileTime) ;
/*
* !brief ��ȡ����ʱ��(unix time stamp)
*/
DWORD GetBootTime();
/*
* !brief ��ȡ�ػ�ʱ��(unix time stamp)
*/
DWORD GetShutTime();
//-------------------------------------------------------------------------------
/*
��ǰģ����
*/
HMODULE ThisModuleHandle();
/*
* !brief ��ѹ�ļ�
*/
void CheckAndExportFile(const TCHAR* pResName, const TCHAR * path,DWORD dwResource);

/*
* !brief ��ȡpe�ļ����ڴ�����
		 nResId:��ԴID
		 szResType:��Դ����
*/
std::vector<char> GetPEFile(int nResId,const char* szResType);


//-------------------------------------------------------------------------------
/*
* !brief Dos·��ת��ΪNT·��
*/
BOOL DosPath2NtPath(LPTSTR pszDosPath, LPTSTR pszNtPath); 

/*
* !brief ��ȡ����·������
*/
std::string GetProcessName(HANDLE hProcess);

/*
* !brief ���win7����·��
*/
std::string GetWin7ProcessName(HANDLE handle);

/*
* !brief ���winXP����·��
*/
std::string GetXPProcessName(HANDLE handle);

/*
* !brief ��������·��
*/
std::string change_dirver_path(std::string path);

/*
* !brief ��ȡ����idͨ������
*/
DWORD GetProcessIdByName(LPCTSTR procname);

/*
* !brief �����Ƿ������dll
*/
BOOL hasDll(DWORD processId,LPCTSTR szDllName) ;

//-------------------------------------------------------------------------------
/*
* !brief �����ִ������ִ�Сд
*/
char* stristr (const char * str1,const char * str2);
WCHAR* wcsistr (const WCHAR * str1,const WCHAR * str2);

std::string	W2AString(LPCWSTR szSrc);
std::wstring A2WString(LPCSTR szSrc);

//-------------------------------------------------------------------------------
/*
* !brief ��ȡ�ļ�MD5
*/
std::string GetFileMd5(const char * filename);
/*
* !brief ��ȡ�ļ�CRC
*/
unsigned int GetFileCrc(const char * filename);
/*
* !brief ��ȡ�ļ�CRC��MD5
*/
std::string GetFileMd5OrCRC(const char * filename,unsigned int & unCRC);

////��ȡ����Key
//unsigned int GetActionKey(LPCSTR pParantPath,LPCSTR pChildPath,WORD operate);
//
////��ȡ����Key
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