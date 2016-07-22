#ifndef __App_H
#define __App_H



class jkBaseDialog;

class jkApp
{
	friend class jkBaseDialog;
public:
	jkApp();
	//void SetInstance(HINSTANCE hIns);
	HINSTANCE GetInstance();

	BOOL SetMainWndInfo(jkBaseDialog* pMainWnd);
protected:
	
private:

};


class jkBaseDialog
{
	friend class jkApp;
	friend BOOL Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
public:
	jkBaseDialog(HWND hParant= NULL);
	virtual ~jkBaseDialog();

	void Show(int nResId);

	//virtual void OnInitDialog();

	virtual LRESULT MessageHandle(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

	HWND GetCtrl(int nResId);

protected:
	HWND hParant;
	HWND mHwnd;
};


//-------------------------------------------------------------------------------

extern jkApp theJKApp;
#endif //__App_H