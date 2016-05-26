// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <process.h>

bool gRun = false;

void threadfunc(void* p)
{
	MessageBox(NULL,"DLL_PROCESS_ATTACH",NULL,IDOK);

	while(gRun)
	{
		OutputDebugStringA("ccccccccccccccccccccccccccccccccccccccccccccccccc");
		Sleep(10);
	}

	MessageBox(NULL,"DLL_PROCESS_DETACH",NULL,IDOK);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		gRun = true;
		_beginthread(threadfunc,0,0);
		OutputDebugStringA("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		gRun = false;
		Sleep(100);
		OutputDebugStringA("mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm");
		break;
	}
	return TRUE;
}

