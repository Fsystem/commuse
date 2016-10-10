#ifndef __TabControl_H
#define __TabControl_H

class TabTitle
{
	friend class TableControl;
public:
	TabTitle();
	void OnDraw(HDC hdc);
	bool OnClick(UINT nFlags,int x, int y);
protected:
	int mIndex;
	std::string mTitle;
	JKRect mRect;
	HWND mParant;
	HWND mSelf;
	bool mCanClose;
	bool mSelected;
};

class TabMainClient
{
public:
protected:
private:
};

struct jkTabControlItem
{
	int index;
	char szTitle[100];
	HWND hWnd;
	bool bCanClose;
};

class TableControl : public jkBaseWindow
{ 
public:
	void CreateTab(HWND hParant);
	void InsertTab(LPCSTR szTitle,HWND hWnd);
	void DeleteTab(int index ,LPCSTR szTitle);
	void ReveryfySize(RECT rc);
	void NotifyRedraw();

	virtual void OnCreate(LPCREATESTRUCT pStruct);
	virtual void OnPaint(HDC hdc);
	virtual BOOL OnEraseBackground(HDC hdc);
	virtual void OnSize(UINT nSizeCmd,int width,int height);
	virtual void OnLButtonUp(UINT nFlags,int x, int y);

	virtual LRESULT MessageHandle(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
protected:
	std::vector<TabTitle> mItems;
	int mCurPage;
	//jkBaseWindow mMainClient;
private:
};
#endif //__TabControl_H