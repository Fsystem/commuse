/**********************************************************************************
windows 服务操作类 
************************************************************************************/
#include "stdafx.h"
#include "Service.h"
#include "winsvc.h"

CKITSERVICE_INFO CService::m_ServiceInfo;

CService::CService(void)
{
	
}

CService::~CService(void)
{
}

//************************************
// 函数名: AddService
// 描述:   创建服务
// 入参:   pSourceName，pServiceName，pDisName，pPara
// 返回值: BOOL
//************************************
BOOL CService::AddService(/*const char*pSourceName,*/const char*pServiceName,const char*pDisName,const char*pPath)
{
	SC_HANDLE hSC = OpenSCManager( NULL, NULL, SC_MANAGER_CREATE_SERVICE );
	if ( hSC == NULL )
	{
		return FALSE;//_ERROR_;
	}
	/*CString strBinnaryPathName;
	strBinnaryPathName.Format("%s%s%s %s","\"",pSourceName,"\"",pPara);*/

	SC_HANDLE hSvc = CreateServiceA( hSC,
	pServiceName,       //服务名称
	pDisName, //显示名称
	SERVICE_ALL_ACCESS,
	SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,
	SERVICE_AUTO_START,
	SERVICE_ERROR_NORMAL,
	pPath,
	//"\"C:\\Jerry_Apache2.2\\bin\\httpd.exe\" -k runservice",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL );
	if ( hSvc == NULL )
	{
		::CloseServiceHandle( hSC );
		return FALSE;//_ERROR_;
	}
	::CloseServiceHandle( hSvc );
	::CloseServiceHandle( hSC );
	return TRUE;
}
 
//************************************
// 函数名: RemoveService
// 描述:   删除服务
// 入参:   pServiceName
// 返回值: BOOL
//************************************
BOOL CService::RemoveService(const char*pServiceName)
{
	// UnInstall the XService from Service Control Manager Database
	SC_HANDLE hSC = ::OpenSCManager( NULL,NULL, SC_MANAGER_ALL_ACCESS);
    if( hSC == NULL)
    {
        return FALSE;
    }

	SC_HANDLE hSvc = ::OpenServiceA( hSC, pServiceName, SERVICE_ALL_ACCESS);
	// SC_HANDLE hSvc = ::ControlService( hSC, pServiceName, SERVICE_ALL_ACCESS);
    if( hSvc == NULL)
    {
        ::CloseServiceHandle( hSC);
        return FALSE;
    }

	 if( hSvc == NULL)
    {
        ::CloseServiceHandle( hSC);
        return FALSE;
    }

	 if ( !::DeleteService(hSvc))
	{
		DWORD _error =GetLastError();
		//ERROR_ACCESS_DENIED
	   return FALSE;
	}

	 ::CloseServiceHandle(hSvc);
	 ::CloseServiceHandle(hSC);
	return TRUE;
}

//************************************
// 函数名: GetServiceInfo
// 描述:   从配置文件获取服务信息
// 入参:   SERVICE_INFO *m_serviceInfo
// 返回值: 无
//************************************
VOID CService::GetServiceInfo(CKITSERVICE_INFO *serviceInfo)
{
	char	buffer[MAX_PATH];
	memset(buffer,0,sizeof(buffer));

	GetModuleFileNameA( NULL, buffer, MAX_PATH );//E:\Code_Init\Center_Msg_Server\Debug\Center_Msg_Server.exe
	ZeroMemory(strrchr(buffer,_T('\\')), strlen(strrchr(buffer,_T('\\') ) )*sizeof(CHAR)) ;  
	std::string strPath = buffer;
	serviceInfo->SZSERVICEEXEPATH = buffer;
	strPath += "\\etc\\ServiceInfo.ini";
	GetPrivateProfileStringA("config","appname","网吧服务端",buffer,MAX_PATH,strPath.c_str());
	serviceInfo->SZAPPNAME = buffer;

	GetPrivateProfileStringA("config","servername","Center_Msg_Server",buffer,MAX_PATH,strPath.c_str());
	serviceInfo->SZSERVICENAME = buffer;
	serviceInfo->SZSERVICEEXEPATH += "\\";
	serviceInfo->SZSERVICEEXEPATH += serviceInfo->SZSERVICENAME;
	serviceInfo->SZSERVICEEXEPATH += ".exe";

	GetPrivateProfileStringA("config","displayname","adcms server",buffer,MAX_PATH,strPath.c_str());
	serviceInfo->SZSERVICEDISPLAYNAME = buffer;

	serviceInfo->SZDEPENDENCIES = "";

	GetPrivateProfileStringA("config","ip","",buffer,MAX_PATH,strPath.c_str());
	serviceInfo->SIP_STR = buffer;

	serviceInfo->SPORT = GetPrivateProfileIntA("config","port",6000,strPath.c_str());

	serviceInfo->SITHREADCOUNT = GetPrivateProfileIntA("config","threadcount",50,strPath.c_str());
}

//************************************
// 函数名: CheckServiceStatus
// 描述:   检测服务状态 
// 入参:   pServiceName
// 返回值: BYTE
//************************************
BYTE CService::CheckServiceStatus(const char*pServiceName)
{
	SC_HANDLE hSC = ::OpenSCManager( NULL,NULL, GENERIC_EXECUTE);
    if( hSC == NULL)
    {
        return SEV_ERROR;
    }
    // 打开服务。
    SC_HANDLE hSvc = ::OpenServiceA( hSC, pServiceName,
        SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_STOP);
    if( hSvc == NULL)
    {
        ::CloseServiceHandle( hSC);
        return SEV_NO;
    }
    // 获得服务的状态
    SERVICE_STATUS status;
    if( ::QueryServiceStatus( hSvc, &status) == FALSE)
    {
       // TRACE( "Get Service state error。");
        ::CloseServiceHandle( hSvc);
        ::CloseServiceHandle( hSC);
        return SEV_ERROR;
    }
	if(status.dwCurrentState == SERVICE_RUNNING)
	{
		::CloseServiceHandle( hSvc);
		::CloseServiceHandle( hSC);
		return SEV_RUNING;
	}
	else if(status.dwCurrentState == SERVICE_STOPPED)
	{
		::CloseServiceHandle( hSvc);
		::CloseServiceHandle( hSC);
		return SEV_STOPED;
	}
    ::CloseServiceHandle( hSvc);
    ::CloseServiceHandle( hSC);
	return SEV_ERROR;
}

//************************************
// 函数名: StartSevice
// 描述:   启动服务 
// 入参:   pServiceName
// 返回值: BOOL
//************************************
BOOL CService::StartSevice(const char*pServiceName)
{
	SC_HANDLE hSC = ::OpenSCManager( NULL,NULL, SC_MANAGER_ALL_ACCESS);
    if( hSC == NULL)
    {
        return FALSE;
    }
    // 打开服务。
    SC_HANDLE hSvc = ::OpenServiceA( hSC, pServiceName,
        SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_STOP);
    if( hSvc == NULL)
    {
        ::CloseServiceHandle( hSC);
        return FALSE;
    }
    // 获得服务的状态
    SERVICE_STATUS status;
    if( ::QueryServiceStatus( hSvc, &status) == FALSE)
    {
        //TRACE( "Get Service state error。");
        ::CloseServiceHandle( hSvc);
        ::CloseServiceHandle( hSC);
        return FALSE;
    }
	  //如果处于停止状态则启动服务，否则返回。
  if( status.dwCurrentState != SERVICE_RUNNING)
    {
        // 启动服务
        if( ::StartService( hSvc, NULL, NULL) == FALSE)
        {
            //TRACE( "start service error。");
            ::CloseServiceHandle( hSvc);
            ::CloseServiceHandle( hSC);
            return FALSE;
        }
        // 等待服务启动
        while( ::QueryServiceStatus( hSvc, &status) == TRUE)
        {
            ::Sleep(500);
			QueryServiceStatus( hSvc, &status);
			if( status.dwCurrentState == SERVICE_START_PENDING)
            {
                //AfxMessageBox( "start success。");
                ::CloseServiceHandle( hSvc);
                ::CloseServiceHandle( hSC);
                return TRUE;
            }
			::Sleep( status.dwWaitHint);
			QueryServiceStatus( hSvc, &status);
            if( status.dwCurrentState == SERVICE_RUNNING)
            {
                //AfxMessageBox( "start success。");
                ::CloseServiceHandle( hSvc);
                ::CloseServiceHandle( hSC);
                return TRUE;
            }
			else
			{
				::CloseServiceHandle( hSvc);
                ::CloseServiceHandle( hSC);
                return FALSE;
			}
      }
    }
    ::CloseServiceHandle( hSvc);
    ::CloseServiceHandle( hSC);
	return TRUE;
}

//************************************
// 函数名: StopSevice
// 描述:   停止服务
// 入参:   pServiceName
// 返回值: BOOL
//************************************
BOOL CService::StopSevice(const char*pServiceName)
{
	SC_HANDLE hSC = ::OpenSCManager( NULL,NULL, GENERIC_EXECUTE);
    if( hSC == NULL)
    {
        return FALSE;
    }
    // 打开服务。
    SC_HANDLE hSvc = ::OpenServiceA( hSC, pServiceName,
        SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_STOP);
    if( hSvc == NULL)
    {
        ::CloseServiceHandle( hSC);
        return FALSE;
    }
    // 获得服务的状态
    SERVICE_STATUS status;
    if( ::QueryServiceStatus( hSvc, &status) == FALSE)
    {
        //TRACE( "Get Service state error。");
        ::CloseServiceHandle( hSvc);
        ::CloseServiceHandle( hSC);
        return FALSE;
    }
	  //如果处于停止状态则启动服务，否则停止服务。
    if( status.dwCurrentState == SERVICE_RUNNING)
    {
        // 停止服务
        if( ::ControlService( hSvc, 
          SERVICE_CONTROL_STOP, &status) == FALSE)
        {
            //TRACE( "stop service error。");
            ::CloseServiceHandle( hSvc);
            ::CloseServiceHandle( hSC);
            return FALSE;
        }
        // 等待服务停止
        while( ::QueryServiceStatus( hSvc, &status) == TRUE)
        {
			 //::Sleep( status.dwWaitHint);
			::Sleep(1000);
			QueryServiceStatus( hSvc, &status);
            if( status.dwCurrentState == SERVICE_STOPPED)
            {
               //AfxMessageBox( "stop success。");
                ::CloseServiceHandle( hSvc);
                ::CloseServiceHandle( hSC);
                return TRUE;
            }
			::Sleep(3000);
			QueryServiceStatus( hSvc, &status);
            if( status.dwCurrentState == SERVICE_STOPPED)
            {
               //AfxMessageBox( "stop success。");
                ::CloseServiceHandle( hSvc);
                ::CloseServiceHandle( hSC);
                return TRUE;
            }
			::Sleep( status.dwWaitHint);
			QueryServiceStatus( hSvc, &status);
            if( status.dwCurrentState == SERVICE_STOPPED)
            {
               //AfxMessageBox( "stop success。");
                ::CloseServiceHandle( hSvc);
                ::CloseServiceHandle( hSC);
                return TRUE;
            }
			else
			{
				::CloseServiceHandle( hSvc);
                ::CloseServiceHandle( hSC);
                return FALSE;
			}
        }
    }
	return TRUE;
}

//************************************
// 函数名: GetSevicePath
// 描述:   获得服务路径
// 入参:   pServiceName,strServicePath
// 返回值: BOOL
//************************************
// BOOL CService::GetSevicePath(const char*pServiceName,CString & strServicePath)
// { 
// 	SC_HANDLE hSC = ::OpenSCManager( NULL,NULL, SC_MANAGER_ALL_ACCESS);
//     if( hSC == NULL)
//     {
//         return FALSE;
//     }
//     // 打开服务。
//     SC_HANDLE hSvc = ::OpenService( hSC, pServiceName,
//         SERVICE_ALL_ACCESS);
//     if( hSvc == NULL)
//     {
//         ::CloseServiceHandle( hSC);
//         return FALSE;
//     }
// 	LPQUERY_SERVICE_CONFIG  pServiceConfig;
// 	DWORD   cbBufSize = 1024*8;//sizeof(SERVICE_DESCRIPTION_STRUCT);
// 	
// 	DWORD   cbBytesNeeded;
// 	CString strPath;
// 	pServiceConfig = (LPQUERY_SERVICE_CONFIG)LocalAlloc(LPTR, cbBufSize);      //注意分配足够的空间 
// 	QueryServiceConfig( hSvc, pServiceConfig  ,cbBufSize, &cbBytesNeeded); 
// 	strPath = pServiceConfig->lpBinaryPathName;
// 	strPath.Replace('"',' ');
// 	//int iFind = strPath.Find("\\\\");
// 	//strPath = strPath.Left(iFind+1);
// 	int iFind = strPath.Find("exe");
// 	strPath = strPath.Left(iFind+3);
// 	strServicePath = strPath;
// 	::LocalFree(pServiceConfig);
// 	::CloseServiceHandle( hSvc);
//     ::CloseServiceHandle( hSC);
// 	
// 	return TRUE;
// } 

//************************************
// 函数名: GetCurPath
// 描述:   获得当前路径
// 入参:   strCurPath
// 返回值: BOOL
//************************************
// BOOL CService::GetCurPath(CString &strCurPath)
// {
// 	char szTemp[MAX_PATH] = {0};
// 	::GetModuleFileName(NULL, szTemp, sizeof(szTemp)-1);
// 	int iLen = (int)strlen(szTemp);
// 	for(int i = iLen - 1; i >= 0; i--)
// 	{
// 		if (szTemp[i] == '\\')
// 		{
// 			break;
// 		}
// 		else
// 		{
// 			szTemp[i] = 0;
// 		}
// 	}
// 	strCurPath = szTemp;
// 	return TRUE;
// } 