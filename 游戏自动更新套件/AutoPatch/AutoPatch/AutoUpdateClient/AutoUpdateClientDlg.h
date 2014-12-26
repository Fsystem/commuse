
// AutoUpdateClientDlg.h : 头文件
//
#ifndef __AutoUpdateClientDlg_H
#define __AutoUpdateClientDlg_H


#pragma once


// CAutoUpdateClientDlg 对话框
class CAutoUpdateClientDlg : public CDialog
{
// 构造
public:
	CAutoUpdateClientDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CAutoUpdateClientDlg();

// 对话框数据
	enum { IDD = IDD_AUTOUPDATECLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

private:
	inline bool IsHttpMode(){ return BST_CHECKED == ((CButton*)GetDlgItem(IDC_RADHTTP))->GetCheck();}

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
	afx_msg void OnBnClickedRadhttp();
	afx_msg void OnBnClickedRadftp();
	afx_msg void OnBnClickedOk();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedCancel();
};
#endif //__AutoUpdateClientDlg_H