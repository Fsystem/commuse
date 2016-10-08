// Win32IE.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Win32IE.h"

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	OleInitialize(0);


	MainDlg d;
	d.Show(IDD_DIALOG1);

	OleUninitialize();
	return 0;
}

