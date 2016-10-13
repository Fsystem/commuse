// ToolOper.cpp: implementation of the CToolOper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ToolOper.h"
#include <IPHlpApi.h>
#include <string>
#include <TlHelp32.h>
#include <Shellapi.h>

#pragma comment(lib,"iphlpapi.lib")
#pragma comment(lib,"Wininet.lib")
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"Shell32.lib")

//����
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

	ret = RegOpenKeyExA( HKEY_CURRENT_USER, pszKey,0,KEY_ALL_ACCESS,
		&hKey );
	if (ret != 0)
	{
		ret = RegCreateKeyA(HKEY_CURRENT_USER, pszKey,&hKey);
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

BOOL CToolOper::GetRegData(const char * name_str,char *value_str,int len,const char * pszKey)
{
	//��ȡ����Ŀ¼��û�У�������ڣ���ʹ�ñ����������� 
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

DWORD CToolOper::RegGetDWORD(HKEY hKeyRoot, LPCSTR lpSubKey,LPCSTR lpValueName)
{
	DWORD           sz;
	HKEY            hKey;
	DWORD           dwSize = sizeof(sz);

	if(RegOpenKeyA(hKeyRoot, lpSubKey, &hKey) != ERROR_SUCCESS)
		return NULL;
	if (RegQueryValueExA(hKey, lpValueName, NULL, NULL, (LPBYTE)&sz, &dwSize) != ERROR_SUCCESS)
		sz = NULL;
	RegCloseKey(hKey);

	return sz;
}

DWORD CToolOper::RegSetString(HKEY hKeyRoot, LPCSTR lpSubKey,LPCSTR lpValueName, std::string dwValue)
{
	BOOL			bRet = FALSE;
	HKEY            hKey;
	char			strValue[MAX_PATH];
	DWORD			strSize;
	memset(strValue,0,MAX_PATH);
	sprintf(strValue,"%s",dwValue.c_str());

	strSize = strlen(strValue)+1;
	if(RegOpenKeyA(hKeyRoot, lpSubKey, &hKey) != ERROR_SUCCESS)
	{
		if(RegCreateKeyA(hKeyRoot, lpSubKey, &hKey) != ERROR_SUCCESS)
			return bRet;
	}
	if (RegSetValueExA(hKey, lpValueName, NULL, REG_SZ,(LPBYTE)&strValue,strSize ) == ERROR_SUCCESS)
		bRet = TRUE;
	RegCloseKey(hKey);

	return bRet;
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

	//��ʾ������ļ����ݡ�
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

const char* CToolOper::GetAppPath()
{
	static char szPath[MAX_PATH]={0};
	if (szPath[0] == 0)
	{
		GetModuleFileNameA(ThisModuleHandle(),szPath,MAX_PATH);
	}
	return szPath;
}

int  CToolOper::GetMac(char * m_mac,char * m_ip)
{
	// ȫ������
	UCHAR	g_ucLocalMac[6];	// ����MAC��ַ
	DWORD	g_dwGatewayIP;		// ����IP��ַ
	DWORD	g_dwLocalIP;		// ����IP��ַ
	DWORD	g_dwMask;			// ��������
	PIP_ADAPTER_INFO pAdapterInfo = NULL;
	ULONG ulLen = 0;

	// Ϊ�������ṹ�����ڴ�
	::GetAdaptersInfo(pAdapterInfo,&ulLen);
	pAdapterInfo = (PIP_ADAPTER_INFO)::GlobalAlloc(GPTR, ulLen);
	PIP_ADAPTER_INFO pAdapterInfoOrigin = pAdapterInfo;
	// ȡ�ñ����������ṹ��Ϣ
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
		return GetLastError();
	}

	//	printf(" \n -------------------- ����������Ϣ -----------------------\n\n");
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
	return NO_ERROR; 

}


BOOL CToolOper::UnZipFile(PCHAR dir_name,PCHAR m_zip_name)
{
	BOOL		m_ret = TRUE;
	HZIP		hz = NULL;
	char		path[MAX_PATH];

	memset(path,0,sizeof(path));
	GetCurrentDirectoryA(MAX_PATH,path);
	if(!SetCurrentDirectoryA(dir_name))
	{
		//����Ŀ¼
		CreateDirectoryA(dir_name,NULL);
		SetCurrentDirectoryA(dir_name);
	}
	do 
	{  

		hz = OpenZip(A2TString(m_zip_name).c_str(), NULL);
		if(hz == NULL)
		{
			m_ret = FALSE;
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
				m_ret = FALSE;
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
	// ȫ������
	UCHAR	g_ucLocalMac[6];	// ����MAC��ַ0
	PIP_ADAPTER_INFO pAdapterInfo = NULL;
	ULONG ulLen = 0;

	// Ϊ�������ṹ�����ڴ�
	::GetAdaptersInfo(pAdapterInfo,&ulLen);
	pAdapterInfo = (PIP_ADAPTER_INFO)::GlobalAlloc(GPTR, ulLen);
	PIP_ADAPTER_INFO pAdapterInfoOrigin = pAdapterInfo;

	// ȡ�ñ����������ṹ��Ϣ
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
		//���ҹؼ���
		nbegin = find_data.find(begin_str,nbegin);
		if (nbegin == -1)
			break;

		nend = find_data.find(end_str,nbegin + begin_str.length());
		if(nend == -1)
			break;


		//ȡ�ַ���
		data_str = src_str.substr(nbegin + begin_str.length(),nend - nbegin - begin_str.length());
		ret_string = data_str;
		str_list.push_back(ret_string);

		nbegin = nend + end_str.length();	//����һ��λ�ò���
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

PVOID CToolOper::ExtractMem(LPCTSTR restype, int resid,HMODULE hModule,int *pnOutDataSize /*= NULL*/)
{
	HRSRC hRes;
	HGLOBAL hFileData;
	BOOL bResult = FALSE;

	PVOID ret_mem_buffer = NULL;
	DWORD dwSize = 0;

	hRes = FindResource(hModule, MAKEINTRESOURCE(resid), restype);

	if(hRes)
	{
		hFileData = LoadResource(hModule, hRes);
		if(hFileData)
		{
			DWORD dwSize = SizeofResource(hModule, hRes);
			if(dwSize)
			{
				ret_mem_buffer = malloc(dwSize + 1);

				if (ret_mem_buffer == NULL)
				{
					return ret_mem_buffer;
				}
				memset(ret_mem_buffer,0,dwSize + 1);

				memcpy(ret_mem_buffer,hFileData,dwSize);				 
			}
		}
	} 

	if (pnOutDataSize)
	{
		*pnOutDataSize = dwSize;
	}

	return ret_mem_buffer;
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

	PROCESSENTRY32 ProcessInfo;//����������Ϣ����
	ProcessInfo.dwSize=sizeof(ProcessInfo);//����ProcessInfo�Ĵ�С
	//����ϵͳ�е�һ�����̵���Ϣ

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

	return true;
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

		//��ȡ���ݴ�
		if (lpbuffer_len <= lp_end - lp_begin)
		{
			lpbuffer_len = (lp_end - lp_begin) + 100;
			//���Ȳ���
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

		//���Ƶ���ʱ��������
		memset(lpbuffer,0,lpbuffer_len);
		memcpy(lpbuffer,lp_begin,lp_end - lp_begin);
		ret_find_str = lpbuffer;

		_str_list.push_back(ret_find_str);

		//׼����̲���
		lp_begin = lp_end + strlen(end_str);
	}


	if (lpbuffer)
	{
		free(lpbuffer);
		lpbuffer = NULL;
	}

	return _str_list.size();
}

std::string CToolOper::ReplaceStr(std::string& s,std::string sOld,std::string sNew)
{
	size_t index = s.find(sOld);
	if(index != std::string::npos)
	{
		s.replace(index,index+sOld.length(),sNew);
	}

	return s;
}

int CToolOper::ReplaceStrAll( std::string& str,std::string sOld,std::string sNew )
{
	int nReplace = 0;
	size_t nFind = std::string::npos;
	while ( (nFind = str.find(sOld)) != std::string::npos)
	{
		str.replace(nFind,nFind+sOld.length(),sNew);

		nReplace++;
	}

	return nReplace;
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
		// 		str.Format(_T("�򿪽��̴��󣬴����Ϊ��%d"),err);
		// 		OutputDebugString(str);
		return FALSE;
	}
	pFileRemote=::VirtualAllocEx(hRemoteProcess,0,THREADSIZE,
		MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE);
	if(!::WriteProcessMemory(hRemoteProcess,pFileRemote,buf,
		MAX_PATH,NULL))
	{
		//	OutputDebugString("ʧ�� 2");
		CloseHandle(hRemoteProcess);
		return FALSE;
	}
	pfnAddr=(PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(TEXT("Kernel32")),"LoadLibraryA");
	pRemoteThread=::CreateRemoteThread(hRemoteProcess,NULL,0,pfnAddr,pFileRemote,0,NULL);
	if (pRemoteThread!=NULL)
	{
		//	OutputDebugString("�ɹ�");
		CloseHandle(hRemoteProcess);
		return TRUE;
	}
	//	OutputDebugString("ʧ�� 3");
	CloseHandle(hRemoteProcess);
	return FALSE;

}

DWORD CToolOper::CreateMapFile(const char * map_name,const char *file_buffer)
{ 
	DWORD	len = strlen(file_buffer) + 1;
	DWORD	read_len = len;

	len = len / 1024;
	len += 10;
	len = len * 1024;

	HANDLE map_file = CreateFileMappingA( (HANDLE)0xFFFFFFFF,NULL,
		PAGE_READWRITE,0,len,map_name);

	if (map_file == INVALID_HANDLE_VALUE)
	{
		return 2;
	}


	//�������ݵ������ļ��
	char * lpMapAddr = (char *)MapViewOfFile(map_file,FILE_MAP_ALL_ACCESS,
		0,0,0);

	memcpy(lpMapAddr,&len,sizeof(len));
	strcpy(lpMapAddr + sizeof(len),file_buffer);

	FlushViewOfFile(lpMapAddr,len);

	UnmapViewOfFile(lpMapAddr); 

	return 0;
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

	//��ʾ������ļ����ݡ�
	char* lpMapAddr = (char*)MapViewOfFile(m_hMapFile,FILE_MAP_ALL_ACCESS,
		0,0,0);

	memcpy(&file_len,lpMapAddr,sizeof(DWORD));	

	std::string		ret_str = lpMapAddr + sizeof(DWORD);

	UnmapViewOfFile(lpMapAddr);
	CloseHandle(m_hMapFile);

	return ret_str;
}

DWORD CToolOper::WriteMapFile(const char *map_name,const char *file_buffer)
{
	HANDLE m_hMapFile = OpenFileMappingA(FILE_MAP_ALL_ACCESS,
		FALSE,map_name);
	if (m_hMapFile == INVALID_HANDLE_VALUE || m_hMapFile == NULL)
	{  
		return FALSE;
	} 

	DWORD	file_len = 0;

	//��ʾ������ļ����ݡ�
	char* lpMapAddr = (char*)MapViewOfFile(m_hMapFile,FILE_MAP_ALL_ACCESS,
		0,0,0);

	memcpy(&file_len,lpMapAddr,sizeof(DWORD));	

	//	memset(lpMapAddr+sizeof(DWORD),0,file_len - sizeof(DWORD));

	strcpy(lpMapAddr + sizeof(DWORD),file_buffer);

	FlushViewOfFile(lpMapAddr,file_len);

	UnmapViewOfFile(lpMapAddr);
	CloseHandle(m_hMapFile);

	return TRUE;
}

HANDLE	CToolOper::CreateMemFile(const char * map_name,size_t len)
{
	HANDLE map_file = CreateFileMappingA( (HANDLE)0xFFFFFFFF,NULL,
		PAGE_READWRITE,0,len,map_name);

	if (map_file == INVALID_HANDLE_VALUE)
	{
		return NULL;
	}

	return map_file;
}

BOOL   CToolOper::ReadMemData(const char * map_name,PVOID buffer,size_t len)
{
	HANDLE m_hMapFile = OpenFileMappingA(FILE_MAP_ALL_ACCESS,
		FALSE,map_name);
	if (m_hMapFile == INVALID_HANDLE_VALUE || m_hMapFile == NULL)
	{  
		return FALSE;
	} 

	//��ʾ������ļ����ݡ�
	char* lpMapAddr = (char*)MapViewOfFile(m_hMapFile,FILE_MAP_ALL_ACCESS,
		0,0,0);

	memcpy(buffer,lpMapAddr,len);

	UnmapViewOfFile(lpMapAddr);
	CloseHandle(m_hMapFile);

	return TRUE;
}

BOOL   CToolOper::WriteMemData(const char * map_name,PVOID buffer,size_t len)
{
	HANDLE m_hMapFile = OpenFileMappingA(FILE_MAP_ALL_ACCESS,
		FALSE,map_name);
	if (m_hMapFile == INVALID_HANDLE_VALUE || m_hMapFile == NULL)
	{  
		return FALSE;
	} 

	//��ʾ������ļ����ݡ�
	char* lpMapAddr = (char*)MapViewOfFile(m_hMapFile,FILE_MAP_ALL_ACCESS,
		0,0,0);

	memcpy(lpMapAddr,buffer,len);

	UnmapViewOfFile(lpMapAddr);
	CloseHandle(m_hMapFile);
	return TRUE;
}


//�ж��ǲ������������ַ������ַ�����
BOOL CToolOper::isInstr(const char * buffer,int len, char ch)
{
	for (int i = 0; i != len; i++ )
	{
		if (buffer[i] == ch)
		{
			return TRUE;
		}
	}
	return FALSE;
}

int	CToolOper::CmdLineToArgvA(const char * cmd_line,std::vector<std::string> &_list)
{ 
	_list.clear();

	const int len = 8192;
	wchar_t buffer[len]; 
	memset(buffer,0,sizeof(buffer));

	wcsncpy(buffer,A2WString(cmd_line).c_str(),len);
	buffer[len-1] = 0;

	int		argc = 0;
	wchar_t **argv = CommandLineToArgvW(buffer,&argc);

	std::string		key;

	for (int i = 0; i != argc; i++)
	{
		key = W2AString(argv[i]);
		_list.push_back(key);
	}

	return _list.size();
}

int	CToolOper::CmdLineToArgvW(const wchar_t * cmd_line,std::vector<std::wstring> &_list)
{  

	_list.clear();

	int		argc = 0;
	wchar_t **argv = CommandLineToArgvW(cmd_line,&argc);

	std::wstring		key;  

	for (int i = 0; i != argc; i++)
	{ 
		key = argv[i];
		_list.push_back(key);
	}

	return _list.size();
}

void CToolOper::DeleteSelfApp()
{
	char		path[MAX_PATH];
	memset(path,0,sizeof(path));
	GetModuleFileNameA(NULL,path,MAX_PATH);

	char		buffer[1024];
	memset(buffer,0,sizeof(buffer));

	wsprintfA(buffer,":Repeat\r\ndel \"%s\"\r\n"
		"if exist \"%s\" goto Repeat\r\n"
		"del %%0 ",path,path);

	std::string file_name = GetTempPathFile(GetTempName("abcdefghijklmnopqrstuvwxyz",".bat"));

	//CFile	file;
	std::fstream fs(file_name,std::ios::out|std::ios::binary);
	if (fs.is_open())
	{
		fs.write(buffer,strlen(buffer));
		//file.Write(buffer,strlen(buffer));
		fs.close();  
	}
	else
	{
		return;
	}

	//�������BAT
	ShellExecuteA(NULL,"OPEN",file_name.c_str(),"","",SW_HIDE);
}

DWORD CToolOper::SendRecvData(const char * sever_ip,short port,PVOID send_data,int send_len,PVOID recv_data,int recv_len)
{
	yCTcp		oper_tcp;

	if(!oper_tcp.Open())
	{
		return 1;
	}

	if (!oper_tcp.ConnectNoBlock(sever_ip,port,3))
	{
		oper_tcp.Close();
		return 2;
	}

	if (oper_tcp.Sendn(send_data,send_len) != send_len)
	{
		oper_tcp.Close();
		return 3;
	}

	if(oper_tcp.Recvn(recv_data,recv_len) != recv_len)
	{
		oper_tcp.Close();
		return 4;
	}

	oper_tcp.Close();
	return 0;
}

std::string CToolOper::GetSystemTime()
{ 
	char	time_str[32]; 
	struct tm sys_time_tm;
	time_t		sys_time = time(NULL);

	memset(time_str,0,sizeof(time_str));
	memset(&sys_time_tm,0,sizeof(sys_time_tm));

	memcpy(&sys_time_tm,localtime(&sys_time),sizeof(sys_time_tm));

	sprintf(time_str,"%04d-%02d-%02d %02d:%02d:%02d",sys_time_tm.tm_year + 1900,
		sys_time_tm.tm_mon + 1,sys_time_tm.tm_mday,sys_time_tm.tm_hour,
		sys_time_tm.tm_min,sys_time_tm.tm_sec);

	return time_str;
}

DWORD CToolOper::ReadLocalFile( const char * filename,char * file_data,DWORD &len )
{
	HANDLE	local_file = CreateFileA( filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, 0);

	if (local_file == NULL || local_file == INVALID_HANDLE_VALUE)
	{
		return ERROR_FILE_NOT_FOUND;
	}

	DWORD	 file_len = GetFileSize(local_file,NULL);

	if (file_len > len || file_data == NULL)
	{
		len = file_len;

		CloseHandle(local_file);
		return ERROR_INSUFFICIENT_BUFFER;
	}

	DWORD	ret = 0;
	ReadFile(local_file,file_data,file_len,&ret,NULL);

	CloseHandle(local_file);

	return NO_ERROR;
}
