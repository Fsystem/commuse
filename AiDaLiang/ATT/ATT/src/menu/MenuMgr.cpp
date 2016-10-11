#include "stdafx.h"
#include "MenuMgr.h"

MenuMgr::MenuMgr()
{
	mRootMenu = NULL;
	mCurMenu = -1;
}

MenuMgr::~MenuMgr()
{

	if (mRootMenu)
	{
		DestroyMenu(mRootMenu);
		mRootMenu = NULL;
	}
	
}

void MenuMgr::MakeMenu(HWND hWnd)
{
	mRootMenu = ::CreateMenu();
	if (mRootMenu==NULL) return;

	
	LoadBaseMenu();
	LoadMenu();
	
	

	SetMenu(hWnd,mRootMenu);
}

void MenuMgr::LoadBaseMenu()
{
	if (mRootMenu==NULL) return ;
	HMENU rootPopMenu = ::CreatePopupMenu();
	AppendMenuA(mRootMenu,MF_POPUP,(UINT_PTR)rootPopMenu,"工具");
	AppendMenuA(rootPopMenu,MF_STRING,IDR_MenuId_Tool_Md5,"标准md5_crc");
	AppendMenuA(rootPopMenu,MF_STRING,IDR_MenuId_Tool_Reg,"正则表达式");
	AppendMenuA(rootPopMenu,MF_SEPARATOR,-1,"");
	AppendMenuA(rootPopMenu,MF_STRING,IDR_MenuId_Tool_Exit,"退出");
	DestroyMenu(rootPopMenu);
}

bool MenuMgr::LoadMenu()
{
	if (mRootMenu==NULL) return false;

	std::ifstream ifs("config.json");
	std::stringstream ss;
	ss<<ifs.rdbuf();
	//std::string sData(std::istreambuf_iterator<char>(ifs),std::istreambuf_iterator<char>());
	rapidjson::Document d;
	//sData.c_str();
	d.Parse(ss.str().c_str());
	if (d.HasParseError()) return false;

	rapidjson::Value& vMenu = d["menus"];

	if(vMenu.MemberCount()==0) return false;

	int nPMenuId = 0;
	for (rapidjson::Value::ConstMemberIterator it = vMenu.MemberBegin();it!=vMenu.MemberEnd();++it)
	{
		std::string sMenuName = it->name.GetString();
		HMODULE hMod = NULL;
		IJKMenu* pJKMenu = NULL;
		HMENU rootPopMenu = ::CreatePopupMenu();

		AppendMenuA(mRootMenu,MF_POPUP,(UINT_PTR)rootPopMenu,sMenuName.c_str());
		hMod =LoadLibraryA(it->value["path"].GetString());
		mMenus[sMenuName] = hMod;

		JKMENU_CreateInstance(hMod,pJKMenu);
		if (pJKMenu)
		{
			LPJKMenuDataArr mMenuArr;
			mMenuArr = pJKMenu->LoadMenu();
			if (mMenuArr)
			{
				int i = 0;
				do
				{
					AppendMenuA(rootPopMenu,MF_STRING,(UINT_PTR)IDR_MenuId_Custom_Base+nPMenuId*1000+i,mMenuArr[i]->szMenuName);
					mMenuCmds[IDR_MenuId_Custom_Base+nPMenuId*1000][i]= mMenuArr[i];

					i++;
				}while(mMenuArr[i]);
			}

		}
		nPMenuId++;
		LOGF_VA(">>>%s:\n",it->name.GetString());

		::DestroyMenu(rootPopMenu);
	}

	return true;
}

void MenuMgr::OnMenuCmd(UINT unId)
{
	if (mCurMenu == unId) return;

	DWORD dwOldMenu = mCurMenu;
	mCurMenu = unId;

	if (mCurMenu>=IDR_MenuId_Custom_Base && mCurMenu <= IDR_MenuId_Custom_Base+mMenuCmds.size()*1000)
	{
		int nParantMenuId = 1000*(dwOldMenu/1000);
		int nChildMenuId = dwOldMenu%1000;
		if (mMenuCmds.find(nParantMenuId) != mMenuCmds.end())
		{
			JKMenuData* pMenuDataOld = mMenuCmds[nParantMenuId][nChildMenuId];
			(pMenuDataOld->pCallPtr->*pMenuDataOld->CloseMenu)(pMenuDataOld->szMenuName);
		}
		
		nParantMenuId = 1000*(mCurMenu/1000);
		nChildMenuId = mCurMenu%1000;
		if (mMenuCmds.find(nParantMenuId) != mMenuCmds.end())
		{
			JKMenuData* pMenuData = mMenuCmds[nParantMenuId][nChildMenuId];
			(pMenuData->pCallPtr->*pMenuData->MenuCall)(pMenuData->szMenuName,*((MainWindow*)theJKApp.GetMainWnd())->GetMainClientWndPtr() );
		}
		
		return;
	}
	switch(mCurMenu)
	{
	case IDR_MenuId_Tool:
		break;
	case IDR_MenuId_Tool_Md5:
		OnMenuMd5();
		break;
	case IDR_MenuId_Tool_Reg:
		OnMenuReg();
		break;
	case IDR_MenuId_Tool_Exit:
		PostQuitMessage(0);
		break;
	}
}

void MenuMgr::OnMenuMd5()
{
	WinExec("md5_password_xor_crc_encrypt_decrypt.exe",SW_SHOWNORMAL);
}

void MenuMgr::OnMenuReg()
{
	WinExec("C-Regex-Test.exe",SW_SHOWNORMAL);
}

void MenuMgr::ReveryfySize()
{
	if (mCurMenu==-1) return;

	if (mCurMenu>=IDR_MenuId_Custom_Base && mCurMenu <= IDR_MenuId_Custom_Base+mMenuCmds.size()*1000)
	{
		int nParantMenuId = 1000*(mCurMenu/1000);
		int nChildMenuId = mCurMenu%1000;
		JKMenuData* pMenuData = mMenuCmds[nParantMenuId][nChildMenuId];
		(pMenuData->pCallPtr->*pMenuData->ReveryfySize)(pMenuData->szMenuName);
	}
	
}