// VersionControlDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CVersionControlDlg 对话框
class CVersionControlDlg : public CDialog
{
// 构造
public:
	CVersionControlDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_VERSIONCONTROL_DIALOG };

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
