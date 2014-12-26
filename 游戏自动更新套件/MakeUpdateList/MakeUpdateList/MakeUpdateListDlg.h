
// MakeUpdateListDlg.h : 头文件
//

#pragma once
#include <string>
#include "xml/tinyxml.h"

using std::string;

// CMakeUpdateListDlg 对话框
class CMakeUpdateListDlg : public CDialog
{
// 构造
public:
	CMakeUpdateListDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CMakeUpdateListDlg();

// 对话框数据
	enum { IDD = IDD_MAKEUPDATELIST_DIALOG };

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

public:
	bool MakeXmlFile(string strFile);
	bool MakeNode(string strNode,string strCrc,string strZip,string strZIPFileName,
		string strZIPFilePath,string strfilePath);
	bool IsRoot(LPCTSTR lpszPath) ;
	void FindAll(LPCTSTR szPath);
	void SpliteStr(string strFull,char cFilter,string* pStrArr,int& nOutNum);
	CString GetExecuteFilePath();
	CString GetExecuteFileName();

	string GetCrc(string );
	string IsZip(string );
	string GetZipFileName(string);
	string GetZipFilePath(string);
	string GetFilePath(string);

private:
	TiXmlDocument*	m_pXmlDoc;
	TiXmlElement*	m_pRootNode;
	CString			m_ExePath;
	CString			m_ExeName;
	CString			m_XmlName;
};
