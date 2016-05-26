// Inject32To64Console.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>
#include "jkInjectDll/jkInjectDll.h"
#include <atlconv.h>

#include <conio.h>

int _tmain()  
{  
	start:
	USES_CONVERSION;
	DWORD pid; 

	//wchar_t sz[] = L"E:\\working\\test\\InjectExplorer\\Debug\\tmpsysprotect.dll";
	//wchar_t sz1[] = L"E:\\working\\test\\InjectExplorer\\Debug\\tmpsysprotect64.dll";
	////GetSystemDirectoryW(sz, sizeof(sz));

	//GetWindowThreadProcessId(FindWindowW(L"notepad",0),&pid);  
	//pid = GetProcessIdByName(TEXT("explorer.exe"));
	//HANDLE ProcessHandle=OpenProcess(2035711,0,pid);  
	//InjectLibrary64(ProcessHandle,L"c:\\windows\\system32\\d3d9.dll",sizeof L"c:\\windows\\system32\\d3d9.dll"-2);

	//InjectLibrary64(ProcessHandle,sz,sizeof sz - 2);  
	//InjectLibrary64(ProcessHandle,sz1,sizeof sz1 - 2);  

	char szExeName[MAX_PATH]={0};
	printf("\n请输入要注入的exe名称:");
	scanf("%s",szExeName);
	printf("\n");

	char sz2[] = "c:\\InjectDll.dll";
	char sz3[] = "c:\\InjectDll64.dll";
	char sz4[] = "c:\\tmpsysprotect.dll";
	char sz5[] = "c:\\windows\\system32\\d3d9.dll";

	jkInjectDllA(sz2,szExeName);
	jkInjectDllA(sz3,szExeName);
	//jkInjectDllA(sz3,szExeName);
	//jkInjectDllA(sz4,szExeName);

	printf("注入结束\n是否退出退出(y/n):");

	int c = getch();
	if (c != 'Y' && c != 'y')
	{
		goto start;
	}

	return 0;  
}