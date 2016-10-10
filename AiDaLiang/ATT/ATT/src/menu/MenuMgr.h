#ifndef __MenuMgr_H
#define __MenuMgr_H

class MenuMgr
{
public:
	MenuMgr();
	~MenuMgr();

public:
	void MakeMenu(HWND hWnd);
	void OnMenuCmd(UINT unId);
	void OnMenuMd5();
	void OnMenuReg();

	//-------------------------------------------------------------------------------
	void ReveryfySize();
protected:
	void LoadBaseMenu();
	bool LoadMenu();
private:
	HMENU mRootMenu;
	DWORD mCurMenu;
	std::map<std::string,HMODULE> mMenus;
	std::map<DWORD,std::map<DWORD,JKMenuData*>> mMenuCmds;
	
};
#endif //__MenuMgr_H