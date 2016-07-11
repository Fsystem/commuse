#include "stdafx.h"
#include "regex/include/regex.h"
#include "SearchIpAndPort.h"
#include <strstream>
#include <map>
#include <algorithm>
#include <regex>
#include <direct.h>

#pragma comment(lib,"regex/lib/regex.lib")

using namespace std;

//-------------------------------------------------------------------------------
char SearchIpAndPort::szAppPath[MAX_PATH]={0};
char SearchIpAndPort::szErrorPath[MAX_PATH]={0};
//-------------------------------------------------------------------------------
SearchIpAndPort::SearchIpAndPort()
{
	if (szAppPath[0] == 0)
	{
		GetModuleFileNameA(NULL,szAppPath,MAX_PATH);
		strrchr(szAppPath,'\\')[1]=0;

		strncpy(szErrorPath,szAppPath,MAX_PATH);
		strcat(szErrorPath,"error\\");
	}
	if(gAnalysisMode==0 || gAnalysisMode==1)
		sprintf(szFileName,"%sRET%09d.TXT",szAppPath,gFileIndex);
	else
		sprintf(szFileName,"%sRETSingle.TXT",szAppPath);
	fp = fopen(szFileName,"a");
	//nErrorFileCount = 0;
	nResultCnt = 0;
}

SearchIpAndPort::~SearchIpAndPort()
{
	if(fp){
		fclose(fp);
		fp = NULL;
	}

	if (nResultCnt == 0)
	{
		DeleteFileA(szFileName);
	}
}

void SearchIpAndPort::AddKey(LPCSTR szKey)
{
	if( szKey== NULL) return;
	sKeys.push_back(szKey);
}

bool SearchIpAndPort::ExistKey(LPCSTR szContent)
{
	if(szContent==NULL) return false;
	std::string sContent = szContent;
	bool bFind = false;
	for (auto it = sKeys.begin(); it != sKeys.end() ; it++)
	{
		bFind = (stristr(szContent,it->c_str())!=NULL);
		if(bFind) break;
	}

	return bFind;
}

std::vector<std::string> SearchIpAndPort::GetIpAndport(LPCSTR szFile,LPCSTR szFileInnerFile,LPCSTR szFileData)
{
	std::vector<string> res;
	std::map<string,int> mapRes,mapAnalysis;

	//bool bKeywordAvalid = false;

	static DWORD dwCur =0;
	dwCur = GetTickCount();
	if (szFile && szFileInnerFile&&szFileData )
	{
		//WriteStringFile("%s\r\n",szFile);
		//WriteStringFile("\t%s\r\n",szFileInnerFile);

		std::string sFileData = szFileData;
		std::remove_if(sFileData.begin(),sFileData.end(),[](char e)->bool{return e=='\\';});
		std::remove_if(sFileData.begin(),sFileData.end(),[](char e)->bool{return e==' ';});
		std::istrstream fin(sFileData.c_str());
		char szLine[4096*10];
		std::string sMatch;

		while(fin.getline(szLine,sizeof(szLine)-1))
		{
			if(sKeys.size()>0)
			{
				if(ExistKey(szLine))
				{
					size_t s_,snpos = std::string::npos;
					std::string sTmp = szLine;
					while((s_=sTmp.find("<a")) != snpos)
					{
						sTmp = sTmp.substr(s_,snpos);
						s_ = sTmp.find("href=");
						if(s_ != snpos){
							char chFind = sTmp[s_+5];
							sTmp = sTmp.substr(s_+6,snpos);
							s_ = sTmp.find(chFind);
							if(s_!=snpos)
							{
								sMatch = sTmp.substr(0,s_);
								mapRes[sMatch] = 0;

								//bKeywordAvalid = true;
							}
						}

						s_ = sTmp.find(">");
						if (s_!= snpos) sTmp = sTmp.substr(s_,snpos);
						else break;
					}
				}
			}
			else //无关键字
			{
				size_t s_,snpos = std::string::npos;
				std::string sTmp = szLine;
				while((s_=sTmp.find("<a")) != snpos)
				{
					sTmp = sTmp.substr(s_,snpos);
					s_ = sTmp.find("href=");
					if(s_ != snpos){
						char chFind = sTmp[s_+5];
						sTmp = sTmp.substr(s_+6,snpos);
						s_ = sTmp.find(chFind);
						if(s_!=snpos)
						{
							sMatch = sTmp.substr(0,s_);
							mapRes[sMatch] = 0;

							//bKeywordAvalid = true;
						}
					}

					s_ = sTmp.find(">");
					if (s_!= snpos) sTmp = sTmp.substr(s_,snpos);
					else break;
				}
			}
		}

		for (auto itMap = mapRes.begin();itMap!=mapRes.end();itMap++)
		{
			std::string sUrl = itMap->first,sUrlParam;
			std::string sOringinUrl = sUrl;
			int nPort = 0;
			std::string sIp;
			size_t s_ = sUrl.find("?"),snpos = std::string::npos;
			if (s_ != snpos)
			{
				sUrlParam = sUrl.substr(s_,snpos);
				int nAdd = 3;
				s_ = sUrlParam.find("://");
				if (s_ == snpos){
					s_ = sUrlParam.find("=");
					nAdd = 1;
				}
				if (s_ != snpos)
				{
					sUrlParam = sUrlParam.substr(s_+nAdd,snpos);
					s_ = sUrlParam.find("/");
					if (s_ != snpos)
					{
						sUrlParam = sUrlParam.substr(0,s_);
					}
					sUrl = sUrlParam;
				}
			}

			if (sUrl.empty() == false && sOringinUrl.find("://")!=snpos)
			{
				//去除html 超链接 自己的
				s_ = sUrl.find("://");
				if(s_!=snpos) sUrl = sUrl.substr(s_+3,snpos);
				//去除路径部分
				s_ = sUrl.find("/");
				if(s_!=snpos) sUrl = sUrl.substr(0,s_);

				//std::replace(sUrl.begin(),sUrl.end(),':',' ');

				if(mapAnalysis.find(sUrl)==mapAnalysis.end())
				{
					mapAnalysis[sUrl] = 0;
					WriteStringFile("%s\r\n",sUrl.c_str());

					sOringinUrl += "解析后:";
					sUrl = sOringinUrl + sUrl;
					res.push_back(sUrl);
				}
			}
		}
	}

	//拷贝错误文件到error文件夹下
	if(res.size()==0)
	{
		_mkdir( szErrorPath);
		char szFileName[MAX_PATH]={0};
		strncat(szFileName,szErrorPath,MAX_PATH);
		strncat(szFileName,strrchr(szFile,'\\')+1,MAX_PATH);
		CopyFileA(szFile,szFileName,FALSE);
		//nErrorFileCount++;
	}

	dwCur = GetTickCount()-dwCur;

	nResultCnt += res.size();
	
	return res;
}

void SearchIpAndPort::WriteStringFile(LPCSTR format,...)
{
	if (fp)
	{
		char szBuffer[4096]={0};
		va_list vl;
		va_start(vl,format);
		vsprintf(szBuffer,format,vl);
		va_end(vl);
		fwrite(szBuffer,1,strlen(szBuffer),fp);
		fflush(fp);
	}
}


//std::vector<std::string> SearchIpAndPort::GetIpAndport(LPCSTR szFileData,LPCSTR szKey)
//{
//	std::vector<string> res;
//	std::map<string,int> mapRes;
//
//	if (szFileData && szKey)
//	{
//		//char pattern[512]="(http(s)?+://)(([a-zA-Z0-9\\._-]+\\.[a-zA-Z]{2,6})|([0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}))(:[0-9]{1,4})*(/[a-zA-Z0-9\\&%_\\./-~-]*)?";
//		//char pattern[]="<a(?: [^>]*)+href=([^ >]*)(?: [^>]*)*>";
//		char pattern[]="<a[^>]+>/?";
//		//char pattern[512]="[a-zA-z]+://[^\s]*";
//		char patternDomain[]="[://]([\\w-]+\\.)+[\\w-]+(:\\d{0,5})?/?";
//		//char patternDomain[]="(://)([0-9A-Za-z\\-_\\.]+)\\.([0-9a-z]+\\.[a-z]{2,3}(\\.[a-z]{2})?)+(:\\d{0,5})?/?";
//		const size_t nmatch = 100;
//		regmatch_t pm[nmatch];
//		regex_t reg;
//
//		regcomp(&reg,pattern,REG_EXTENDED|REG_ICASE);
//
//		int nErr = regexec(&reg,szFileData,nmatch,pm,REG_NOTBOL);
//
//		std::istrstream fin(szFileData);
//		char szLine[4096*10];
//		std::string sMatch;
//		while(fin.getline(szLine,sizeof(szLine)-1))
//		{
//			if(szKey[0]!=0)
//			{
//				if(stristr(szLine,szKey))
//				{
//					int nErr = regexec(&reg,szLine,nmatch,pm,REG_NOTBOL);
//					if (nErr == REG_NOMATCH) continue;
//					if(nErr == 0)
//					{
//						for (int k=0;k<_countof(pm);k++)
//						{
//							sMatch.assign((char*)szLine+pm[k].rm_so,(char*)szLine+pm[k].rm_eo);
//							mapRes[sMatch] = 0;
//						}
//
//					}
//				}
//			}
//			else
//			{
//				int nErr = regexec(&reg,szLine,nmatch,pm,REG_NOTBOL);
//				if (nErr == REG_NOMATCH) continue;
//				if(nErr == 0)
//				{
//					for (int k=0;k<_countof(pm);k++)
//					{
//						sMatch.assign((char*)szLine+pm[k].rm_so,(char*)szLine+pm[k].rm_eo);
//						mapRes[sMatch] = 0;
//					}
//				}
//			}
//
//		}
//
//		regfree(&reg);
//
//		//匹配域名
//		regcomp(&reg,patternDomain,REG_EXTENDED|REG_ICASE);
//		std::string sLineDomain;
//		for (auto itMap = mapRes.begin();itMap != mapRes.end();itMap++)
//		{
//			int nErr = regexec(&reg,itMap->first.c_str(),nmatch,pm,REG_NOTBOL);
//			if (nErr == REG_NOMATCH) continue;
//			if(nErr == 0)
//			{
//				sLineDomain = "";
//				for (int k=0;k<_countof(pm);k++)
//				{
//					if (pm[k].rm_so!=-1 && pm[k].rm_eo!=-1)
//					{
//						sMatch.assign(itMap->first.begin()+pm[k].rm_so,itMap->first.begin()+pm[k].rm_eo);
//						std::remove_if(sMatch.begin(),sMatch.end(),[](char c)->bool{return c=='/' || c=='\\';});
//						sLineDomain +=sMatch;
//						sLineDomain += " ";
//					}
//				}
//
//				if (!sLineDomain.empty())
//				{
//					res.push_back(sLineDomain);
//				}
//
//			}
//		}
//
//		regfree(&reg);
//	}
//
//	if(res.size()==0)
//	{
//		MessageBoxA(NULL,"没有关键字部分内容，请重新输入关键字","提示",MB_OK);
//	}
//
//	return res;
//}


//std::vector<std::string> SearchIpAndPort::GetIpAndport(LPCSTR szFileData,LPCSTR szKey)
//{
//	std::vector<string> res;
//	std::map<string,int> mapRes;
//
//	if (szFileData && szKey)
//	{
//		
//		//char pattern[512]="(http(s)?+://)(([a-zA-Z0-9\\._-]+\\.[a-zA-Z]{2,6})|([0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}))(:[0-9]{1,4})*(/[a-zA-Z0-9\\&%_\\./-~-]*)?";
//		char pattern[]="/<a(?: [^>]*)+href=([^ >]*)(?: [^>]*)*>/";
//		//char pattern[512]="[a-zA-z]+://[^\s]*";
//		//char patternDomain[]="[://]([\\w-]+\\.)+[\\w-]+(:\\d{0,5})?/?";
//		//char patternDomain[]="(://)([0-9A-Za-z\\-_\\.]+)\\.([0-9a-z]+\\.[a-z]{2,3}(\\.[a-z]{2})?)+(:\\d{0,5})?/?";
//	
//		std::regex rgx("<a[^>]+>",std::regex_constants::extended);
//		std::cmatch match;
//
//		std::string sFileData = szFileData;
//		std::remove_if(sFileData.begin(),sFileData.end(),[](char e)->bool{return e=='\\';});
//		//std::remove_if(sFileData.begin(),sFileData.end(),[](char e)->bool{return e==' ';});
//		std::istrstream fin(sFileData.c_str());
//		char szLine[4096*10],szUrl[1024];
//		std::string sMatch;
//
//		std::regex_search(sFileData.c_str(),match,rgx,std::regex_constants::match_not_bol);
//
//		while(fin.getline(szLine,sizeof(szLine)-1))
//		{
//			if(szKey[0]!=0)
//			{
//				if(stristr(szLine,szKey))
//				{
//					sscanf(szLine,"*herf=\"%s\"*",szUrl);
//					//std::regex_search(szLine,match,rgx);
//					/*int nErr = regexec(&reg,szLine,nmatch,pm,REG_NOTBOL);
//					if (nErr == REG_NOMATCH) continue;
//					if(nErr == 0)
//					{
//						for (int k=0;k<_countof(pm);k++)
//						{
//							sMatch.assign((char*)szLine+pm[k].rm_so,(char*)szLine+pm[k].rm_eo);
//							mapRes[sMatch] = 0;
//						}
//
//					}*/
//				}
//			}
//			else
//			{
//				sscanf(szLine,"*herf=\"%s\"*",szUrl);
//				//std::regex_search(szLine,match,rgx);
//				/*int nErr = regexec(&reg,szLine,nmatch,pm,REG_NOTBOL);
//				if (nErr == REG_NOMATCH) continue;
//				if(nErr == 0)
//				{
//					for (int k=0;k<_countof(pm);k++)
//					{
//						sMatch.assign((char*)szLine+pm[k].rm_so,(char*)szLine+pm[k].rm_eo);
//						mapRes[sMatch] = 0;
//					}
//				}*/
//			}
//
//		}
//
//	//	regfree(&reg);
//
//	//	//匹配域名
//	//	regcomp(&reg,patternDomain,REG_EXTENDED|REG_ICASE);
//	//	std::string sLineDomain;
//	//	for (auto itMap = mapRes.begin();itMap != mapRes.end();itMap++)
//	//	{
//	//		int nErr = regexec(&reg,itMap->first.c_str(),nmatch,pm,REG_NOTBOL);
//	//		if (nErr == REG_NOMATCH) continue;
//	//		if(nErr == 0)
//	//		{
//	//			sLineDomain = "";
//	//			for (int k=0;k<_countof(pm);k++)
//	//			{
//	//				if (pm[k].rm_so!=-1 && pm[k].rm_eo!=-1)
//	//				{
//	//					sMatch.assign(itMap->first.begin()+pm[k].rm_so,itMap->first.begin()+pm[k].rm_eo);
//	//					std::remove_if(sMatch.begin(),sMatch.end(),[](char c)->bool{return c=='/' || c=='\\';});
//	//					sLineDomain +=sMatch;
//	//					sLineDomain += " ";
//	//				}
//	//			}
//
//	//			if (!sLineDomain.empty())
//	//			{
//	//				res.push_back(sLineDomain);
//	//			}
//
//	//		}
//	//	}
//
//	//	regfree(&reg);
//	}
//
//	if(res.size()==0)
//	{
//		MessageBoxA(NULL,"没有关键字部分内容，请重新输入关键字","提示",MB_OK);
//	}
//
//	return res;
//}
