// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"

MenuCmd* gMenuCmd;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		gMenuCmd = new MenuCmd;
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		delete gMenuCmd;
		break;
	}
	return TRUE;
}

JK_MENU_EXPORT_INSTANCE(gMenuCmd);