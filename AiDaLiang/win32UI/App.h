#ifndef __App_H
#define __App_H
#include <CommCtrl.h>

struct jkMessageLoop
{
	virtual LRESULT MessageHandle(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam) = 0;
};
/**
 * @brief 
 *
 * œÍœ∏√Ë ˆ£∫
 */
struct jkWidget
{
	HWND mHwnd;
	jkMessageLoop* mMessageLoop;
};

/**
 * @brief 
 *
 * œÍœ∏√Ë ˆ£∫
 */
class jkApp
{
	friend class jkBaseDialog;
public:
	jkApp();
	void SetInstance(HINSTANCE hIns);
	HINSTANCE GetInstance();

	//BOOL SetMainWndInfo(jkBaseDialog* pMainWnd);
	jkWidget* GetMainWnd();
	void RunLoop(jkWidget* wnd);
protected:
	
private:

};

/**
 * @brief 
 *
 * œÍœ∏√Ë ˆ£∫
 */
class jkBaseDialog : public jkWidget , public jkMessageLoop
{
	friend class jkApp;
	friend BOOL Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
public:
	jkBaseDialog(HWND hParant= NULL);
	virtual ~jkBaseDialog();

	operator HWND()const throw();
	void Show(int nResId);
	void UpdateWindow();
	BOOL CenterWindow();
	void MoveWindow(int x,int y,int w,int h,BOOL bRePaint = FALSE);

	void SetTimer(UINT unId,int uElapse);
	void KillTimer(UINT unId);

	virtual void OnInitDialog();
	virtual void OnOk();
	virtual void OnCancel();
	virtual LRESULT OnCommand(WPARAM w,LPARAM l);
	virtual LRESULT OnDropFiles(HDROP hDrop);
	virtual void OnTimer(UINT unTimerId);

	virtual LRESULT MessageHandle(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

	HWND GetCtrl(int nResId);

protected:
	HWND hParant;
};

/**
 * @brief 
 *
 * œÍœ∏√Ë ˆ£∫
 */
class jkBaseWindow : public jkWidget , public jkMessageLoop
{
	friend class jkApp;
	friend LRESULT CALLBACK CLS_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
public:
	jkBaseWindow();
	operator HWND()const throw();
	void Create(HWND hParant,LPCSTR szTitle = NULL,DWORD dwStyle = 0,LPCSTR szWndClass = NULL,int nCtrlId = 0);
	void Show(int nCmd);
	void UpdateWindow();
	BOOL CenterWindow();
	void MoveWindow(int x,int y,int w,int h,BOOL bRePaint = FALSE);
	void SetBgColor(COLORREF bgColor);
	void SetTimer(UINT unId,int uElapse);
	void KillTimer(UINT unId);

	virtual void OnCreate(LPCREATESTRUCT pStruct);
	virtual void OnPaint(HDC hdc);
	virtual BOOL OnNcPaint(HDC hdc);
	virtual void OnSize(UINT nSizeCmd,int width,int height);
	virtual HBRUSH OnCtrlColor();
	virtual BOOL OnEraseBackground(HDC hdc);
	virtual void OnDestroy();
	virtual void OnTimer(UINT unTimerId);
	virtual void OnLButtonDown(UINT nFlags,int x, int y);
	virtual void OnLButtonUp(UINT nFlags,int x, int y);
	virtual void OnLButtonDBClick(UINT nFlags,int x, int y);
	virtual LRESULT MessageHandle(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
protected:
	COLORREF mBGColor;
private:
	void PaintBG(HDC hdc);
};


//-------------------------------------------------------------------------------

extern jkApp theJKApp;

//-------------------------------------------------------------------------------
//control
class jkControl : public jkWidget,public jkMessageLoop
{
public:
	jkControl();
	virtual ~jkControl();
public:
	operator HWND()const throw();
public:
	void SetCtrlInfo(LPCSTR wcControl,int nCtrlId);
	virtual void Create(HWND hWnd,int x,int y,int w,int h);
	virtual void Create(HWND hWnd,RECT rc);
	virtual void Destroy();
	virtual void SetStyle(DWORD dwStyle);
	virtual DWORD GetStyle();
	
public:
	//virtual void OnDestroy();
	virtual BOOL OnPaint(HDC hdc);
	virtual BOOL OnNcPaint(HDC hdc);
	virtual BOOL OnEraseBackground(HDC hdc);
	virtual BOOL OnCtrlColor(HDC hdc);
	virtual LRESULT MessageHandle(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
protected:
	std::string mClassName;
	DWORD mCtrlId;
	HWND mParant;
	DWORD mStyle;
	DWORD mStyleEx;
	WNDPROC mOldCall;

	//Œª÷√
	int x,y,wid,hei;
private:
	void PaintBG(HDC hdc);
};

//wc control
//edit WC_EDITA
//button WC_BUTTONA ...
//-------------------------------------------------------------------------------

////edit
//class jkEdit : public jkControl
//{
//public:
//	jkEdit();
//	virtual ~jkEdit();
//public:
//	
//public:
//	virtual void Create(HWND hWnd,int x,int y,int w,int h);
//	virtual void Create(HWND hWnd,RECT rc);
//	virtual void Destroy();
//protected:
//	//int mCtrlId;
//private:
//};
//
////jkbutton
//class jkButton : public jkControl
//{
//public:
//	jkButton();
//	virtual ~jkButton();
//public:
//
//protected:
//private:
//};
//
////jkLabel
//class jkLabel : public jkControl
//{
//public:
//	jkLabel();
//	virtual ~jkLabel();
//protected:
//private:
//};
//
////jkCombox
//class jkComboBox : public jkControl
//{
//public:
//	jkComboBox();
//	virtual ~jkComboBox();
//protected:
//private:
//};
//
////jkListBox
//class jkListBox : public
//{
//public:
//protected:
//private:
//};
//
////jkListCtrl
//class jkClassCtrl : public jkControl
//{
//public:
//	jkClassCtrl();
//	virtual ~jkClassCtrl();
//protected:
//private:
//};
#endif //__App_H