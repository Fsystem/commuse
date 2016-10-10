#include "stdafx.h"
#include "MainWindow.h"

LRESULT MainWindow::MessageHandle(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	int wmId,wmEvent;

	switch(uMsg)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		mMenu.OnMenuCmd(wmId);
		break;
	}
	return __super::MessageHandle(hWnd,uMsg,wParam,lParam);
}

void MainWindow::OnCreate(LPCREATESTRUCT pStruct)
{
	//SetClassLongPtr(mHwnd, GCLP_HBRBACKGROUND,(long)(HBRUSH)GetStockObject(WHITE_BRUSH));

	mMenu.MakeMenu(*this);

	//RECT rcClientWnd;
	//GetClientRect(mHwnd,&rcClientWnd);
	//HWND hTab = CreateWindowA(WC_TABCONTROLA,NULL,WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS
	//	//,rcClientWnd.left,rcClientWnd.top,rcClientWnd.right-rcClientWnd.left,rcClientWnd.bottom-rcClientWnd.top
	//	,CW_USEDEFAULT, 0, CW_USEDEFAULT, 0
	//	,mHwnd,NULL,theJKApp.GetInstance(),NULL);
	//TCITEMA ti;
	//ti.dwStateMask = TCIF_TEXT | TCIF_IMAGE;
	//ti.iImage = -1;
	//ti.pszText = "主页";
	//TabCtrl_InsertItem(hTab,0,&ti);
	//
	//::MoveWindow(hTab,rcClientWnd.left,rcClientWnd.top,rcClientWnd.right-rcClientWnd.left,rcClientWnd.bottom-rcClientWnd.top,TRUE);

	//ShowWindow(hTab,SW_SHOWNORMAL);
	//return;

	mTabMain.CreateTab(mHwnd);
	for (int i=0;i<40;i++)
	{
		mTabMain.InsertTab("主页",NULL);
		mTabMain.InsertTab("主页1",NULL);
		mTabMain.InsertTab("主页2",NULL);
		mTabMain.InsertTab("主页3",NULL);
		mTabMain.InsertTab("主页4",NULL);
	}

	mTabMain.Show(SW_SHOWNORMAL);

	//main client
	mClient.Create(*this);
	RECT rcClient = GetMainClientRect();
	mClient.ResizeClient(mHwnd,rcClient);
	mClient.Show(SW_SHOW);
}

MainClient* MainWindow::GetMainClientWndPtr()
{
	return &mClient;
}

RECT MainWindow::GetClientRect()
{
	JKRect rc = JKRect::ZERO();
	if (mHwnd)
	{
		::GetClientRect(mHwnd,&rc);
	}
	return rc;
}

RECT MainWindow::GetMainTabRect()
{
	JKRect rcTab = JKRect::ZERO();
	if ((HWND)mTabMain)
	{
		GetWindowRect(mTabMain,&rcTab);
	}
	
	return rcTab;
}

RECT MainWindow::GetMainClientRect()
{
	JKRect rc = GetClientRect();
	JKRect rcTab = GetMainTabRect();
	rc.top += rcTab.Height();
	//rc.bottom -= rcTab.Height();
	rc.bottom = max(rc.top,rc.bottom);
	return rc;
}

void MainWindow::OnPaint(HDC hdc)
{

}

// HBRUSH MainWindow::OnCtrlColor()
// {
// 	return (HBRUSH)GetStockObject(WHITE_BRUSH);
/*}*/

void MainWindow::OnSize(UINT nSizeCmd,int width,int height)
{
	JKRect rcTab = GetMainTabRect();
	JKRect rcClient = GetClientRect();
	rcTab.top = rcClient.top;
	rcTab.left = rcClient.left;
	rcTab.right = rcClient.right;
	mTabMain.ReveryfySize(rcTab);

	RECT rcMainClient = GetMainClientRect();
	mClient.ResizeClient(mHwnd,rcMainClient);

	mMenu.ReveryfySize();
}