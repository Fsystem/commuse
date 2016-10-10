// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once
#pragma warning(disable:4996 4244)
#include "targetver.h"
#include <vld.h>
//#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件:
#include <windows.h>
#include <CommCtrl.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: 在此处引用程序需要的其他头文件



//std
#include <string>
#include <map>
#include <sstream>
#include <fstream>
#include <iterator>

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

//rapidjson
#include "src/rapidjson/rapidjson.h"
#include "src/rapidjson/document.h"

//
#include "src/jkinc.h"

