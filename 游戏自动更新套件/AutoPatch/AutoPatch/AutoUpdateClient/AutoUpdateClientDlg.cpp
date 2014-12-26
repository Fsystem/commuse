
// AutoUpdateClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AutoUpdateClient.h"
#include "AutoUpdateClientDlg.h"

#include "../Debug/MyAutoPatch.h"

#pragma comment(lib,"../Debug/AutoPatch.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CMyAutoPatch* pAp = NULL;
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
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

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CAutoUpdateClientDlg 对话框




CAutoUpdateClientDlg::CAutoUpdateClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAutoUpdateClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CAutoUpdateClientDlg::~CAutoUpdateClientDlg()
{
	delete pAp;
	pAp=NULL;
}

void CAutoUpdateClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAutoUpdateClientDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_RADHTTP, &CAutoUpdateClientDlg::OnBnClickedRadhttp)
	ON_BN_CLICKED(IDC_RADFTP, &CAutoUpdateClientDlg::OnBnClickedRadftp)
	ON_BN_CLICKED(IDOK, &CAutoUpdateClientDlg::OnBnClickedOk)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDCANCEL, &CAutoUpdateClientDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CAutoUpdateClientDlg 消息处理程序

BOOL CAutoUpdateClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

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

	OnBnClickedRadhttp();
	::SetDlgItemTextA(this->m_hWnd,IDC_EDITHTTP,"http://down.3qqp.cn/updatelist/");

	::SetDlgItemTextA(this->m_hWnd,IDC_EDITFTP,"192.168.1.29");
	::SetDlgItemTextA(this->m_hWnd,IDC_EDITFTPUSR,"lgw");
	::SetDlgItemTextA(this->m_hWnd,IDC_EDITFTPPWD,"lgw");

	((CListCtrl*)GetDlgItem(IDC_LIST_DOWN))->InsertColumn(0,TEXT("文件名"),LVCFMT_LEFT,300);

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CAutoUpdateClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CAutoUpdateClientDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CAutoUpdateClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CAutoUpdateClientDlg::OnBnClickedRadhttp()
{
	// TODO: 在此添加控件通知处理程序代码
	((CButton*)GetDlgItem(IDC_RADHTTP))->SetCheck(BST_CHECKED);
	((CButton*)GetDlgItem(IDC_RADFTP))->SetCheck(BST_UNCHECKED);
	GetDlgItem(IDC_EDITHTTP)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDITFTP)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDITFTPUSR)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDITFTPPWD)->EnableWindow(FALSE);
}

void CAutoUpdateClientDlg::OnBnClickedRadftp()
{
	// TODO: 在此添加控件通知处理程序代码
	((CButton*)GetDlgItem(IDC_RADFTP))->SetCheck(BST_CHECKED);
	((CButton*)GetDlgItem(IDC_RADHTTP))->SetCheck(BST_UNCHECKED);
	GetDlgItem(IDC_EDITHTTP)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDITFTP)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDITFTPUSR)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDITFTPPWD)->EnableWindow(TRUE);
}

void CAutoUpdateClientDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//OnOK();
	char strUrl[MAX_PATH]={0};
	//char strUsr1[MAX_PATH]={0};
	char strUsr[MAX_PATH]={0};
	char strPwd[MAX_PATH]={0};
	if (IsHttpMode()) ::GetDlgItemTextA(this->m_hWnd,IDC_EDITHTTP,strUrl,_countof(strUrl));
	else ::GetDlgItemTextA(this->m_hWnd,IDC_EDITFTP,strUrl,_countof(strUrl));

	::GetDlgItemTextA(this->m_hWnd,IDC_EDITFTPUSR,strUsr,_countof(strUsr));
	::GetDlgItemTextA(this->m_hWnd,IDC_EDITFTPPWD,strPwd,_countof(strPwd));
	
	//if (IsHttpMode()) sprintf(strUsr1,"%s",strUrl);
	//else sprintf(strUsr1,"Ftp://%s",strUrl);

	
	if(pAp) delete pAp;
	pAp = CMyAutoPatch::CreateInstance(!IsHttpMode(),strUrl,"down.cfg",
		"filelist_000.txt","download.txt",strUsr,strPwd);
 	if(pAp->DownLoadFileList())
 	{
 		SetTimer(0,10,NULL);
 		pAp->BeginDown();
 	}
 	else
 	{
 		AfxMessageBox(_T("已经是最新"));
 	}
}

void CAutoUpdateClientDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (0 == nIDEvent)
	{
		tagDownLoadStatus tag;
		pAp->GetDownLoadStatus(tag);

		switch(tag.DownLoadStatus) 
		{
		case enDownLoadStatus_Finish:
		case enDownLoadStatus_Fails:
			KillTimer(nIDEvent);
			GetDlgItem(IDC_STATIC_STATUS)->SetWindowText(CString(tag.szStatus));

			((CProgressCtrl*)GetDlgItem(IDC_PROGRESS_DOWN))->SetRange32(0,100);
			((CProgressCtrl*)GetDlgItem(IDC_PROGRESS_DOWN))->SetPos(100);
			break;
		case enDownLoadStatus_DownLoadIng:
			{
				
				tagDownStatusInfo tagFile=pAp->GetCurDownFileInfo();

				((CProgressCtrl*)GetDlgItem(IDC_PROGRESS_DOWN))->SetRange32(0,tagFile.dwTotal);
				((CProgressCtrl*)GetDlgItem(IDC_PROGRESS_DOWN))->SetPos(tagFile.dwTransfered);

				int nCount = ((CListCtrl*)GetDlgItem(IDC_LIST_DOWN))->GetItemCount();
				bool bAdd = true;
				CString strName(tagFile.szDownFile);
				CString sT;
				
				for (int i=0;i<nCount;i++ )
				{
					sT=((CListCtrl*)GetDlgItem(IDC_LIST_DOWN))->GetItemText(i,0);
					if( !strName.IsEmpty() && sT == strName )
					{
						bAdd = false;
						break;
					}

				}

				if (bAdd&& !strName.IsEmpty())
				{
					((CListCtrl*)GetDlgItem(IDC_LIST_DOWN))->InsertItem(nCount,strName);
				}

				
				nCount = ((CListCtrl*)GetDlgItem(IDC_LIST_DOWN))->GetItemCount();
				
				sT.Format(_T("文件数:%u / %u"),pAp->GetDownedFileNum(),pAp->GetNeedDownFileNum() );
				GetDlgItem(IDC_STATIC_DOWNNUM)->SetWindowText(sT);
				
				sT.Format(_T("文件名:%s,已下载:%u,总大小:%u"),strName,
					tagFile.dwTransfered,tagFile.dwTotal);
				//OutputDebugString(sT);
				GetDlgItem(IDC_STATIC_STATUS)->SetWindowText(sT);
			}
			break;
		case enDownLoadStatus_Compressing:
			{
				GetDlgItem(IDC_STATIC_STATUS)->SetWindowText(CString(tag.szStatus));
				((CProgressCtrl*)GetDlgItem(IDC_PROGRESS_DOWN))->SetRange32(0,100);
				((CProgressCtrl*)GetDlgItem(IDC_PROGRESS_DOWN))->SetPos(100);

				CString sT;
				//int nCount = ((CListCtrl*)GetDlgItem(IDC_LIST_DOWN))->GetItemCount();

				sT.Format(_T("文件数:%u / %u"),pAp->GetDownedFileNum(),pAp->GetNeedDownFileNum() );
				GetDlgItem(IDC_STATIC_DOWNNUM)->SetWindowText(sT);
			}
			break;
		case enDownLoadStatus_Unknow:
			break;
		case enDownLoadStatus_Ready:
			break;
		}
	}
	CDialog::OnTimer(nIDEvent);
}

void CAutoUpdateClientDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}
