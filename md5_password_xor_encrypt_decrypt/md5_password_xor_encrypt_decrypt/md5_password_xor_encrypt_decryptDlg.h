
// md5_password_xor_encrypt_decryptDlg.h : 头文件
//

#pragma once
#include "afxwin.h"


// Cmd5_password_xor_encrypt_decryptDlg 对话框
class Cmd5_password_xor_encrypt_decryptDlg : public CDialog
{
// 构造
public:
	Cmd5_password_xor_encrypt_decryptDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MD5_PASSWORD_XOR_ENCRYPT_DECRYPT_DIALOG };

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
	CEdit m_edt_src_string;
	CEdit m_edt_src_md5;
	CEdit m_edt_src_password;
	CEdit m_edt_src_xor;
	CEdit m_edt_file;
	CEdit m_edt_file_md5;
	CEdit m_edt_file_crc32;
	afx_msg void OnEnChangeEdit1();
	CEdit m_edt_xor_src;
	CEdit m_edt_xor_decode;
	afx_msg void OnEnChangeEdit8();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	CEdit m_edt_str_crc;
};
