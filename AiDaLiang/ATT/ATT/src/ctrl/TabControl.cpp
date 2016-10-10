#include "stdafx.h"
#include "TabControl.h"
#include <algorithm>
#include <iterator>

static const int nTabWid = 80;
static const int nTabHei = 30;
static const int nTabCloseWid = 12;
static const int nTabCloseHei = 10;

static const int WM_CLOSE_TAB = WM_USER+100;
static const int WM_BUTTON_TAB = WM_USER+101;
static const int WM_BUTTON_MORE = WM_USER+102;



//-------------------------------------------------------------------------------
TabTitle::TabTitle()
{
	mIndex = -1;
	mTitle = "";
	//mRect;
	mParant = NULL;
	mSelf = NULL;

	mCanClose = true;
	mSelected = false;
}

void TabTitle::OnDraw(HDC hdc)
{
	SetBkMode(hdc,TRANSPARENT);

	JKRect rcTab(0,0,mRect.Width(),mRect.Height());
	rcTab = GetRelativeRect(&mRect,&rcTab);

	JKRect rcClose(rcTab.right-nTabCloseWid-1,0,nTabCloseWid,nTabCloseHei);


	Graphics gs(hdc);
	Font fnt(L"宋体", Gdiplus::REAL(12), Gdiplus::FontStyleUnderline, Gdiplus::UnitPixel);

	//绘制选中
	if(mSelected){
		JKRect rcTabScale = rcTab.Scale(2,2);
		FillRoundRectangle(gs,Color(10,185,10),&rcTabScale);
	}

	//绘制线框
	//FillRoundRectangle(gs,Color(180,255,0,0),&rcTab);
	DrawRoundRectange(gs,Color(180,150,0,0),&rcTab);
	DrawText(gs,fnt,mTitle,Color(255,255,255),&rcTab);

	//绘制关闭
	if (mCanClose)
	{
		Font fnt1(L"宋体", Gdiplus::REAL(12), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
		FillRoundRectangle(gs,Color(80,0,0),&rcClose);
		DrawText(gs,fnt1,"x",Color(255,255,255),&rcClose);
	}
}

bool TabTitle::OnClick(UINT nFlags,int x, int y)
{
	POINT pt={x,y};

	JKRect rcTab(0,0,mRect.Width(),mRect.Height());
	rcTab = GetRelativeRect(&mRect,&rcTab);

	//JKRect rcParant;
	//GetClientRect(mParant,&rcParant);
	JKRect rcClose(rcTab.right-nTabCloseWid-1,0,nTabCloseWid,nTabCloseHei);

	if( PtInRect(&rcClose,pt) && mCanClose)
	{
		PostMessage(mParant,WM_CLOSE_TAB,(WPARAM)mIndex,0);
		return true;
	}
	else if ( PtInRect(&rcTab,pt) )
	{
		PostMessage(mParant,WM_BUTTON_TAB,(WPARAM)mIndex,0);
		return true;
	}

	return false;
}

//-------------------------------------------------------------------------------


void TableControl::CreateTab(HWND hParant)
{
	JKRect rcClient;
	GetClientRect(hParant,&rcClient);

	Create(hParant,NULL,WS_CHILD|WS_VISIBLE,NULL);
	MoveWindow(rcClient.left,rcClient.top,rcClient.Width(),nTabHei+5,TRUE);

// 	CreateWindowA(WC_TABCONTROLA,NULL,WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS
// 		,rcClient.left,rcClient.top,rcClient.Width(),rcClient.Height()
// 		,hParant,NULL,theJKApp.GetInstance(),NULL);

	//HWND hParant = GetParent(mHwnd);
	
	
}

void TableControl::InsertTab(LPCSTR szTitle,HWND hWnd)
{
	TabTitle tl;
	tl.mIndex = mItems.size();
	tl.mParant = mHwnd;
	tl.mRect = JKRect(nTabWid*tl.mIndex,0,nTabWid,nTabHei);
	tl.mTitle = szTitle;
	tl.mCanClose = true;
	tl.mSelf = hWnd;

	if (tl.mIndex == 0)
	{
		tl.mSelected = true;
		tl.mCanClose = false;
		mCurPage = 0;
	}

	mItems.push_back(tl);
}

void TableControl::DeleteTab(int index ,LPCSTR szTitle)
{
	if(index >= 0) mItems.erase(mItems.begin()+index);
	else mItems.erase(std::remove_if(mItems.begin(),mItems.end(),[szTitle](TabTitle ti){
		return (strcmp(ti.mTitle.c_str() , szTitle) == 0 );
	})
		,mItems.end());

	std::vector<TabTitle> tmp ;
	std::copy(mItems.begin(),mItems.end(),std::back_inserter(tmp));
	mItems.clear();

	for(auto it = tmp.begin();it!=tmp.end();it++)
	{
		InsertTab(it->mTitle.c_str(),it->mSelf);
	}

	NotifyRedraw();
}

void TableControl::NotifyRedraw()
{
	::InvalidateRect(mHwnd,NULL,TRUE);
}

void TableControl::ReveryfySize(RECT rc)
{
	HWND hParant = ::GetParent(mHwnd);
	JKRect rcClient=rc;
	//GetClientRect(hParant,&rcClient);
	rcClient.bottom = rcClient.top + nTabHei+5;
	MoveWindow(rcClient.left,rcClient.top,rcClient.Width(),rcClient.Height(),TRUE);

	for(auto it = mItems.begin();it!=mItems.end();it++)
	{
		if (::IsWindow(it->mSelf))
		{
			::MoveWindow(it->mSelf,rcClient.left,rcClient.top+nTabHei + 2,rcClient.Width(),rcClient.Height()-nTabHei-2,TRUE);
		}
	}
}

void TableControl::OnCreate(LPCREATESTRUCT pStruct)
{
	//SetClassLongPtr(mHwnd, GCLP_HBRBACKGROUND,(long)(HBRUSH)GetStockObject(GRAY_BRUSH));
	//HWND hParant = GetParent(mHwnd);
	//RECT rcClient;
	//GetClientRect(hParant,&rcClient);
	//MoveWindow(rcClient.left,rcClient.top,rcClient.right-rcClient.left,rcClient.bottom-rcClient.top,TRUE);


	//mMainClient.Create(mHwnd);
	//RECT rcWin;
	//GetWindowRect(mHwnd,&rcWin);
	//mMainClient.MoveWindow(0,nTabHei,rcWin.right-rcWin.left,rcWin.bottom-rcWin.top);
	//mMainClient.Show(SW_SHOWNORMAL);
	//mMainClient.UpdateWindow();
}

void TableControl::OnPaint(HDC hdc)
{
	JKRect rcClient;
	GetClientRect(mHwnd,&rcClient);
	JKRect rcClose(rcClient.right-nTabCloseWid-2,0,nTabCloseWid,nTabHei);

	HDC hMemDc = ::CreateCompatibleDC(hdc);
	HBITMAP hComBmp = ::CreateCompatibleBitmap(hdc,rcClient.Width(),rcClient.Height());
	SelectObject(hMemDc,hComBmp);

	Graphics g(hMemDc);
	LinearGradientBrush lineBr(Rect(rcClient.left,rcClient.top,rcClient.Width(),rcClient.Height())
		,Color(80,125,60),Color(20,8,20)
		,REAL(45));
	g.FillRectangle(&lineBr, Gdiplus::Rect(0, 0, rcClient.Width(), rcClient.Height()));

	for (auto it = mItems.begin();it!=mItems.end();it++)
	{
		if (it->mRect.right > rcClose.left) break;
	
		it->OnDraw(hMemDc);
	}

	Font ft(L"宋体",12,FontStyleRegular,UnitPixel);
	FillRoundRectangle(g,Color(255,255,255),&rcClose);
	DrawText(g,ft,"▼",Color(255,0,0),&rcClose);

	::BitBlt(hdc,0,0,rcClient.Width(),rcClient.Height(),hMemDc,0,0,SRCCOPY);

	::DeleteObject(hComBmp);
	::DeleteDC(hMemDc);
}

BOOL TableControl::OnEraseBackground(HDC hdc)
{
	return TRUE;
	HBRUSH hbr = (HBRUSH)GetStockObject(GRAY_BRUSH);
	RECT rc;
	GetClientRect(mHwnd,&rc);
	FillRect(hdc,&rc,hbr);

	return TRUE;
}

void TableControl::OnSize(UINT nSizeCmd,int width,int height)
{
	//InvalidateRect(mHwnd,NULL,TRUE);
}

void TableControl::OnLButtonUp(UINT nFlags,int x, int y)
{
	for (auto it = mItems.begin();it!=mItems.end();it++)
	{
		if( it->OnClick(nFlags,x,y) ) break;
	}
}

LRESULT TableControl::MessageHandle(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE_TAB:
		DeleteTab((UINT)wParam,NULL);
		return 0;
	case WM_BUTTON_TAB:
		if(mCurPage == (int)wParam) return 0;

		mItems[mCurPage].mSelected=false;
		if (::IsWindow(mItems[mCurPage].mSelf))
		{
			::ShowWindow(mItems[mCurPage].mSelf,SW_HIDE);
		}

		mCurPage = (int)wParam;

		mItems[mCurPage].mSelected=true;
		if (::IsWindow(mItems[mCurPage].mSelf))
		{
			::ShowWindow(mItems[mCurPage].mSelf,SW_SHOW);
		}

		NotifyRedraw();
		return 0;
	}
	return __super::MessageHandle(hWnd,uMsg,wParam,lParam);
}