#include "stdafx.h"
#include "HttpDownFile.h"
#include <Wininet.h>
#include <atltrace.h>


#pragma comment(lib,"wininet.lib")

const std::vector<char>& HttpDownFile::OpentUrl(std::string sUrl)
{
	mRecievData.clear();

	if (sUrl.find("://") == std::string::npos)
	{
		sUrl = "http://"+sUrl;
	}

	HINTERNET hNet = ::InternetOpenA("HttpDownFile",
		PRE_CONFIG_INTERNET_ACCESS,
		NULL,
		INTERNET_INVALID_PORT_NUMBER,
		0) ;
	if (hNet) 
	{
		HINTERNET hUrlFile = ::InternetOpenUrlA(hNet,
			sUrl.c_str(),
			NULL,
			0,
			INTERNET_FLAG_RELOAD|INTERNET_FLAG_NO_AUTO_REDIRECT,
			0) ;
		if (hUrlFile)
		{
			char buffer[1024] ;
			DWORD dwBytesRead = 0;
			while(::InternetReadFile(hUrlFile,buffer,sizeof(buffer),&dwBytesRead))
			{
				if(dwBytesRead == 0) break;
				mRecievData.insert(mRecievData.end(),buffer,buffer+dwBytesRead);
			}
			::InternetCloseHandle(hUrlFile) ;
		}
		else
		{
			DWORD dwError = GetLastError();
			printf("%u\n",dwError);
		}

		::InternetCloseHandle(hNet) ;
	}

	return mRecievData;
}