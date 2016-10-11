#ifndef __App_H
#define __App_H
/**
 * @brief 
 *
 * œÍœ∏√Ë ˆ£∫
 */
struct jkWidget
{
	HWND mHwnd;
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
class jkBaseDialog : public jkWidget
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
class jkBaseWindow : public jkWidget
{
	friend class jkApp;
	friend LRESULT CALLBACK CLS_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
public:
	jkBaseWindow();
	operator HWND()const throw();
	void Create(HWND hParant,LPCSTR szTitle = NULL,DWORD dwStyle = 0,LPCSTR szWndClass = NULL);
	void Show(int nCmd);
	void UpdateWindow();
	BOOL CenterWindow();
	void MoveWindow(int x,int y,int w,int h,BOOL bRePaint = FALSE);
	void SetBgColor(COLORREF bgColor);

	virtual void OnCreate(LPCREATESTRUCT pStruct);
	virtual void OnPaint(HDC hdc);
	virtual BOOL OnNcPaint(HDC hdc);
	virtual void OnSize(UINT nSizeCmd,int width,int height);
	virtual HBRUSH OnCtrlColor();
	virtual BOOL OnEraseBackground(HDC hdc);
	virtual void OnDestroy();
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
//edit
class jkEdit : public jkBaseWindow
{
public:
	jkEdit();
	virtual ~jkEdit();
public:
	virtual void Create(HWND hWnd,int x,int y,int w,int h);
	virtual void Create(HWND hWnd,RECT rc);
	virtual void Destroy();
protected:
private:
};
#endif //__App_H