
// StagetyTestDlg.h : ͷ�ļ�
//

#pragma once

#include "Stagety/SafeCriticalSection/SafeCriticalSection.h"

using namespace COMMUSE;


// CStagetyTestDlg �Ի���
class CStagetyTestDlg : public CDialogEx,public IActionResultDelegate
{
// ����
public:
	CStagetyTestDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CStagetyTestDlg();

// �Ի�������
	enum { IDD = IDD_STAGETYTEST_DIALOG };

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
