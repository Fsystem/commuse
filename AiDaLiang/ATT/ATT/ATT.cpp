// ATT.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "ATT.h"

#define MAX_LOADSTRING 100

// ȫ�ֱ���:
HINSTANCE hInst;								// ��ǰʵ��
char szTitle[MAX_LOADSTRING];					// �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];			// ����������
HWND hMainWnd;
HWND hClientWnd = NULL;
MenuMgr gMenu;

// �˴���ģ���а����ĺ�����ǰ������:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	//-------------------------------------------------------------------------------
	SetupGdiPlus();

	//// ��ʼ��ȫ���ַ���
	LoadStringA(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	//-------------------------------------------------------------------------------
	//MyRegisterClass(hInstance);
	//InitInstance(hInstance,SW_SHOW);
	//MSG msg;
	//// ����Ϣѭ��:
	//while (GetMessage(&msg, NULL, 0, 0))
	//{
	//	//if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
	//	{
	//		TranslateMessage(&msg);
	//		DispatchMessage(&msg);
	//	}
	//}
	//return 0;
	//-------------------------------------------------------------------------------
	MainWindow window;
	window.Create(NULL,szTitle);
	window.MoveWindow(0,0,800,600,TRUE);
	window.CenterWindow();
	window.Show(SW_SHOW);
	window.UpdateWindow();

	theJKApp.RunLoop(&window);

	//-------------------------------------------------------------------------------
	UnstallGdiPlus();

	return (int) 0;
}



//
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
//
//  ע��:
//
//    ����ϣ��
//    �˴�������ӵ� Windows 95 �еġ�RegisterClassEx��
//    ����֮ǰ�� Win32 ϵͳ����ʱ������Ҫ�˺��������÷������ô˺���ʮ����Ҫ��
//    ����Ӧ�ó���Ϳ��Ի�ù�����
//    ����ʽ��ȷ�ġ�Сͼ�ꡣ
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ATT));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_ATT);
	wcex.lpszClassName	= TEXT("jkWindowClass");
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��:
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

   hMainWnd = CreateWindowA("jkWindowClass", szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hMainWnd)
   {
      return FALSE;
   }

   ShowWindow(hMainWnd, nCmdShow);
   UpdateWindow(hMainWnd);

   return TRUE;
}

LRESULT CALLBACK ClientWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, message, wParam, lParam);
}

//
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��: ���������ڵ���Ϣ��
//
//  WM_COMMAND	- ����Ӧ�ó���˵�
//  WM_PAINT	- ����������
//  WM_DESTROY	- �����˳���Ϣ������
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rcClientWnd;
	

	switch (message)
	{
	case WM_CREATE:
		{
			GetClientRect(hWnd,&rcClientWnd);

			//WNDCLASSA cla;
			//ZeroMemory(&cla,sizeof cla);
			//
			////�Ӵ�����
			//cla.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
			//cla.hInstance=hInst;//��������������
			//cla.lpfnWndProc=ClientWndProc;//����������������
			//cla.lpszClassName="ClientWnd";
			//cla.style=CS_HREDRAW|CS_VREDRAW;
			//RegisterClassA(&cla);
			//HWND hClt = ::CreateWindowA("ClientWnd",NULL,WS_CHILDWINDOW|WS_VISIBLE
			//	,rcClientWnd.left,rcClientWnd.top,rcClientWnd.right-rcClientWnd.left,rcClientWnd.bottom-rcClientWnd.top
			//	,hWnd,NULL,hInst,NULL);
			HWND hTab = CreateWindowA(WC_TABCONTROLA,NULL,WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS
				,rcClientWnd.left,rcClientWnd.top,rcClientWnd.right-rcClientWnd.left,rcClientWnd.bottom-rcClientWnd.top
				,hWnd,NULL,hInst,NULL);
			TCITEMA ti;
			ti.dwStateMask = TCIF_TEXT | TCIF_IMAGE;
			ti.iImage = -1;
			ti.pszText = "��ҳ";
			TabCtrl_InsertItem(hTab,0,&ti);
			ShowWindow(hTab,SW_SHOWNORMAL);
			break;
		}
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// �����˵�ѡ��:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			gMenu.OnMenuCmd(wmId);
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: �ڴ���������ͼ����...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// �����ڡ������Ϣ�������
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
