// VersionControlDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CVersionControlDlg �Ի���
class CVersionControlDlg : public CDialog
{
// ����
public:
	CVersionControlDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_VERSIONCONTROL_DIALOG };

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
	afx_msg void OnBnClickedButtonBrowse();
	CListCtrl m_lcFiles;
	CButton m_btnBrowse;
	afx_msg void OnBnClickedButtonGenfilelist();
	CProgressCtrl m_progress;
	afx_msg void OnBnClickedButtonGenuploadfiles();
	CEdit m_editGenFileExt;
	CEdit m_editVersion;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
