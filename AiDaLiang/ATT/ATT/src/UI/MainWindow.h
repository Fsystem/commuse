#ifndef __MainWindow_H
#define __MainWindow_H
class MainWindow : public jkBaseWindow
{
public:
	MainClient* GetMainClientWndPtr();
	RECT GetMainClientRect();
	RECT GetMainTabRect();
	RECT GetClientRect();
protected:
	virtual void OnCreate(LPCREATESTRUCT pStruct);
	virtual void OnPaint(HDC hdc);
	//virtual HBRUSH OnCtrlColor();
	virtual void OnSize(UINT nSizeCmd,int width,int height);
	virtual LRESULT MessageHandle(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
protected:
	MenuMgr mMenu;
	TableControl mTabMain;
	MainClient mClient;
private:
};
#endif //__MainWindow_H