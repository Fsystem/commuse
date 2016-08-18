
// IETestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IETest.h"
#include "IETestDlg.h"
#include "afxdialogex.h"
#include <MsHTML.h>
#include "MyControlSite.h"

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


// CIETestDlg 对话框




CIETestDlg::CIETestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CIETestDlg::IDD, pParent)
	, m_sUrl(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CIETestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EXPLORER1, m_ieCtrl);
	DDX_Text(pDX, IDC_EDIT1, m_sUrl);
	DDX_Control(pDX, IDOK, m_BnAccess);
	DDX_Control(pDX, IDC_EDIT1, m_edUrl);
	DDX_Control(pDX, IDC_BUTTON1, m_bnBack);
}

BEGIN_MESSAGE_MAP(CIETestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CIETestDlg::OnBnClickedOk)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON1, &CIETestDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CIETestDlg 消息处理程序

BOOL CIETestDlg::OnInitDialog()
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
	m_sUrl = _T("http://go.kaishiwa.com/t/kaishiwa_1.php");
	UpdateData(FALSE);
	OnBnClickedOk();

	CString title;
	CString pid;
	GetWindowText(title);
	title += _itot(GetCurrentProcessId(),pid.GetBuffer(10),10);
	pid.ReleaseBuffer();
	SetWindowText(title);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CIETestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CIETestDlg::OnPaint()
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
HCURSOR CIETestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CIETestDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//CDialogEx::OnOK();
	UpdateData(TRUE);
	//VARIANT vUrl;
	//vUrl.vt = VT_BSTR;
	//vUrl.bstrVal = SysAllocString(m_sUrl);
	m_ieCtrl.Navigate(m_sUrl,NULL,NULL,NULL,NULL);
	//SysFreeString(vUrl.bstrVal);
}


void CIETestDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	int nBnW = 50,nBnH=24,nUrlW = cx-nBnW-15,nUrlH=24,nBnBackW = 40,nBnBackH = 24;

	::SetWindowPos(m_BnAccess.GetSafeHwnd(),NULL,cx-5-nBnW,5,nBnW,nBnH,SWP_NOZORDER);
	::SetWindowPos(m_edUrl.GetSafeHwnd(),NULL,5+nBnBackW,5,cx-5-15-nBnW-nBnBackW,nUrlH,SWP_NOZORDER);
	::SetWindowPos(m_bnBack.GetSafeHwnd(),NULL,5,5,nBnBackW,nBnBackH,SWP_NOZORDER);

	::SetWindowPos(m_ieCtrl.GetSafeHwnd(),HWND_TOP,5,32,cx-10,cy-60>0?cy-60:0,SWP_NOZORDER);
}
BEGIN_EVENTSINK_MAP(CIETestDlg, CDialogEx)
	ON_EVENT(CIETestDlg, IDC_EXPLORER1, 273, CIETestDlg::NewWindow3Explorer1, VTS_PDISPATCH VTS_PBOOL VTS_UI4 VTS_BSTR VTS_BSTR)
//	ON_EVENT(CIETestDlg, IDC_EXPLORER1, 252, CIETestDlg::NavigateComplete2Explorer1, VTS_DISPATCH VTS_PVARIANT)
END_EVENTSINK_MAP()


void CIETestDlg::NewWindow3Explorer1(LPDISPATCH* ppDisp, BOOL* Cancel, unsigned long dwFlags, LPCTSTR bstrUrlContext, LPCTSTR bstrUrl)
{
	// TODO: 在此处添加消息处理程序代码
	*Cancel = TRUE;
	CString strUrl;
	strUrl = bstrUrl;
	// 网页导航,新的对话框中的webbrowser控件!  
	VARIANT vInfo;
	VariantInit(&vInfo);
	m_ieCtrl.Navigate(bstrUrl, &vInfo, &vInfo, &vInfo, &vInfo);
}


void CIETestDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ieCtrl.GoBack();
}

BOOL CIETestDlg::CreateControlSite(COleControlContainer* pContainer,   
	COleControlSite** ppSite, UINT  nID , REFCLSID  clsid )  
{  
	if(ppSite == NULL)  
	{  
		ASSERT(FALSE);  
		return FALSE;  
	}  

	CMyControlSite *pBrowserSite =   
		new CMyControlSite (pContainer);//  
	if (!pBrowserSite)  
		return FALSE;  

	*ppSite = pBrowserSite;  
	return TRUE;  
}  


//void CIETestDlg::NavigateComplete2Explorer1(LPDISPATCH pDisp, VARIANT* URL)
//{
//	// TODO: 在此处添加消息处理程序代码
//	CComPtr<IDispatch>   spDisp   =   m_ieCtrl.get_Document(); 
//
//	if(spDisp   !=   NULL) 
//
//	{ 
//
//		CComPtr<IHTMLDocument2> doc;
//
//		spDisp->QueryInterface(IID_IHTMLDocument2, reinterpret_cast<void**>(&doc));
//
//		if(doc != NULL)
//
//		{   
//
//			IHTMLWindow2 * pIhtmlwindow2 = NULL;
//
//			doc->get_parentWindow(&pIhtmlwindow2);
//
//			if(pIhtmlwindow2 != NULL)
//
//			{
//
//				//屏蔽javascript脚本错误的javascript脚本
//
//				CString strJavaScriptCode = _T("function fnOnError(msg,url,lineno){alert('script error:\\n\\nURL:'+url+'\\n\\nMSG:'+msg +'\\n\\nLine:'+lineno);return true;}window.onerror=fnOnError;");
//
//				BSTR bstrScript = strJavaScriptCode.AllocSysString();
//
//				CString strLanguage("JavaScript");
//
//				BSTR bstrLanguage = strLanguage.AllocSysString();
//
//				long lTime = 1 * 1000;
//
//				long lTimeID = 0;
//
//				VARIANT varLanguage;
//
//				varLanguage.vt = VT_BSTR;
//
//				varLanguage.bstrVal = bstrLanguage;
//
//				VARIANT pRet;
//
//				//把window.onerror函数插入入当前页面中去
//
//				pIhtmlwindow2->execScript(bstrScript, bstrLanguage, &pRet);
//
//				::SysFreeString(bstrScript);
//
//				::SysFreeString(bstrLanguage);
//
//
//
//				pIhtmlwindow2->Release();
//
//			}
//
//		}
//
//	}
//}
