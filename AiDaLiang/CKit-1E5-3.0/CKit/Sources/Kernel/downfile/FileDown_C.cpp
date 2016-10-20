#include "StdAfx.h"
#include "FileDown_C.h"

#pragma comment(lib,"wininet.lib")

#define		MAX_LEN		4096
#define		HTTP_TIME_OUT	20000

typedef	struct	
{
	DWORD_PTR	CompleteResult;
	DWORD	dwError;
	HANDLE	comp_event;
}HTTP_INFO,*PHTTP_INFO;

CFileDown_C::CFileDown_C(void)
{
}

CFileDown_C::~CFileDown_C(void)
{
}



std::string CFileDown_C::HttpGet(std::string url) 
{
	std::string		ret_string;

	HINTERNET hInet = NULL;
	HINTERNET hUrl = NULL;


	hInet = InternetOpenA(NULL,INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,INTERNET_FLAG_ASYNC);
	if (hInet == NULL)
	{
		return "";
	}

	InternetSetStatusCallback(hInet,CFileDown_C::InternetCallback);

	HTTP_INFO		info;
	info.CompleteResult = 0;
	info.comp_event = CreateEvent(NULL,FALSE,FALSE,NULL);

	hUrl = InternetOpenUrlA(hInet,url.c_str(),NULL,0,INTERNET_FLAG_TRANSFER_BINARY|INTERNET_FLAG_NO_CACHE_WRITE|INTERNET_FLAG_RELOAD,(DWORD_PTR)&info);

	if (hUrl == NULL)
	{
		if (GetLastError() == ERROR_IO_PENDING)
		{
			if (WaitForSingleObject(info.comp_event,HTTP_TIME_OUT) != WAIT_OBJECT_0 || info.dwError != 0)
			{
				InternetCloseHandle(hInet);
				return "";			 
			}

			hUrl = (HINTERNET)info.CompleteResult;
		}
		else
		{
			InternetCloseHandle(hInet);
			return "";
		}
	}

	DWORD dwLen = MAX_PATH;
	char	http_status[MAX_PATH];

	memset(http_status,0,sizeof(http_status)); 
	HttpQueryInfo(hUrl, HTTP_QUERY_STATUS_CODE, http_status, &dwLen, 0);

	if (_stricmp(http_status,"200") != 0)
	{
		InternetCloseHandle(hUrl);
		InternetCloseHandle(hInet);
		return "";		
	}
 
	memset(http_status,0,sizeof(http_status)); 
	dwLen = MAX_PATH;
	HttpQueryInfo(hUrl, HTTP_QUERY_CONTENT_LENGTH, http_status, &dwLen, 0);

	DWORD		net_file_len = atol(http_status);

	char	buffer[MAX_LEN];
	memset(buffer,0,sizeof(buffer));
	DWORD readBytes;
	DWORD	ret = 0;
	DWORD	dwRead = 0;

	while (net_file_len == 0 || net_file_len != dwRead)
	{
		readBytes = 0; 
		if(!InternetReadFile(hUrl, buffer, MAX_LEN - 1, &readBytes))
		{
			if (GetLastError() == ERROR_IO_PENDING)
			{
				if (WaitForSingleObject(info.comp_event,HTTP_TIME_OUT) != WAIT_OBJECT_0 || info.dwError != 0)
				{
					InternetSetStatusCallback(hUrl,NULL);
					InternetSetStatusCallback(hInet,NULL);
					break;		 
				}
			}
		}

		buffer[readBytes] = '\0';
		ret_string += buffer;

		dwRead += readBytes;

		memset(buffer,0,sizeof(buffer));
	}  
 
	InternetCloseHandle(hUrl);
	InternetCloseHandle(hInet);
	return ret_string;
}

void CALLBACK CFileDown_C::InternetCallback(HINTERNET hInternet,
									  DWORD dwContext,
									  DWORD dwInternetStatus,
									  LPVOID lpvStatusInformation,
									  DWORD dwStatusInformationLength)
{
	if (dwInternetStatus == INTERNET_STATUS_REQUEST_COMPLETE)
	{
		INTERNET_ASYNC_RESULT *Result = (INTERNET_ASYNC_RESULT*)lpvStatusInformation;
		HTTP_INFO *p = (HTTP_INFO*)dwContext;
		p->CompleteResult = Result->dwResult;
		p->dwError = Result->dwError;
		SetEvent(p->comp_event);
	}
}


BOOL CFileDown_C::HttpGet(std::string url,std::string file_name) 
{
	HINTERNET hInet = NULL;
	HINTERNET hUrl = NULL;

	
	hInet = InternetOpenA(NULL,INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,INTERNET_FLAG_ASYNC);
	if (hInet == NULL)
	{
		return FALSE;
	}

	InternetSetStatusCallback(hInet,CFileDown_C::InternetCallback);

	HTTP_INFO		info;
	info.CompleteResult = 0;
	info.comp_event = CreateEvent(NULL,FALSE,FALSE,NULL);

	hUrl = InternetOpenUrlA(hInet,url.c_str(),NULL,0,INTERNET_FLAG_TRANSFER_BINARY|INTERNET_FLAG_NO_CACHE_WRITE|INTERNET_FLAG_RELOAD,(DWORD_PTR)&info);

	if (hUrl == NULL)
	{
		if (GetLastError() == ERROR_IO_PENDING)
		{
			 if (WaitForSingleObject(info.comp_event,HTTP_TIME_OUT) != WAIT_OBJECT_0 || info.dwError != 0)
			 {
				 InternetSetStatusCallback(hInet,NULL);
				 InternetCloseHandle(hInet);
				 return FALSE;			 
			 }

			 hUrl = (HINTERNET)info.CompleteResult;
		}
		else
		{
			InternetCloseHandle(hInet);
			return FALSE;
		}
	}
 
	DWORD dwLen = MAX_PATH;
	char	http_status[MAX_PATH];

	memset(http_status,0,sizeof(http_status)); 
	HttpQueryInfo(hUrl, HTTP_QUERY_STATUS_CODE, http_status, &dwLen, 0);

	if (_stricmp(http_status,"200") != 0)
	{
		InternetCloseHandle(hUrl);
		InternetCloseHandle(hInet);
		return FALSE;		
	}

	memset(http_status,0,sizeof(http_status)); 
	dwLen = MAX_PATH;
	HttpQueryInfo(hUrl, HTTP_QUERY_CONTENT_LENGTH, http_status, &dwLen, 0);

	DWORD		net_file_len = atol(http_status);
	 

	HANDLE hCreatefile;
	hCreatefile = CreateFileA(file_name.c_str(), GENERIC_WRITE|GENERIC_READ, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if(INVALID_HANDLE_VALUE == hCreatefile || NULL == hCreatefile)
	{ 
		return FALSE;
	}

	char	buffer[MAX_LEN];
	memset(buffer,0,sizeof(buffer));
	DWORD readBytes;
	DWORD	ret = 0;
	DWORD	dwRead = 0;

	while (net_file_len == 0 || net_file_len != dwRead)
	{
		readBytes = 0; 
		if(!InternetReadFile(hUrl, buffer, MAX_LEN, &readBytes))
		{
			if (GetLastError() == ERROR_IO_PENDING)
			{
				if (WaitForSingleObject(info.comp_event,HTTP_TIME_OUT) != WAIT_OBJECT_0 || info.dwError != 0)
				{
					InternetSetStatusCallback(hUrl,NULL);
					InternetSetStatusCallback(hInet,NULL);
					break;		 
				}
			}
		}

		WriteFile(hCreatefile,buffer,readBytes,&ret,NULL);

		dwRead += readBytes;

		memset(buffer,0,sizeof(buffer));
	} 

	CloseHandle(hCreatefile);
	InternetCloseHandle(hUrl);
	InternetCloseHandle(hInet);
	return TRUE;
}