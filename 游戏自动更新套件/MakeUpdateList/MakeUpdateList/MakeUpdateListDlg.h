
// MakeUpdateListDlg.h : ͷ�ļ�
//

#pragma once
#include <string>
#include "xml/tinyxml.h"

using std::string;

// CMakeUpdateListDlg �Ի���
class CMakeUpdateListDlg : public CDialog
{
// ����
public:
	CMakeUpdateListDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CMakeUpdateListDlg();

// �Ի�������
	enum { IDD = IDD_MAKEUPDATELIST_DIALOG };

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
