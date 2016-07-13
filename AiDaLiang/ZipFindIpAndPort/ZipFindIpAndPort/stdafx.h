// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�:
#include <windows.h>

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <string>
#include <vector>
#include <list>
#include <map>
#include <strstream>
#include <algorithm>
#include <iterator>

#include <assert.h>
#include <direct.h>

#include <boost/regex.hpp>

#include "zlib/ILibInterface.h"
#include "FunctionHelper.h"
#include "JKThread.h"
#include "CRC.h"
#include "UrlCode.h"

#include "RegexOpt.h"


#include "HttpDownFile.h"
#include "SafeFileFromHttp.h"
// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�

extern HWND gMainHwnd;
extern HWND gParantHwnd;
extern int gFileIndex ;
extern char gszZipFile[4096];
extern char gszKeys[4096];
extern int gAnalysisMode;

#define MSG_DOWN_FINISH WM_USER+1000

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