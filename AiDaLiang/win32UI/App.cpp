#include "stdafx.h"
#include "App.h"
#include <WindowsX.h>
#include <map>

jkApp theJKApp;

//-------------------------------------------------------------------------------
static HINSTANCE mInstance = NULL;
static jkWidget* mpMainWnd = NULL;
static std::map<HWND,jkBaseDialog*> mapDialogHandle;
static std::map<HWND,jkBaseWindow*> mapWindowHandle;
static char* szDefaultClass="jkWindowClass";
//-------------------------------------------------------------------------------
BOOL Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	jkBaseDialog* pThisDlg = (jkBaseDialog*)lParam;
	if (pThisDlg)
	{
		mapDialogHandle[hwnd] = pThisDlg;
		pThisDlg->mHwnd = hwnd;
		mapDialogHandle[hwnd]->MessageHandle(hwnd,WM_INITDIALOG,(WPARAM)hwndFocus,lParam);
	}
	return TRUE;
}

INT_PTR CALLBACK DialogCallBack(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
		HANDLE_MSG(hwnd,WM_INITDIALOG,Cls_OnInitDialog); return TRUE;
	}

	if (mapDialogHandle[hwnd] != NULL)
	{
		return mapDialogHandle[hwnd]->MessageHandle(hwnd,uMsg,wParam,lParam);
	}

	return 0L;
}

LRESULT CALLBACK CLS_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_CREATE)
	{
		jkBaseWindow* pWnd = (jkBaseWindow*) ((LPCREATESTRUCT)lParam)-> lpCreateParams  ;
		mapWindowHandle[hWnd] = pWnd;
		pWnd->mHwnd = hWnd;
	}

	if (mapWindowHandle.find(hWnd)!=mapWindowHandle.end())
	{
		return mapWindowHandle[hWnd]->MessageHandle(hWnd,message,wParam,lParam);
	}

	return DefWindowProc(hWnd,message,wParam,lParam);
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
ATOM CLS_RegisterClass(LPCSTR szWndClass)
{
	TCHAR tszWndClass[250*2];
	memset(tszWndClass,0,sizeof tszWndClass);
#if defined(UNICODE) || defined(_UNICODE)
	if (szWndClass)
		::MultiByteToWideChar(CP_ACP,0,szWndClass,strlen(szWndClass),tszWndClass,sizeof tszWndClass);
	else
		::MultiByteToWideChar(CP_ACP,0,szDefaultClass,strlen(szDefaultClass),tszWndClass,sizeof tszWndClass);
#else
	if (szWndClass)
		strcpy(tszWndClass,szWndClass);
	else
		strcpy(tszWndClass,szDefaultClass);
#endif
	WNDCLASSEX wcex;

	//if( GetClassInfoEx(theJKApp.GetInstance(),tszWndClass,&wcex) ==FALSE)
	{
		wcex.cbSize = sizeof(wcex);

		wcex.style			= CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc	= CLS_WndProc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= theJKApp.GetInstance();
		wcex.hIcon			= NULL;//(theJKApp.GetInstance(), MAKEINTRESOURCE(IDI_ATT));
		wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
		wcex.lpszMenuName	= NULL;
		wcex.lpszClassName	= tszWndClass;
		wcex.hIconSm		= NULL;//(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

		return RegisterClassEx(&wcex);
	}
// 	else
// 	{
// 		return 0;
// 	}
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
HWND CLS_InitInstance(HWND hParant,int nCmdShow,LPARAM lp,DWORD dwStyle,LPCSTR szWndClass)
{
	if(dwStyle == 0) 
	{
		dwStyle = WS_OVERLAPPEDWINDOW;
		if(hParant) dwStyle = WS_CHILD | WS_CLIPSIBLINGS;
	}
	
	char szClass[100];
	strcpy(szClass,szDefaultClass);
	if(szWndClass)strcpy(szClass,szWndClass);

	HWND hMainWnd = CreateWindowA(szClass, NULL, dwStyle,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, hParant, NULL, theJKApp.GetInstance(), (LPVOID)lp);

	if (!hMainWnd)
	{
		return NULL;
	}

	//jkBaseWindow* pWnd = (jkBaseWindow*)lp;
	//mapWindowHandle[hMainWnd] = pWnd;

	//ShowWindow(hMainWnd, nCmdShow);
	//UpdateWindow(hMainWnd);

	return hMainWnd;
}

//-------------------------------------------------------------------------------
HINSTANCE GetThisInstance()
{
	if (mInstance==NULL)
	{
		MEMORY_BASIC_INFORMATION mbi;
		memset(&mbi,0,sizeof(mbi));

		VirtualQuery(GetThisInstance,&mbi,sizeof(mbi));

		mInstance = (HINSTANCE)mbi.AllocationBase;
	}
	

	return mInstance;
}

jkApp::jkApp()
{
	mpMainWnd = NULL;
	mInstance = GetThisInstance();
}

//BOOL jkApp::SetMainWndInfo(jkBaseDialog* pMainWnd)
//{
//	if(pMainWnd == NULL) return FALSE;
//
//	if (mpMainWnd == NULL)
//	{
//		mpMainWnd = pMainWnd;
//	}
//	
//	return TRUE;
//}

void jkApp::SetInstance(HINSTANCE hIns)
{
	mInstance = hIns;
}

HINSTANCE jkApp::GetInstance()
{
	return GetThisInstance();
}

jkWidget* jkApp::GetMainWnd()
{
	return mpMainWnd;
}

void jkApp::RunLoop(jkWidget* wnd)
{
	mpMainWnd = wnd;
	MSG msg;
	// 主消息循环:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		//if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

//-------------------------------------------------------------------------------
jkBaseDialog::jkBaseDialog(HWND hParant/*= NULL*/)
{
	this->hParant = hParant;
	mHwnd = NULL;
}

jkBaseDialog::~jkBaseDialog()
{

}


jkBaseDialog::operator HWND()const throw()
{
	return mHwnd;
}

void jkBaseDialog::Show(int nResId)
{
	DialogBoxParamA(mInstance,MAKEINTRESOURCEA(nResId),hParant,DialogCallBack,(LPARAM)this);
}

void jkBaseDialog::UpdateWindow()
{
	::UpdateWindow(mHwnd);
}

BOOL jkBaseDialog::CenterWindow()
{
	HWND hwndParent;  
	RECT rectWindow, rectParent;  

	// make the window relative to its parent  
	if ((hwndParent = GetParent(mHwnd)) != NULL)  
	{  
		GetWindowRect(mHwnd, &rectWindow);  
		GetWindowRect(hwndParent, &rectParent);  
	}  
	else
	{
		SystemParametersInfo(SPI_GETWORKAREA,0,(PVOID)&rectParent,0);  
		GetWindowRect(mHwnd, &rectWindow);
	}

	int nWidth = rectWindow.right - rectWindow.left;  
	int nHeight = rectWindow.bottom - rectWindow.top;  

	int nX = ((rectParent.right - rectParent.left) - nWidth) / 2 + rectParent.left;  
	int nY = ((rectParent.bottom - rectParent.top) - nHeight) / 2 + rectParent.top;  

	int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);  
	int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);  

	// make sure that the dialog box never moves outside of the screen  
	if (nX < 0) nX = 0;  
	if (nY < 0) nY = 0;  
	if (nX + nWidth > nScreenWidth) nX = nScreenWidth - nWidth;  
	if (nY + nHeight > nScreenHeight) nY = nScreenHeight - nHeight;  

	::MoveWindow(mHwnd, nX, nY, nWidth, nHeight, FALSE);  

	return TRUE;  
}

void jkBaseDialog::MoveWindow(int x,int y,int w,int h,BOOL bRePaint)
{
	::MoveWindow(mHwnd,x, y, w, h, bRePaint);
}

LRESULT jkBaseDialog::MessageHandle(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:OnInitDialog();return TRUE;
	case WM_COMMAND:
		{
			switch(wParam)
			{
			case IDOK:OnOk();return TRUE;
			case IDCANCEL:OnCancel();return TRUE;
			default:OnCommand(wParam,lParam); return TRUE;
			}

			break;
		}
	case WM_DROPFILES:return OnDropFiles((HDROP)wParam);
	}
	return 0L;
}

HWND jkBaseDialog::GetCtrl(int nResId)
{
	return GetDlgItem(mHwnd,nResId);
}

void jkBaseDialog::OnInitDialog()
{

}
LRESULT jkBaseDialog::OnCommand(WPARAM w,LPARAM l)
{
	return 0L;
}
void jkBaseDialog::OnOk()
{
	EndDialog(mHwnd,IDOK);
}
void jkBaseDialog::OnCancel()
{
	EndDialog(mHwnd,IDCANCEL);
}

LRESULT jkBaseDialog::OnDropFiles(HDROP hDrop)
{
	return 0L;
}

//-------------------------------------------------------------------------------
// jkBaseWindow
//-------------------------------------------------------------------------------
jkBaseWindow::jkBaseWindow()
{
	mHwnd = NULL;
	mBGColor = RGB(50,50,50);
}

jkBaseWindow::operator HWND()const throw()
{
	return mHwnd;
}

void jkBaseWindow::Create(HWND hParant,LPCSTR szTitle,DWORD dwStyle/* = 0*/,LPCSTR szWndClass /*= NULL*/)
{
	CLS_RegisterClass(szWndClass);
	mHwnd = CLS_InitInstance(hParant,SW_SHOWNORMAL,(LPARAM)this,dwStyle,szWndClass);
	SetWindowTextA(mHwnd,szTitle);
}

void jkBaseWindow::UpdateWindow()
{
	::UpdateWindow(mHwnd);
}

void jkBaseWindow::Show(int nCmd)
{
	ShowWindow(mHwnd,nCmd);
}

BOOL jkBaseWindow::CenterWindow()
{
	HWND hwndParent;  
	RECT rectWindow, rectParent;  

	// make the window relative to its parent  
	if ((hwndParent = GetParent(mHwnd)) != NULL)  
	{  
		GetWindowRect(mHwnd, &rectWindow);  
		GetWindowRect(hwndParent, &rectParent);  
	}  
	else
	{
		SystemParametersInfo(SPI_GETWORKAREA,0,(PVOID)&rectParent,0);  
		GetWindowRect(mHwnd, &rectWindow);
	}

	int nWidth = rectWindow.right - rectWindow.left;  
	int nHeight = rectWindow.bottom - rectWindow.top;  

	int nX = ((rectParent.right - rectParent.left) - nWidth) / 2 + rectParent.left;  
	int nY = ((rectParent.bottom - rectParent.top) - nHeight) / 2 + rectParent.top;  

	int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);  
	int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);  

	// make sure that the dialog box never moves outside of the screen  
	if (nX < 0) nX = 0;  
	if (nY < 0) nY = 0;  
	if (nX + nWidth > nScreenWidth) nX = nScreenWidth - nWidth;  
	if (nY + nHeight > nScreenHeight) nY = nScreenHeight - nHeight;  

	::MoveWindow(mHwnd, nX, nY, nWidth, nHeight, FALSE);  

	return TRUE;  
}

void jkBaseWindow::MoveWindow(int x,int y,int w,int h,BOOL bRePaint)
{
	::MoveWindow(mHwnd,x, y, w, h, bRePaint);
}

void jkBaseWindow::SetBgColor(COLORREF bgColor)
{
	mBGColor = bgColor;
}

LRESULT jkBaseWindow::MessageHandle(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch(uMsg)
	{
	case WM_CREATE:
		OnCreate((LPCREATESTRUCT)lParam);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		OnPaint(hdc);
		EndPaint(hWnd, &ps);
		return 0;
	case WM_SIZE:
		OnSize((UINT)wParam,LOWORD(lParam),HIWORD(lParam));
		return 0;
	case WM_ERASEBKGND:
		if( OnEraseBackground((HDC)wParam) ) return TRUE;
		return DefWindowProc(hWnd,uMsg,wParam,lParam); 
	case WM_LBUTTONDOWN:
		OnLButtonDown((UINT)wParam,LOWORD(lParam),HIWORD(lParam));
		return 0;
	case WM_LBUTTONUP:
		OnLButtonUp((UINT)wParam,LOWORD(lParam),HIWORD(lParam));
		return 0;
	case WM_LBUTTONDBLCLK:
		OnLButtonDBClick((UINT)wParam,LOWORD(lParam),HIWORD(lParam));
		return 0;
 //	case WM_CTLCOLORSTATIC:
	//case WM_CTLCOLOR:
	//case WM_CTLCOLOREDIT:
 //		OnCtrlColor();
 //		break;
	case WM_DESTROY:
		OnDestroy();
		return 0;
	default:
		return DefWindowProc(hWnd,uMsg,wParam,lParam);
	}
	return 0L;
}

void jkBaseWindow::OnCreate(LPCREATESTRUCT pStruct)
{

}

void jkBaseWindow::OnPaint(HDC hdc)
{
	PaintBG(hdc);
}

void jkBaseWindow::PaintBG(HDC hdc)
{
	RECT rcClient;
	GetClientRect(mHwnd,&rcClient);
	
	HDC hMemDc = ::CreateCompatibleDC(hdc);
	HBITMAP hComBmp = ::CreateCompatibleBitmap(hdc,rcClient.right-rcClient.left,rcClient.bottom-rcClient.top);
	SelectObject(hMemDc,hComBmp);

	HBRUSH hBHBr = ::CreateSolidBrush(mBGColor);

	FillRect(hMemDc,&rcClient,hBHBr);

	::BitBlt(hdc,0,0,rcClient.right-rcClient.left,rcClient.bottom-rcClient.top,hMemDc,0,0,SRCCOPY);

	::DeleteObject(hComBmp);
	::DeleteDC(hMemDc);
	::DeleteObject(hBHBr);

}

HBRUSH jkBaseWindow::OnCtrlColor()
{
	return (HBRUSH)GetStockObject(BLACK_BRUSH);
}

BOOL jkBaseWindow::OnEraseBackground(HDC hdc)
{
	return TRUE;
}

void jkBaseWindow::OnDestroy()
{
	::DestroyWindow(mHwnd);
	mHwnd = NULL;
}

void jkBaseWindow::OnLButtonDown(UINT nFlags,int x, int y){}
void jkBaseWindow::OnLButtonUp(UINT nFlags,int x, int y){}
void jkBaseWindow::OnLButtonDBClick(UINT nFlags,int x, int y){}
void jkBaseWindow::OnSize(UINT nSizeCmd,int width,int height){}