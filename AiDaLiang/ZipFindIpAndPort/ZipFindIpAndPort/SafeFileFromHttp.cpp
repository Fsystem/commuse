#include "stdafx.h"
#include "SafeFileFromHttp.h"
#include <fstream>

static std::string sConfig;
static std::string sLocalDownDir;
static int nTotal;
static int nCurCnt;

int SaveFileFromHttp::GetTotal()
{
	return nTotal;
}
int SaveFileFromHttp::GetCurCnt()
{
	return nCurCnt;
}
bool SaveFileFromHttp::IsDownFinish()
{
	return nCurCnt == nTotal;
}

void SaveFileFromHttp::DownFromHttp(std::string sConfig_,std::string sLocalDownDir_)
{
	sConfig = sConfig_;
	sLocalDownDir = sLocalDownDir_;
	std::ifstream infile(sConfig.c_str(),std::ifstream::in);

	nTotal = 0;
	nCurCnt = 0;
	
	if (infile.is_open())
	{
		std::string szLine;
		while(std::getline(infile,szLine,'\n'))
		{
			if(szLine.length()==0) break;
			char* pData = new char[szLine.size()+1];
			memset(pData,0,szLine.size()+1);
			strncpy(pData,szLine.data(),szLine.size());
			HANDLE hThr = (HANDLE)JKThread<SaveFileFromHttp>::Start(&SaveFileFromHttp::DownThread,pData);
			//if( WaitForSingleObject(hThr,6000) == WAIT_TIMEOUT)
			//{
			//	::TerminateThread(hThr,1);
			//	CloseHandle(hThr);
			//}

			nTotal ++ ;
		}
	}
	
}

void SaveFileFromHttp::DownThread(void* p)
{
	std::string sUrl = (char*)p;
	delete p;
	CCRC crc;
	HttpDownFile httpDown;
	auto fileData = httpDown.OpentUrl(sUrl);
	std::string sData ;
	sData.assign(fileData.begin(),fileData.end());
	std::string sCrc = crc.GetCrcHexString(sData.c_str(),sData.size());

	std::string sZipFile = sLocalDownDir;
	sZipFile += sCrc;
	sZipFile += ".zip";

	int nZip = OpenZip(sZipFile.c_str(),NULL);
	if (nZip)
	{
		if ( sData.size()>0)
		{
			AddUpdateFile(nZip,"web.html",(char*)sData.c_str(),sData.size());
			UpdateZipFile(nZip);
		}

		CloseZip(nZip);
	}

	InterlockedExchangeAdd((long*)&nCurCnt,1);

	if (nCurCnt == nTotal)
	{
		PostMessage(gMainHwnd,WM_USER+1000,0,0);
	}
}