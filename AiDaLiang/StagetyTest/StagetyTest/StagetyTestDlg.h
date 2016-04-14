
// StagetyTestDlg.h : 头文件
//

#pragma once

#include "Stagety/SafeCriticalSection/SafeCriticalSection.h"

using namespace COMMUSE;

typedef BOOL (WINAPI *SYS_RegCallBackFun)(CALLBACK_FILTERFUN fun);			//注册回调
typedef BOOL (WINAPI *SYS_OperSysFilter)(BOOL start);						//加载，停止驱动
typedef BOOL (WINAPI *SYS_OperMsgFilter)(BOOL start);						//加载消息过滤保护


// CStagetyTestDlg 对话框
class CStagetyTestDlg : public CDialogEx,public IActionResultDelegate
{
// 构造
public:
	CStagetyTestDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CStagetyTestDlg();

// 对话框数据
	enum { IDD = IDD_STAGETYTEST_DIALOG };

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
	afx_msg void OnTimer(UINT uId);
	afx_msg LRESULT OnProcessResult(WPARAM w,LPARAM p);
	DECLARE_MESSAGE_MAP()

public:
	SYS_RegCallBackFun		m_pRegCallBackFun;
	SYS_OperSysFilter		m_pOperSysFilter;
	SYS_OperMsgFilter		m_pOperMsgFilter;

	BOOL StartDriver();

	HMODULE m_hDllModule;
	CString m_strPath;

	CCritiSection mLock;
	CList<ActionOperateResult,ActionOperateResult&> mResultList;
	bool mStopOut;

public:
	virtual BOOL OnHandleResultByStagety(ActionOperateResult* pResult);

protected:
	afx_msg void OnBnClickedOk();
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnDestroy();
};
