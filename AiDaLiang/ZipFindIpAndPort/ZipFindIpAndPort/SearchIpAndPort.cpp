#include "stdafx.h"
#include "SearchIpAndPort.h"


using namespace std;

//-------------------------------------------------------------------------------
char SearchIpAndPort::szAppPath[MAX_PATH]={0};
char SearchIpAndPort::szErrorPath[MAX_PATH]={0};
char cbInsteadSpace='|';
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
	if( szKey== NULL || szKey[0] == 0) return;
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
		//std::remove_if(sFileData.begin(),sFileData.end(),[](char e)->bool{return e==' ';});
		std::replace(sFileData.begin(),sFileData.end(),' ',cbInsteadSpace);
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
							size_t s_1 = sTmp.find(cbInsteadSpace);
							if (s_1!=snpos && s_1<s_) s_ = s_1;
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
			else //�޹ؼ���
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
						size_t s_1 = sTmp.find(cbInsteadSpace);
						if (s_1!=snpos && s_1<s_) s_ = s_1;
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
				size_t s_1 = s_;
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
				else//ɾ����֮�����ò���
				{
					sUrl = sUrl.substr(0,s_1);
				}
			}

			if (sUrl.empty() == false && sOringinUrl.find("://")!=snpos)
			{
				sUrl = urldecode(sUrl);
				//ȥ��html ������ �Լ���
				s_ = sUrl.find("://");
				if(s_!=snpos) sUrl = sUrl.substr(s_+3,snpos);
				//ȥ��·������
				s_ = sUrl.find("/");
				if(s_!=snpos) sUrl = sUrl.substr(0,s_);

				s_ = sUrl.find(".");
				if(s_==snpos) continue;

				std::string sStart = sUrl;
				bool bFinded = false;
				while((s_=sStart.find("=")) != snpos)
				{
					std::string sOld = sStart.substr(0,s_);
					if(IsUrl(sOld)) {
						sUrl = sOld;
						bFinded = true;
						break;
					}
					sStart = sStart.substr(s_+1,snpos);
				}

				if(bFinded == false) sUrl = sStart;

				if(IsUrl(sUrl)==false) continue;

				//ɾ�������url�ַ����綺�ŵ�
				auto itFinded = std::find_if(sUrl.begin(),sUrl.end(),[](char c)->bool{
					return c == ',' || c == '��' || c == '\'' || c == '\"';
				});

				if (itFinded!=sUrl.end())
				{
					sUrl = sUrl.substr(0,itFinded-sUrl.begin());
					//sUrl.erase(itFinded);
				}

				//std::replace(sUrl.begin(),sUrl.end(),':',' ');

				if(mapAnalysis.find(sUrl)==mapAnalysis.end())
				{
					mapAnalysis[sUrl] = 0;
					WriteStringFile("%s\r\n",sUrl.c_str());

					sOringinUrl += "������:";
					sUrl = sOringinUrl + sUrl;
					res.push_back(sUrl);
				}
			}
		}
	}

	//���������ļ���error�ļ�����
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

std::vector<std::string> SearchIpAndPort::GetIpAndportRegex(LPCSTR szFile,LPCSTR szFileInnerFile,LPCSTR szFileData)
{
	std::vector<string> res;
	std::map<string,int> mapRes,mapAnalysis;

	static DWORD dwCur =0;
	dwCur = GetTickCount();
	if (szFile && szFileInnerFile&&szFileData )
	{
		std::string sAherfPattern = "href[/s]*=[^\\s]+\\s";//��ȡ��������
		//std::string sAherfPattern1 = "(\'|\")https?://\\w+\\.\\w+(\\.\\w+)*(:\\d{2,5})?(\'|\")";
		std::string sAherfPattern1 = "(\'|\")(https?://)?[\\w-]+\\.[\\w-]+(\\.[\\w-]+)*(:\\d{2,5})?[^\'\"\\s]*(\'|\"|\\s)";
		//std::string sUrlParttrn = "(//|=)(\\w+-?(\\w+)?)+\\.(\\w+-?(\\w+)?)+(\\.{0,1}(\\w+-?(\\w+)?))*(:{0,1}\\d{2,5})?(/|&|\\?)?";//��Ҫȥ��//��/,\,&,",',=,' '
		std::string sUrlParttrn = "(//|=)[\\w-]+\\.[\\w-]+(\\.[\\w-]+)*(:\\d{2,5})*";//��Ҫȥ��//��/,\,&,",',=,' ',?

		std::string sFileData = szFileData;
		//std::remove_if(sFileData.begin(),sFileData.end(),[](char e)->bool{return e=='\\';});
		//std::remove_if(sFileData.begin(),sFileData.end(),[](char e)->bool{return e==' ';});
		//std::replace(sFileData.begin(),sFileData.end(),' ',cbInsteadSpace);
		std::istrstream fin(sFileData.c_str());
		char szLine[4096*10];

		if(sKeys.size()>0)
		{
			while(fin.getline(szLine,sizeof(szLine)-1))
			{
				if(ExistKey(szLine))
				{
					RegOpt ro;
					std::list<std::string> vecResHerf;
					auto vecResHerf0 = ro.GetRegexResult(sAherfPattern,szLine);
					auto vecResHerf1 = ro.GetRegexResult(sAherfPattern1,szLine);

					std::copy(vecResHerf0.begin(),vecResHerf0.end(),std::back_inserter(vecResHerf));
					std::copy(vecResHerf1.begin(),vecResHerf1.end(),std::back_inserter(vecResHerf));

					for (auto itVecRes = vecResHerf.begin();itVecRes!=vecResHerf.end();itVecRes++)
					{
						if (mapAnalysis.find(*itVecRes)!=mapAnalysis.end()) continue;
						mapAnalysis[*itVecRes] = 0;

						auto vecUrl = ro.GetRegexResult(sUrlParttrn,*itVecRes);
						if(vecUrl.size()>1) vecUrl.pop_front();//ɾ����һ����url
						for_each(vecUrl.begin(),vecUrl.end(),[&mapRes](std::string sElem){

							auto itErase = std::remove_if(sElem.begin(),sElem.end(),[](char c){
								return (c=='/' || c=='&' ||c=='\"' || c=='\'' || c=='=' || c==' ' || c == '\\');
							} );
							sElem.erase(itErase,sElem.end());
							mapRes[sElem] = 0;
						});
					}
				}
			}
		}
		else //�޹ؼ���
		{
			RegOpt ro;
			std::list<std::string> vecResHerf;
			auto vecResHerf0 = ro.GetRegexResult(sAherfPattern,sFileData);
			auto vecResHerf1 = ro.GetRegexResult(sAherfPattern1,sFileData);

			std::copy(vecResHerf0.begin(),vecResHerf0.end(),std::back_inserter(vecResHerf));
			std::copy(vecResHerf1.begin(),vecResHerf1.end(),std::back_inserter(vecResHerf));
			
			//vecResHerf.insert(vecResHerf.end(),ro.GetRegexResult(sAherfPattern1,sFileData));
			for (auto itVecRes = vecResHerf.begin();itVecRes!=vecResHerf.end();itVecRes++)
			{
				if (mapAnalysis.find(*itVecRes)!=mapAnalysis.end()) continue;
				mapAnalysis[*itVecRes] = 0;
				std::string sUrlDecode = URLDecode(*itVecRes);
				
				auto vecUrl = ro.GetRegexResult(sUrlParttrn,sUrlDecode);
				if(vecUrl.size()>1) vecUrl.pop_front();//ɾ����һ����url
				for_each(vecUrl.begin(),vecUrl.end(),[&mapRes](std::string sElem){

					auto itErase = std::remove_if(sElem.begin(),sElem.end(),[](char c){
						return (c=='/' || c=='&' ||c=='\"' || c=='\'' || c=='=' || c==' ' || c == '\\' || c == '?');
					} );
					sElem.erase(itErase,sElem.end());
					mapRes[sElem] = 0;
				});
			}
		}
	}

	for (auto itMap = mapRes.begin();itMap!=mapRes.end();itMap++)
	{
		res.push_back(itMap->first);
		WriteStringFile("%s\r\n",itMap->first.c_str());
	}

	//���������ļ���error�ļ�����
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

bool SearchIpAndPort::IsUrl(std::string sTxtUrl)
{
	char* filterExt[]={".com",".cn",".org",".wang",".cc",".xin",".net",".pub",".ink",".info",".xyz",".win",};
	for (int i=0;i<_countof(filterExt);i++)
	{
		if(stristr(sTxtUrl.c_str(),filterExt[i])) return true;
	}

	int nC = 0;
	for (auto it = sTxtUrl.begin();it != sTxtUrl.end();it++)
	{
		if (*it == '.')
		{
			if (++nC >= 3)
			{
				return true;
			}
		}
	}
	
	return false;
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
