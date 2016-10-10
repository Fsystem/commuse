// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件:
#include <windows.h>
#include <ShellAPI.h>

//std
#include <sstream>

//
#include "resource.h"
// TODO: 在此处引用程序需要的其他头文件
#define		MARKUP_STL
//-------------------------------------------------------------------------------
// 公共库
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