// load64dll.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <Windows.h>

#include <conio.h>

int _tmain(int argc, _TCHAR* argv[])
{
#ifdef _WIN64
	HMODULE h = ::LoadLibrary(TEXT("InjectDll64.dll"));
#else
	HMODULE h = ::LoadLibrary(TEXT("InjectDll.dll"));
#endif
	
	getch();

	return 0;
}

