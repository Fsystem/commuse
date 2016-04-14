
// StagetyTestDlg.h : ͷ�ļ�
//

#pragma once

#include "Stagety/SafeCriticalSection/SafeCriticalSection.h"

using namespace COMMUSE;

typedef BOOL (WINAPI *SYS_RegCallBackFun)(CALLBACK_FILTERFUN fun);			//ע��ص�
typedef BOOL (WINAPI *SYS_OperSysFilter)(BOOL start);						//���أ�ֹͣ����
typedef BOOL (WINAPI *SYS_OperMsgFilter)(BOOL start);						//������Ϣ���˱���


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
