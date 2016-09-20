#include "stdafx.h"
#include "HttpDownFile.h"
#include <string>
#include <strstream>
#include <fstream>
#include <iterator>

#define CountArr(arr) (sizeof(arr)/sizeof(arr[0]))

HttpDownFile::HttpDownFile()
{
	mInet = NULL;
	mHinetConn = NULL;

	mInet = InternetOpenA("MSIE8.0",INTERNET_OPEN_TYPE_DIRECT,NULL,NULL,0);
}

HttpDownFile::~HttpDownFile()
{
	if(mHinetConn) 
	{
		InternetCloseHandle(mHinetConn);
		mHinetConn = NULL;
	}

	if (mInet)
	{
		InternetCloseHandle(mInet);
		mInet = NULL;
	}
}

int HttpDownFile::SetServer(char * ServerIp, INTERNET_PORT ServerPort)
{
	if (ServerIp == NULL || ServerPort == 0)
	{
		return 1;
	}

	mHttpRequestFlags = HSR_DOWNLOAD | INTERNET_FLAG_EXISTING_CONNECT |INTERNET_FLAG_NO_AUTO_REDIRECT|INTERNET_FLAG_NO_CACHE_WRITE ;
	mHttpsRequestFlags = INTERNET_FLAG_NO_AUTO_REDIRECT |  
		INTERNET_FLAG_KEEP_CONNECTION |  
		INTERNET_FLAG_NO_AUTH |  
		INTERNET_FLAG_NO_COOKIES |  
		INTERNET_FLAG_NO_UI |  
		//设置启用HTTPS  
		INTERNET_FLAG_SECURE |  
		INTERNET_FLAG_IGNORE_CERT_CN_INVALID|  
		INTERNET_FLAG_RELOAD;  

	INTERNET_PER_CONN_OPTION_LIST    List; 
	INTERNET_PER_CONN_OPTION         Option[3]; 

	Option[0].dwOption = INTERNET_OPTION_CONNECT_TIMEOUT;
	Option[0].Value.dwValue = 1000;

	Option[1].dwOption = INTERNET_OPTION_DATA_RECEIVE_TIMEOUT;
	Option[1].Value.dwValue = 1000;

	Option[2].dwOption = INTERNET_OPTION_DATA_SEND_TIMEOUT;
	Option[2].Value.dwValue = 1000;

	List.dwSize = sizeof(INTERNET_PER_CONN_OPTION_LIST); 
	List.pszConnection = NULL; 
	List.dwOptionCount = CountArr(Option); 
	List.dwOptionError = 0; 
	List.pOptions = Option; 

	InternetSetOptionA(mInet,INTERNET_OPTION_PER_CONNECTION_OPTION,&List,List.dwOptionCount);

	if(mHinetConn) {
		InternetCloseHandle(mHinetConn);
		mHinetConn = NULL;
	}

	mHinetConn = InternetConnectA(mInet,ServerIp,ServerPort,NULL,NULL,INTERNET_SERVICE_HTTP,0,NULL);

	return 0;
}

DWORD HttpDownFile::TryUrlDownFile(std::string url,std::string localFile,int nTryCnt)
{
	for (int i = 0; i != nTryCnt; i++)
	{
		if(UrlDownFile(url,localFile) == 0)
		{
			return 0;
		}
	}

	return 1;
}

DWORD HttpDownFile::UrlDownFile(std::string url,std::string localFile)
{
	std::vector<char> v = DownServerFile(url.c_str());

	std::ofstream of(localFile.c_str(),std::ios::out|std::ios::binary);

	if (of.is_open())
	{
		std::copy(v.begin(),v.end(),std::ostream_iterator<char>(of));
	}

	of.close();

	return v.size()==0?1:0;
}

std::string HttpDownFile::UrlDownFile(std::string url)
{
	std::string sRes="";

	std::vector<char> v = DownServerFile(url.c_str());

	sRes.assign(v.begin(),v.end());

	return sRes;
}

// 下载一个文件下来
std::vector<char> HttpDownFile::DownServerFile(const char* url,DWORD dwBegin,DWORD dwEnd)
{
	std::vector<char> vRes;

	URL_COMPONENTSA		url_info;
	memset(&url_info,0,sizeof(url_info));
	url_info.dwStructSize = sizeof(URL_COMPONENTS);
	char	hostname[1024];
	char	szurlpath[1024];

	memset(hostname,0,sizeof(hostname));
	memset(szurlpath,0,sizeof(szurlpath));
	url_info.lpszHostName = hostname;
	url_info.dwHostNameLength = 1024;
	url_info.lpszUrlPath = szurlpath;
	url_info.dwUrlPathLength = 1024;

	if (!InternetCrackUrlA(url,(DWORD)strlen(url),0,&url_info))
	{
		return vRes;
	}

	int nRes = SetServer(url_info.lpszHostName,url_info.nPort);
	mLastErr = GetLastError();
	if(nRes != 0) return vRes;

	//-------------------------------------------------------------------------------
	if (!mHinetConn)
	{
		return vRes;
	}

	char* szAccept[]={"*/*",NULL};
	DWORD dwFlag = mHttpRequestFlags;
	if(url_info.nScheme == INTERNET_SCHEME_HTTPS) dwFlag = mHttpsRequestFlags;
	HINTERNET hRequest = HttpOpenRequestA(mHinetConn,"GET",url_info.lpszUrlPath,HTTP_VERSIONA,url,(LPCSTR*)szAccept,dwFlag,0);
	mLastErr = GetLastError();
	if (hRequest)
	{
		DWORD dwFixedLen = 0;
		std::string	strHeaders;
		char szNum[32];
		if (dwBegin>0 || dwEnd>0)
		{
			strHeaders = "RANGE: bytes=";
			if(dwEnd>0)
			{
				strHeaders += itoa(dwBegin,szNum,10);
				strHeaders += "-";
				strHeaders += itoa(dwEnd,szNum,10);
				dwFixedLen = dwEnd - dwBegin +1;
			}
			else
			{
				strHeaders += itoa(dwBegin,szNum,10);
				strHeaders += "-";
			}

			strHeaders += "\r\n";
		}

		BOOL bSendReq = true;
		if (strHeaders.empty()==false)
		{
			bSendReq = HttpAddRequestHeadersA(hRequest,strHeaders.c_str(),strHeaders.length(),HTTP_ADDREQ_FLAG_ADD|HTTP_ADDREQ_FLAG_REPLACE);
		}
		
		if(bSendReq)
		{
			if (HttpSendRequestA(hRequest,NULL,0,NULL,0))
			{
				char szErr[1024]={0};
				DWORD dwszErr = CountArr(szErr);
				HttpQueryInfoA(hRequest,HTTP_QUERY_STATUS_CODE,szErr,&dwszErr,NULL);
				if (strcmp(szErr,"200")==0 || strcmp(szErr,"206")==0)
				{
					dwszErr = CountArr(szErr);
					if( !HttpQueryInfoA(hRequest,HTTP_QUERY_CONTENT_LENGTH,szErr,&dwszErr,NULL) )
					{
						mLastErr = GetLastError();

						char szBufferr[1024];
						while( true )
						{
							DWORD dwReaded = 0;
							if( InternetReadFile(hRequest,szBufferr,1024,&dwReaded) == FALSE) break;
							if(dwReaded == 0) break;
							std::copy(szBufferr,szBufferr+dwReaded,std::back_inserter(vRes));
						}
					}
					else
					{
						int nDataLen = atoi(szErr);
						if(dwFixedLen>0) nDataLen = dwFixedLen;
						int nRead = 0;
						char szBufferr[1024];
						while(nRead < nDataLen )
						{
							DWORD dwReaded = 0;
							if( InternetReadFile(hRequest,szBufferr,1024,&dwReaded) == FALSE) break;
							nRead += dwReaded;
							std::copy(szBufferr,szBufferr+dwReaded,std::back_inserter(vRes));
						}
					}
					
				}
			}
		}
		
		mLastErr = GetLastError();

		InternetCloseHandle(hRequest);
	}

	return vRes;
}

DWORD HttpDownFile::UrlDownFileForData(std::string url,char* buffer,DWORD buffer_len,DWORD &file_len,DWORD dwbgein,DWORD dwend)
{
	std::vector<char> v = DownServerFile(url.c_str(),dwbgein,dwend);
	file_len = v.size();
	if (buffer && buffer_len>0)
	{
		memcpy(buffer,&v[0],buffer_len);
		//std::copy_n(v.begin(),buffer_len,buffer);
	}

	return 0;
}