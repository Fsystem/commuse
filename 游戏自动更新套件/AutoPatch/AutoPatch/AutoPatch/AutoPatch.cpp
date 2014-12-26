// AutoPatch.cpp : 定义应用程序的类行为。
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

// 是否ftp下载，否的话就是http下载，
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
	
	// 得到Windows目录
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


// CAutoPatchApp 构造

CAutoPatchApp::CAutoPatchApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CAutoPatchApp 对象

CAutoPatchApp theApp;


// CAutoPatchApp 初始化

BOOL CAutoPatchApp::InitInstance()
{

	char szPath[MAX_PATH];
	if( GetModuleFileName( NULL, szPath, MAX_PATH ) == 0 )
		return FALSE;
	// exe完整路径
	g_strExeFilename = szPath;

	char szCurrentDir[MAX_PATH];
	GetCurrentDirectory( MAX_PATH, szCurrentDir );

	// 默认目录是当前目录
	g_strLocalRoot = szCurrentDir;


	LoadCfg();


	if( g_bNeedClone )
	{
		STARTUPINFO AppSI;
		GetStartupInfo( &AppSI );
		// 当前exe已经是clone版本，
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

			// 临时变量
			PROCESS_INFORMATION pi;
			STARTUPINFO CloneSI = {sizeof(CloneSI)};

			CloneSI.dwX |= eCloneFile;

			CString strCommand( szCloneFilename );
			//strCommand += " "+strPassword;

			char* ppp = strCommand.GetBuffer();

			// 得到Windows目录
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




	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControls()。否则，将无法创建窗口。
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();


	CAutoPatchDlg dlg;
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
