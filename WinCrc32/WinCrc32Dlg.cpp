
// WinCrc32Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WinCrc32.h"
#include "WinCrc32Dlg.h"
#include "crc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWinCrc32Dlg 对话框




CWinCrc32Dlg::CWinCrc32Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWinCrc32Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWinCrc32Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CWinCrc32Dlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DROPFILES()
END_MESSAGE_MAP()


// CWinCrc32Dlg 消息处理程序

BOOL CWinCrc32Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CWinCrc32Dlg::OnPaint()
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
HCURSOR CWinCrc32Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CWinCrc32Dlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	char sPath[_MAX_PATH]={0};
	int nFile = DragQueryFile(hDropInfo,0,sPath,_MAX_PATH);

	if (0 < nFile )
	{
		FILE* fp = fopen(sPath,"rb");
		if (fp)
		{
			fseek(fp,0,SEEK_END);
			int nSize = ftell(fp);
			fseek(fp,0,SEEK_SET);
			char* pbuf = new char[nSize];
			fread(pbuf,1,nSize,fp);
			CString sCrc ;
			sCrc.Format(_T("%X"),GetCrcLong(pbuf,nSize) );
			delete[] pbuf;
			GetDlgItem(IDC_EDIT_CRC)->SetWindowText(sCrc);
			fclose(fp);
		}
	}
	CDialog::OnDropFiles(hDropInfo);
}
