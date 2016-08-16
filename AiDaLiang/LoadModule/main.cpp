#include <windows.h>
#include <windowsx.h>
#include "Resource.h"
#include <PROCESS.H>

#include <WinSock.h>

#pragma comment(lib,"ws2_32.lib")

HWND hMainWnd = NULL;
char szDllFilePath[MAX_PATH]={0};

void LoadDll(void* pData)
{
	const char* szFilePath = (LPCSTR)pData;
	HMODULE hDll = NULL;
	hDll = LoadLibrary(szFilePath);
	if (hDll)MessageBox(NULL,TEXT("¼ÓÔØÍê³É"),NULL,MB_OK);
}

void Thread1(void* pData)
{
	(void)(pData);

	while (1)
	{
		WSADATA sockData;
		if (WSAStartup (MAKEWORD (2, 2), &sockData))
		{
			OutputDebugStringA("WSAStartup failed");
			WSACleanup();
			return;
		}
		
		char szHostName[MAX_PATH];
		if (gethostname(szHostName, sizeof(szHostName)) == SOCKET_ERROR) 
		{
			OutputDebugStringA("WSAStartup failed");
			WSACleanup();
			return;
		}
		
		WSACleanup();
		Sleep(100);
	}

}

BOOL Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	hMainWnd = hwnd;

	SetWindowLong(hwnd,GWL_EXSTYLE,GetWindowLong(hwnd,GWL_EXSTYLE)|WS_EX_ACCEPTFILES);

	if (__argc > 1)
	{
		strcpy(szDllFilePath,__argv[1]);
		Edit_SetText(GetDlgItem(hwnd,IDC_DLL_PATH),szDllFilePath);
	}

	return TRUE;
}

void Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id)
	{
	case IDOK:
		{
			_beginthread(LoadDll,0,szDllFilePath);
			//_beginthread(Thread1,0,NULL);
			break;
		}
	case IDCANCEL:
		{
			EndDialog(hwnd,id);
			break;
		}
	}
}

void Cls_OnDropFiles(HWND hwnd, HDROP hdrop)
{
	memset(szDllFilePath,0,sizeof szDllFilePath);
	DragQueryFile(hdrop,0,szDllFilePath,MAX_PATH-1);
	
	
	Edit_SetText(GetDlgItem(hwnd,IDC_DLL_PATH),szDllFilePath);
}

BOOL CALLBACK DialogProc(HWND hwndDlg, UINT UMsg, WPARAM wParam,LPARAM lParam)
{
	switch(UMsg)
	{
		HANDLE_MSG(hwndDlg,WM_INITDIALOG,Cls_OnInitDialog); return TRUE;
		HANDLE_MSG(hwndDlg,WM_COMMAND,Cls_OnCommand);  return TRUE;
		HANDLE_MSG(hwndDlg,WM_DROPFILES,Cls_OnDropFiles); return TRUE;
	}

	return FALSE;
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
	DialogBox(hInstance,MAKEINTRESOURCE(IDD_MAIN_DLG),NULL,DialogProc);
	return 0;
}