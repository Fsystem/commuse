#include "StdAfx.h"
#include "WinServer.h"


CWinServer::CWinServer(void)
{
	m_bDebug = FALSE;
}


CWinServer::~CWinServer(void)
{
}

//CWinServer* CWinServer::MyCreateServer( )
//{
//	static	CWinServer * only_one =	NULL;
//
//	if (only_one)
//	{
//		return only_one;
//	}
//
//	only_one = new CWinServer;
//
//	return only_one;
//}


BOOL	CWinServer::Init()
{
	char	buffer[MAX_PATH];
	char	path[MAX_PATH];

	memset(path,0,sizeof(path));
	memset(buffer,0,sizeof(buffer));

	GetModuleFileNameA(NULL,path,MAX_PATH);

	char	*lpstr = strrchr(path,'\\');
	if (lpstr == NULL)
	{
		return FALSE;
	}

	*lpstr = '\0';
	strcat_s(path,MAX_PATH,"\\server.ini");


	GetPrivateProfileStringA("config","app_name","完成端口网络模型",buffer,MAX_PATH,path);
	m_sz_appname = buffer;

	GetPrivateProfileStringA("config","server_name","OneServer",buffer,MAX_PATH,path);
	m_sz_service_name = buffer;

	GetPrivateProfileStringA("config","display_name","IOCP One SERVER",buffer,MAX_PATH,path);
	m_sz_serviced_displayname = buffer;

	return TRUE;

}



BOOL CWinServer::StartRun()
{
	Init();

	m_server_interface->GetServerInfo(m_sz_appname,m_sz_service_name,m_sz_serviced_displayname);

	char m_server_name[MAX_PATH];
	memset(m_server_name,0,sizeof(m_server_name));
	strcpy_s(m_server_name,MAX_PATH,m_sz_service_name.c_str());

	SERVICE_TABLE_ENTRYA dispatchTable[] =
	{
		{ m_server_name, (LPSERVICE_MAIN_FUNCTIONA)service_main },
		{ NULL, NULL }
	};

	if ( (__argc > 1) &&
		((__argv[1][0] == '-') || (__argv[1][0] == '/')) )
	{
		if ( _stricmp("install", __argv[1]+1 ) == 0 )
		{
			CmdInstallService();
		}
		else if ( _stricmp("remove", __argv[1]+1 ) == 0 )
		{
			CmdRemoveService();
		}
		else if ( _stricmp("debug", __argv[1]+1 ) == 0 )
		{
			m_bDebug = TRUE;
			CmdDebugService(__argc, __argv);
		}
		else
		{

		}
		ExitProcess(0);
		return TRUE;
	}

	if (!StartServiceCtrlDispatcherA(dispatchTable))
		AddToMessageLog("StartServiceCtrlDispatcher failed.",GetLastError());

	return TRUE;
}



void CWinServer::CmdRemoveService()
{
	SC_HANDLE			schService;
	SC_HANDLE			schSCManager;
	SERVICE_STATUS		ssStatus;

	schSCManager = OpenSCManagerA(
		NULL,                   // machine (NULL == local)
		NULL,                   // database (NULL == default)
		SC_MANAGER_ALL_ACCESS   // access required
		);
	if ( schSCManager )
	{
		schService = OpenServiceA(schSCManager, m_sz_service_name.c_str() , SERVICE_ALL_ACCESS);

		if (schService)
		{
			// try to stop the service
			if ( ControlService( schService, SERVICE_CONTROL_STOP, &ssStatus ) )
			{
				printf("Stopping %s.",  m_sz_serviced_displayname.c_str() );
				Sleep( 1000 );

				while( QueryServiceStatus( schService, &ssStatus ) )
				{
					if ( ssStatus.dwCurrentState == SERVICE_STOP_PENDING )
					{
						printf(".");
						Sleep( 1000 );
					}
					else
						break;
				}

				if ( ssStatus.dwCurrentState == SERVICE_STOPPED )
					printf("\n%s stopped.\n",  m_sz_serviced_displayname.c_str() );
				else
					printf("\n%s failed to stop.\n",  m_sz_serviced_displayname.c_str());

			}

			// now remove the service
			if( DeleteService(schService) )
				printf("%s removed.\n",m_sz_serviced_displayname.c_str()); 

			CloseServiceHandle(schService);
		}
		CloseServiceHandle(schSCManager);
	} 
}


VOID WINAPI CWinServer::service_ctrl(DWORD dwCtrlCode)
{
	// Handle the requested control code.
	//
	switch(dwCtrlCode)
	{
		// Stop the service.
		//
		// SERVICE_STOP_PENDING should be reported before
		// setting the Stop Event - hServerStopEvent - in
		// ServiceStop().  This avoids a race condition
		// which may result in a 1053 - The Service did not respond...
		// error.
	case SERVICE_CONTROL_STOP:
		CWinServer::Instance().ReportStatusToSCMgr(SERVICE_STOP_PENDING);
		CWinServer::Instance().m_server_interface->ServiceStop();
		return;

		// Update the service status.
		//
	case SERVICE_CONTROL_INTERROGATE:
		break;

		// invalid control code
		//
	default:
		break;

	}

	//MyCreateServer()->ReportStatusToSCMgr(SERVICE_RUNNING);
}

void CWinServer::SetServiceStatusHandle( SERVICE_STATUS_HANDLE Handle )
{
	m_StatusHandle = Handle;
}

std::string CWinServer::GetAppName()
{
	return	m_sz_appname;
}


void WINAPI CWinServer::service_main(DWORD dwArgc, LPSTR *lpszArgv)
{ 
	SERVICE_STATUS_HANDLE sshStatusHandle = RegisterServiceCtrlHandlerA(Instance().GetAppName().c_str(), service_ctrl);

	if (!sshStatusHandle)
	{
		DWORD dwErr = GetLastError();
		goto cleanup;
	}

	Instance().SetServiceStatusHandle(sshStatusHandle);
	  
	// report the status to the service control manager.
	//
	if ( !Instance().ReportStatusToSCMgr(SERVICE_START_PENDING) )                 //正在启动
		goto cleanup;

	if ( !Instance().ReportStatusToSCMgr(SERVICE_RUNNING) )                 // 启动成功
		goto cleanup;
	Instance().m_server_interface->ServiceStart( dwArgc, lpszArgv );

	if ( !Instance().ReportStatusToSCMgr(SERVICE_STOP) )                 // 停止
		goto cleanup;
cleanup:

	// try to report the stopped status to the service control manager.
	//
	if (sshStatusHandle)
		Instance().ReportStatusToSCMgr(SERVICE_STOPPED);
}



BOOL CWinServer::ReportStatusToSCMgr(DWORD dwCurrentState)
{
	static DWORD dwCheckPoint = 1;
	BOOL fResult = TRUE;
	SERVICE_STATUS ssStatus; 

	if ( !m_bDebug ) // when debugging we don't report to the SCM
	{
		if (dwCurrentState == SERVICE_START_PENDING)
			ssStatus.dwControlsAccepted = 0;
		else
			ssStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

		ssStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
		ssStatus.dwServiceSpecificExitCode = 0;
		ssStatus.dwCurrentState = dwCurrentState;
		ssStatus.dwWin32ExitCode = NO_ERROR;
// 		if (dwCurrentState == SERVICE_RUNNING)
// 		{
// 			ssStatus.dwWaitHint = 0;
// 		}
// 		else
		{
			ssStatus.dwWaitHint = 3000;
		}

		if ( ( dwCurrentState == SERVICE_RUNNING ) ||
			( dwCurrentState == SERVICE_STOPPED ) )
		{ 
			ssStatus.dwCheckPoint = 0;
		}
		else
			ssStatus.dwCheckPoint = dwCheckPoint++; 
		fResult = SetServiceStatus( m_StatusHandle, &ssStatus); 
	}
	return fResult;
}




void CWinServer::AddToMessageLog(LPSTR lpszMsg,DWORD dwErr)
{
	char   szMsg[MAX_PATH];
	HANDLE  hEventSource;
	LPSTR  lpszStrings[2];

	char m_server_name[MAX_PATH];

	memset(m_server_name,0,sizeof(m_server_name));
	strcpy_s(m_server_name,MAX_PATH,m_sz_appname.c_str());


	if ( !m_bDebug )
	{  

		hEventSource = RegisterEventSourceA(NULL, m_server_name);

		sprintf_s(szMsg, MAX_PATH,"%s error: %d", m_server_name, dwErr);
		lpszStrings[0] = szMsg;
		lpszStrings[1] = lpszMsg;

		if (hEventSource != NULL) {
			ReportEventA(hEventSource, // handle of event source
				EVENTLOG_ERROR_TYPE,  // event type
				0,                    // event category
				0,                    // event ID
				NULL,                 // current user's SID
				2,                    // strings in lpszStrings
				0,                    // no bytes of raw data
				(const char**)lpszStrings,          // array of error strings
				NULL);                // no raw data

			(VOID) DeregisterEventSource(hEventSource);
		}
	}
}


void CWinServer::CmdInstallService()
{
	SC_HANDLE   schService;
	SC_HANDLE   schSCManager;

	char szPath[MAX_PATH];
	 
	if ( GetModuleFileNameA( NULL, szPath, MAX_PATH ) == 0 )
	{ 
		return;
	}

	schSCManager = OpenSCManager(
		NULL,                   // machine (NULL == local)
		NULL,                   // database (NULL == default)
		SC_MANAGER_ALL_ACCESS   // access required
		);
	if ( schSCManager )
	{
		schService = CreateServiceA(
			schSCManager,               // SCManager database
			m_sz_service_name.c_str(),        // name of service
			m_sz_serviced_displayname.c_str(), // name to display
			SERVICE_ALL_ACCESS,         // desired access
			SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS,  // service type
			SERVICE_AUTO_START,         // start type
			SERVICE_ERROR_NORMAL,       // error control type
			szPath,                     // service's binary
			NULL,                       // no load ordering group
			NULL,                       // no tag identifier
			NULL,// dependencies
			NULL,                       // LocalSystem account
			NULL);                      // no password

		if ( schService )
		{ 
			CloseServiceHandle(schService);
		}
		else
		{
			printf(szPath,"CreateService failed - %s\n", GetLastError());
			//	AfxMessageBox(szPath);
		}

		CloseServiceHandle(schSCManager);
	}
	else
	{
		printf(szPath,"OpenSCManager failed - %s\n", GetLastError());
	}
}


BOOL WINAPI CWinServer::ControlHandler ( DWORD dwCtrlType )
{
	switch( dwCtrlType )
	{
	case CTRL_BREAK_EVENT:  // use Ctrl+C or Ctrl+Break to simulate
	case CTRL_C_EVENT:      // SERVICE_CONTROL_STOP in debug mode
		Instance().m_server_interface->ServiceStop();
		return TRUE;
		break;

	}
	return FALSE;
}


void CWinServer::CmdDebugService(int argc, char ** argv)
{
	SetConsoleCtrlHandler(CWinServer::ControlHandler, TRUE );

	m_server_interface->ServiceStart( argc, argv );
}

void CWinServer::SetInterface( CWinServerInterface * _interface )
{
	m_server_interface = _interface;
}

