
// IETestDlg.h : ͷ�ļ�
//

#pragma once
#include "explorer1.h"
#include "afxwin.h"


// CIETestDlg �Ի���
class CIETestDlg : public CDialogEx
{
// ����
public:
	CIETestDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_IETEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
