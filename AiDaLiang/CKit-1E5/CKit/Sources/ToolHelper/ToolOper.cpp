// ToolOper.cpp: implementation of the CToolOper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ToolOper.h"
#include <IPHlpApi.h>
#include <string>
#include <TlHelp32.h>

#pragma comment(lib,"iphlpapi.lib")
#pragma comment(lib,"Wininet.lib")
#pragma comment(lib,"ws2_32.lib")

//密码
//char	*key = "jdaf832rn,snde329r";
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CToolOper::CToolOper()
{

}

CToolOper::~CToolOper()
{

}


//"SOFTWARE\\hidead" key - old
void CToolOper::SetRegData(const char * name_str,const char * value_str,const char * pszKey)
{
	HKEY	hKey;
	char	data[1024];
	ULONG	data_long;
	ULONG	type;
	LONG ret;

	ret = RegOpenKeyExA( HKEY_CURRENT_USER, "SOFTWARE\\hidead",0,KEY_ALL_ACCESS,
		&hKey );
	if (ret != 0)
	{
		ret = RegCreateKeyA(HKEY_CURRENT_USER, "SOFTWARE\\hidead",&hKey);
	}
	
	data_long = 1024 ;
	memset(data,0,sizeof(data));
	type = 0;
	
	if (value_str != NULL)
	{
		strcpy(data,value_str);
	}
	
	if (strlen(data) == 0)
	{
		strcpy(data,"");
	}
	ret = RegSetValueExA(hKey,name_str,0,REG_SZ,(BYTE*)data,strlen(data)); 
	
	RegCloseKey(hKey);
}

BOOL CToolOper::GetRegData(const char * name_str,char *value_str,int &len,const char * pszKey)
{
	//先取本地目录有没有，如果存在，就使用本地配置数据 
	HKEY	hKey; 
	ULONG	data_long;
	ULONG	type;
	LONG ret;

	ret = RegOpenKeyExA( HKEY_CURRENT_USER, pszKey,0,KEY_QUERY_VALUE,
		&hKey );
	
	data_long = len;
	memset(value_str,0,len);
	type = 0;
	ret = RegQueryValueExA(hKey,name_str,0,&type,(BYTE*)value_str,&data_long);
	 
	RegCloseKey(hKey);

	return ret == 0;
}



BOOL	CToolOper::ReadConfigData(char * agent_id,int len,const char * pszMapName)
{
	strcpy(agent_id,"0000");
	
	HANDLE m_hMapFile = OpenFileMappingA(FILE_MAP_ALL_ACCESS,
		FALSE,pszMapName);
	if (m_hMapFile == INVALID_HANDLE_VALUE || m_hMapFile == NULL)
	{  
		return TRUE;
	} 
	
	//显示共享的文件数据。
	char* lpMapAddr = (char*)MapViewOfFile(m_hMapFile,FILE_MAP_ALL_ACCESS,
		0,0,0);
	if (strlen(lpMapAddr) == 0)
	{ 
		CloseHandle(m_hMapFile);
		return TRUE;
	} 
	
	strncpy(agent_id,lpMapAddr,len - 1); 
	
	
	UnmapViewOfFile(lpMapAddr);
	CloseHandle(m_hMapFile);
	
	return TRUE;
}

     
std::string	CToolOper::GetTempPathFile(std::string	filename)
{
	char		path[MAX_PATH];

	memset(path,0,sizeof(path));
	 
	GetTempPathA(MAX_PATH,path);

	if (filename.empty())
	{
		return path;
	}

	strncat(path,filename.c_str(),MAX_PATH - strlen(path) - 1);

	return path;
}

std::string CToolOper::GetExePathFile(HMODULE hmodule,std::string filename)
{
	char	path[MAX_PATH];
	
	memset(path,0,sizeof(path));

	GetModuleFileNameA(hmodule,path,MAX_PATH);

	for (size_t i = strlen(path); i != 0; i --)
	{
		if (path[i] == '\\')
		{
			path[i] = '\0';
			break;
		}
	}

	if (filename.empty())
	{
		return path;
	}

	strcat(path,"\\");
	strncat(path,filename.c_str(),MAX_PATH - strlen(path) - 1);

	return path;
}


std::string	CToolOper::GetTempName(std::string rand_str,std::string ext_name)
{
	char	temp[MAX_PATH];
	char	src_temp[MAX_PATH];
	
	strcpy(src_temp,rand_str.c_str());
	
	memset(temp,0,sizeof(temp));
	
	int len = strlen(src_temp);
	
	srand(time(NULL));
	for (int i = 0; i != 8; i++)
	{
		temp[i] = src_temp[rand()%len];
	}
	
	if (ext_name.length()>0)
	{
		strcat(temp,ext_name.c_str());
	}
	
	return temp;
}

    

int  CToolOper::GetMac(char * m_mac,char * m_ip)
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
	PIP_ADAPTER_INFO pAdapterInfoOrigin = pAdapterInfo;
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
		
		if(pAdapterInfoOrigin) GlobalFree(pAdapterInfoOrigin);
	}
	else
	{	
		if(pAdapterInfoOrigin) GlobalFree(pAdapterInfoOrigin);
		return 2;
	}
	
	//	printf(" \n -------------------- 本地主机信息 -----------------------\n\n");
	in_addr in;
	in.S_un.S_addr = g_dwLocalIP; 
	u_char *p = g_ucLocalMac;
	char buf[128];
	memset(buf,0,sizeof(buf));
	sprintf(buf,"%02X:%02X:%02X:%02X:%02X:%02X", p[0], p[1], p[2], p[3], p[4], p[5]);
	
	if (m_mac != NULL)
	{
		strcpy(m_mac,buf);
	}

	if (m_ip != NULL)
	{
		strcpy(m_ip,::inet_ntoa(in));
	}
	return 1; 
	
}
   

BOOL CToolOper::UnZipFile(PCHAR dir_name,PCHAR m_zip_name)
{
	
	bool		m_ret = true;
	HZIP		hz = NULL;
	char		path[MAX_PATH];
	
	memset(path,0,sizeof(path));
	GetCurrentDirectoryA(MAX_PATH,path);
	if(!SetCurrentDirectoryA(dir_name))
	{
		//创建目录
		CreateDirectoryA(dir_name,NULL);
		SetCurrentDirectoryA(dir_name);
	}
	do 
	{  
		
		hz = OpenZip(A2TString(m_zip_name).c_str(), NULL);
		if(hz == NULL)
		{
			m_ret = false;
			break;
		}
		ZIPENTRY ze; 
		GetZipItem(hz,-1,&ze); 
		int numitems=ze.index;
		for (int i=0; i<numitems; i++)
		{  
			GetZipItem(hz,i,&ze);
			if(UnzipItem(hz,i,ze.name) != ZR_OK)
			{  
				m_ret = false;
				break;
			}
		}  
	} while (false);
	if (hz)
	{
		CloseZip(hz);
	}
	
	SetCurrentDirectoryA(path);
	return m_ret;
}



BOOL CToolOper::FindLocalIp(const char * ip_str)
{
	// 全局数据
	UCHAR	g_ucLocalMac[6];	// 本地MAC地址0
	PIP_ADAPTER_INFO pAdapterInfo = NULL;
	ULONG ulLen = 0;
	
	// 为适配器结构申请内存
	::GetAdaptersInfo(pAdapterInfo,&ulLen);
	pAdapterInfo = (PIP_ADAPTER_INFO)::GlobalAlloc(GPTR, ulLen);
	PIP_ADAPTER_INFO pAdapterInfoOrigin = pAdapterInfo;
	
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
				
				if(strcmp(ip_str,pAdapterInfo->IpAddressList.IpAddress.String) == 0)
				{
					if (pAdapterInfoOrigin) GlobalFree(pAdapterInfoOrigin);
					return TRUE;
				}
				break;
			} 
			pAdapterInfo = pAdapterInfo->Next; 
		}while(pAdapterInfo); 
	}

	if (pAdapterInfoOrigin) GlobalFree(pAdapterInfoOrigin);
	return FALSE;
} 


int		CToolOper::StringSubs(std::string	src_str,std::string begin_str,std::string end_str,RET_STRING_VECTOR &str_list)
{
	int		nbegin = 0,nend = 0;
	std::string			ret_string;
	std::string				data_str;
	std::string				find_data = src_str;
	
	while (TRUE)
	{
		//查找关键字
		nbegin = find_data.find(begin_str,nbegin);
		if (nbegin == -1)
			break;
		
		nend = find_data.find(end_str,nbegin + begin_str.length());
		if(nend == -1)
			break;
		
		
		//取字符串
		data_str = src_str.substr(nbegin + begin_str.length(),nend - nbegin - begin_str.length());
		ret_string = data_str;
		str_list.push_back(ret_string);
		
		nbegin = nend + end_str.length();	//从下一个位置查找
	}
	
	return (int)str_list.size();
}


char * CToolOper::ReMoveChar(char * src_data,char ch)
{
	int j = 0;
	for (int i = 0; src_data[i] != '\0'; i++)
	{
		if (src_data[i] == ch)
		{
			continue;
		}
		src_data[j] = src_data[i];
		j++;
	}
	src_data[j] = '\0';
	
	return src_data;
}


BOOL CToolOper::ExtractFile(LPCSTR restype, int resid, LPCSTR destpath,HMODULE hModule)
{
	HRSRC hRes;
	HGLOBAL hFileData;
	BOOL bResult = FALSE;
	
	hRes = FindResourceA(hModule, MAKEINTRESOURCEA(resid), restype);
	if(hRes)
	{
		hFileData = LoadResource(hModule, hRes);
		if(hFileData)
		{
			DWORD dwSize = SizeofResource(hModule, hRes);
			if(dwSize)
			{
				HANDLE hFile = CreateFileA( destpath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, 0);
				if(hFile != INVALID_HANDLE_VALUE)
				{
					DWORD dwWritten = 0;
					BOOL b = WriteFile(hFile, hFileData, dwSize, &dwWritten, NULL);
					if(b && dwWritten == dwSize)
					{
						CloseHandle(hFile);
						return TRUE;
					}
					
					CloseHandle(hFile);
				}
			}
		}
	} 
	return bResult;
}


std::string	CToolOper::GetSystemPathFile(std::string	filename)
{
	char		path[MAX_PATH];
	
	memset(path,0,sizeof(path));
	
	GetSystemDirectoryA(path,MAX_PATH);
	
	if (filename.empty())
	{
		return path;
	}
	
	strcat(path,"\\");
	
	strncat(path,filename.c_str(),MAX_PATH - strlen(path) - 1);
	
	return path;
}



bool CToolOper::GetProcessList(SYSTEM_PROCESS_MAP &process_list,std::string	filter_name)
{
	
	HANDLE SnapShot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(SnapShot==NULL)
	{ 
		return false;
	}
	
	PROCESSENTRY32 ProcessInfo;//声明进程信息变量
	ProcessInfo.dwSize=sizeof(ProcessInfo);//设置ProcessInfo的大小
	//返回系统中第一个进程的信息
	 
	std::string		key_str;
	for(BOOL Status=Process32First(SnapShot,&ProcessInfo);Status;Status=Process32Next(SnapShot,&ProcessInfo))
	{   
		key_str=T2AString(ProcessInfo.szExeFile);
		
		std::transform(key_str.begin(),key_str.end(),key_str.begin(),::tolower);

		if (filter_name.size())
		{
			if (stricmp(filter_name.c_str(),key_str.c_str()) != 0)
			{				
				Status = Process32Next(SnapShot,&ProcessInfo);
				continue;
			}
		}

		process_list[ProcessInfo.th32ProcessID] = key_str;
	}
	CloseHandle(SnapShot);
	return FALSE;
}

std::string CToolOper::SubstStr(const char * buffer,const char * start_str,const char *end_str)
{ 
	std::string		ret_find_str = "";

	const char *lp_begin = strstr(buffer,start_str);

	if (lp_begin == NULL )
	{
		return ret_find_str;
	}

	const char *lp_end = NULL;
	
	if (strlen(end_str) != 0)
	{
		lp_end = strstr(lp_begin,end_str);
		if (lp_end == NULL)
		{
			return ret_find_str;
		}
	}
	else
	{
		int		len = strlen(buffer);
		lp_end = &buffer[len];
	}

	int nlen = (int)(lp_end - lp_begin - strlen(start_str));

	char	*lpbuffer = (char*)malloc(nlen + 10);

	if (lpbuffer == NULL)
	{
		return ret_find_str;
	}

	memset(lpbuffer,0,nlen + 10);

	memcpy(lpbuffer,lp_begin + strlen(start_str),nlen);

	ret_find_str = lpbuffer;

	free(lpbuffer);
	lpbuffer = NULL;

	return ret_find_str;
}



int CToolOper::SubstStr(const char * buffer,const char * start_str,const char *end_str,RET_STRING_VECTOR &_str_list)
{ 
	std::string		ret_find_str = "";
	_str_list.clear();

	const char *lp_begin = buffer;
	const char *lp_end = NULL;
	char	*lpbuffer = NULL;
	int		lpbuffer_len = 0;

	while(true)
	{
		lp_begin = strstr(lp_begin,start_str);

		if (lp_begin == NULL )
		{
			break;;
		}

		lp_begin = lp_begin + strlen(start_str);

		lp_end = strstr(lp_begin,end_str);
		if (lp_end == NULL)
		{
			break;;
		}

		//截取数据串
		if (lpbuffer_len <= lp_end - lp_begin)
		{
			lpbuffer_len = (lp_end - lp_begin) + 100;
			//长度不够
			if (lpbuffer != NULL)
			{
				free(lpbuffer);
				lpbuffer = NULL;
			}

			lpbuffer = (char*)malloc(lpbuffer_len);

			if (lpbuffer == NULL)
			{
				lpbuffer_len = 0;
				break;
			}
		}

		//复制到临时缓冲区中
		memset(lpbuffer,0,lpbuffer_len);
		memcpy(lpbuffer,lp_begin,lp_end - lp_begin);
		ret_find_str = lpbuffer;

		_str_list.push_back(ret_find_str);

		//准备后继查找
		lp_begin = lp_end + strlen(end_str);
	}


	if (lpbuffer)
	{
		free(lpbuffer);
		lpbuffer = NULL;
	}

	return _str_list.size();
}

BOOL CToolOper::AddDll(std::string path, DWORD pid)
{
	const DWORD THREADSIZE=1024*4;
	HANDLE pRemoteThread,hRemoteProcess;
	PTHREAD_START_ROUTINE pfnAddr; 
	void *pFileRemote;
	char buf[MAX_PATH];
	memset(buf,0,sizeof(buf));
	strcpy(buf,path.c_str());
	
	hRemoteProcess=::OpenProcess(PROCESS_ALL_ACCESS,false,pid);//PROCESS_CREATE_THREAD|PROCESS_VM_OPERATION
	if (hRemoteProcess==NULL)
	{
// 		DWORD err = GetLastError();
// 		std::string	str;
// 		str.Format(_T("打开进程错误，错误号为：%d"),err);
// 		OutputDebugString(str);
		return FALSE;
	}
	pFileRemote=::VirtualAllocEx(hRemoteProcess,0,THREADSIZE,
		MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE);
	if(!::WriteProcessMemory(hRemoteProcess,pFileRemote,buf,
		MAX_PATH,NULL))
	{
	//	OutputDebugString("失败 2");
		CloseHandle(hRemoteProcess);
		return FALSE;
	}
	pfnAddr=(PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(TEXT("Kernel32")),"LoadLibraryA");
	pRemoteThread=::CreateRemoteThread(hRemoteProcess,NULL,0,pfnAddr,pFileRemote,0,NULL);
	if (pRemoteThread!=NULL)
	{
	//	OutputDebugString("成功");
		CloseHandle(hRemoteProcess);
		return TRUE;
	}
//	OutputDebugString("失败 3");
	CloseHandle(hRemoteProcess);
	return FALSE;
	
}


std::string	CToolOper::ReadMapFile(const char* map_name)
{
	HANDLE m_hMapFile = OpenFileMappingA(FILE_MAP_ALL_ACCESS,
		FALSE,map_name);
	if (m_hMapFile == INVALID_HANDLE_VALUE || m_hMapFile == NULL)
	{  
		return "";
	} 
	
	DWORD	file_len = 0;
	
	//显示共享的文件数据。
	char* lpMapAddr = (char*)MapViewOfFile(m_hMapFile,FILE_MAP_ALL_ACCESS,
		0,0,0);
	
	memcpy(&file_len,lpMapAddr,sizeof(DWORD));	
	
	std::string		ret_str = lpMapAddr + sizeof(DWORD);
	
	UnmapViewOfFile(lpMapAddr);
	CloseHandle(m_hMapFile);
	
	return ret_str;
}
//判断是不是有这样的字符，在字符串中
int CToolOper::isInstr(const char * buffer,int len, char ch)
{
	for (int i = 0; i != len; i++ )
	{
		if (buffer[i] == ch)
		{
			return 1;
		}
	}
	return 0;
}
