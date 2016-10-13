#include "StdAfx.h"
#include "DeviceOper.h"

#include <atlconv.h>

#include <WinIoCtl.h>
#include <WinSock2.h>
#include  <iphlpapi.h>
#include <tchar.h>
#include <TlHelp32.h>

#pragma comment(lib,"iphlpapi.lib")
#pragma comment(lib,"ws2_32.lib")

CDeviceOper::CDeviceOper(void)
{
}

CDeviceOper::~CDeviceOper(void)
{
}



mystring	CDeviceOper::GetDiskSnId()
{
	char pchDiskPhysicalSN[ 14 ];
	BYTE IdOutCmd[530];

	memset(pchDiskPhysicalSN,0,sizeof(pchDiskPhysicalSN));
	memset(IdOutCmd,0,sizeof(IdOutCmd));

	HANDLE drive=CreateFile(_T("\\\\.\\PhysicalDrive0"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL );
	if ( drive == INVALID_HANDLE_VALUE )
	{ 
		return _T("");
	}
	GETVERSIONOUTPARAMS VersionParams;
	DWORD cbBytesReturned = 0;
	memset( ( void* ) &VersionParams, 0, sizeof( VersionParams ) );
	if ( ! DeviceIoControl( drive, DFP_GET_VERSION, NULL, 0, &VersionParams, sizeof( VersionParams ), &cbBytesReturned, NULL ) )
	{ 
		CloseHandle(drive);
		return _T("");
	}
	if (VersionParams.bIDEDeviceMap<=0)
	{ 
		CloseHandle(drive);
		return _T("");
	}
	BYTE bIDCmd = 0;
	SENDCMDINPARAMS scip;
	bIDCmd = ( VersionParams.bIDEDeviceMap >> 0 & 0x10 ) ? IDE_ATAPI_IDENTIFY : IDE_ATA_IDENTIFY;
	memset( &scip, 0, sizeof( scip) );
	memset( IdOutCmd, 0, sizeof( IdOutCmd ) );
	scip.cBufferSize = IDENTIFY_BUFFER_SIZE;
	scip.irDriveRegs.bFeaturesReg = 0;
	scip.irDriveRegs.bSectorCountReg = 1;
	scip.irDriveRegs.bSectorNumberReg = 1;
	scip.irDriveRegs.bCylLowReg = 0;
	scip.irDriveRegs.bCylHighReg = 0;
	scip.irDriveRegs.bDriveHeadReg= 0xA0 | ( ( ( BYTE ) drive & 1 ) << 4 );
	scip.irDriveRegs.bCommandReg = bIDCmd;
	scip.bDriveNumber = ( BYTE ) drive;
	scip.cBufferSize = IDENTIFY_BUFFER_SIZE;
	if ( ! DeviceIoControl( drive, DFP_RECEIVE_DRIVE_DATA, &scip, sizeof( SENDCMDINPARAMS)- 1, ( LPVOID )&IdOutCmd, sizeof( SENDCMDOUTPARAMS ) + IDENTIFY_BUFFER_SIZE - 1, &cbBytesReturned, NULL ) )
	{ 
		CloseHandle(drive);
		return _T("");
	}
	USHORT *pIdSector = ( USHORT * )( ( PSENDCMDOUTPARAMS )IdOutCmd )->bBuffer;
	int nPosition = 0;
	for( int nIndex = 13; nIndex < 20; nIndex++ )
	{
		pchDiskPhysicalSN[ nPosition++ ]=( unsigned char )( pIdSector[ nIndex ] / 256 );
		pchDiskPhysicalSN[ nPosition++ ]=( unsigned char )( pIdSector[ nIndex ] % 256 );
	}


	int nSerial[ 14 ];
	for( int i = 0 ; i < 14; ++ i )
	{
		nSerial[ i ] = pchDiskPhysicalSN[ i ];
		nSerial[ i ] &= 0x000000ff;
	}

	TCHAR		ret_buffer[MAX_PATH];

	memset(ret_buffer,0,sizeof(ret_buffer));

	wsprintf( ret_buffer, _T("%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x"),
		nSerial[ 0 ],
		nSerial[ 1 ],
		nSerial[ 2 ],
		nSerial[ 3 ],
		nSerial[ 4 ],
		nSerial[ 5 ],
		nSerial[ 6 ],
		nSerial[ 7 ],
		nSerial[ 8 ],
		nSerial[ 9 ],
		nSerial[ 10 ],
		nSerial[ 11 ],
		nSerial[ 12 ],
		nSerial[ 13 ]);

	CloseHandle(drive);

	return ret_buffer;
}



mystring  CDeviceOper::GetMac(std::string &ip_str)
{
	// 全局数据
	UCHAR	g_ucLocalMac[6];	// 本地MAC地址
	DWORD	g_dwGatewayIP;		// 网关IP地址
	DWORD	g_dwLocalIP;		// 本地IP地址
	DWORD	g_dwMask;			// 子网掩码
	PIP_ADAPTER_INFO pAdapterInfo = NULL;
	ULONG ulLen = 0;

	// 为适配器结构申请内存
	::GetAdaptersInfo(pAdapterInfo,&ulLen);
	pAdapterInfo = (PIP_ADAPTER_INFO)::GlobalAlloc(GPTR, ulLen);

	// 取得本地适配器结构信息
	if(::GetAdaptersInfo(pAdapterInfo,&ulLen) ==  ERROR_SUCCESS)
	{
		do { 
			if (pAdapterInfo->Type == MIB_IF_TYPE_ETHERNET)
			{  
				memcpy(g_ucLocalMac, pAdapterInfo->Address, 6);
				if (g_ucLocalMac[0]==0 && g_ucLocalMac[1]==0 &&
					g_ucLocalMac[2]==0 && g_ucLocalMac[3]==0 &&
					g_ucLocalMac[4]==0 && g_ucLocalMac[5]==0)
				{
					pAdapterInfo = pAdapterInfo->Next; 
					continue;
				}
				g_dwGatewayIP = inet_addr(pAdapterInfo->GatewayList.IpAddress.String);
				g_dwLocalIP = inet_addr(pAdapterInfo->IpAddressList.IpAddress.String);
				if (g_dwLocalIP  <= 16843009)
				{ 
					pAdapterInfo = pAdapterInfo->Next; 
					continue;
				}
				g_dwMask = inet_addr(pAdapterInfo->IpAddressList.IpMask.String);

				break;
			} 
			pAdapterInfo = pAdapterInfo->Next; 
		}while(pAdapterInfo);

	}
	else
	{	
		return _T("");
	}

	//	printf(" \n -------------------- 本地主机信息 -----------------------\n\n");
	in_addr in;
	in.S_un.S_addr = g_dwLocalIP; 
	u_char *p = g_ucLocalMac;
	TCHAR buf[128];
	memset(buf,0,sizeof(buf));
	wsprintf(buf,_T("%02X:%02X:%02X:%02X:%02X:%02X"), p[0], p[1], p[2], p[3], p[4], p[5]);

	ip_str = ::inet_ntoa(in);
 
 
	return buf; 

}


mystring	CDeviceOper::GetIeVer()
{
	HKEY	hKey; 
	ULONG	data_long;
	ULONG	type;
	TCHAR	value_str[MAX_PATH];
	long	 ret;
	DWORD	ie_ver = 0;
	ret = RegOpenKeyEx( HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Internet Explorer"),0,KEY_QUERY_VALUE,
		&hKey );

	data_long = sizeof(value_str);
	memset(value_str,0,sizeof(value_str));
	type = 0;

	ret = RegQueryValueEx(hKey,_T("svcVersion"),0,&type,(BYTE*)value_str,&data_long);
	if (ret == ERROR_SUCCESS)
	{
		RegCloseKey(hKey);

		return value_str; 
	}

	data_long = sizeof(value_str);
	memset(value_str,0,sizeof(value_str));
	type = 0;
	ret = RegQueryValueEx(hKey,_T("Version"),0,&type,(BYTE*)value_str,&data_long);

	if (ret == ERROR_SUCCESS)
	{
		RegCloseKey(hKey);

		return value_str; 
	}

	RegCloseKey(hKey);

	return _T("");
}


//HKEY_LOCAL_MACHINE\SOFTWARE\Macromedia\FlashPlayer



mystring	CDeviceOper::GetFlashVer()
{
	HKEY	hKey; 
	ULONG	data_long;
	ULONG	type;
	TCHAR	value_str[MAX_PATH];
	long	 ret; 
	ret = RegOpenKeyEx( HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Macromedia\\FlashPlayer"),0,KEY_QUERY_VALUE,
		&hKey );

	data_long = MAX_PATH;
	memset(value_str,0,sizeof(value_str));
	type = 0;

	ret = RegQueryValueEx(hKey,_T("CurrentVersion"),0,&type,(BYTE*)value_str,&data_long);

	RegCloseKey(hKey);

	if (ret == ERROR_SUCCESS)
	{
		return value_str;
	}
  
	return _T("");
}



mystring	CDeviceOper::GetSystemVer()
{
	DWORD  dwMajorVersion;		//主版本号
	DWORD   dwMinorVersion;		//副版本
	DWORD  dwBuildNumber;		//创建号
	DWORD  dwPlatformId;		//ID号
	OSVERSIONINFO osvi;//定义OSVERSIONINFO数据结构对象
	memset(&osvi, 0, sizeof(OSVERSIONINFO));//开空间 
	osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);//定义大小 
	GetVersionEx (&osvi);//获得版本信息 


	dwMajorVersion=osvi.dwMajorVersion;//主版本号
	dwMinorVersion=osvi.dwMinorVersion;//副版本
	dwBuildNumber=osvi.dwBuildNumber;//创建号
	dwPlatformId=osvi.dwPlatformId;//ID号

	TCHAR		ret_buffer[MAX_PATH];

	memset(ret_buffer,0,sizeof(ret_buffer));

	wsprintf(ret_buffer,_T("%d.%d.%d.%d"),dwMajorVersion,dwMinorVersion,dwBuildNumber,dwPlatformId);

	return ret_buffer;
}

//检查指定进程个数
DWORD	CDeviceOper::GetProcessName(PROCESS_NAME_COUNT_MAP &process_map)
{ 

	HANDLE SnapShot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(SnapShot == NULL)
	{ 
		return 0;
	}
	
	PROCESSENTRY32 ProcessInfo;//声明进程信息变量
	ProcessInfo.dwSize=sizeof(ProcessInfo);//设置ProcessInfo的大小
	//返回系统中第一个进程的信息
	BOOL Status=Process32First(SnapShot,&ProcessInfo); 
	 
	USES_CONVERSION;
	std::string		key_str;
	while(Status)
	{    
		std::string		process_name = T2A(ProcessInfo.szExeFile);
	
		if (process_map.find(process_name) == process_map.end())
		{
			process_map[process_name] = 1;
		}
		else
		{
			process_map[process_name] ++;
		}
		
		Status=Process32Next(SnapShot,&ProcessInfo);
	}
	CloseHandle(SnapShot);


	return process_map.size();

}