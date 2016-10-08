#include "stdafx.h"
#include "MainDlg.h"
#include "AXContainer.h"

void MainDlg::OnInitDialog()
{
	if (!AXRegister())
		return ;

	RECT rc;
	GetClientRect(mHwnd,&rc);
	hIe = ::CreateWindowExA(CS_HREDRAW | CS_VREDRAW,"AXContainer","{8856F961-340A-11D0-A96B-00C04FD705A2}",WS_VISIBLE|WS_CHILD,rc.left,rc.top,rc.right-rc.left,rc.bottom-rc.top,mHwnd,NULL,theJKApp.GetInstance(),NULL);
	if (hIe)
	{
		SendMessage(hIe,AX_INPLACE,1,0);

		// Navigate
		IWebBrowser2* wb = 0;
		SendMessage(hIe,AX_QUERYINTERFACE,(WPARAM)&IID_IWebBrowser2,(LPARAM)&wb);
		if (wb)
		{
			wb->Navigate(L"http://www.baidu.com",0,0,0,0);
			wb->Release();
		}
	}

	DWORD dwErr = GetLastError();

	void(0);
}