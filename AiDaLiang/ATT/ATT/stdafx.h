// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once
#pragma warning(disable:4996 4244)
#include "targetver.h"
#include <vld.h>
//#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�:
#include <windows.h>
#include <CommCtrl.h>

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�



//std
#include <string>
#include <map>
#include <sstream>
#include <fstream>
#include <iterator>

#define		MARKUP_STL
//-------------------------------------------------------------------------------
// ������
//-------------------------------------------------------------------------------
#include "..\CKit-1E5-lib\CKit.h"
#if defined(UNICODE) || defined(_UNICODE)
#	if defined(DEBUG) || defined(_DEBUG)
#		pragma comment(lib,"../CKit-1E5-lib/CKit-1E5ud.lib")
#	else
#		pragma comment(lib,"../CKit-1E5-lib/CKit-1E5u.lib")
#	endif
#else
#	if defined(DEBUG) || defined(_DEBUG)
#		pragma comment(lib,"../CKit-1E5-lib/CKit-1E5d.lib")
#	else
#		pragma comment(lib,"../CKit-1E5-lib/CKit-1E5.lib")
#	endif
#endif

//rapidjson
#include "src/rapidjson/rapidjson.h"
#include "src/rapidjson/document.h"

//
#include "src/jkinc.h"

