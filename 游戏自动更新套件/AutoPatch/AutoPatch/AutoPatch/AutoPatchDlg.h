// AutoPatchDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "FCool.h"
//#include "afxwin.h"
//#include "afxcmn.h"
 

#define DRAGGING	0x01	// dragging flag

// CAutoPatchDlg �Ի���
class CAutoPatchDlg : public CDialog
{
	//zizi add-----------------
	HDC m_dcBkGrnd;			// background bitmap dc

	DWORD m_dwWidth;		// mask width
	DWORD m_dwHeight;		// mask height

	DWORD	m_dwFlags;		// flags
	POINT	m_pntMouse;		// Mouse position for dragging
	//------------------------

// ����
public:
	HRGN CreateRgnFromFile( HBITMAP hBmp, COLORREF color );

	CAutoPatchDlg(CWnd* pParent = NULL);	// ��׼���캯��

	void Printf( const char* pszString, ... );
	BOOL ReBuildLocal();
// �Ի�������
	enum { IDD = IDD_AUTOPATCH_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
