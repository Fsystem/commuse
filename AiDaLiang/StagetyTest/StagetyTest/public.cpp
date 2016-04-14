
#include "stdafx.h"
#include "public.h"
#include <io.h>


#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "ws2_32.lib")

#define STAT_SWOFF				0x0001
#define STAT_LOGOFF				0x0002
#define STAT_RESTART			0x0004





/*
char g_LogFileName[MAX_PATH] = {0};
void LocalWriteLog(UINT nModuleId, const char* pLogName,  LPCSTR pFileName,const UINT nLine, LPCSTR pFormat,...)
{
	char cLogText[1024];
	char cTmpLogFile[1024];
	SYSTEMTIME stNow;
	::GetLocalTime(&stNow);
	va_list vaList;
	va_start(vaList, pFormat);
	_vsnprintf(cLogText, sizeof(cLogText) - 1, pFormat,
		vaList);
	va_end(vaList);
	char cLog[1024 + MAX_PATH];
	memset(cLog, 0, sizeof(cLog));
	_snprintf(cLog, sizeof(cLog) - 1, "[%s-%d][module=0x%08x]\r\n[%02d:%02d:%02d]%s\r\n",  pFileName, nLine, nModuleId, 
		stNow.wHour, stNow.wMinute, stNow.wSecond, cLogText);
	TRACE("%s\n", cLog);
	char* pLogFile = (char*)pLogName;
	if( NULL == pLogFile)
	{
		if( '\0' == g_LogFileName[0])
		{
			CString strPath;
			GetModuleFileName(ThisModuleHandle(), strPath.GetBuffer(MAX_PATH), MAX_PATH - 1);
			strPath.ReleaseBuffer();
			int nIndex = strPath.ReverseFind('\\');
			strPath = strPath.Left(nIndex);
			_snprintf(g_LogFileName, sizeof(g_LogFileName) - 1, "%s", strPath);
		}
		_snprintf(cTmpLogFile, sizeof(cTmpLogFile) - 1, "%s\\%d-%02d-%02d_log.log", g_LogFileName, 
			stNow.wYear, stNow.wMonth, stNow.wDay);
		pLogFile = cTmpLogFile;
	}
	FILE* fp = fopen(pLogFile, "a+");
	if( NULL == fp)
	{
		return;
	}
	fwrite(cLog, 1, strlen(cLog), fp);
	fclose(fp);
//	g_log.SaveLog(PRINTF_FIRST_LOG, pLogName, cLog);
}
*/

void PopTipMessage(LPCSTR pTitle, LPCSTR pMsgStart, LPCSTR pMsgEnd, int nTipTime)
{
	if( NULL == pTitle)
	{
		return;
	}
	if( NULL == pMsgStart)
	{
		return;
	}
	if(NULL == pMsgEnd)
	{
		return;
	}
	if( nTipTime < 1)
	{
		return;
	}
	CString strPath;
	GetModuleFileName(ThisModuleHandle(), strPath.GetBuffer(MAX_PATH), MAX_PATH - 1);
	strPath.ReleaseBuffer();
	int nIndex = strPath.ReverseFind('\\');
	strPath = strPath.Left(nIndex);

	CString strTxtFile;
	strTxtFile.Format("%s/mydisp.txt", strPath);
	FILE* fp = fopen(strTxtFile, "wt");
	if( NULL == fp)
	{
		return;
	}
	CString str;
	CString strText;
	strText = pTitle;
	nIndex = 0;
	while(AfxExtractSubString(str, strText, nIndex++))
	{
		str.Replace("\r", "");
		str.Replace("\n", "");
		str += " n\n";
		fwrite((LPCSTR)str, 1, str.GetLength(), fp);
	}
	fclose(fp);
	CString strCfgFile;
	strCfgFile.Format("%s/config.ini", strPath);
	CString strApp = "setting";
	CString strKey;
	strKey = "startTip";
	WritePrivateProfileString(strApp, strKey, pMsgStart, strCfgFile);
	strKey = "endTip";
	WritePrivateProfileString(strApp, strKey, pMsgEnd, strCfgFile);
	strKey = "timeout";
	str.Format("%d", nTipTime);
	WritePrivateProfileString(strApp, strKey, str, strCfgFile);
	strKey="align";
	WritePrivateProfileString(strApp, strKey, "4", strCfgFile);
	strKey="alignspace";
	WritePrivateProfileString(strApp, strKey, "30", strCfgFile);

	CString strCmd;
#ifdef _DEBUG
	strCmd.Format("%s/../../adlTip.exe \"%s\" \"%s\"", strPath, strTxtFile, strCfgFile);
#else
	strCmd.Format("%s/../adlTip.exe \"%s\" \"%s\"", strPath, strTxtFile, strCfgFile);
#endif
	WinExec(strCmd, SW_SHOW);

}



bool LocalExitWindows(DWORD dwAction)
{
	HANDLE hToken; 
	TOKEN_PRIVILEGES tkp; 
	bool bRet = false;
	if (OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
	{// open and check the privileges for to perform the actions
		LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid); 
 		
		tkp.PrivilegeCount = 1; 
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 

		if(AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0))
		{/// adjust the privilege to perform the action
			if(dwAction&STAT_SWOFF)
			{
				ExitWindowsEx(EWX_SHUTDOWN|EWX_POWEROFF|EWX_FORCE,0);
				bRet = true;
			}
			else if(dwAction&STAT_LOGOFF)
			{
				ExitWindowsEx(EWX_LOGOFF|EWX_FORCE,0);
				bRet = true;
			}
			else if(dwAction&STAT_RESTART)
			{
				ExitWindowsEx(EWX_REBOOT|EWX_FORCE,0);
				bRet = true;
			}
		}
	}
	else
	{
#ifndef _NOLOG
		DWORD dwError = ::GetLastError();
		//LocalWriteLog(9999, true, NULL, MYSELF_LOG_INFO, "OpenProcessToken failed[error=%d]", dwError);
		LOGEVEN(TEXT("OpenProcessToken failed[error=%d]"),dwError);
#endif
	}
	return bRet;
}


bool RestartMyComputer()
{
	return LocalExitWindows(STAT_RESTART);
}

bool ShutDownMyComputer()
{
	return LocalExitWindows(STAT_SWOFF);
}



