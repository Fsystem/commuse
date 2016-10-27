#include "stdafx.h"
#include "MainWnd.h"
#include <WindowsX.h>

//BOOL MainWnd::OnEraseBackground(HDC hdc)
//{
//
//}
enum CTRL_ID
{
	ID_NOUSE=0,
	ID_EDIT_CMD = 100,
	ID_BN_CMD,
};

void MainWnd::OnCreate( LPCREATESTRUCT pStruct )
{
	mLabel.SetCtrlInfo(WC_STATICA,ID_NOUSE);
	mLabel.Create(mHwnd,10,10,120,20);
	mLabel.SetText("«Î ‰»Î√¸¡Ó––£∫");

	mEdit.SetCtrlInfo(WC_EDITA,ID_EDIT_CMD);
	mEdit.SetStyle(ES_AUTOHSCROLL );
	mEdit.Create(mHwnd,132,10,600,20);
	//SendMessage(mEdit,EM_LIMITTEXT,(WPARAM)-1,0);
	Edit_LimitText(mHwnd,-1);//DEFAULT_GUI_FONT
	SendMessage(mHwnd, WM_SETFONT, (WPARAM)GetStockObject(SYSTEM_FONT), 0);  
	

	mButton.SetCtrlInfo(WC_BUTTONA,ID_BN_CMD);
	mButton.Create(mHwnd,10,40,100,20);
	//mButton.SetStyle(BS_PUSHBUTTON|WS_BORDER);
	mButton.SetText("CreateProcess");

	//mListBox.SetCtrlInfo(WC_LISTBOXA,102);
	//mListBox.Create(mHwnd,20,50,100,20);

	//mComboBox.SetCtrlInfo(WC_COMBOBOXA,103);
	//mComboBox.Create(mHwnd,330,50,100,20);

	//mListCtrl.SetCtrlInfo(WC_LISTVIEWA,104);
	//mListCtrl.Create(mHwnd,20,80,100,20);

	//mTreeCtrl.SetCtrlInfo(WC_TREEVIEWA,105);
	//mTreeCtrl.Create(mHwnd,20,110,100,20);

}

void MainWnd::OnDestroy()
{
	__super::OnDestroy();
}

LRESULT MainWnd::OnCommand( WPARAM w,LPARAM l )
{
	UINT uId = (UINT)w;
	switch(uId)
	{
	case ID_BN_CMD:
		{
			std::string sText = mEdit.GetText();
			char szCmd[1024*10];
			sprintf(szCmd,"\"CreateProcess.exe\" %s",sText.c_str());
			//strcpy(szCmd,sText.c_str());

			STARTUPINFOA si={sizeof(STARTUPINFO)};
			PROCESS_INFORMATION pi;
			memset(&pi,0,sizeof(pi));
			memset(&si,0,sizeof(si));
			si.cb = sizeof(STARTUPINFO);
			si.dwFlags = STARTF_USESHOWWINDOW;
			si.wShowWindow = SW_SHOWMAXIMIZED;

			CreateProcessA(NULL,szCmd,NULL,NULL,FALSE,0,NULL,NULL,&si,&pi);

			break;
		}
	}

	return 0L;
}
