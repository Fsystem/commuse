#pragma once
#include <string>
#include <WinInet.h>

class CFileDown_C
{
public:
	CFileDown_C(void);
	~CFileDown_C(void);

public:
	BOOL HttpGet(std::string url,std::string file_name);

	std::string HttpGet(std::string url);

	static void CALLBACK InternetCallback(HINTERNET hInternet
		, DWORD dwContext, DWORD dwInternetStatus
		, LPVOID lpvStatusInformation
		, DWORD dwStatusInformationLength);
};
