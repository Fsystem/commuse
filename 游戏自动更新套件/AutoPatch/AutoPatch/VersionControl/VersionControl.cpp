// VersionControl.cpp : ����Ӧ�ó��������Ϊ��
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
			OutputDebugString("�ļ�������\n");
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
	OutputDebugString("�̺߳�����ʼ\n");
	g_files.clear();
	g_pDlg->m_lcFiles.DeleteAllItems();
	char* pszDir = (char*)p;

	g_pDlg->m_progress.SetPos( 0 );
	int nTotalFileCount = Recurse( pszDir, pszDir, NULL );
	g_pDlg->m_progress.SetRange( 0, nTotalFileCount );
	

	
	Recurse( pszDir, pszDir, &g_files );
	g_pDlg->m_progress.SetPos( nTotalFileCount );

	g_pDlg->m_btnBrowse.SetWindowText( "��Ŀ¼" );
	g_bProcess = FALSE;
	OutputDebugString("�̺߳�������\n");
	return 0;
}

// CVersionControlApp

BEGIN_MESSAGE_MAP(CVersionControlApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CVersionControlApp ����

CVersionControlApp::CVersionControlApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CVersionControlApp ����

CVersionControlApp theApp;


// CVersionControlApp ��ʼ��

BOOL CVersionControlApp::InitInstance()
{
	InitializeCriticalSection(&cs);
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControls()�����򣬽��޷��������ڡ�
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	CoInitialize(NULL);

	CVersionControlDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO���ڴ˷��ô����ʱ�á�ȷ�������ر�
		//�Ի���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO���ڴ˷��ô����ʱ�á�ȡ�������ر�
		//�Ի���Ĵ���
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	// ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}

int CVersionControlApp::ExitInstance()
{
	// TODO: �ڴ����ר�ô����/����û���
	CoUninitialize();
	DeleteCriticalSection(&cs);
	return CWinApp::ExitInstance();
}
