// VersionControlDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "VersionControl.h"
#include "VersionControlDlg.h"
#include "../Common/OpenFileDlg.h"
#include "../Common/FilePath.h"
#include <vector>
#include <atlimage.h>
extern BOOL g_bProcess;
extern UINT ProcessAllFiles( LPVOID p );
extern std::vector<CFileDesc> g_files;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static char g_szBrowseDir[MAX_PATH];
// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CVersionControlDlg �Ի���
CVersionControlDlg* g_pDlg = NULL;









CVersionControlDlg::CVersionControlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVersionControlDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	g_pDlg = this;
}

void CVersionControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FILES2, m_lcFiles);
	DDX_Control(pDX, IDC_BUTTON_BROWSE, m_btnBrowse);
	DDX_Control(pDX, IDC_PROGRESS, m_progress);
	DDX_Control(pDX, IDC_EDIT_FILEEXT, m_editGenFileExt);
	DDX_Control(pDX, IDC_EDIT_VERSION, m_editVersion);
}

BEGIN_MESSAGE_MAP(CVersionControlDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnBnClickedButtonBrowse)
	ON_BN_CLICKED(IDC_BUTTON_GENFILELIST, OnBnClickedButtonGenfilelist)
	ON_BN_CLICKED(IDC_BUTTON_GENUPLOADFILES, OnBnClickedButtonGenuploadfiles)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CVersionControlDlg ��Ϣ�������

BOOL CVersionControlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��\������...\���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO���ڴ���Ӷ���ĳ�ʼ������
// 	LVBKIMAGE   lv; 
// 	ZeroMemory(&lv,   sizeof(lv)); 
// 	lv.ulFlags			=   LVBKIF_SOURCE_URL | LVBKIF_STYLE_TILE; 
// 	lv.pszImage			=   "c:\\1.jpg"; 
// 	lv.cchImageMax		= _tcslen(lv.pszImage);
// 	//lv.hbm			=   (HBITMAP)::LoadImage( AfxGetInstanceHandle(), /*"c:\\1.bmp"*/MAKEINTRESOURCE(IDB_BMP_LIST_BK), IMAGE_BITMAP, 0,0, LR_CREATEDIBSECTION );
// 	lv.xOffsetPercent   =   0; 
// 	lv.yOffsetPercent   =   0; 
// 
// 	ListView_SetBkImage(m_lcFiles.GetSafeHwnd(),     &lv);

	//LPTSTR lps=MAKEINTRESOURCE(IDB_BMP_LIST_BK);

	CImage img;
	img.LoadFromResource(AfxGetInstanceHandle(),IDB_BMP_LIST_BK);
	img.Save(TEXT("listbk.bmp"));

	TCHAR szPath[MAX_PATH]={0};
	GetCurrentDirectory(MAX_PATH,szPath);
	if(szPath[_tcslen(szPath)-1] != '\\')szPath[_tcslen(szPath)]='\\';
	_tcsncat(szPath+_tcslen(szPath),TEXT("listbk.bmp"),MAX_PATH);

	m_lcFiles.InsertColumn( 0, "�ļ���", LVCFMT_LEFT, 120 );
	m_lcFiles.InsertColumn( 1, "�ļ���С", LVCFMT_LEFT, 80 );
	m_lcFiles.InsertColumn( 2, "�ֽں�", LVCFMT_LEFT, 80 );
	m_lcFiles.InsertColumn( 3, "λ�ܺ�", LVCFMT_LEFT, 80 );
	m_lcFiles.InsertColumn( 4, "CRC", LVCFMT_LEFT, 80 );
	m_lcFiles.InsertColumn( 5, "�Ƿ�ZIP", LVCFMT_LEFT, 80 );
	m_lcFiles.InsertColumn( 6, "ZIP�ļ���", LVCFMT_LEFT, 120 );
	m_lcFiles.InsertColumn( 7, "ZIP�ļ�·��", LVCFMT_LEFT, 120 );
	m_lcFiles.InsertColumn( 8, "�ļ�·��", LVCFMT_LEFT,120 );

	m_lcFiles.SetExtendedStyle(LVCFMT_IMAGE|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_GRIDLINES|LVS_EX_HEADERDRAGDROP);
	//m_lcFiles.SetExtendedStyle(m_lcFiles.GetExtendedStyle()|LVS_EX_FULLROWSELECT| LVS_EX_FLATSB|LVS_EX_HEADERDRAGDROP);

	m_lcFiles.SetTextBkColor(CLR_NONE);

 	LVBKIMAGE bki;
 
	static CBitmap bmp;
	bmp.LoadBitmap(IDB_BMP_LIST_BK);
 	if (m_lcFiles.GetBkImage(&bki) && (bki.ulFlags ==
 		LVBKIF_SOURCE_NONE))
 	{
		m_lcFiles.SetBkImage(szPath ,1);
		
		//m_lcFiles.SetBkImage(MAKEINTRESOURCE(IDB_BMP_LIST_BK),1);
		//m_lcFiles.SetBkImage( h,0);
 		// Use your own Image Address Here
 	}

	m_editGenFileExt.SetWindowText( ".html" );
	
	return TRUE;  // ���������˿ؼ��Ľ��㣬���򷵻� TRUE
}

void CVersionControlDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CVersionControlDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
HCURSOR CVersionControlDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CVersionControlDlg::OnBnClickedButtonBrowse()
{
	if( g_bProcess )
	{
		g_bProcess = FALSE;
		m_btnBrowse.SetWindowText( "��Ŀ¼" );
		return;
	}
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	
	if( BrowseBox( NULL, g_szBrowseDir, MAX_PATH )  )
	{
		CString sDir(g_szBrowseDir);
		sDir+=_T("filelist_000.txt");
		::DeleteFile(sDir);
		g_bProcess = TRUE;
		m_btnBrowse.SetWindowText( "ֹͣ" );
		g_pDlg->SetWindowText( g_szBrowseDir );

		//g_szBrowseDir[strlen(g_szBrowseDir)-1] = 0;
		CWinThread* thd = AfxBeginThread( ProcessAllFiles , g_szBrowseDir );
		HANDLE hThd=thd->m_hThread;

		//for( int i = 0; i < g_files.size(); i++ )
		//{
		//	int nItemCount = m_lcFiles.GetItemCount();
		//	m_lcFiles.InsertItem( nItemCount, g_files[i].m_strFilename.c_str() );
		//	m_lcFiles.SetItemText( nItemCount, 1, "0" );	
		//}

	}

}

void CVersionControlDlg::OnBnClickedButtonGenfilelist()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	char msg[1024];
	char s[MAX_PATH];
	CString strVersion;
	GetDlgItem( IDC_EDIT_VERSION )->GetWindowText( strVersion );
	sprintf( s, "%sfilelist_%03ld.txt", g_szBrowseDir, atoi( strVersion )  );
	
	CFileDescMgr mgr;
	if( !mgr.CreateFileList( s, g_files ) )
	{
		sprintf( msg, "�б��ļ�%s����ʧ��", s );
		AfxMessageBox( msg );
		return;
	}
	sprintf( msg, "�б��ļ�%s�����ɹ�", s );
	AfxMessageBox( msg );
	
}

void CVersionControlDlg::OnBnClickedButtonGenuploadfiles()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	char szGenDir[MAX_PATH];
	if( BrowseBox( NULL, szGenDir, MAX_PATH )  )
	{
		g_pDlg->m_lcFiles.DeleteAllItems();
		
		
		char szCopyFilename[MAX_PATH];
		char szSrcFilename[MAX_PATH];
		char szGenFileExt[MAX_PATH];
		m_editGenFileExt.GetWindowText( szGenFileExt, MAX_PATH );

		for( int i = 0; i < g_files.size(); i++ )
		{
			CFileDesc* pDesc = &g_files[i];
			int nItemCount = m_lcFiles.GetItemCount();
			
			sprintf( szSrcFilename, "%s%s", 
				g_szBrowseDir, 
				//pDesc->m_strFilename.c_str() );
				pDesc->m_szFilename );
			sprintf( szCopyFilename, "%s%s%s", 
				szGenDir, 
				//pDesc->m_strFilename.c_str(),
				pDesc->m_szFilename,
				szGenFileExt );
			CFilePath path;
			char s[1024];
			if( !path.MakeDirectory( szCopyFilename ) )
			{
				sprintf( s, "%s make director failed", szCopyFilename );
				m_lcFiles.InsertItem( nItemCount, s );
				continue;
			}
			if(	CopyFile( szSrcFilename, szCopyFilename, FALSE ) )
			{
				sprintf( s, "%s copy ok", szCopyFilename );
				m_lcFiles.InsertItem( nItemCount, s );
			}
			else
			{
				sprintf( s, "%s copy failed", szCopyFilename );
				m_lcFiles.InsertItem( nItemCount, s );
			}

		}
	}

}

HBRUSH CVersionControlDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �ڴ˸��� DC ���κ�����

	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
// 	if (pWnd->GetSafeHwnd()==m_lcFiles.GetSafeHwnd())
// 	{
// 		HBITMAP h=LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_BMP_LIST_BK));
// 		static HBRUSH br=::CreatePatternBrush(h);
// 		return br;
// 		
// 		//return (HBRUSH)::GetStockObject(NULL_BRUSH);
// 		 
// 	}
	return hbr;
}
