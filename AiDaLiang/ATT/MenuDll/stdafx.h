// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�:
#include <windows.h>
#include <ShellAPI.h>

//std
#include <sstream>

//
#include "resource.h"
// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
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

#include "../Interface/jkInterface.h"

//
#include "App.h"
#include "MainWnd.h"
//
#include "Logic/MenuCmd.h"