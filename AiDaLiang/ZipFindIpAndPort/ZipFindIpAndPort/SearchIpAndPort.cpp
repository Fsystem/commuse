#include "stdafx.h"
#include "regex/include/regex.h"
#include "SearchIpAndPort.h"
#include <strstream>
#include <map>
#include <algorithm>
#include <regex>
#include <string>

#pragma comment(lib,"regex/lib/regex.lib")

using namespace std;
//-------------------------------------------------------------------------------
#define RESULT_FILE_NAME "RET.TXT"
//-------------------------------------------------------------------------------
SearchIpAndPort::SearchIpAndPort()
{
	char szPath[MAX_PATH]={0};
	GetModuleFileNameA(NULL,szPath,MAX_PATH);
	strrchr(szPath,'\\')[1]=0;
	strcat(szPath,RESULT_FILE_NAME);
	fp = fopen(szPath,"a");
}

SearchIpAndPort::~SearchIpAndPort()
{
	fclose(fp);
}

std::vector<std::string> SearchIpAndPort::GetIpAndport(LPCSTR szFile,LPCSTR szFileInnerFile,LPCSTR szFileData,LPCSTR szKey)
{
	std::vector<string> res;
	std::map<string,int> mapRes,mapAnalysis;

	if (szFile && szFileInnerFile&&szFileData && szKey)
	{
		WriteStringFile("%s\r\n",szFile);
		WriteStringFile("\t%s\r\n",szFileInnerFile);

		std::string sFileData = szFileData;
		std::remove_if(sFileData.begin(),sFileData.end(),[](char e)->bool{return e=='\\';});
		std::remove_if(sFileData.begin(),sFileData.end(),[](char e)->bool{return e==' ';});
		std::istrstream fin(sFileData.c_str());
		char szLine[4096*10];
		std::string sMatch;

		while(fin.getline(szLine,sizeof(szLine)-1))
		{
			if(szKey[0]!=0)
			{
				if(stristr(szLine,szKey))
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
							}
						}

						s_ = sTmp.find(">");
						if (s_!= snpos) sTmp = sTmp.substr(s_,snpos);
						else break;
					}
				}
			}
			else
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
				//ȥ��html ������ �Լ���
				s_ = sUrl.find("://");
				if(s_!=snpos) sUrl = sUrl.substr(s_+3,snpos);
				//ȥ��·������
				s_ = sUrl.find("/");
				if(s_!=snpos) sUrl = sUrl.substr(0,s_);

				std::replace(sUrl.begin(),sUrl.end(),':',' ');

				if(mapAnalysis.find(sUrl)==mapAnalysis.end())
				{
					mapAnalysis[sUrl] = 0;
					WriteStringFile("\t\t%s\r\n",sUrl.c_str());
				}

				sOringinUrl += "������:";
				sUrl = sOringinUrl + sUrl;
				res.push_back(sUrl);

				

				/*s_ = sUrl.find(":");
				if (s_!=snpos)
				{
					sIp = sUrl.substr(0,s_);
					sUrl = sUrl.substr(s_+1,snpos);
					nPort = atoi(sUrl.c_str());
				}
				else
				{
					sIp = sUrl;
				}*/
			}

			/*if (sIp.empty()==false)
			{
				std::string s = sIp;
				if(nPort) {
					s += " ";
					s += std::to_string((LONGLONG)nPort);
				}

				res.push_back(s);
			}*/
		}

		if(res.size()==0)
		{
			//MessageBoxA(NULL,"û�йؼ��ֲ������ݣ�����������ؼ���","��ʾ",MB_OK);
		}

		
	}

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
//		//ƥ������
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
//		MessageBoxA(NULL,"û�йؼ��ֲ������ݣ�����������ؼ���","��ʾ",MB_OK);
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
//	//	//ƥ������
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
//		MessageBoxA(NULL,"û�йؼ��ֲ������ݣ�����������ؼ���","��ʾ",MB_OK);
//	}
//
//	return res;
//}
