#ifndef __MenuCmd_H
#define __MenuCmd_H
__declspec(selectany) char* szMuName[]={
	"创建进程"
};
class MenuCmd : public IJKMenu
{
public:
	MenuCmd();
	~MenuCmd();
	virtual HWND GetMainHwd();
	virtual LPJKMenuDataArr LoadMenu();
protected:
	bool MenuCall(const char* szMenuName,HWND hClientMain);
	void ReSize(const char* szMenuName);
	void CloseMenu(const char* szMenuName);
private:
	JKMenuData mMenus[CountArr(szMuName)];
	char*	mMenuName[CountArr(szMuName)];
	LPJKMenuDataArr mMenusPtr;
	MainWnd* mpMainWnd;
};

#endif //__MenuCmd_H