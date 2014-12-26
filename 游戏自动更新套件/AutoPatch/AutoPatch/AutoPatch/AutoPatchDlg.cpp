// AutoPatchDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "AutoPatch.h"
#include "AutoPatchDlg.h"
#include <afxinet.h>
#include <stdio.h>
#include <Shlwapi.h>
#include <string>
#include <vector>
#include <assert.h>
#include "../common/scriptparser.h"
#include "../common/FilePath.h"
#include "../Common/MapStrToID.h"
#include "webgrab.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//CWebGrab g_grab;
std::vector<CFileDesc> g_LocalFilesDesc;
std::vector<CFileDesc> g_RemoteFilesDesc;
std::vector<CFileDesc> g_BadFilesDesc;
CMapStrToIDMgr g_LocalFilesMgr;
CFtpConnection* g_pFtpConnect = NULL;
BOOL	g_bProcess = FALSE;
BOOL	g_bDownComplete = FALSE;


extern BOOL	g_bFtpDownload;

extern CString g_strURLRoot;
extern CString g_strLocalRoot;

extern CString g_strFtpSite;
extern CString g_strFtpUsername;
extern CString g_strFtpPassword;

extern CString g_strGenExt;

extern CString g_strRemoteListFile;
extern CString g_strDownloadListFile;
extern CString g_strLocalListFile;

extern BOOL LaunchApp();

BOOL AnalyseListFile( const char* pszFilename, std::vector<CFileDesc>& vectorFiles );

CAutoPatchDlg* g_pDlg = NULL;

BOOL GetRemoteFile( CString& strURL, CString& strLocalFile, void* pParam )
{
	CFilePath path;
	path.Split( strLocalFile );
	// �ڵ�ǰ��ϵ��,û����չ�����ļ���Ϊ��Ŀ¼,
	// ǿ���Ե�
	if( !path.GetExt() )
		return FALSE;
	if( !path.MakeDirectory( strLocalFile ) )
		return FALSE;


	CString strBuffer;
	CString strRealURL = strURL+g_strGenExt;
	if( g_bFtpDownload )
	{
		if( !g_pFtpConnect )
		{
			assert( false && "ftp has not connectted" );
			return FALSE;
		}
		if( !g_pFtpConnect->GetFile( strRealURL, strLocalFile, FALSE ) )
			return FALSE;
	}
	else
	{
		CWebGrab grab;
		//if( URLDownloadToFile( NULL, strRealURL, strLocalFile, 0, NULL ) != S_OK )
		if( !grab.GetFile(strRealURL, strBuffer, _T("MotingPlatformAutoPatch"), (CWnd*)pParam ) )
			return FALSE;

		int nSize = strBuffer.GetLength();
		// ����Щ�����,�����0���ȵ��ļ�
		// ��ʱ�ļ����س���
		if( nSize == 0 )
			//return FALSE;
			return TRUE;

		FILE* fp = fopen( strLocalFile.GetBuffer(), "wb" );
		if( !fp )
			return FALSE;
		fwrite( strBuffer.GetBuffer(), nSize-1, 1, fp );
		fclose( fp );
	}

	return TRUE;
}
BOOL AnalyseListFile( const char* pszFilename, std::vector<CFileDesc>& vectorFiles )
{
	CProgressCtrl* pc = &g_pDlg->m_pcDownload;
	pc->SetRange( 0, vectorFiles.size() );
	pc->SetPos( 0 );

	assert( vectorFiles.size() == 0 );
	if( !pszFilename || 
		pszFilename[0] == 0 )
		return FALSE;
	FILE* fp = fopen( pszFilename, "rb" );
	if( !fp )
		return FALSE;

	int nFileCount = 0;//
	fread( &nFileCount, sizeof(int), 1, fp  );
	vectorFiles.reserve( nFileCount );
	vectorFiles.resize( nFileCount );
	fread( &vectorFiles[0], sizeof( CFileDesc ), nFileCount, fp   );

	fclose( fp );
	return TRUE;
}

void Download()
{
	if( GetRemoteFile( g_strRemoteListFile,  g_strDownloadListFile, &g_pDlg->m_State ) )
	{
		g_pDlg->m_State.SetWindowText( _T("�����б��ļ��ɹ�." ) );


		if( AnalyseListFile( g_strDownloadListFile, g_RemoteFilesDesc ) )
		{
			CProgressCtrl* pc = &g_pDlg->m_pcDownload;
			pc->SetRange( 0, g_RemoteFilesDesc.size() );
			pc->SetPos( 0 );

			for( int i = 0; i < g_RemoteFilesDesc.size(); i++ )
			{
				// ����û�ȡ��������,
				if( !g_bProcess )
				{
					//g_pDlg->Printf( "ȡ������." );
					break;
				}


				CFileDesc* remote = &g_RemoteFilesDesc[i];
				//assert( remote->m_dwSize > 0 );
				CFileDesc* local = 
					(CFileDesc*)g_LocalFilesMgr.GetDstByName( remote->m_szFilename );// remote->m_strFilename.c_str() );

				CString src = g_strURLRoot+"/"+remote->m_szFilename;//remote->m_strFilename.c_str();
				CString dst = g_strLocalRoot+"\\"+remote->m_szFilename;//remote->m_strFilename.c_str();

				// ������ļ��ڱ��ش���
				if( local )
				{
					// ������߱���Ϊ��һ���ģ��Ͳ���Ҫ������
					if( stricmp( local->m_szFilename, remote->m_szFilename ) == 0 &&
						//local->m_strFilename	== remote->m_strFilename &&
						local->m_dwSize			== remote->m_dwSize &&
						local->m_dwCheckSum		== remote->m_dwCheckSum &&
						local->m_dwBitSum		== remote->m_dwBitSum )
					{
						// Ϊ�˱������,�ٿ���local���ļ��Ƿ���ڣ��������һ���ļ���С
						CFilePath path;
						if( path.IsExist( dst ) &&
							path.GetFileSize( dst ) == remote->m_dwSize )
						{
							continue;
						}

					}
				}


				BOOL bDownloadSucc = FALSE;
				int const MAX_TRY = 10;
				// �������ɴε�����
				for( int nTry = 0; nTry < MAX_TRY; nTry++ )
				{
					if( GetRemoteFile( src, dst, &g_pDlg->m_State ) )
					{
						bDownloadSucc = TRUE;
						CFilePath path;
						if( path.GetFileSize( dst ) == 0 )
							assert( false );
						break;
					}
				}
				//if( GetRemoteFile( src, dst, &g_pDlg->m_State ) )
				//{
				//	bDownloadSucc = TRUE;
				//	CFilePath path;
				//	if( path.GetFileSize( dst ) == 0 )
				//		assert( false );
				//}

				// ������سɹ�
				if( bDownloadSucc )
				{
					g_pDlg->Printf( "%s ok.", remote->m_szFilename );// remote->m_strFilename.c_str() );
					// ���سɹ��ˣ��޸ı��ص�����
					if( local )
					{
						local->m_dwSize = remote->m_dwSize;
						local->m_dwCheckSum = remote->m_dwCheckSum;
						local->m_dwBitSum = remote->m_dwBitSum;
					}
					else
					{
						g_LocalFilesDesc.push_back( *remote );

					}
				}
				else 
				{
					g_pDlg->Printf( "%s failed.", remote->m_szFilename );//remote->m_strFilename.c_str() );
					g_BadFilesDesc.push_back( *remote );
					g_pDlg->m_lbBadFiles.AddString( remote->m_szFilename );//remote->m_strFilename.c_str() );
				}

				pc->SetPos( i+1 );

			}
			pc->SetPos( g_RemoteFilesDesc.size() );
		}
		else
			g_pDlg->Printf( "���������б��ļ�ʧ��." );

		if( g_bProcess )
		{
			// ֻ��ȫ���ļ����سɹ����ŻḲ�Ǳ����б�
			if( g_BadFilesDesc.size() == 0 )
			{
				//// ��������ص��°�listfile���Ǳ��ص�
				//if( CopyFile( g_strDownloadListFile, g_strLocalListFile, FALSE ) )
				//	g_pDlg->Printf( "�°��б��ļ����ǳɹ�." );
				//else
				//	g_pDlg->Printf( "�°��б��ļ����Ǹ�ʧ��." );

				g_bDownComplete = TRUE;

			}
			else
			{
				g_pDlg->Printf( "��������ʧ�ܵ��ļ�������ʧ��." );
			}

			remove( g_strDownloadListFile );

		}

	}
	else
	{
		g_pDlg->m_State.SetWindowText( _T("�����б��ļ�ʧ��." ) );
	}

	FILE* fp = fopen( g_strLocalListFile, "wb" );
	if( fp )
	{
		int nFileCount = g_LocalFilesDesc.size();
		fwrite( &nFileCount, sizeof(int), 1, fp );
		for( int i = 0; i < g_LocalFilesDesc.size(); i++ )
		{
			CFileDesc* pDesc = &g_LocalFilesDesc[i];
			fwrite( pDesc, sizeof( CFileDesc ), 1, fp );
		}
		fclose( fp );
	}

}
UINT DownloadProcess( LPVOID p )
{
	g_bDownComplete = FALSE;
	// �������
	g_RemoteFilesDesc.clear();
	//g_pDlg->ReBuildLocal();

	CListBox* lb = &g_pDlg->m_lbMsg;
	lb->ResetContent();
	CButton* button = (CButton*)g_pDlg->GetDlgItem( IDC_BUTTON_START );

	g_pDlg->Printf( "[������Ϸ����]" );

	if( g_bFtpDownload )
	{
		CInternetSession sess(_T("������Ϸ����"));
		try
		{
			g_pFtpConnect = sess.GetFtpConnection(
				g_strFtpSite,
				g_strFtpUsername,
				g_strFtpPassword );
			if( g_pFtpConnect )
				Download();
		}
		catch (CInternetException* pEx)
		{
			TCHAR sz[1024];
			pEx->GetErrorMessage(sz, 1024);
			printf("ERROR!  %s\n", sz);
			pEx->Delete();
		}

		if( g_pFtpConnect )
		{
			g_pFtpConnect->Close();	
			delete g_pFtpConnect;
			g_pFtpConnect = NULL;
		}
	}
	else
	{
		Download();
	}


	g_bProcess = FALSE;
	button->SetWindowText( "��ʼ" );
	//g_pDlg->SendMessage( WM_CLOSE, 0, 0 );
	if( g_bDownComplete )
	{
		if( LaunchApp() )
			g_pDlg->SendMessage( WM_CLOSE, 0, 0 );
	}

	return 0;
}

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


// CAutoPatchDlg �Ի���



CAutoPatchDlg::CAutoPatchDlg(CWnd* pParent /*=NULL*/)
: CDialog(CAutoPatchDlg::IDD, pParent) ,_btnOK(IDB_BACK_NORMAL,IDB_BACK_HOVER,IDB_BACK_DOWN)
,_btnExit()
{
	g_pDlg = this;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	//zizi ��ʼ��
	m_dcBkGrnd = NULL;
	m_dwWidth = 0;
	m_dwHeight = 0;
	m_dwFlags = 0;
	//-----------
}

void CAutoPatchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MSG, m_lbMsg);
	DDX_Control(pDX, IDC_PROGRESS_DOWNLOAD, m_pcDownload);
	DDX_Control(pDX, IDC_STATIC_STATE, m_State);
	DDX_Control(pDX, IDC_LIST_BADFILES, m_lbBadFiles);

	DDX_Control(pDX,IDOK,_btnOK);
	DDX_Control(pDX,IDCANCEL,_btnExit);
}

BEGIN_MESSAGE_MAP(CAutoPatchDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_START, OnBnClickedButtonStart)
END_MESSAGE_MAP()


// CAutoPatchDlg ��Ϣ�������

BOOL CAutoPatchDlg::OnInitDialog()
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

	HBITMAP hBmp = (HBITMAP)LoadImage( AfxGetInstanceHandle(), _T("UI\\bg.bmp"),IMAGE_BITMAP, 0, 0,LR_LOADFROMFILE );
	if(hBmp == NULL)
		return false;

	HRGN hRgn = CreateRgnFromFile(hBmp, RGB(255, 0, 0));

	// build memory dc for background
	CDC* dc = GetDC();
	m_dcBkGrnd = CreateCompatibleDC( dc->m_hDC );
	ReleaseDC( dc );
	// select background image
	SelectObject( m_dcBkGrnd, hBmp );
	// set window size the same as image size
	SetWindowPos( NULL, 0, 0, m_dwWidth, m_dwHeight, SWP_NOZORDER | SWP_NOMOVE );
	// assign region to window
	SetWindowRgn( hRgn, FALSE );

	UpdateData(FALSE);
	_btnOK.MoveWindow(30,30,100,40);

	//-----
	ReBuildLocal();
	OnBnClickedButtonStart();
	return TRUE;  // ���������˿ؼ��Ľ��㣬���򷵻� TRUE
}

void CAutoPatchDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CAutoPatchDlg::OnPaint() 
{
	//if (IsIconic())
	//{
	//	CPaintDC dc(this); // ���ڻ��Ƶ��豸������

	//	SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

	//	// ʹͼ���ڹ��������о���
	//	int cxIcon = GetSystemMetrics(SM_CXICON);
	//	int cyIcon = GetSystemMetrics(SM_CYICON);
	//	CRect rect;
	//	GetClientRect(&rect);
	//	int x = (rect.Width() - cxIcon + 1) / 2;
	//	int y = (rect.Height() - cyIcon + 1) / 2;

	//	// ����ͼ��
	//	dc.DrawIcon(x, y, m_hIcon);
	//}
	//else
	//{
	//	CDialog::OnPaint();
	//}




	CPaintDC dc(this); // device context for painting
	if ( m_dcBkGrnd )
		BitBlt( dc.m_hDC, 0, 0, m_dwWidth, m_dwHeight, m_dcBkGrnd, 0, 0, SRCCOPY );
}

BOOL CAutoPatchDlg::OnEraseBkgnd( CDC* pDC )
{
	return BitBlt( pDC->m_hDC, 0, 0, m_dwWidth, m_dwHeight, m_dcBkGrnd, 0, 0, SRCCOPY );
}


//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
HCURSOR CAutoPatchDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CAutoPatchDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnOK();
}

//CString strURL =  _T("http://www.hrgames.com/demo/ZhanGuoPK.rmvb" );
//CString strURL = _T("http://moting.2288.org/moting/filelist.txt");
BOOL CAutoPatchDlg::ReBuildLocal()
{
	// TODO���ڴ���Ӷ���ĳ�ʼ������
	g_LocalFilesDesc.clear();
	//g_LocalFilesMgr.Destroy();
	if( !AnalyseListFile( g_strLocalListFile, g_LocalFilesDesc ) )
		return FALSE;
	for( int i = 0; i < g_LocalFilesDesc.size(); i++ )
	{
		CFileDesc* pDesc = &g_LocalFilesDesc[i];
		g_LocalFilesMgr.Register( pDesc->m_szFilename, pDesc );// pDesc->m_strFilename.c_str(), pDesc );
	}
	return TRUE;
}

void CAutoPatchDlg::OnBnClickedButtonStart()
{
	CButton* button = (CButton*)GetDlgItem( IDC_BUTTON_START );

	if( g_bProcess )
	{
		g_bProcess = FALSE;
		button->SetWindowText( "��ʼ" );
	}
	else
	{
		g_bProcess = TRUE;
		button->SetWindowText( "ȡ��" );
		CWinThread* thd = AfxBeginThread( DownloadProcess, NULL );
	}
}

void CAutoPatchDlg::Printf( const char* pszString, ... )
{
	va_list	va;
	static char data[1024];

	va_start( va, pszString );
	wvsprintf( (char*)data, pszString, va );
	va_end( va );

	CListBox* lb = &m_lbMsg;
	lb->AddString( data );
	int nMaxId = lb->GetCount();
	lb->SetCurSel( nMaxId-1 );
	lb->RedrawWindow();

}
HRGN CAutoPatchDlg::CreateRgnFromFile( HBITMAP hBmp, COLORREF color )
{

	BITMAP bmp = {0};
	GetObject(hBmp, sizeof(BITMAP), &bmp);

	//
	LPBITMAPINFO bi = (LPBITMAPINFO) new BYTE[sizeof(BITMAP) + 108];
	memset( bi, 0, sizeof(BITMAP)+108);

	bi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

	m_dwWidth	= bmp.bmWidth;
	m_dwHeight	= bmp.bmHeight;

	HDC dc = CreateIC("DISPLAY", NULL, NULL, NULL);
	DWORD res = GetDIBits( dc, hBmp, 0, bmp.bmHeight, 0, bi, DIB_RGB_COLORS);
	LPBYTE pBits = new BYTE[bi->bmiHeader.biSizeImage + 4];

	if(bi->bmiHeader.biBitCount == 8)
	{
		LPBITMAPINFO old_bi = bi;
		bi = (LPBITMAPINFO)new char[sizeof(BITMAPINFO) + 255 * sizeof(RGBQUAD)];
		memcpy(bi, old_bi, sizeof(BITMAPINFO));
		delete old_bi;
	}

	BITMAPINFOHEADER& bih = bi->bmiHeader;
	LPDWORD clr_tbl = (LPDWORD)&bi->bmiColors;

	res = GetDIBits(dc, hBmp, 0, bih.biHeight, pBits, bi, DIB_RGB_COLORS);
	DeleteDC(dc);
	BITMAP bm;
	GetObject( hBmp, sizeof(BITMAP), &bm);
	LPBYTE pClr = (LPBYTE)&color;

	BYTE tmp = pClr[0]; pClr[0] = pClr[2]; pClr[2] = tmp;

	if(bih.biBitCount == 16)
	{
		color = ((DWORD)(pClr[0] & 248) >> 3) | 
			((DWORD)(pClr[1] & 252) << 3) |
			((DWORD)(pClr[2] & 248) << 8);

	}


	const DWORD RGNDATAHEADER_SIZE	= sizeof(RGNDATAHEADER);
	const DWORD ADD_RECTS_COUNT		= 40;			// number of rects to be appended
	// to region data buffer

	// BitPerPixel
	BYTE	Bpp = bih.biBitCount >> 3;				// bytes per pixel
	// bytes per line in pBits is DWORD aligned and bmp.bmWidthBytes is WORD aligned
	// so, both of them not
	DWORD m_dwAlignedWidthBytes = (bmp.bmWidthBytes & ~0x3) + (!!(bmp.bmWidthBytes & 0x3) << 2);
	// DIB image is flipped that's why we scan it from the last line
	LPBYTE	pColor = pBits + (bih.biHeight - 1) * m_dwAlignedWidthBytes;
	DWORD	dwLineBackLen = m_dwAlignedWidthBytes + bih.biWidth * Bpp;	// offset of previous scan line
	// (after processing of current)
	DWORD	dwRectsCount = bih.biHeight;			// number of rects in allocated buffer
	INT		i, j;									// current position in mask image
	INT		first = 0;								// left position of current scan line
	// where mask was found
	bool	wasfirst = false;						// set when mask has been found in current scan line
	bool	ismask;									// set when current color is mask color

	// allocate memory for region data
	// region data here is set of regions that are rectangles with height 1 pixel (scan line)
	// that's why first allocation is <bm.biHeight> RECTs - number of scan lines in image
	RGNDATAHEADER* pRgnData = 
		(RGNDATAHEADER*)new BYTE[ RGNDATAHEADER_SIZE + dwRectsCount * sizeof(RECT) ];
	// get pointer to RECT table
	LPRECT pRects = (LPRECT)((LPBYTE)pRgnData + RGNDATAHEADER_SIZE);
	// zero region data header memory (header  part only)
	memset( pRgnData, 0, RGNDATAHEADER_SIZE + dwRectsCount * sizeof(RECT) );
	// fill it by default
	pRgnData->dwSize	= RGNDATAHEADER_SIZE;
	pRgnData->iType		= RDH_RECTANGLES;

	for ( i = 0; i < bih.biHeight; i++ )
	{
		for ( j = 0; j < bih.biWidth; j++ )
		{
			// get color
			switch ( bih.biBitCount )
			{
			case 8:
				ismask = (clr_tbl[ *pColor ] != color);
				break;
			case 16:
				ismask = (*(LPWORD)pColor != (WORD)color);
				break;
			case 24:
				ismask = ((*(LPDWORD)pColor & 0x00ffffff) != color);
				break;
			case 32:
				ismask = (*(LPDWORD)pColor != color);
			}
			// shift pointer to next color
			pColor += Bpp;
			// place part of scan line as RECT region if transparent color found after mask color or
			// mask color found at the end of mask image
			if ( wasfirst )
			{
				if ( !ismask )
				{
					// save current RECT
					pRects[ pRgnData->nCount++ ] = CRect( first, i, j, i + 1 );
					// if buffer full reallocate it with more room
					if ( pRgnData->nCount >= dwRectsCount )
					{
						dwRectsCount += ADD_RECTS_COUNT;
						// allocate new buffer
						LPBYTE pRgnDataNew = new BYTE[ RGNDATAHEADER_SIZE + dwRectsCount * sizeof(RECT) ];
						// copy current region data to it
						memcpy( pRgnDataNew, pRgnData, RGNDATAHEADER_SIZE + pRgnData->nCount * sizeof(RECT) );
						// delte old region data buffer
						delete pRgnData;
						// set pointer to new regiondata buffer to current
						pRgnData = (RGNDATAHEADER*)pRgnDataNew;
						// correct pointer to RECT table
						pRects = (LPRECT)((LPBYTE)pRgnData + RGNDATAHEADER_SIZE);
					}
					wasfirst = false;
				}
			}
			else if ( ismask )		// set wasfirst when mask is found
			{
				first = j;
				wasfirst = true;
			}
		}

		if ( wasfirst && ismask )
		{
			// save current RECT
			pRects[ pRgnData->nCount++ ] = CRect( first, i, j, i + 1 );
			// if buffer full reallocate it with more room
			if ( pRgnData->nCount >= dwRectsCount )
			{
				dwRectsCount += ADD_RECTS_COUNT;
				// allocate new buffer
				LPBYTE pRgnDataNew = new BYTE[ RGNDATAHEADER_SIZE + dwRectsCount * sizeof(RECT) ];
				// copy current region data to it
				memcpy( pRgnDataNew, pRgnData, RGNDATAHEADER_SIZE + pRgnData->nCount * sizeof(RECT) );
				// delte old region data buffer
				delete pRgnData;
				// set pointer to new regiondata buffer to current
				pRgnData = (RGNDATAHEADER*)pRgnDataNew;
				// correct pointer to RECT table
				pRects = (LPRECT)((LPBYTE)pRgnData + RGNDATAHEADER_SIZE);
			}
			wasfirst = false;
		}

		pColor -= dwLineBackLen;
	}
	// release image data
	delete pBits;
	//delete bi;
	// create region
	HRGN hRgn = ExtCreateRegion( NULL, RGNDATAHEADER_SIZE + pRgnData->nCount * sizeof(RECT), (LPRGNDATA)pRgnData );
	// release region data
	delete pRgnData;

	return hRgn;
}

void CAutoPatchDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if ( !(m_dwFlags & DRAGGING) )
	{
		m_pntMouse = point;
		m_dwFlags |= DRAGGING;
		SetCapture();
	}

	CDialog::OnLButtonDown(nFlags, point);
}

void CAutoPatchDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if ( m_dwFlags & DRAGGING )
	{
		m_dwFlags &= ~DRAGGING;
		ReleaseCapture();
	}

	CDialog::OnLButtonUp(nFlags, point);
}

void CAutoPatchDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	if ( m_dwFlags & DRAGGING )
	{
		RECT rect;
		GetWindowRect( &rect );

		rect.left += point.x - m_pntMouse.x;
		rect.top += point.y - m_pntMouse.y;

		SetWindowPos( NULL, rect.left, rect.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE );
	}

	CDialog::OnMouseMove(nFlags, point);
}

/*

*/