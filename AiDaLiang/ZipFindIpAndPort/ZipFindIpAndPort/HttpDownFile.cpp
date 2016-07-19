#include "stdafx.h"
#include "HttpDownFile.h"
#include <Wininet.h>
#include <atltrace.h>
#include "curl/curl.h"

#pragma comment(lib,"wininet.lib")


#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"wldap32.lib")
#ifdef _DEBUG
#pragma comment(lib,"curl/libcurl_a_debug.lib")
#else
#pragma comment(lib,"curl/libcurl_a.lib")
#endif

//-------------------------------------------------------------------------------
static bool bInitCUrl = false;
//-------------------------------------------------------------------------------
HttpDownFile::HttpDownFile()
{
	if (bInitCUrl == false)
	{
		curl_global_init(CURL_GLOBAL_DEFAULT);
		atexit(OnExitInstance);
		bInitCUrl = true;
	}
}

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

static size_t write_html_data(void* ptr,size_t isize,size_t icount,void* userdata)
{
	auto html = reinterpret_cast<std::vector<char>*> (userdata);
	std::copy((char*)ptr,(char*)ptr+isize*icount,std::back_inserter(*html));

	return isize*icount;
}

static size_t write_head_data(void* ptr,size_t isize,size_t icount,void* userdata)
{
	auto html = reinterpret_cast<std::string*> (userdata);
	std::copy((char*)ptr,(char*)ptr+isize*icount,std::back_inserter(*html));
	
	return isize*icount;
}

const std::vector<char>& HttpDownFile::OpentUrlByCURL(std::string sUrl)
{
	mRecievData.clear();

	std::string sHead;
	std::string sHtml;

	CURL* curl = curl_easy_init();

	if (curl)
	{
		curl_easy_setopt(curl,CURLOPT_URL,sUrl.c_str());//设置url
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);//设为不验证证书
		curl_easy_setopt(curl,CURLOPT_TIMEOUT,20);//设置超时
		//curl_easy_setopt(curl,CURLOPT_REDIR_PROTOCOLS,1);//设置重定向
		curl_easy_setopt(curl,CURLOPT_FOLLOWLOCATION,1);//递归抓取重定向
		
		curl_easy_setopt(curl, CURLOPT_HEADERDATA, &sHead);      //下载数据包  
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, write_head_data);      //下载数据包 
		curl_easy_setopt(curl,CURLOPT_WRITEDATA,&mRecievData);//设置写入数据流的类型
		curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,write_html_data);

		int err = curl_easy_perform(curl);
		if (CURLE_OK == err)
		{
			sHtml.assign(mRecievData.begin(),mRecievData.end());
		}
		else
		{
			mRecievData.clear();
		}

		curl_easy_cleanup(curl);
	}

	return mRecievData;
}

void HttpDownFile::OnExitInstance()
{
	curl_global_cleanup();
}

std::string HttpDownFile::DownFile(std::string sUrl)
{
	std::string sFileData;
	auto urlData = OpentUrlByCURL(sUrl);

	sFileData.assign(urlData.begin(),urlData.end());

	return sFileData;
}