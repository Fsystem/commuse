
// md5_password_xor_encrypt_decryptDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "md5_password_xor_encrypt_decrypt.h"
#include "md5_password_xor_encrypt_decryptDlg.h"
#include "Encrypt.h"
#include "crc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// Cmd5_password_xor_encrypt_decryptDlg 对话框




Cmd5_password_xor_encrypt_decryptDlg::Cmd5_password_xor_encrypt_decryptDlg(CWnd* pParent /*=NULL*/)
	: CDialog(Cmd5_password_xor_encrypt_decryptDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	//srand(GetTickCount());
}

void Cmd5_password_xor_encrypt_decryptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_edt_src_string);
	DDX_Control(pDX, IDC_EDIT2, m_edt_src_md5);
	DDX_Control(pDX, IDC_EDIT3, m_edt_src_password);
	DDX_Control(pDX, IDC_EDIT4, m_edt_src_xor);
	DDX_Control(pDX, IDC_EDIT5, m_edt_file);
	DDX_Control(pDX, IDC_EDIT6, m_edt_file_md5);
	DDX_Control(pDX, IDC_EDIT7, m_edt_file_crc32);
	DDX_Control(pDX, IDC_EDIT8, m_edt_xor_src);
	DDX_Control(pDX, IDC_EDIT9, m_edt_xor_decode);
}

BEGIN_MESSAGE_MAP(Cmd5_password_xor_encrypt_decryptDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_EN_CHANGE(IDC_EDIT1, &Cmd5_password_xor_encrypt_decryptDlg::OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT8, &Cmd5_password_xor_encrypt_decryptDlg::OnEnChangeEdit8)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()


// Cmd5_password_xor_encrypt_decryptDlg 消息处理程序

BOOL Cmd5_password_xor_encrypt_decryptDlg::OnInitDialog()
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

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void Cmd5_password_xor_encrypt_decryptDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void Cmd5_password_xor_encrypt_decryptDlg::OnPaint()
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
HCURSOR Cmd5_password_xor_encrypt_decryptDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void Cmd5_password_xor_encrypt_decryptDlg::OnEnChangeEdit1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CString sSrcString;
	TCHAR szOut[1024]={0};
	m_edt_src_string.GetWindowText(sSrcString);
	if (0<sSrcString.GetLength())
	{
		memset(szOut,0,sizeof(szOut));
		CMD5Encrypt::EncryptData(sSrcString,szOut);
		m_edt_src_md5.SetWindowText(szOut);

		memset(szOut,0,sizeof(szOut));
		CMySqlPWEncrypt::EncryptData(sSrcString,szOut);
		m_edt_src_password.SetWindowText(szOut);

		memset(szOut,0,sizeof(szOut));
		CXOREncrypt::EncryptData(sSrcString,szOut,sizeof(szOut));
		m_edt_src_xor.SetWindowText(szOut);
	}
	else
	{
		m_edt_src_md5.SetWindowText(szOut);
		m_edt_src_password.SetWindowText(szOut);
		m_edt_src_xor.SetWindowText(szOut);
	}
}

void Cmd5_password_xor_encrypt_decryptDlg::OnEnChangeEdit8()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码

	CString sSrcString;
	TCHAR szOut[1024]={0};

	m_edt_xor_src.GetWindowText(sSrcString);

	if (0<sSrcString.GetLength())
	{
		CXOREncrypt::CrevasseData(sSrcString,szOut,sizeof(szOut));
		m_edt_xor_decode.SetWindowText(szOut);
	}
	else
	{
		m_edt_xor_decode.SetWindowText(szOut);
	}
}

void Cmd5_password_xor_encrypt_decryptDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	char sPath[_MAX_PATH]={0};
	int nFile = DragQueryFileA(hDropInfo,0,sPath,_MAX_PATH);

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
			TCHAR szOut[33]={0};
			CMD5Encrypt::EncryptData(pbuf,szOut);
			delete[] pbuf;
			//GetDlgItem(IDC_EDIT_CRC)->SetWindowText(sCrc);
			m_edt_file.SetWindowText(sPath);
			m_edt_file_crc32.SetWindowText(sCrc);
			m_edt_file_md5.SetWindowText(szOut);
			fclose(fp);

		}
	}

	CDialog::OnDropFiles(hDropInfo);
}
