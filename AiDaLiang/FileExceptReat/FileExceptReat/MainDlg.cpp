#include "stdafx.h"
#include "MainDlg.h"
#include "DelRepeat.h"

LRESULT MainDlg::MessageHandle(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:return OnInitDialog();
	case WM_COMMAND:return OnCommand((int)wParam);
	case WM_DROPFILES:return OnDropFiles((HDROP)wParam);
	case WM_CONTEXTMENU:return OnRButtonUp(HWND(wParam),lParam);
	}
	return 0L;
}

LRESULT MainDlg::OnInitDialog()
{
	mRetainFile = -1;
	HWND hList = GetCtrl(IDC_LIST_FILES);
	SetWindowLong(mHwnd,GWL_EXSTYLE,GetWindowLong(mHwnd,GWL_EXSTYLE)|WS_EX_ACCEPTFILES);
	//mListOldFunc = (WNDPROC)SetWindowLong(hList, GWL_WNDPROC, (LONG)ListWndMessage); 
	//SetWindowLong(hList,GWL_USERDATA,(LONG)this);
	return TRUE;
}
LRESULT MainDlg::OnCommand(int nId)
{
	switch(nId)
	{
	case IDCANCEL:OnCancel();break;
	case IDOK:OnOk();break;
	case ID_ME_RETAIN:OnMenuRetain();break;
	case ID_ME_CLEAR_LIST:OnMenuClearList();break;
	}
	return TRUE;
}
LRESULT MainDlg::OnDropFiles(HDROP hd)
{
	//listCompFile.clear();
	int nCnt = DragQueryFileA(hd,-1,NULL,0);
	for (int i=0;i<nCnt;i++)
	{
		char szFilePath[MAX_PATH]={0};
		DragQueryFileA(hd,i,szFilePath,sizeof(szFilePath));
		if (PathIsDirectoryA(szFilePath)==FALSE)
		{
			listCompFile.push_back(szFilePath);
		}
	}
	if(listCompFile.size()>0)mRetainFile = 0;
	UpdateCompFileListControl();
	UpdateRetainFile();
	return TRUE;
}

LRESULT MainDlg::OnRButtonUp(HWND hCtrl,LPARAM lp)
{
	HWND hList = GetCtrl(IDC_LIST_FILES);
	if(hCtrl == hList)
	{
		if (listCompFile.size()>0)
		{
			POINT pt={GET_X_LPARAM(lp),GET_Y_LPARAM(lp)},ptOriginal = pt;
			ScreenToClient(hList,&pt);
			int nItem = SendMessage(hList,LB_ITEMFROMPOINT,0,MAKELPARAM(pt.x,pt.y));
			SendMessage(hCtrl,LB_SETCURSEL,nItem,0);

			HMENU hPopupMenu = LoadMenu(theJKApp.GetInstance(),MAKEINTRESOURCE(IDR_MENU_RETAIN));
			HMENU hpop = GetSubMenu(hPopupMenu,0);     
			TrackPopupMenu(hpop, TPM_TOPALIGN | TPM_LEFTALIGN, ptOriginal.x,ptOriginal.y, 0, mHwnd, NULL);
			DestroyMenu(hPopupMenu);
		}
	}
	
	return TRUE;
}
//-------------------------------------------------------------------------------
void MainDlg::OnCancel()
{
	EndDialog(mHwnd,IDCANCEL);
}

void MainDlg::OnOk()
{
	if (listCompFile.size()<2)
	{
		MessageBoxA(mHwnd,"请添加2个以上的文件","温馨提示",MB_OK);
		return;
	}
	DelRepeat dr(listCompFile,listCompFile.front());
	dr.Execute();

	MessageBoxA(mHwnd,"去重完成","温馨提示",MB_OK);
}

void MainDlg::OnMenuRetain()
{
	HWND hList = GetCtrl(IDC_LIST_FILES);
	mRetainFile = SendMessage(hList,LB_GETCURSEL,0,0);
	UpdateRetainFile();

	std::string sRetain = listCompFile[mRetainFile];
	listCompFile.erase(listCompFile.begin()+mRetainFile);
	listCompFile.insert(listCompFile.begin(),sRetain);

	UpdateCompFileListControl();
}

void MainDlg::OnMenuClearList()
{
	listCompFile.clear();
	UpdateCompFileListControl();
}

//-------------------------------------------------------------------------------
void MainDlg::UpdateCompFileListControl()
{
	HWND hList = GetCtrl(IDC_LIST_FILES);
	ListBox_ResetContent(hList);
	int index = 0;
	std::for_each(listCompFile.begin(),listCompFile.end(),[hList,&index](std::string item){
		ListBox_InsertString(hList,index++,A2TString(item.c_str()).c_str());
	});
}

void MainDlg::UpdateRetainFile()
{
	if (mRetainFile != -1)
	{
		//Edit_SetText(GetCtrl(IDC_EDIT_RETAIN_FILE),A2TString(listCompFile[mRetainFile].c_str()).c_str());

		Edit_SetText(GetCtrl(IDC_EDIT_RETAIN_FILE),A2TString("保留顺序为列表的顺序").c_str());
	}
}