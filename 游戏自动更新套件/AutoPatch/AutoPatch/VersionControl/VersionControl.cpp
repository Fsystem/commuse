// VersionControl.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "VersionControl.h"
#include "VersionControlDlg.h"
#include "../Common/FilePath.h"
#include "MarkupMSXML.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CRITICAL_SECTION cs;
std::vector<CFileDesc> g_files;
BOOL g_bProcess = FALSE;
extern CVersionControlDlg* g_pDlg;
CMarkupMSXML               g_LoadFileXml;


void AddFile( CFileDesc* pDesc )
{
	//EnterCriticalSection(&cs);
	int nItemCount = g_pDlg->m_lcFiles.GetItemCount();
	g_pDlg->m_lcFiles.InsertItem( nItemCount, pDesc->m_szFilename );//pDesc->m_strFilename.c_str() );

	char s[256];
	sprintf( s, "%ld", pDesc->m_dwSize );
	g_pDlg->m_lcFiles.SetItemText( nItemCount, 1, s );
	sprintf( s, "%ld", pDesc->m_dwCheckSum );
	g_pDlg->m_lcFiles.SetItemText( nItemCount, 2, s );
	sprintf( s, "%ld", pDesc->m_dwBitSum );
	g_pDlg->m_lcFiles.SetItemText( nItemCount, 3, s );
	sprintf( s, "%s", pDesc->m_szCRC );
	g_pDlg->m_lcFiles.SetItemText( nItemCount, 4, s );
	sprintf( s, "%ld", pDesc->m_nIsZip);
	g_pDlg->m_lcFiles.SetItemText( nItemCount, 5, s );
	sprintf( s, "%s", pDesc->m_szZipFileName );
	g_pDlg->m_lcFiles.SetItemText( nItemCount, 6, s );
	sprintf( s, "%s", pDesc->m_szZipFilePath);
	g_pDlg->m_lcFiles.SetItemText( nItemCount, 7, s );
	sprintf( s, "%s", pDesc->m_szFilePath);
	g_pDlg->m_lcFiles.SetItemText( nItemCount, 8, s );

	//LeaveCriticalSection(&cs);

}


int Recurse( const char* pszRoot, const char* pszDir, std::vector<CFileDesc>* pFiles )
{
	if( !g_bProcess )
		return 0;
	int nFileCount = 0;
	
	CFileFind finder;

	// build a string with wildcards
	CString strWildcard(pszDir);
	strWildcard += _T("\\*.*");

	// start working for files
	BOOL bWorking = finder.FindFile(strWildcard);

	std::vector<CString> vectorDir;
	if(!g_LoadFileXml.Load("XML/DownLoad.xml"))
	{
		ASSERT(false);
		return 0;
	}
	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		// skip . and .. files; otherwise, we'd
		// recur infinitely!

		if (finder.IsDots())
			continue;

		// if it's a directory, recursively search it

		if (finder.IsDirectory())
		{
			CString str = finder.GetFilePath();
			vectorDir.push_back( str );
		}
		else if( pFiles )
		{
			OutputDebugString("文件进来了\n");
			CString str = finder.GetFilePath();
			char* pszFilename = str.GetBuffer();
			int nLength = strlen( pszFilename );
			int nSkipLength = strlen( pszRoot );
			CFileDesc desc;
			//if( GetFileDesc( str, &desc ) )
			if( desc.Create( str, &g_bProcess ) )
			{	
				//desc.m_strFilename = &pszFilename[nSkipLength];
				strcpy( desc.m_szFilename, &pszFilename[nSkipLength] );
				while ( g_LoadFileXml.FindChildElem("file") )
				{
					g_LoadFileXml.IntoElem();
					CString s1=g_LoadFileXml.GetAttrib("name");
					if (strcmp(s1.GetBuffer(),desc.m_szFilename)==0)
					{
						g_LoadFileXml.FindChildElem( "CRC" );
						CString s2 = g_LoadFileXml.GetChildData();
						strcpy(desc.m_szCRC,s2.GetBuffer());
						s2.ReleaseBuffer();

						g_LoadFileXml.FindChildElem( "ZIP" );
						CString s3 = g_LoadFileXml.GetChildData();
						desc.m_nIsZip = atoi(s3.GetBuffer());
						s2.ReleaseBuffer();

						g_LoadFileXml.FindChildElem( "ZIPFileName" );
						CString s4= g_LoadFileXml.GetChildData();
						strcpy(desc.m_szZipFileName,s4.GetBuffer());
						s3.ReleaseBuffer();

						g_LoadFileXml.FindChildElem( "ZIPFilePath" );
						CString s5 = g_LoadFileXml.GetChildData();
						strcpy(desc.m_szZipFilePath,s5.GetBuffer());
						s4.ReleaseBuffer();

						g_LoadFileXml.FindChildElem( "filePath" );
						CString s6 = g_LoadFileXml.GetChildData();
						strcpy(desc.m_szFilePath,s6.GetBuffer());
						s5.ReleaseBuffer();
					}
					s1.ReleaseBuffer();
					g_LoadFileXml.OutOfElem();
				}
				pFiles->push_back(desc);
				AddFile( &desc );
				g_LoadFileXml.ResetPos();
			}
			g_pDlg->m_progress.SetPos( g_pDlg->m_progress.GetPos()+1 );
		}
		nFileCount++;

	}
	for( int i = 0; i < vectorDir.size(); i++ )
	{
		nFileCount += Recurse( pszRoot, vectorDir[i], pFiles );
	}

	finder.Close();
	
	return nFileCount;
}

UINT ProcessAllFiles( LPVOID p )
{	
	OutputDebugString("线程函数开始\n");
	g_files.clear();
	g_pDlg->m_lcFiles.DeleteAllItems();
	char* pszDir = (char*)p;

	g_pDlg->m_progress.SetPos( 0 );
	int nTotalFileCount = Recurse( pszDir, pszDir, NULL );
	g_pDlg->m_progress.SetRange( 0, nTotalFileCount );
	

	
	Recurse( pszDir, pszDir, &g_files );
	g_pDlg->m_progress.SetPos( nTotalFileCount );

	g_pDlg->m_btnBrowse.SetWindowText( "打开目录" );
	g_bProcess = FALSE;
	OutputDebugString("线程函数结束\n");
	return 0;
}

// CVersionControlApp

BEGIN_MESSAGE_MAP(CVersionControlApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CVersionControlApp 构造

CVersionControlApp::CVersionControlApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CVersionControlApp 对象

CVersionControlApp theApp;


// CVersionControlApp 初始化

BOOL CVersionControlApp::InitInstance()
{
	InitializeCriticalSection(&cs);
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControls()。否则，将无法创建窗口。
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	CoInitialize(NULL);

	CVersionControlDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO：在此放置处理何时用“确定”来关闭
		//对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO：在此放置处理何时用“取消”来关闭
		//对话框的代码
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	// 而不是启动应用程序的消息泵。
	return FALSE;
}

int CVersionControlApp::ExitInstance()
{
	// TODO: 在此添加专用代码和/或调用基类
	CoUninitialize();
	DeleteCriticalSection(&cs);
	return CWinApp::ExitInstance();
}
