/** ȫ�ָ�������
*   @FileName  : FunctionHelper.h
*   @Author    : Double Sword(doublesword.jack@gmail.com)
*   @date      : 2016-4-7
*   @Copyright : Copyright belong to Author and 9u2,CO.LTD
*   @RefDoc    : --
*/

#ifndef __FunctionHelper_H
#define __FunctionHelper_H

//�Ƿ�֧��64λ
BOOL GetWOW64PlatformSupport();

//�ļ�ǩ���Ƿ����-΢����֤
//BOOL CheckFileTrust( LPCWSTR lpFileName);

//��ȡ�ļ�MD5
std::string GetFileMd5(const char * filename);

//����Ȩ��
bool SetPrivilege();

//��ѹ����
void CheckAndExportFile(const TCHAR* pResName, const TCHAR * path,DWORD dwResource);

//-------------------------------------------------------------------------------
//Dos·��ת��ΪNT·��
BOOL DosPath2NtPath(LPTSTR pszDosPath, LPTSTR pszNtPath); 

//��ȡ����·������
std::string GetProcessName(HANDLE hProcess);

//���win7����·��
std::string GetWin7ProcessName(HANDLE handle);

//���winXP����·��
std::string GetXPProcessName(HANDLE handle);

//��������·��
std::string change_dirver_path(std::string path);

#endif //__FunctionHelper_H