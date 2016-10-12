#include "stdafx.h"
#include "MainWnd.h"

//BOOL MainWnd::OnEraseBackground(HDC hdc)
//{
//
//}

void MainWnd::OnCreate( LPCREATESTRUCT pStruct )
{
	mEdit.SetCtrlInfo(WC_EDITA,100);
	mEdit.Create(mHwnd,20,20,300,18);

	mButton.SetCtrlInfo(WC_BUTTONA,101);
	mButton.Create(mHwnd,330,20,100,20);

	mListBox.SetCtrlInfo(WC_LISTBOXA,102);
	mListBox.Create(mHwnd,20,50,100,20);

	mComboBox.SetCtrlInfo(WC_COMBOBOXA,103);
	mComboBox.Create(mHwnd,330,50,100,20);

	mListCtrl.SetCtrlInfo(WC_LISTVIEWA,104);
	mListCtrl.Create(mHwnd,20,80,100,20);

	mLabel.SetCtrlInfo(WC_STATICA,0);
	mLabel.Create(mHwnd,330,80,100,20);

	mTreeCtrl.SetCtrlInfo(WC_TREEVIEWA,105);
	mTreeCtrl.Create(mHwnd,20,110,100,20);

}

void MainWnd::OnDestroy()
{
	__super::OnDestroy();
}
