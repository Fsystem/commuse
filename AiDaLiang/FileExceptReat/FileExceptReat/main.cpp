#include "stdafx.h"
#include "MainDlg.h"

int WINAPI _tWinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPTSTR lpCmdLine, __in int nShowCmd )
{
	MainDlg dlg;
	dlg.Show(IDD_DLG_MAIN);
	return 0;
}