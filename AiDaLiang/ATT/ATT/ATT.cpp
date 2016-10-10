// ATT.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "ATT.h"

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;								// 当前实例
char szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名
HWND hMainWnd;
HWND hClientWnd = NULL;
MenuMgr gMenu;

// 此代码模块中包含的函数的前向声明:
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

	//// 初始化全局字符串
	LoadStringA(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	//-------------------------------------------------------------------------------
	//MyRegisterClass(hInstance);
	//InitInstance(hInstance,SW_SHOW);
	//MSG msg;
	//// 主消息循环:
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
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
//  注释:
//
//    仅当希望
//    此代码与添加到 Windows 95 中的“RegisterClassEx”
//    函数之前的 Win32 系统兼容时，才需要此函数及其用法。调用此函数十分重要，
//    这样应用程序就可以获得关联的
//    “格式正确的”小图标。
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
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

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
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: 处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
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
			////子窗口类
			//cla.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
			//cla.hInstance=hInst;//？？？？？？？
			//cla.lpfnWndProc=ClientWndProc;//处理函数定义在下面
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
			ti.pszText = "主页";
			TabCtrl_InsertItem(hTab,0,&ti);
			ShowWindow(hTab,SW_SHOWNORMAL);
			break;
		}
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择:
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
		// TODO: 在此添加任意绘图代码...
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

// “关于”框的消息处理程序。
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
