#pragma once
#include "stdafx.h"
#include <Windows.h>
#include <io.h>
#include <stdio.h>
#include <atlconv.h>
#include "CommLog.h"
#include <tchar.h>

#define MAX_BUFFER	5000

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

		InitLogState();

	}
	//-------------------------------------------------------------------------------
	void ComPrintLog::PrintUnInit()
	{
		EnterCriticalSection(&_cs_log);
		if ( _LogFile )
		{
			fclose( _LogFile );
			_LogFile = NULL;
		}
		LeaveCriticalSection(&_cs_log);

		DeleteCriticalSection(&_cs_log);
	}

	//-------------------------------------------------------------------------------
	void ComPrintLog::InitLogState()
	{
		_nLogState = GetPrivateProfileIntA("CommLog", "Print", 0, _szLogStateIniPath);
	}

	//-------------------------------------------------------------------------------
	void ComPrintLog::LogEventW(wchar_t* format,...)
	{
		InitLogState();
		if(_nLogState == 0) return;
		USES_CONVERSION;

		wchar_t tem[MAX_BUFFER]={0};
		SYSTEMTIME st;

		GetLocalTime(&st);

		swprintf_s(tem,MAX_BUFFER,L"[PID:%d] [%02d:%02d:%02d] ",
			GetCurrentProcessId(),st.wHour,st.wMinute,st.wSecond);

		va_list vl;
		va_start(vl,format);
		vswprintf(&tem[wcslen(tem)],format,vl);
		va_end(vl);

		if(_nLogState == 1)
		{
			LogEvenDW(tem);
		}
		else
		{
			LogEvenFW(tem);
		}
	}
	//-------------------------------------------------------------------------------
	void ComPrintLog::LogEventA(char* format,...)
	{
		InitLogState();
		if(_nLogState == 0) return;

		char tem[MAX_BUFFER]={0};
		SYSTEMTIME st;

		GetLocalTime(&st);

		sprintf_s(tem,MAX_BUFFER,"[PID:%d] [%02d:%02d:%02d] ",
			GetCurrentProcessId(),st.wHour,st.wMinute,st.wSecond);

		va_list vl;
		va_start(vl,format);
		vsprintf(&tem[strlen(tem)],format,vl);
		va_end(vl);

		if(_nLogState == 1)
		{
			LogEvenDA(tem);
		}
		else
		{
			LogEvenFA(tem);
		}
	}

	//-------------------------------------------------------------------------------

	void ComPrintLog::LogEvenDW (wchar_t* message)
	{
		OutputDebugStringW(message);
	}

	void ComPrintLog::LogEvenDA (char* message)

	{
		OutputDebugStringA(message);
	}
	//-------------------------------------------------------------------------------

	void ComPrintLog::LogEvenFW (wchar_t* message)
	{
		char	szLogFileName[MAX_PATH];//log文件名(临时用)
		SYSTEMTIME st;

		GetLocalTime(&st);

		//生成文件名
		_snprintf(szLogFileName , MAX_PATH , "%s%s[%d-%d-%d].log",
			_szLogFilePath,_szAppFileName,st.wYear , st.wMonth , st.wDay );

		//判断是不是需要重新创建文件
		if( -1 == _access(szLogFileName,0) ) 
		{
			if (_LogFile) 
			{
				fclose(_LogFile);
				_LogFile = NULL;
			}
			_LogFile = fopen(szLogFileName,"a");
		}

		if (_LogFile)//文件句柄存在
		{
			USES_CONVERSION;

			EnterCriticalSection(&_cs_log);

			fwrite(W2A(message) , 1, strlen(W2A(message)) , _LogFile);

			fflush(_LogFile);
			LeaveCriticalSection(&_cs_log);
		}
	}

	//-------------------------------------------------------------------------------
	void ComPrintLog::LogEvenFA (char* message)
	{
		char	szLogFileName[MAX_PATH];//log文件名(临时用)
		SYSTEMTIME st;

		GetLocalTime(&st);

		//生成文件名
		_snprintf(szLogFileName , MAX_PATH , "%s%s[%d-%d-%d].log",
			_szLogFilePath,_szAppFileName,st.wYear , st.wMonth , st.wDay );

		//判断是不是需要重新创建文件
		if( -1 == _access(szLogFileName,0) ) 
		{
			if (_LogFile) 
			{
				fclose(_LogFile);
				_LogFile = NULL;
			}
			_LogFile = fopen(szLogFileName,"a");
		}

		if (_LogFile)//文件句柄存在
		{

			EnterCriticalSection(&_cs_log);

			fwrite(message , 1, strlen(message) , _LogFile);

			fflush(_LogFile);
			LeaveCriticalSection(&_cs_log);
		}
	}
	//-------------------------------------------------------------------------------
//#	ifdef UNICODE
//	void ComPrintLog::LogEvenE (wchar_t* format,...)
//#	else
//	void ComPrintLog::LogEvenE (char* format,...)
//#	endif
//	{
//		if(_nLogState == 0) return;
//
//		TCHAR tem[1024]={0};
//		SYSTEMTIME st;
//
//		GetLocalTime(&st);
//#	ifdef UNICODE
//		USES_CONVERSION;
//		swprintf_s(tem,1024,TEXT("[%s][PID:%d] [%04d-%02d-%02d %02d:%02d:%02d] "),
//			A2W(_szAppFileName),GetCurrentProcessId(),st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
//#	else
//		sprintf_s(tem,1024,"[%s][PID:%d] [%04d-%02d-%02d %02d:%02d:%02d] ",
//			_szAppFileName,GetCurrentProcessId(),st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
//
//#	endif
//
//		va_list vl;
//		va_start(vl,format);
//		TVSPINRTF(&tem[_tcslen(tem)],format,vl);
//		va_end(vl);
//
//		OutputDebugString(tem);
//	}
	//-------------------------------------------------------------------------------
} // COMMUSE



