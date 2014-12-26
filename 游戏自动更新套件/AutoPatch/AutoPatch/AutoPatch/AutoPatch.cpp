// AutoPatch.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "AutoPatch.h"
#include "AutoPatchDlg.h"
#include <assert.h>
#include "../common/FilePath.h"
#include "../common/ScriptParser.h"
#include "../Common/CfgFileLoader.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif



CString g_strExeFilename;
BOOL	g_bIsClone = FALSE;

// �Ƿ�ftp���أ���Ļ�����http���أ�
BOOL	g_bFtpDownload = FALSE;//by seven

CString g_strURLRoot;// = _T("http://192.168.1.30/update/");

CString g_strFtpSite;
CString g_strFtpUsername;
CString g_strFtpPassword;

CString g_strLocalRoot;// = _T("E://ftpdownload/");

CString g_strRemoteListFile;
CString g_strDownloadListFile;
CString g_strLocalListFile;

CString g_strGenExt;

CString g_strLaunchExe;

BOOL	g_bClone = FALSE;


BOOL	g_bNeedClone = FALSE;

BOOL LoadCfg()
{
	CCfgFileLoader loader;
	if( !loader.LoadFromFile( "AutoPatch.ini" ) )
		return FALSE;


	if( !loader.OpenSession( "main" ) )
		return FALSE;

	g_bFtpDownload = loader.GetIntValue( "FtpDownload" );
	
	if( g_bFtpDownload )
	{
		if( !loader.OpenSession( "Ftp" ) )
			return FALSE;
		g_strURLRoot = loader.GetStringValue( "URLRoot" );
		g_strFtpSite = loader.GetStringValue( "FtpSite" );
		g_strFtpUsername = loader.GetStringValue( "FtpUsername" );
		g_strFtpPassword = loader.GetStringValue( "FtpPassword" );
		g_strLocalListFile = g_strLocalRoot+"\\"+loader.GetStringValue( "LocalListFile" );
		g_strRemoteListFile = g_strURLRoot+loader.GetStringValue( "RemoteListFile" );
		g_strDownloadListFile = g_strLocalRoot+"\\"+loader.GetStringValue( "DownloadListFile" );
	}
	else
	{
		if( !loader.OpenSession( "Http" ) )
			return FALSE;
		g_strURLRoot = loader.GetStringValue( "URLRoot" );
		g_strLocalListFile = g_strLocalRoot+"\\"+loader.GetStringValue( "LocalListFile" );
		g_strRemoteListFile = g_strURLRoot+"/"+loader.GetStringValue( "RemoteListFile" );
		//g_strRemoteListFile.Format("%s//%s",g_strURLRoot,strTmp);
		g_strDownloadListFile = g_strLocalRoot+"\\"+loader.GetStringValue( "DownloadListFile" );
	}
	

	loader.OpenSession( "Loader" );
	g_strLaunchExe = loader.GetStringValue( "LoadExe" );


	return TRUE;
}

BOOL LaunchApp()
{
	PROCESS_INFORMATION pi;
	STARTUPINFO si = {sizeof(si)};
	
	// �õ�WindowsĿ¼
	BOOL ret = CreateProcess(
		NULL,
		g_strLaunchExe.GetBuffer(),  
		NULL, 
		NULL, 
		FALSE, 
		0, 
		NULL, 
		NULL, 
		&si,
		&pi );
	if(!ret) 
	{
		assert( false && "cannot lunch app" );
		return FALSE;
	}
	else
	{
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}
	return TRUE;
}
// CAutoPatchApp

BEGIN_MESSAGE_MAP(CAutoPatchApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CAutoPatchApp ����

CAutoPatchApp::CAutoPatchApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CAutoPatchApp ����

CAutoPatchApp theApp;


// CAutoPatchApp ��ʼ��

BOOL CAutoPatchApp::InitInstance()
{

	char szPath[MAX_PATH];
	if( GetModuleFileName( NULL, szPath, MAX_PATH ) == 0 )
		return FALSE;
	// exe����·��
	g_strExeFilename = szPath;

	char szCurrentDir[MAX_PATH];
	GetCurrentDirectory( MAX_PATH, szCurrentDir );

	// Ĭ��Ŀ¼�ǵ�ǰĿ¼
	g_strLocalRoot = szCurrentDir;


	LoadCfg();


	if( g_bNeedClone )
	{
		STARTUPINFO AppSI;
		GetStartupInfo( &AppSI );
		// ��ǰexe�Ѿ���clone�汾��
		if( AppSI.dwX & eCloneFile )
		{

		}
		else
		{
			char szCloneFilename[MAX_PATH];
			strcpy( szCloneFilename, g_strExeFilename );
			szCloneFilename[strlen(szCloneFilename)-4] = 0;
			strcat( szCloneFilename, "_Clone.exe" );
			CopyFile( g_strExeFilename, szCloneFilename, FALSE );

			// ��ʱ����
			PROCESS_INFORMATION pi;
			STARTUPINFO CloneSI = {sizeof(CloneSI)};

			CloneSI.dwX |= eCloneFile;

			CString strCommand( szCloneFilename );
			//strCommand += " "+strPassword;

			char* ppp = strCommand.GetBuffer();

			// �õ�WindowsĿ¼
			BOOL ret = CreateProcess(
				NULL,
				strCommand.GetBuffer(),
				//NULL,
				//szCloneFilename,  
				//szCommandLine,
				
				//NULL,
				NULL, 
				NULL, 
				FALSE, 
				0, 
				NULL, 
				NULL, 
				&CloneSI,
				&pi );
			if(!ret) 
			{
				assert( false && "cannot run clone" );
				return FALSE;
			}
			else
			{
				CloseHandle(pi.hThread);
				CloseHandle(pi.hProcess);
			}
			return TRUE;
		}

	}




	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControls()�����򣬽��޷��������ڡ�
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();


	CAutoPatchDlg dlg;
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
