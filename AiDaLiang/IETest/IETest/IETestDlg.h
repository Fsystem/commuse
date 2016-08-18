
// IETestDlg.h : 头文件
//

#pragma once
#include "explorer1.h"
#include "afxwin.h"


// CIETestDlg 对话框
class CIETestDlg : public CDialogEx
{
// 构造
public:
	CIETestDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_IETEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CExplorer1 m_ieCtrl;
	CString m_sUrl;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CButton m_BnAccess;
	CEdit m_edUrl;
	DECLARE_EVENTSINK_MAP()
	void NewWindow3Explorer1(LPDISPATCH* ppDisp, BOOL* Cancel, unsigned long dwFlags, LPCTSTR bstrUrlContext, LPCTSTR bstrUrl);
	CButton m_bnBack;
	afx_msg void OnBnClickedButton1();
//	void NavigateComplete2Explorer1(LPDISPATCH pDisp, VARIANT* URL);

	virtual BOOL CreateControlSite(COleControlContainer* pContainer,   
		COleControlSite** ppSite, UINT  nID , REFCLSID  clsid );  
};
