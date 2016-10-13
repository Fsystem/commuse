// Server.cpp: implementation of the CServer class.
//
//此文件功能是相对服务和驱动相关的函数，可提供服务安装，停止，删除，暂停等
//
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Server.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define SERVER_LIB

CServer::CServer()
{
	m_last_error=0;
}

CServer::~CServer()
{
	m_last_error=0;
}

bool CServer::Test(char *msg)
{

	MessageBoxA(NULL,msg,"test",0+64);
	return true;
}

bool CServer::InstallServer(LPCSTR ServiceName, LPCSTR ServiceExe, DWORD servicetype, DWORD starttype)
{
	SC_HANDLE hSCManager = NULL;
	SC_HANDLE  schService;
	hSCManager = OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if(hSCManager == NULL)
	{ 
		m_last_error=GetLastError();
		char	buffer[MAX_PATH];
		memset(buffer,0,sizeof(buffer));
		sprintf(buffer,"OpenSCManager:%d\r\n",m_last_error);
		OutputDebugStringA(buffer);
		return FALSE;
	}
 
	schService = CreateServiceA( hSCManager,				 // SCManager database
		ServiceName,             // name of service
		ServiceName,             // name to display
		SERVICE_ALL_ACCESS,      // desired access
		servicetype,   // service type
		starttype,    // start type
		SERVICE_ERROR_NORMAL,    // error control type
		ServiceExe,              // service's binary
		NULL,                    // no load ordering group
		NULL,                    // no tag identifier
		NULL,                    // no dependencies
		NULL,                    // LocalSystem account
		NULL                     // no password
		);
    if (schService == NULL)
	{
		m_last_error=GetLastError();
		char	buffer[MAX_PATH];
		memset(buffer,0,sizeof(buffer));
		sprintf(buffer,"CreateService:%d\r\n",m_last_error);
		OutputDebugStringA(buffer);
		CloseServiceHandle(schService);
		CloseServiceHandle(hSCManager);
        return FALSE;
	}
    CloseServiceHandle(schService);
	CloseServiceHandle(hSCManager);
    return TRUE;
}

bool CServer::ReMoveServer(LPCTSTR ServiceName)
{
	SC_HANDLE schService;
    int	      nRet = 0;
	SC_HANDLE hSCManager = NULL; 

	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if(hSCManager == NULL)
	{ 
		m_last_error=GetLastError(); 
		return FALSE;
	}
    schService = OpenService(hSCManager, ServiceName, SERVICE_ALL_ACCESS);
    if(schService == NULL)
	{
		m_last_error=GetLastError(); 
		CloseServiceHandle(hSCManager);
		return FALSE;
	}
    nRet = DeleteService(schService);
	m_last_error=GetLastError();

    CloseServiceHandle(schService);
	CloseServiceHandle(hSCManager);

    return nRet;
}

DWORD CServer::MyStartService(LPCTSTR ServiceName)
{
	SC_HANDLE schService;
    int	      nRet = 0;
	SC_HANDLE hSCManager = NULL; 
	
	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if(hSCManager == NULL)
	{ 
		m_last_error=GetLastError();
		return m_last_error;
	}

    schService = OpenService(hSCManager, ServiceName, SERVICE_ALL_ACCESS);
    if(schService == NULL)
	{
		m_last_error=GetLastError(); 
		CloseServiceHandle(hSCManager);
        return m_last_error;
	}
	
    nRet = StartService(schService, 0, NULL);
	if(!nRet)
	{
		
		m_last_error=GetLastError();
		CloseServiceHandle(schService);
		CloseServiceHandle(hSCManager);
		return m_last_error;
	}
	
    CloseServiceHandle(schService);
	CloseServiceHandle(hSCManager);
    return 0;

}

bool CServer::StopService(LPCTSTR ServiceName)
{
	SC_HANDLE      schService = NULL;
    SERVICE_STATUS ServiceStatus;
	int            nRet = 0; 
	SC_HANDLE hSCManager = NULL; 
	
	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if(hSCManager == NULL)
	{ 
		m_last_error=GetLastError();
		return FALSE;
	}

    schService = OpenService(hSCManager, ServiceName, SERVICE_ALL_ACCESS);
    if(schService == NULL)
	{
		m_last_error=GetLastError();
		CloseServiceHandle(hSCManager);
        return FALSE;
	}
	
    nRet = ControlService(schService, SERVICE_CONTROL_STOP, &ServiceStatus);
	if(!nRet)
	{
		m_last_error=GetLastError();
	}
    CloseServiceHandle(hSCManager);
    CloseServiceHandle(schService);
    return nRet;
	
}

bool CServer::IsAdmin()
{
	HANDLE                   hAccessToken;
    BYTE                     *InfoBuffer;
    PTOKEN_GROUPS            ptgGroups;
    DWORD                    dwInfoBufferSize;
    PSID                     psidAdministrators;
    SID_IDENTIFIER_AUTHORITY siaNtAuthority = SECURITY_NT_AUTHORITY;
    UINT                     i;
    bool                     bRet = FALSE;
	
    if(!OpenProcessToken(GetCurrentProcess(),TOKEN_QUERY,&hAccessToken))
		goto cleanup;
	
    InfoBuffer = new BYTE[1024];
    if(!InfoBuffer)
		goto cleanup;
	
    bRet = GetTokenInformation(hAccessToken,
		TokenGroups,
		InfoBuffer,
		1024,
		&dwInfoBufferSize);
	
    CloseHandle(hAccessToken);
	
    if(!bRet)
		goto cleanup;
	
    if( !AllocateAndInitializeSid(&siaNtAuthority,
		2,
		SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS,
		0,0,0,0,0,0,
		&psidAdministrators) )
		goto cleanup;
	
    bRet = FALSE;
	
    ptgGroups = (PTOKEN_GROUPS)InfoBuffer;
	
    for(i = 0; i < ptgGroups->GroupCount; i++)
    {
        if(EqualSid(psidAdministrators,ptgGroups->Groups[i].Sid))
        {
            bRet = TRUE;
            break;
        }
    }
	
    FreeSid(psidAdministrators);
	
cleanup:
	
    if(InfoBuffer)
		delete InfoBuffer;
	
    return bRet;
}


bool CServer::ServerCtrl(LPCTSTR server_link_name,DWORD ctrl_code,LPVOID m_input,
		DWORD m_input_num,LPVOID m_output,DWORD m_output_num)
{
	HANDLE  Device;
    DWORD   BytesReturned;
	Device = CreateFile(
        server_link_name,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_NO_BUFFERING,
        NULL
        ); 
	
    if (Device == INVALID_HANDLE_VALUE)
    {  
	 
        return false;
    }
	
    if (!DeviceIoControl(
        Device,
        ctrl_code,//控制代码
        m_input,
        m_input_num,
        m_output,
        m_output_num,
        &BytesReturned,
        NULL
        ))
    {  
		::CloseHandle(Device);  
        return false;
    }
	
	::CloseHandle(Device);
	return true;
}