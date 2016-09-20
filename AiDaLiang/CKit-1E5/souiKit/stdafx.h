// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//  are changed infrequently
//

#pragma once

#define  _CRT_SECURE_NO_WARNINGS
#define	 DLL_SOUI
#include <souistd.h>
#include <core/SHostDialog.h>
#include <control/SMessageBox.h>
#include <control/souictrls.h>
#include <res.mgr/sobjdefattr.h>
#include <com-cfg.h>
#include "resource.h"

using namespace SOUI;


 #include "../CKit-lib/CKit.h"
 
 #ifdef _DEBUG
 #	ifdef UNICODE
 #		pragma comment(lib,"../CKit-lib/Ckitud.lib")
 #	else
 #		pragma comment(lib,"../CKit-lib/Ckitd.lib")	
 #	endif
 #else
 #	ifdef UNICODE
 #		pragma comment(lib,"../CKit-lib/Ckitu.lib")
 #	else
 #		pragma comment(lib,"../CKit-lib/Ckit.lib")	
 #	endif
 #endif

