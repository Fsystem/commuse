#include "stdafx.h"
#include "MenuCmd.h"

MenuCmd::MenuCmd()
{
	mMenusPtr = NULL;
	mpMainWnd = NULL;
}

MenuCmd::~MenuCmd()
{
	if(mMenusPtr)
	{
		delete[] mMenusPtr;
		mMenusPtr = NULL;
	}
}

HWND MenuCmd::GetMainHwd()
{
	return NULL;
}

LPJKMenuDataArr MenuCmd::LoadMenu()
{
	if(mMenusPtr) return mMenusPtr;

	mMenusPtr = new JKMenuData*[_countof(mMenus)+1];
	for (int i=0;i<_countof(mMenus);i++)
	{
		std::ostringstream sMenu;
		sMenu << szMuName[i];
		strncpy(mMenus[i].szMenuName , sMenu.str().c_str(),MAX_MENUNAME_LEN-1);
		mMenus[i].MenuCall = (bool (IJKMenu::*)(const char*,HWND))&MenuCmd::MenuCall;
		mMenus[i].ReveryfySize = (void (IJKMenu::*)(const char*))&MenuCmd::ReSize;
		mMenus[i].CloseMenu = (void (IJKMenu::*)(const char*))&MenuCmd::CloseMenu;
		mMenus[i].pCallPtr = this;

		mMenusPtr[i] = &mMenus[i];
	}

	mMenusPtr[_countof(mMenus)] = NULL;

	return mMenusPtr;
}

bool MenuCmd::MenuCall(const char* szMenuName,HWND hClientMain)
{
	if (mpMainWnd == NULL)
	{
		mpMainWnd = new MainWnd;
		JKRect rc;
		GetClientRect(hClientMain,&rc);

		mpMainWnd->Create(hClientMain);
		mpMainWnd->SetBgColor(RGB(rand()%256,rand()%256,rand()%256));
		mpMainWnd->MoveWindow(rc.left+5,rc.top+5,rc.Width()-10,rc.Height()-10);
		mpMainWnd->Show(SW_SHOW);
	}
	//MessageBoxA(NULL,szMenuName,NULL,MB_OK);
	
	return true;
}

void MenuCmd::ReSize(const char* szMenuName)
{
	LOGF_VA(">>>[%s]MenuCmd::ReSize\n",szMenuName);
	if (mpMainWnd)
	{
		JKRect rc;
		GetClientRect(GetParent(*mpMainWnd),&rc);
		mpMainWnd->MoveWindow(rc.left+5,rc.top+5,rc.Width()-10,rc.Height()-10);
	}
	
}

void MenuCmd::CloseMenu(const char* szMenuName)
{
	if (mpMainWnd)
	{
		SendMessage(*mpMainWnd,WM_DESTROY,0,0);
		delete mpMainWnd;
		mpMainWnd = NULL;
	}
}