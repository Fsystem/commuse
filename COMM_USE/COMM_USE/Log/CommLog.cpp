#pragma once
#include "stdafx.h"
#include <Windows.h>
#include <io.h>
#include <stdio.h>
#include <atlconv.h>
#include "CommLog.h"
#include <tchar.h>

#ifdef UNICODE
#	define TVSPINRTF vswprintf
#else
#	define TVSPINRTF vsprintf
#endif

#pragma warning(disable:4996)
namespace COMMUSE
{
	//-------------------------------------------------------------------------------
	ComPrintLog::ComPrintLog()
	{
		_LogFile = NULL;
		memset((void*)_szLogFilePath,0,sizeof(_szLogFilePath));
		memset(_szAppFileName,0,sizeof(_szAppFileName));
		PrintInit();
	}
	//-------------------------------------------------------------------------------
	ComPrintLog::~ComPrintLog()
	{
		_LogFile = NULL;
		memset((void*)_szLogFilePath,0,sizeof(_szLogFilePath));
		memset(_szAppFileName,0,sizeof(_szAppFileName));
		PrintUnInit();
	}
	//-------------------------------------------------------------------------------
	void ComPrintLog::PrintInit()
	{
		char* p = _szLogFilePath;
		GetModuleFileNameA(NULL,_szLogFilePath,MAX_PATH);
		p += strlen(_szLogFilePath);
		while(*p != '\\')
		{
			p--;
		}
		memcpy(_szAppFileName , p+1 , strlen(p+1) );//取到程序名

		strncpy(_szLogStateIniPath,_szLogFilePath,MAX_PATH);
		strrchr(_szLogStateIniPath,'\\')[1]=0;
		strcat(_szLogStateIniPath,"CommLogOpt.ini");
		
#ifdef DEBUG_FILE
		if (!_LogFile)
		{
			InitializeCriticalSection(&_cs_log);
			char tem[1024]={0};
			
			SYSTEMTIME t;
			GetLocalTime(&t);

			memcpy(p+1,"Log\\\0",strlen("Log\\\0")+1 );//添加目录(_szLogFilePath路径)

			CreateDirectoryA(_szLogFilePath,NULL);	//创建LOG文件目录
			sprintf(tem , "%s%s[%d-%d-%d].log",_szLogFilePath,_szAppFileName,t.wYear , t.wMonth , t.wDay );

			_LogFile = fopen(tem,"a");
		}
#endif

		InitLogState();

	}
	//-------------------------------------------------------------------------------
	void ComPrintLog::PrintUnInit()
	{
#ifdef DEBUG_FILE
		EnterCriticalSection(&_cs_log);
		if ( _LogFile )
		{
			fclose( _LogFile );
			_LogFile = NULL;
		}
		LeaveCriticalSection(&_cs_log);

		DeleteCriticalSection(&_cs_log);
#endif
	}

	//-------------------------------------------------------------------------------
	void ComPrintLog::InitLogState()
	{
		_nLogState = GetPrivateProfileIntA("CommLog", "Print", 0, _szLogStateIniPath);
	}

	//-------------------------------------------------------------------------------
#	ifdef UNICODE
	void ComPrintLog::LogEvenD (wchar_t* format,...)
#	else
	void ComPrintLog::LogEvenD (char* format,...)
#	endif
	{
#ifdef DEBUG_LOG
		TCHAR tem[1024]={0};
		SYSTEMTIME st;

		GetLocalTime(&st);
#	ifdef UNICODE
		USES_CONVERSION;
		swprintf_s(tem,1024,TEXT("[%s][PID:%d] [%04d-%02d-%02d %02d:%02d:%02d] "),
			A2W(_szAppFileName),GetCurrentProcessId(),st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
#	else
		sprintf_s(tem,1024,"[%s][PID:%d] [%04d-%02d-%02d %02d:%02d:%02d] ",
			_szAppFileName,GetCurrentProcessId(),st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
		
#	endif

		va_list vl;
		va_start(vl,format);
		TVSPINRTF(&tem[_tcslen(tem)],format,vl);
		va_end(vl);

		OutputDebugString(tem);
#endif

	}
	//-------------------------------------------------------------------------------
#	ifdef UNICODE
	void ComPrintLog::LogEvenF (wchar_t* format,...)
#	else
	void ComPrintLog::LogEvenF (char* format,...)
#	endif
	{
#ifdef DEBUG_FILE
		TCHAR	tem[1024]	= {0};
		char	szLogFileName[MAX_PATH];//log文件名(临时用)
		SYSTEMTIME st;

		GetLocalTime(&st);
		_stprintf(tem,TEXT("[PID:%d][%04d-%02d-%02d %02d:%02d:%02d] "),
			GetCurrentProcessId(),st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);

		//生成文件名
		_snprintf(szLogFileName , MAX_PATH , "%s%s[%d-%d-%d].log",
			_szLogFilePath,_szAppFileName,st.wYear , st.wMonth , st.wDay );

		//判断是不是需要重新创建文件
		if( -1 == _access(szLogFileName,0) ) 
		{//新的一天需要重新创建文件,释放昨天的文件
			if (_LogFile) 
			{
				fclose(_LogFile);
				_LogFile = NULL;
			}
			_LogFile = fopen(szLogFileName,"a");
		}

		if (_LogFile)//文件句柄存在
		{
			
			va_list vl;
			va_start(vl,format);
			TVSPINRTF(&tem[_tcslen(tem)],format,vl);
			va_end(vl);

			EnterCriticalSection(&_cs_log);
#ifdef UNICODE
			USES_CONVERSION;
			fwrite(W2A(tem) , strlen(W2A(tem)), 1 , _LogFile);
#else
			fwrite(tem , strlen(tem), 1 , _LogFile);
#endif
			fflush(_LogFile);
			LeaveCriticalSection(&_cs_log);
		}
#endif

	}
	//-------------------------------------------------------------------------------
#	ifdef UNICODE
	void ComPrintLog::LogEvenE (wchar_t* format,...)
#	else
	void ComPrintLog::LogEvenE (char* format,...)
#	endif
	{
		if(_nLogState == 0) return;

		TCHAR tem[1024]={0};
		SYSTEMTIME st;

		GetLocalTime(&st);
#	ifdef UNICODE
		USES_CONVERSION;
		swprintf_s(tem,1024,TEXT("[%s][PID:%d] [%04d-%02d-%02d %02d:%02d:%02d] "),
			A2W(_szAppFileName),GetCurrentProcessId(),st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
#	else
		sprintf_s(tem,1024,"[%s][PID:%d] [%04d-%02d-%02d %02d:%02d:%02d] ",
			_szAppFileName,GetCurrentProcessId(),st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);

#	endif

		va_list vl;
		va_start(vl,format);
		TVSPINRTF(&tem[_tcslen(tem)],format,vl);
		va_end(vl);

		OutputDebugString(tem);
	}
	//-------------------------------------------------------------------------------
} // COMMUSE



