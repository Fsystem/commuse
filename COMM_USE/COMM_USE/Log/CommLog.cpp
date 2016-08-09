#pragma once
#include "stdafx.h"
#include <Windows.h>
#include <io.h>
#include <stdio.h>
#include <atlconv.h>
#include "CommLog.h"
#include <tchar.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>

#define MAX_BUFFER	5000
#define INVALID_FILE INVALID_HANDLE_VALUE

#ifdef UNICODE
#	define TVSPINRTF vswprintf
#else
#	define TVSPINRTF vsprintf
#endif



#pragma warning(disable:4996)
namespace COMMUSE
{
	HMODULE ThisModule()
	{
		static HMODULE h = NULL;
		if (!h)
		{
			MEMORY_BASIC_INFORMATION mbi;
			::VirtualQuery(ThisModule,&mbi,sizeof(mbi));
			h = (HMODULE)mbi.AllocationBase;
		}
		return h;
	}

	//-------------------------------------------------------------------------------
	ComPrintLog::ComPrintLog()
	{
		_LogFile = INVALID_FILE;
		memset((void*)_szLogFilePath,0,sizeof(_szLogFilePath));
		memset(_szAppFileName,0,sizeof(_szAppFileName));
		PrintInit();
	}
	//-------------------------------------------------------------------------------
	ComPrintLog::~ComPrintLog()
	{
		_LogFile = INVALID_FILE;
		memset((void*)_szLogFilePath,0,sizeof(_szLogFilePath));
		memset(_szAppFileName,0,sizeof(_szAppFileName));
		PrintUnInit();
	}
	//-------------------------------------------------------------------------------
	void ComPrintLog::PrintInit()
	{
		char* p = _szLogFilePath;
		GetModuleFileNameA(ThisModule(),_szLogFilePath,MAX_PATH);
		p += strlen(_szLogFilePath);
		while(*p != '\\')
		{
			p--;
		}
		memcpy(_szAppFileName , p+1 , strlen(p+1) );//取到程序名

		strncpy(_szLogStateIniPath,_szLogFilePath,MAX_PATH);
		strrchr(_szLogStateIniPath,'\\')[1]=0;
		strcat(_szLogStateIniPath,"CommLogOpt.ini");
		
		InitializeCriticalSection(&_cs_log);
		memcpy(p+1,"Log\\\0",strlen("Log\\\0")+1 );//添加目录(_szLogFilePath路径)

		InitLogState();

	}
	//-------------------------------------------------------------------------------
	void ComPrintLog::PrintUnInit()
	{
		EnterCriticalSection(&_cs_log);
		if ( _LogFile  != INVALID_FILE )
		{
			CloseHandle( _LogFile );
			_LogFile = INVALID_FILE;
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
		if (_access(_szLogFilePath,0) == -1)
		{
			CreateDirectoryA(_szLogFilePath,NULL);	//创建LOG文件目录
		}

		char	szLogFileName[MAX_PATH];//log文件名(临时用)
		SYSTEMTIME st;

		GetLocalTime(&st);

		//生成文件名
		_snprintf(szLogFileName , MAX_PATH , "%s%s[%d-%d-%d].log",
			_szLogFilePath,_szAppFileName,st.wYear , st.wMonth , st.wDay );

		//判断是不是需要重新创建文件
		EnterCriticalSection(&_cs_log);
		if( -1 == _access(szLogFileName,0) ) 
		{
			HANDLE fileOld = _LogFile;
			_LogFile = CreateFileA(szLogFileName,GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_DELETE,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
			if (fileOld != INVALID_FILE)
			{
				CloseHandle(fileOld);
			}
		}
		else
		{
			if (_LogFile == INVALID_FILE)
			{
				_LogFile = CreateFileA(szLogFileName,GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_DELETE,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
			}
		}

		
		if (_LogFile != INVALID_FILE)//文件句柄存在
		{
			
			USES_CONVERSION;

			DWORD dwWritten = 0;
			WriteFile(_LogFile,W2A(message) , strlen(W2A(message)),&dwWritten,NULL);
			//FlushFileBuffers(_LogFile);
			
		}
		else 
		{
			DWORD err = 0;
			err = GetLastError();
		}
		LeaveCriticalSection(&_cs_log);
	}

	//-------------------------------------------------------------------------------
	void ComPrintLog::LogEvenFA (char* message)
	{
		if (_access(_szLogFilePath,0) == -1)
		{
			CreateDirectoryA(_szLogFilePath,NULL);	//创建LOG文件目录
		}

		char	szLogFileName[MAX_PATH];//log文件名(临时用)
		SYSTEMTIME st;

		GetLocalTime(&st);

		//生成文件名
		_snprintf(szLogFileName , MAX_PATH , "%s%s[%d-%d-%d].log",
			_szLogFilePath,_szAppFileName,st.wYear , st.wMonth , st.wDay );

		//判断是不是需要重新创建文件
		EnterCriticalSection(&_cs_log);
		if( -1 == _access(szLogFileName,0) ) 
		{
			HANDLE fileOld = _LogFile;
			_LogFile = CreateFileA(szLogFileName,GENERIC_WRITE,FILE_SHARE_WRITE|FILE_SHARE_READ|FILE_SHARE_DELETE,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
			if (fileOld != INVALID_FILE)
			{
				CloseHandle(fileOld);
			}
		}
		else
		{
			if (_LogFile == INVALID_FILE)
			{
				_LogFile = CreateFileA(szLogFileName,GENERIC_WRITE,FILE_SHARE_WRITE|FILE_SHARE_READ|FILE_SHARE_DELETE,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
			}
		}

		
		if (_LogFile != INVALID_FILE)//文件句柄存在
		{
			DWORD dwWritten = 0;
			WriteFile(_LogFile,message , strlen(message) ,&dwWritten,NULL);
			//FlushFileBuffers(_LogFile);
		}
		else 
		{
			DWORD err = 0;
			err = GetLastError();
		}
		LeaveCriticalSection(&_cs_log);
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



