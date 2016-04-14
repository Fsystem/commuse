/** 全局辅助函数
*   @FileName  : FunctionHelper.h
*   @Author    : Double Sword(doublesword.jack@gmail.com)
*   @date      : 2016-4-7
*   @Copyright : Copyright belong to Author and 9u2,CO.LTD
*   @RefDoc    : --
*/

#ifndef __FunctionHelper_H
#define __FunctionHelper_H

//是否支持64位
BOOL GetWOW64PlatformSupport();

//文件签名是否可信-微软验证
//BOOL CheckFileTrust( LPCWSTR lpFileName);

//获取文件MD5
std::string GetFileMd5(const char * filename);

//提升权限
bool SetPrivilege();

//解压驱动
void CheckAndExportFile(const TCHAR* pResName, const TCHAR * path,DWORD dwResource);

//-------------------------------------------------------------------------------
//Dos路径转化为NT路径
BOOL DosPath2NtPath(LPTSTR pszDosPath, LPTSTR pszNtPath); 

//获取进程路径名称
std::string GetProcessName(HANDLE hProcess);

//获得win7进程路径
std::string GetWin7ProcessName(HANDLE handle);

//获得winXP进程路径
std::string GetXPProcessName(HANDLE handle);

//翻译驱动路径
std::string change_dirver_path(std::string path);

#endif //__FunctionHelper_H