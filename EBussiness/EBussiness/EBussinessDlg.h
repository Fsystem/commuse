
// EBussinessDlg.h : 头文件
//

#pragma once
#include "resource.h"		// 主符号
#include "explorer_ie.h"
#include "EKeyHook.h"
#include "IKeyboardSink.h"
#include "BaseDefine.h"
//#include "WebBrowser.h"第二种方法

// CEBussinessDlg 对话框
class CEBussinessDlg : public CDialog,public IKeyboardSink
{
// 构造
public:
	CEBussinessDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CEBussinessDlg();

// 对话框数据
	enum { IDD = IDD_EBUSSINESS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	//implement
public:
	virtual void ScanGunCallBackData(char* pszData,int nNum); 

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	afx_msg void OnCommandResolve(UINT nID);
	afx_msg LRESULT OnNotifyIcon(WPARAM wParam,LPARAM IParam);
	afx_msg LRESULT OnScanGunData(WPARAM wParam,LPARAM IParam);
	//afx_msg void SetLoginState(int nState);

	DECLARE_MESSAGE_MAP()

	void NewWindow2ExplorerIe(LPDISPATCH* ppDisp, BOOL* Cancel);
	void NewWindow3ExplorerIe(LPDISPATCH* ppDisp, BOOL* Cancel, unsigned long dwFlags, 
		LPCTSTR bstrUrlContext, LPCTSTR bstrUrl);
	//virtual void OnOK();
	virtual void OnCancel();
	DECLARE_EVENTSINK_MAP()

	//DECLARE_DISPATCH_MAP()
public:
	void ShowWindowTopmost();
	void HidenWindowMe();
	void AboutUs();
	void QuitMe();
	void ECreatePopupMenu();
	void SetLoginState(int nState);

	//task bar
private:
	//CWebBrowser* m_pIE;
	NOTIFYICONDATA	m_ncBar;
	CIEExplorer		m_ctrlIE;
	CMenu			m_EMenu;
	CEKeyHook*		m_pKeySink;
	CString			m_strScanGunData;
	bool			m_bLogin;
public:
	void DocumentCompleteExplorerIe(LPDISPATCH pDisp, VARIANT* URL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
