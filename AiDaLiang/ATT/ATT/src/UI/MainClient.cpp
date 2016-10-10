#include "stdafx.h"
#include "MainClient.h"

void MainClient::OnCreate(LPCREATESTRUCT pStruct)
{
	//SetClassLongPtr(mHwnd, GCLP_HBRBACKGROUND,(long)(HBRUSH)GetStockObject(BLACK_BRUSH));
}

// HBRUSH MainClient::OnCtrlColor()
//{
//	return (HBRUSH)GetStockObject(BLACK_BRUSH);
//}

//void MainClient::OnSize(UINT nSizeCmd,int width,int height)
//{
//	//mTabMain.ReveryfySize();
//}

void MainClient::ResizeClient(HWND hPrant,RECT rcClient)
{
	MoveWindow(rcClient.left,rcClient.top,rcClient.right-rcClient.left,rcClient.bottom-rcClient.top,TRUE);
}

void MainClient::OnPaint(HDC hdc)
{
	__super::OnPaint(hdc);
}