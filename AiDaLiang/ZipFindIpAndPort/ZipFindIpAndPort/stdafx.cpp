// stdafx.cpp : 只包括标准包含文件的源文件
// ZipFindIpAndPort.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"
#pragma comment(linker,"/STACK:65536")
// TODO: 在 STDAFX.H 中
// 引用任何所需的附加头文件，而不是在此文件中引用


void SendResultNotify(LPCSTR szFile,LPCSTR szFileName,DWORD dwResultCnt)
{
	IPC_Data data;
	memset(&data,0,sizeof(data));
	data.nResultCnt = dwResultCnt;
	if(szFile) strncpy(data.szFile,szFile,MAX_PATH);
	if(szFileName) strncpy(data.szRetFile,szFileName,MAX_PATH);
	data.bProcessed = dwResultCnt>0?true:false;

	COPYDATASTRUCT cds;
	cds.dwData = 1;
	cds.lpData = &data;
	cds.cbData = sizeof(data);

	if (gAnalysisMode == 0)
	{
		if (gParantHwnd)
		{
			::SendMessage(gParantHwnd,WM_COPYDATA,(WPARAM)gMainHwnd,(LPARAM)&cds);
		}
	}
	else
	{
		::SendMessage(gMainHwnd,WM_COPYDATA,(WPARAM)gMainHwnd,(LPARAM)&cds);
	}
}


void LogTrace(LPCSTR pszFormat, ...)  
{  
	va_list pArgs;  

	char szMessageBuffer[1024]={0};  
	va_start( pArgs, pszFormat );  
	_vsnprintf( szMessageBuffer, 1024, pszFormat, pArgs );  
	va_end( pArgs );  

	OutputDebugStringA(szMessageBuffer);  
}  