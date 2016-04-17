
// StagetyTestDlg.h : 头文件
//

#pragma once

#include "Stagety/SafeCriticalSection/SafeCriticalSection.h"

using namespace COMMUSE;


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
	CString m_strPath;

	CCritiSection mLock;
	CList<ActionOperateResult,ActionOperateResult&> mResultList;
	bool mStopOut;

public:
	virtual void OnHandleResultByStagety(ActionOperateResult* pResult);

protected:
	afx_msg void OnBnClickedOk();
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnDestroy();
};
