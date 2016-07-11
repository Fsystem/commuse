// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件:
#include <windows.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <string>
#include <vector>
#include <list>
#include <map>

#include "zlib/ILibInterface.h"
#include "FunctionHelper.h"
#include "JKThread.h"
// TODO: 在此处引用程序需要的其他头文件

extern HWND gMainHwnd;
extern HWND gParantHwnd;
extern int gFileIndex ;
extern char gszZipFile[4096];
extern char gszKeys[4096];
extern int gAnalysisMode;

struct IPC_Data 
{
	bool bProcessed;
	int nResultCnt;
	char szFile[MAX_PATH];
	char szRetFile[MAX_PATH];
};

struct CreateAnalysisPath
{
	std::string sFile;
	std::string sKey;
};


void SendResultNotify(LPCSTR szFile,LPCSTR szFileName,DWORD dwResultCnt);