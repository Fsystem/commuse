// AutoPatchDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "FCool.h"
//#include "afxwin.h"
//#include "afxcmn.h"
 

#define DRAGGING	0x01	// dragging flag

// CAutoPatchDlg 对话框
class CAutoPatchDlg : public CDialog
{
	//zizi add-----------------
	HDC m_dcBkGrnd;			// background bitmap dc

	DWORD m_dwWidth;		// mask width
	DWORD m_dwHeight;		// mask height

	DWORD	m_dwFlags;		// flags
	POINT	m_pntMouse;		// Mouse position for dragging
	//------------------------

// 构造
public:
	HRGN CreateRgnFromFile( HBITMAP hBmp, COLORREF color );

	CAutoPatchDlg(CWnd* pParent = NULL);	// 标准构造函数

	void Printf( const char* pszString, ... );
	BOOL ReBuildLocal();
// 对话框数据
	enum { IDD = IDD_AUTOPATCH_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListBox m_lbMsg;
	CProgressCtrl m_pcDownload;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonStart();

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	CStatic m_State;
	CListBox m_lbBadFiles;

	CCoolButton		_btnExit;
	CCoolButton		_btnOK;
	HBITMAP _hBmp;
	HRGN _hRgn;
};
