#include "stdafx.h"
#include "App.h"

jkApp theJKApp;

//-------------------------------------------------------------------------------
static HINSTANCE mInstance = NULL;
static jkBaseDialog* mpMainWnd = NULL;
static std::map<HWND,jkBaseDialog*> mapDialogHandle;
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

//-------------------------------------------------------------------------------
HINSTANCE GetThisInstance()
{
	static HINSTANCE hInstance = NULL;
	if (hInstance==NULL)
	{
		MEMORY_BASIC_INFORMATION mbi;
		memset(&mbi,0,sizeof(mbi));

		VirtualQuery(GetThisInstance,&mbi,sizeof(mbi));

		hInstance = (HINSTANCE)mbi.AllocationBase;
	}
	

	return hInstance;
}

jkApp::jkApp()
{
	mInstance = GetThisInstance();
}

BOOL jkApp::SetMainWndInfo(jkBaseDialog* pMainWnd)
{
	if(pMainWnd == NULL) return FALSE;

	if (mpMainWnd == NULL)
	{
		mpMainWnd = pMainWnd;
	}
	
	return TRUE;
}

//void jkApp::SetInstance(HINSTANCE hIns)
//{
//	mInstance = hIns;
//}
//
HINSTANCE jkApp::GetInstance()
{
	return GetThisInstance();
}

//-------------------------------------------------------------------------------
jkBaseDialog::jkBaseDialog(HWND hParant/*= NULL*/)
{
	this->hParant = hParant;
	mHwnd = NULL;
	theJKApp.SetMainWndInfo(this);
}

jkBaseDialog::~jkBaseDialog()
{

}

void jkBaseDialog::Show(int nResId)
{
	DialogBoxParamA(mInstance,MAKEINTRESOURCEA(nResId),hParant,DialogCallBack,(LPARAM)this);
}


LRESULT jkBaseDialog::MessageHandle(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	return 0L;
}

HWND jkBaseDialog::GetCtrl(int nResId)
{
	return GetDlgItem(mHwnd,nResId);
}

// void jkBaseDialog::OnInitDialog()
// {
// 
// }


