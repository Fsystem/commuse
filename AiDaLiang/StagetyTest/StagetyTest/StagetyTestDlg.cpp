
// StagetyTestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "StagetyTest.h"
#include "StagetyTestDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CStagetyTestDlg 对话框




CStagetyTestDlg::CStagetyTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CStagetyTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	GetModuleFileName(NULL, m_strPath.GetBuffer(MAX_PATH), MAX_PATH);
	m_strPath.ReleaseBuffer();
	int nIndex = m_strPath.ReverseFind('\\');
	m_strPath = m_strPath.Left(nIndex);

	m_hDllModule = NULL;

	mStopOut = false;
}

CStagetyTestDlg::~CStagetyTestDlg()
{
	if(m_hDllModule)
	{
		m_pOperSysFilter(FALSE);
		m_pOperMsgFilter(FALSE);
		FreeLibrary(m_hDllModule);
		m_hDllModule = NULL;
	}
}

void CStagetyTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CStagetyTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	//ON_MESSAGE(NOTIFY_HANDLE_STAGETY_RESULT,&CStagetyTestDlg::OnProcessResult)
	ON_BN_CLICKED(IDOK, &CStagetyTestDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CStagetyTestDlg::OnBnClickedButton1)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CStagetyTestDlg 消息处理程序

BOOL CStagetyTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	SetPrivilege();

	StagetyManager::Instance().BuildAllStagety(this);
	StartDriver();
	SetTimer(1,2000,NULL);
	SetTimer(2,100,NULL);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

BOOL CStagetyTestDlg::OnHandleResultByStagety(ActionOperateResult* pResult)
{
	ActionOperateResult result = *pResult;
	mResultList.AddTail(result);

	return TRUE;
}

void CStagetyTestDlg::OnTimer(UINT uId)
{
	if (uId == 1)
	{
		if (m_hDllModule == NULL)
		{
			StartDriver();
		}
	}
	else if (uId == 2)
	{
		if(!mStopOut) 
		{
 			if (mResultList.GetCount()>0)
 			{
 				ActionOperateResult result;
				{
					result = mResultList.RemoveHead();
				}
 
 				CString sEdit ;
 				GetDlgItem(IDC_EDIT_DESCRIBE)->GetWindowText(sEdit);
 
 				sEdit += result.szDescriber;

 				GetDlgItem(IDC_EDIT_DESCRIBE)->SetWindowText(sEdit);
 
 				//((CEdit*)GetDlgItem(IDC_EDIT_DESCRIBE))->LineScroll(((CEdit*)GetDlgItem(IDC_EDIT_DESCRIBE))->GetLineCount(),0);
 			}
		}

		
	}

	__super::OnTimer(uId);
}

LRESULT CStagetyTestDlg::OnProcessResult(WPARAM w,LPARAM p)
{
//	mResultList.Insert((LPVOID)w);
	
// 	NotifyStagety* pNotify = (NotifyStagety*)w;
// 
// 	CString sEdit ;
// 	GetDlgItem(IDC_EDIT_DESCRIBE)->GetWindowText(sEdit);
// 
// 	TCHAR szMessage[1025]={0};
// 	memcpy(szMessage,pNotify->GetData(),pNotify->GetDataSize());
// 	szMessage[pNotify->GetDataSize()]=0;
// 
// 	pNotify->Release();
// 
// 	sEdit += szMessage;
// 	GetDlgItem(IDC_EDIT_DESCRIBE)->SetWindowText(sEdit);

	return 0L;
}

void CStagetyTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CStagetyTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CStagetyTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL WINAPI Sys_Filter_fun(UINT type, PVOID lpata, DWORD size_len)
{
	return StagetyManager::Instance().HandleAction(type,lpata,size_len);
}

BOOL CStagetyTestDlg::StartDriver()
{
	CString strFile;
	strFile.Format(TEXT("%s/tmpsys.dll"), m_strPath);
	CheckAndExportFile(TEXT("DLL"), strFile, IDR_SYSIO);	
	m_hDllModule = LoadLibrary(strFile);
	if( NULL != m_hDllModule)
	{
		m_pRegCallBackFun = (SYS_RegCallBackFun)GetProcAddress(m_hDllModule, "RegCallBackFun");
		m_pOperMsgFilter = (SYS_OperMsgFilter)GetProcAddress(m_hDllModule, "OperMsgFilter");
		m_pOperSysFilter = (SYS_OperSysFilter)GetProcAddress(m_hDllModule, "OperSysFilter");
		if( NULL == m_pRegCallBackFun||
			NULL == m_pOperMsgFilter||
			NULL == m_pOperSysFilter)
		{
			FreeLibrary(m_hDllModule);
			m_hDllModule = NULL;
			return FALSE;
		}
	}

	if( !m_pRegCallBackFun(Sys_Filter_fun) )
	{
		LOGEVEN(TEXT("注册回调失败\n"));
	}
	m_pOperSysFilter(TRUE);
	m_pOperMsgFilter(TRUE);
	return TRUE;
}



void CStagetyTestDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//CDialogEx::OnOK();
	((CEdit*)GetDlgItem(IDC_EDIT_DESCRIBE))->SetWindowText(TEXT(""));
}


void CStagetyTestDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码

	mStopOut = !mStopOut;

	GetDlgItem(IDC_BUTTON1)->SetWindowText(mStopOut?TEXT("继续"):TEXT("暂停"));
}


void CStagetyTestDlg::OnDestroy()
{
	__super::OnDestroy();

	// TODO: 在此处添加消息处理程序代码

	KillTimer(1);
	KillTimer(2);
}
