
// AutoUpdateClientDlg.h : ͷ�ļ�
//
#ifndef __AutoUpdateClientDlg_H
#define __AutoUpdateClientDlg_H


#pragma once


// CAutoUpdateClientDlg �Ի���
class CAutoUpdateClientDlg : public CDialog
{
// ����
public:
	CAutoUpdateClientDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CAutoUpdateClientDlg();

// �Ի�������
	enum { IDD = IDD_AUTOUPDATECLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

private:
	inline bool IsHttpMode(){ return BST_CHECKED == ((CButton*)GetDlgItem(IDC_RADHTTP))->GetCheck();}

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
	afx_msg void OnBnClickedRadhttp();
	afx_msg void OnBnClickedRadftp();
	afx_msg void OnBnClickedOk();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedCancel();
};
#endif //__AutoUpdateClientDlg_H