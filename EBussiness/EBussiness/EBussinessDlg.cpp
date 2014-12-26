
// EBussinessDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "EBussiness.h"
#include "EBussinessDlg.h"
#include <atlconv.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//MESSAGE
enum EMSG{
	WM_MSG_NCTASKBAR=WM_USER+100,
	WM_MSG_SCAN_GUN_DATA,
};

//CONTROL'S ID
enum EIDC{
	IDC_EXPLORER_IE1=1000,
};
//COMMAND
enum EIDM{
	IDM_FIRST=32768,
	IDM_SHOWMAIN,
	IDM_ABOUT_US,
	IDM_QUIT,

	IDM_LAST,
};

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


// CEBussinessDlg 对话框




CEBussinessDlg::CEBussinessDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEBussinessDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pKeySink=CEKeyHook::CreateInstance();
	m_pKeySink->SetKeyCallBackSink(this);
	m_bLogin = false;
}

CEBussinessDlg::~CEBussinessDlg()
{
	SAFE_DELETE(m_pKeySink);
}

void CEBussinessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EXPLORER_IE, m_ctrlIE);
}

BEGIN_MESSAGE_MAP(CEBussinessDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_MESSAGE(WM_MSG_NCTASKBAR,OnNotifyIcon)
	ON_MESSAGE(WM_MSG_SCAN_GUN_DATA,OnScanGunData)
	ON_COMMAND_RANGE(IDM_FIRST,IDM_LAST,OnCommandResolve)
END_MESSAGE_MAP()


// CEBussinessDlg 消息处理程序



BOOL CEBussinessDlg::OnInitDialog()
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
	TCHAR szTemp[MAX_PATH]={0};
	int nWidth=::GetPrivateProfileInt(_T("info"),_T("windowWidth"),0,_T("./config.ini"));
	int nHeight=::GetPrivateProfileInt(_T("info"),_T("windowHeight"),0,_T("./config.ini"));
	::GetPrivateProfileString(_T("info"),_T("mainWebAddress"),_T(""),szTemp,MAX_PATH,_T("./config.ini"));
	CString strWeb=CString(szTemp);

	//m_pIE = new CWebBrowser;
	

	if (0<nWidth&&0<nHeight&&!strWeb.IsEmpty())
	{
		CRect rc;
		rc.left=(::GetSystemMetrics(SM_CXSCREEN)-nWidth)/2;
		rc.top=(::GetSystemMetrics(SM_CYSCREEN)-nHeight)/2;
		rc.right=rc.left+nWidth;
		rc.bottom=rc.top+nHeight;
		MoveWindow(&rc);
		
		GetClientRect(&rc);
		//m_pIE->Create(NULL,NULL,WS_VISIBLE|WS_CHILD,rc,this,IDC_EXPLORER_IE1);
		//m_pIE->Navigate(strWeb);
		m_ctrlIE.MoveWindow(&rc);
		m_ctrlIE.Navigate(strWeb,0,0,0,0);

		m_ncBar.cbSize=sizeof(NOTIFYICONDATA);
		m_ncBar.hIcon=AfxGetApp()->LoadIcon(IDR_MAINFRAME);
		m_ncBar.hWnd=m_hWnd;
		lstrcpy(m_ncBar.szTip,_T("NotifyIcon Test"));
		m_ncBar.uCallbackMessage=WM_MSG_NCTASKBAR;
		m_ncBar.uFlags=NIF_ICON | NIF_MESSAGE | NIF_TIP;
		Shell_NotifyIcon(NIM_ADD,&m_ncBar);

	}
	else
	{
		AfxMessageBox(_T("配置错误"),MB_OK|MB_ICONERROR);
		::PostQuitMessage(0);
	}
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CEBussinessDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX || IDM_ABOUT_US == nID)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if (SC_CLOSE == nID || SC_MINIMIZE == nID || SC_MAXIMIZE == nID)
	{
		HidenWindowMe();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CEBussinessDlg::OnPaint()
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
HCURSOR CEBussinessDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BEGIN_EVENTSINK_MAP(CEBussinessDlg, CDialog)
	ON_EVENT(CEBussinessDlg, IDC_EXPLORER_IE, 251, CEBussinessDlg::NewWindow2ExplorerIe, VTS_PDISPATCH VTS_PBOOL)
	ON_EVENT(CEBussinessDlg, IDC_EXPLORER_IE, 273, CEBussinessDlg::NewWindow3ExplorerIe, VTS_PDISPATCH VTS_PBOOL VTS_UI4 VTS_BSTR VTS_BSTR)
	ON_EVENT(CEBussinessDlg, IDC_EXPLORER_IE, 259, CEBussinessDlg::DocumentCompleteExplorerIe, VTS_DISPATCH VTS_PVARIANT)
END_EVENTSINK_MAP()

// BEGIN_DISPATCH_MAP(CEBussinessDlg, CDialog)
// 	//{{AFX_DISPATCH_MAP(CMyHtmlView)
// 	DISP_FUNCTION(CEBussinessDlg, "SetLoginState", CEBussinessDlg::SetLoginState,VT_NULL, VTS_I4)
// 	//}}AFX_DISPATCH_MAP
// END_DISPATCH_MAP()

void CEBussinessDlg::NewWindow2ExplorerIe(LPDISPATCH* ppDisp, BOOL* Cancel)
{
	// TODO: 在此处添加消息处理程序代码
	// Get a pointer to the application object
	*ppDisp=m_ctrlIE.get_Application();

}



void CEBussinessDlg::NewWindow3ExplorerIe(LPDISPATCH* ppDisp, BOOL* Cancel, unsigned long dwFlags, LPCTSTR bstrUrlContext, LPCTSTR bstrUrl)
{
	// TODO: 在此处添加消息处理程序代码
	*Cancel=TRUE;
	m_ctrlIE.Navigate(bstrUrl,0,0,0,0);
}

void CEBussinessDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	//m_pIE->DestroyWindow();
	//delete m_pIE;
}

void CEBussinessDlg::ShowWindowTopmost()
{
	if(m_bLogin)m_pKeySink->StopMaskKeyboard();
	ModifyStyleEx(0,WS_EX_TOPMOST);
	ShowWindow(SW_SHOW);
}

void CEBussinessDlg::HidenWindowMe()
{
	ShowWindow(SW_HIDE);
	if (m_bLogin)m_pKeySink->StartMaskKeyboard();
}

void CEBussinessDlg::AboutUs()
{
	PostMessage(WM_SYSCOMMAND,IDM_ABOUT_US);
}

void CEBussinessDlg::QuitMe()
{
	m_pKeySink->StopMaskKeyboard();
	Shell_NotifyIcon(NIM_DELETE,&m_ncBar);
	PostQuitMessage(0);
}

// void CEBussinessDlg::OnOK()
// {
// 	::SendMessage(m_ctrlIE.GetSafeHwnd(),WM_KEYDOWN,VK_RETURN,0x001C0001);
// 	//::SendMessage(m_ctrlIE.GetSafeHwnd(),WM_KEYDOWN,VK_RETURN,0);
// }

void CEBussinessDlg::OnCancel()
{

}

void CEBussinessDlg::ECreatePopupMenu()
{
	m_EMenu.CreatePopupMenu();
	m_EMenu.AppendMenu(MF_STRING,IDM_SHOWMAIN,_T("显示窗口"));
	m_EMenu.AppendMenu(MF_STRING,IDM_ABOUT_US,_T("关于我们"));
	m_EMenu.AppendMenu(MF_SEPARATOR);
	m_EMenu.AppendMenu(MF_STRING,IDM_QUIT,_T("退出程序"));

	POINT ptMouse;
	GetCursorPos(&ptMouse);
	m_EMenu.TrackPopupMenu(TPM_LEFTALIGN,ptMouse.x,ptMouse.y,this);

	m_EMenu.DestroyMenu();
}


LRESULT CEBussinessDlg::OnNotifyIcon(WPARAM wParam,LPARAM IParam)
{
	switch(IParam)
	{
	case WM_LBUTTONDBLCLK:
		{
			ShowWindowTopmost();
			break;
		}
	case WM_RBUTTONDOWN:
		{
			ECreatePopupMenu();
			break;
		}
	}
	return 0L;
}


void CEBussinessDlg::ScanGunCallBackData(char* pszData,int nNum)
{
	USES_CONVERSION;
	LOGEVEN(_T(">>>%s\n"),A2W(pszData));
	m_strScanGunData = CString(pszData);
	SendMessage(WM_MSG_SCAN_GUN_DATA/*,(WPARAM)pszData,nNum*/);
}

LRESULT CEBussinessDlg::OnScanGunData(WPARAM wParam,LPARAM IParam)
{
	ShowWindowTopmost();

	LPDISPATCH lpDispatch = m_ctrlIE.get_Document(); 
	IHTMLDocument2* pDocument; 
	lpDispatch-> QueryInterface(IID_IHTMLDocument2,   (void**)&pDocument); 
	lpDispatch-> Release();

	if   ( pDocument )
	{ 
		CComQIPtr <IHTMLWindow2> pHTMLWnd; 
		pDocument-> get_parentWindow(&pHTMLWnd); 
		if(   pHTMLWnd != NULL ) 
		{ 
			CString js_str=_T("hello(");
			js_str += m_strScanGunData;
			js_str += _T(");");//这段js代码是禁止弹出一些对话框以及容错的

			LOGEVEN(_T(">>>%s\n"),js_str);

			VARIANT pvarRet;
			pHTMLWnd->execScript(CComBSTR(js_str), CComBSTR("javascript"), &pvarRet);
		} 
	} 

	return 0L;
}

void CEBussinessDlg::SetLoginState(int nState)
{
	m_bLogin = (bool)nState;

	AfxMessageBox(_T("js called me"));
}

void CEBussinessDlg::OnCommandResolve(UINT nID)
{
	switch(nID)
	{
	case IDM_SHOWMAIN:
		{
			ShowWindowTopmost();
			break;
		}
	case IDM_ABOUT_US:
		{
			AboutUs();
			break;
		}
	case IDM_QUIT:
		{
			QuitMe();
			break;
		}
	}
	//return 0L;
}
void CEBussinessDlg::DocumentCompleteExplorerIe(LPDISPATCH pDisp, VARIANT* URL)
{
	// TODO: 在此处添加消息处理程序代码

	//__super::DocumentComplete(pDisp, URL);

	LPDISPATCH lpDispatch = m_ctrlIE.get_Document(); 
	IHTMLDocument2* pDocument; 
	lpDispatch-> QueryInterface(IID_IHTMLDocument2,   (void**)&pDocument); 
	lpDispatch-> Release();

	if (pDocument)
	{
		IHTMLElement*  ipBody;
		pDocument->get_body(&ipBody);
		LPDISPATCH pDispatch = theApp.getDispath();
		HRESULT hr = ipBody->setAttribute(_T("external"), _variant_t(pDispatch, false), 0);
	}  
}
BOOL CEBussinessDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	switch(pMsg->message)
	{
	case WM_KEYDOWN:
		{
			switch(pMsg->wParam)
			{
			case VK_RETURN:
				{
					return ::SendMessage(m_ctrlIE.GetSafeHwnd(),WM_KEYDOWN,VK_RETURN,0x001C0001);
				}
			}
			break;
		}
	}

	return __super::PreTranslateMessage(pMsg);
}
