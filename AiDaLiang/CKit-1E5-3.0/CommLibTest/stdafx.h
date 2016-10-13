#ifndef __stdafx_H
#define __stdafx_H

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

extern HWND hMainWnd;

#endif //__stdafx_H