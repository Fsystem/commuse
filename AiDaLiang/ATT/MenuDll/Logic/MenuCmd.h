#ifndef __MenuCmd_H
#define __MenuCmd_H
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
	JKMenuData mMenus[10];
	LPJKMenuDataArr mMenusPtr;
	MainWnd* mpMainWnd;
};

#endif //__MenuCmd_H