#include "stdafx.h"
#include "SearchIpAndPort.h"
#include <fstream>

using namespace std;

//-------------------------------------------------------------------------------
char SearchIpAndPort::szAppPath[MAX_PATH]={0};
char SearchIpAndPort::szErrorPath[MAX_PATH]={0};
char cbInsteadSpace='|';
long lCurFilterCnt = 0;
long lTotalFilterCnt = 0;
long lFilterThreadCnt = 0;
//-------------------------------------------------------------------------------
SearchIpAndPort::SearchIpAndPort(bool bMakeFile)
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
	fp = NULL;
	if (bMakeFile) fp = fopen(szFileName,"w");
	
	//nErrorFileCount = 0;
	nResultCnt = 0;
}

SearchIpAndPort::~SearchIpAndPort()
{
	if(fp){
		fclose(fp);
		fp = NULL;

		if (nResultCnt == 0)
		{
			DeleteFileA(szFileName);
		}
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
				else//删除？之后无用参数
				{
					sUrl = sUrl.substr(0,s_1);
				}
			}

			if (sUrl.empty() == false && sOringinUrl.find("://")!=snpos)
			{
				sUrl = urldecode(sUrl);
				//去除html 超链接 自己的
				s_ = sUrl.find("://");
				if(s_!=snpos) sUrl = sUrl.substr(s_+3,snpos);
				//去除路径部分
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

				//删除后面非url字符，如逗号等
				auto itFinded = std::find_if(sUrl.begin(),sUrl.end(),[](char c)->bool{
					return c == ',' || c == '，' || c == '\'' || c == '\"';
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

std::vector<std::string> SearchIpAndPort::GetIpAndportRegex(LPCSTR szFile,LPCSTR szFileInnerFile,LPCSTR szFileData)
{
	std::vector<string> res;
	std::map<string,int> mapRes,mapAnalysis;

	static DWORD dwCur =0;
	dwCur = GetTickCount();
	if (szFile && szFileInnerFile&&szFileData )
	{
		//1.去herf超链接,2，取脚本中的网址
		std::vector<std::string> sAherfPatterns;
		sAherfPatterns.push_back("href[/s]*=[^\\s]+\\s");
		sAherfPatterns.push_back("(\'|\")(https?://)?[\\w-]+\\.[\\w-]+(\\.[\\w-]+)*(:\\d{2,5})?[^\'\"\\s]*(\'|\"|\\s)");
		
		//std::string sAherfPattern = "href[/s]*=[^\\s]+\\s";//获取超链正则
		//std::string sAherfPattern1 = "(\'|\")https?://\\w+\\.\\w+(\\.\\w+)*(:\\d{2,5})?(\'|\")";
		//
		//std::string sAherfPattern1 = "(\'|\")(https?://)?[\\w-]+\\.[\\w-]+(\\.[\\w-]+)*(:\\d{2,5})?[^\'\"\\s]*(\'|\"|\\s)";
		//std::string sAherfPattern1 = "(\'|\")(https?://)?[^\\s]+\\s";
		//3取超链中参数带合法url
		//std::string sUrlParttrn0 = "\\?.+=.+(https?://)?[\'\"\\s]*[\\w-]+(\\.[\\w-]+)+(:\\d{2,5})*";//优先取后面的
		//4如果3没有，则证明参数中没有合法的url，取主url
		std::string sUrlParttrn = "(//|=)[\'\"\\s]*[\\w-]+\\.[\\w-]+(\\.[\\w-]+)*(:\\d{2,5})*";//需要去除//，/,\,&,",',=,' ',?
		
		std::string sFileData = szFileData;
		RegOpt::RegexReplace(sFileData,"<a","\n<a");
		//std::remove_if(sFileData.begin(),sFileData.end(),[](char e)->bool{return e=='\\';});
		//std::remove_if(sFileData.begin(),sFileData.end(),[](char e)->bool{return e==' ';});
		//std::replace(sFileData.begin(),sFileData.end(),' ',cbInsteadSpace);
		std::istrstream fin(sFileData.c_str());
		std::string sLine;

		if(sKeys.size()>0)
		{
			while(std::getline(fin,sLine,'\n'))
			{
				if(sLine.length()==0) break;
				if(ExistKey(sLine.c_str()))
				{
					ParseUrlRegex(sLine,sAherfPatterns,sUrlParttrn,mapAnalysis,mapRes);
				}
			}
		}
		else //无关键字
		{
			ParseUrlRegex(sFileData,sAherfPatterns,sUrlParttrn,mapAnalysis,mapRes);
		}
	}

	for (auto itMap = mapRes.begin();itMap!=mapRes.end();itMap++)
	{
		if(IsUrl(itMap->first) == false) continue;
		res.push_back(itMap->first);
		WriteStringFile("%s\n",itMap->first.c_str());
	}

	//拷贝错误文件到error文件夹下
	if(res.size()==0)
	{
		_mkdir( szErrorPath);
		char szFileName[MAX_PATH]={0};
		strncat(szFileName,szErrorPath,MAX_PATH);
		strncat(szFileName,strrchr(szFile,'\\')+1,MAX_PATH);
		CopyFileA(szFile,szFileName,FALSE);
	}

	dwCur = GetTickCount()-dwCur;

	nResultCnt += res.size();

	return res;
}

void SearchIpAndPort::ParseUrlRegex(std::string szContent, std::vector<std::string>& sAhrefPatterns
	,std::string& sUrlPartten
	,std::map<string,int>& mapAnalysis
	,std::map<string,int>& mapRes)
{
	RegOpt ro;
	std::list<std::string> vecResHerf;
	for (auto it = sAhrefPatterns.begin();it != sAhrefPatterns.end(); it++)
	{
		auto vecResHerf0 = ro.GetRegexResult(*it,szContent);
		std::copy(vecResHerf0.begin(),vecResHerf0.end(),std::back_inserter(vecResHerf));
	}

	//vecResHerf.insert(vecResHerf.end(),ro.GetRegexResult(sAherfPattern1,sFileData));
	for (auto itVecRes = vecResHerf.begin();itVecRes!=vecResHerf.end();itVecRes++)
	{
		std::string sUrlDecode = urldecode(*itVecRes);

		if (mapAnalysis.find(sUrlDecode)!=mapAnalysis.end()) 
		{
			mapAnalysis[sUrlDecode]++;
			continue;
		}
		mapAnalysis[sUrlDecode] = 1;
		
		std::list<std::string> vecUrl ;
		if(sUrlPartten.empty()==false) vecUrl = ro.GetRegexResult(sUrlPartten,sUrlDecode);

		if(vecUrl.size()>1) vecUrl.pop_front();//删除第一个主url

		for_each(vecUrl.begin(),vecUrl.end(),[&mapRes](std::string sElem){

			auto itEnd = std::remove_if(sElem.begin(),sElem.end(),[](char c){
				return (c=='/' || c=='&' ||c=='\"' || c=='\'' || c=='=' || c==' ' || c == '\\' || c == '?' || c == '\r' || c == '\n');
			});
			sElem.erase(itEnd,sElem.end());
			mapRes[sElem]++;
		});
	}
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

//-------------------------------------------------------------------------------
bool ParseLineAnd(std::list<std::string> sPartternList,std::string sContent)
{
	std::string sParttern;
	int nCount = 1;
	int nCurCount = 0;

	bool bRet = true;

	RegOpt reg;
	for (auto itList = sPartternList.begin();itList!=sPartternList.end();)
	{
		sParttern = *itList;
		itList = sPartternList.erase(itList);
		nCount = 1;
		if (itList!=sPartternList.end())
		{
			nCount = (int)std::stoll(*itList);
			itList = sPartternList.erase(itList);
		}

		auto listRes = reg.GetRegexResult(sParttern,sContent);
		if(listRes.size()<nCount) return false;
		for (auto itRes = listRes.begin();itRes!=listRes.end();itRes++)
		{
			bRet &= ParseLineAnd(sPartternList,*itRes);
			if(bRet==false) return false;
		}
	}

	return bRet;
}

bool ParseCustom(std::string sFileData)
{
	char szAppDir[MAX_PATH];
	GetModuleFileNameA(NULL,szAppDir,MAX_PATH);
	strrchr(szAppDir,'\\')[1]=0;

	std::string sFilePath=szAppDir;
	sFilePath += "partten.ini";
	std::ifstream ifile(sFilePath);
	if (ifile.is_open()==false)
	{
		{
			std::ofstream ofile(sFilePath);
			ofile<<"<tr[\\s\\S]*?</tr>|20"<<std::endl;
		}
		
		ifile.open(sFilePath);
	}

	if (ifile.is_open())
	{
		std::string sLine;
		
		while(std::getline(ifile,sLine,'\n'))
		{
			if(sLine.length()==0) break;

			std::string sParttern;
			int nCount = 1;
			int nCurCount = 0;

			RegOpt reg;
			auto list = reg.GetRegexResult("[^|]+",sLine);
			//if( ParseLineAnd(list,sFileData) ) return true;
			if (list.size()>0)
			{
				sParttern = list.front();
				list.pop_front();

				if(list.size()>0) {
					nCount = std::stoll(list.front());
				}
			}

			auto listRes = reg.GetRegexResult(sParttern,sFileData);
			if(listRes.size()>=nCount) return true;

		}
		
	}
	
	return false;
}

void FilterUrlThreadFile(void* pData)
{
	std::vector<std::string> vecs = *(std::vector<std::string>*)pData;
	delete pData;

	static std::list<std::string> sFilterFile;
	static bool bInit = false;
	if (bInit == false)
	{
		bInit = true;

		char szAppDir[MAX_PATH];
		GetModuleFileNameA(NULL,szAppDir,MAX_PATH);
		strrchr(szAppDir,'\\')[1]=0;

		std::string sFilePath=szAppDir;
		sFilePath += "Filter.txt";
		std::ifstream ifile(sFilePath);
		if (ifile.is_open())
		{
			std::string sLine;
			while(std::getline(ifile,sLine,'\n'))
			{
				if(sLine.length()==0) break;
				sFilterFile.push_back(sLine);
			}
		}
	}

	for (auto it = vecs.begin();it!=vecs.end();it++)
	{
		std::string szTmp = *it;
		
		auto itFind = std::find(sFilterFile.begin(),sFilterFile.end(),szTmp);
		if ( itFind!=sFilterFile.end() )
		{
			SendMessage(gMainHwnd,MSG_FILTER_FINISH,(WPARAM)&szTmp,1);
		}
		else
		{
			SendMessage(gMainHwnd,MSG_FILTER_FINISH,(WPARAM)&szTmp,0);
		}
	}
}

void FilterUrlThreadTr(void* pData)
{
	std::vector<std::string> vecs = *(std::vector<std::string>*)pData;
	delete pData;

	for (auto it = vecs.begin();it!=vecs.end();it++)
	{
		std::string szTmp = *it;
		//std::string* szOut = new std::string;
		//*szOut = szTmp;
		HttpDownFile hdf;

		std::string sFileData;
		if (SearchIpAndPort::IsUrl(szTmp))
		{
			sFileData = hdf.DownFile(szTmp);
		}
		
		if (sFileData.size()>0)
		{
			if (ParseCustom(sFileData))
			{
				SendMessage(gMainHwnd,MSG_FILTER_FINISH,(WPARAM)&szTmp,1);
			}
			else
			{
				SendMessage(gMainHwnd,MSG_FILTER_FINISH,(WPARAM)&szTmp,0);
			}
		}
		else
		{
			SendMessage(gMainHwnd,MSG_FILTER_FINISH,(WPARAM)&szTmp,0);
		}

	}
	/*
	static long nCount = 0;
	InterlockedExchangeAdd(&nCount,1);
	LogTrace("当前线程:%d\n",nCount);*/
	
}

void FilterUrlThread(void* pData)
{
	std::vector<std::string> vecs = *(std::vector<std::string>*)pData;
	delete pData;

	std::vector<std::string> sAherfPatterns;
	sAherfPatterns.push_back("href[/s]*=[^\\s]+\\s");
	sAherfPatterns.push_back("(\'|\")(https?://)?[\\w-]+\\.[\\w-]+(\\.[\\w-]+)*(:\\d{2,5})?[^\'\"\\s]*(\'|\"|\\s)");
	//4如果3没有，则证明参数中没有合法的url，取主url
	std::string sUrlParttrn = "(//|=)[\'\"\\s]*[\\w-]+\\.[\\w-]+(\\.[\\w-]+)*(:\\d{2,5})*";

	static long nNodeCnt = 0;
	InterlockedExchangeAdd(&nNodeCnt,vecs.size());

	InterlockedExchangeAdd(&lFilterThreadCnt,1);
	LogTrace(">>>++++++++真实线程计数[%ld]节点总数:%ld当前节点数:%ld\n",lFilterThreadCnt,nNodeCnt,vecs.size());

	for (auto it = vecs.begin();it!=vecs.end();it++)
	{
		std::string szTmp = *it;
		//std::string* szOut = new std::string;
		//*szOut = szTmp;
		HttpDownFile hdf;

		std::string sFileData;
		if (SearchIpAndPort::IsUrl(szTmp))
		{
			sFileData = hdf.DownFile(szTmp);
		}
		
		if (sFileData.size()>0)
		{
			std::map<string,int> mapRes,mapAnalysis;
			std::vector<std::string> vecDels;
			vecDels.push_back(szTmp);
			vecDels.push_back("www.w3.org");
			vecDels.push_back("www.w3c.org");
			vecDels.push_back(".baidu.");
			vecDels.push_back(".360.");
			vecDels.push_back(".163.");
			vecDels.push_back(".114so.");
			vecDels.push_back(".ku6.");
			vecDels.push_back(".youku.");
			vecDels.push_back(".tudou.");


			SearchIpAndPort::ParseUrlRegex(sFileData,sAherfPatterns,sUrlParttrn,mapAnalysis,mapRes);
			
			//bool bPublish = false;

			for (auto itmap = mapRes.begin();itmap!=mapRes.end();)
			{
				if (SearchIpAndPort::IsUrl(itmap->first) == false) 
				{
					mapRes.erase(itmap++);
					continue;
				}

				bool bDeleted = false;
				for (auto itDel = vecDels.begin();itDel!=vecDels.end();itDel++)
				{
					if (  itmap->first.find(*itDel) != std::string::npos )
					{
						bDeleted = true;
						break;

					}

				}

				if(bDeleted) {
					mapRes.erase(itmap++);
					continue;
				}

				itmap++;
			}

			if (mapRes.size()>=20&&ParseCustom(sFileData))
			{
				SendMessage(gMainHwnd,MSG_FILTER_FINISH,(WPARAM)&szTmp,1);
			}
			else
			{
				SendMessage(gMainHwnd,MSG_FILTER_FINISH,(WPARAM)&szTmp,0);
			}
		}
		else
		{
			SendMessage(gMainHwnd,MSG_FILTER_FINISH,(WPARAM)&szTmp,0);
		}

		InterlockedExchangeAdd(&lCurFilterCnt,1);

		LogTrace(">>>====当前进度[%ld/%ld]\n",lCurFilterCnt,lTotalFilterCnt);
	}
}

void SearchIpAndPort::FilterPublisherUrl(void* p)
{
	std::string szTmp;

	int mode = (int)p;

	char szAppDir[MAX_PATH];
	GetModuleFileNameA(NULL,szAppDir,MAX_PATH);
	strrchr(szAppDir,'\\')[1]=0;

	szTmp = szAppDir+std::string("RET.TXT");
	std::ifstream infile(szTmp,std::ios::in);

	if (infile.is_open())
	{
		//int nCount = 0;
		std::vector<std::string> vecUrl;
		while(std::getline(infile,szTmp,'\n'))
		{
			if(szTmp.length()==0) break;

			vecUrl.push_back(szTmp);
		}

		int nMaxThread = 500;

		int nItemCnt = vecUrl.size()/nMaxThread+(vecUrl.size()%nMaxThread>0?1:0);

		int nLast = vecUrl.size()/nItemCnt;

		lTotalFilterCnt = vecUrl.size();
		lCurFilterCnt = 0;
		lFilterThreadCnt = 0;
		int nDefaultStackSize = 0;

		LogTrace(">>>总线程数:%ld\n",nLast+(vecUrl.size()%nItemCnt>0?1:0));

		SendMessage(gMainHwnd,MSG_FILTER_FINISH,WPARAM(lTotalFilterCnt),2);

		for (int i =0 ;i< nLast;i ++)
		{
			std::vector<std::string>* vecs = new std::vector<std::string>;
			
			std::copy(vecUrl.begin()+i*nItemCnt,vecUrl.begin()+i*nItemCnt+nItemCnt,std::back_inserter(*vecs));

			HANDLE hThr = NULL;
			if(mode==0){
				hThr = (decltype(hThr))_beginthread(FilterUrlThread,nDefaultStackSize,vecs);
			}
			else
			{
				hThr = (decltype(hThr))_beginthread(FilterUrlThreadFile,nDefaultStackSize,vecs);
			}

			if (hThr == NULL|| hThr == INVALID_HANDLE_VALUE)
			{
				LogTrace(">>>----线程创建失败[%u]\n",errno);
			}

		}

		if (vecUrl.size()%nItemCnt>0)
		{
			std::vector<std::string>* vecs = new std::vector<std::string>;

			std::copy(vecUrl.begin()+nLast*nItemCnt,vecUrl.end(),std::back_inserter(*vecs));

			HANDLE hThr = NULL;
			if(mode==0){
				hThr = (decltype(hThr))_beginthread(FilterUrlThread,nDefaultStackSize,vecs);
			}
			else
			{
				hThr = (decltype(hThr))_beginthread(FilterUrlThreadFile,nDefaultStackSize,vecs);
			}

			if (hThr == NULL|| hThr == INVALID_HANDLE_VALUE)
			{
				LogTrace(">>>----线程创建失败[%u]\n",errno);
			}
		}

		assert(nLast*nItemCnt+(vecUrl.size()%nItemCnt>0?1:0) == lTotalFilterCnt);
		if (nLast*nItemCnt+(vecUrl.size()%nItemCnt>0?1:0) != lTotalFilterCnt)
		{
			MessageBoxA(NULL,"异常错误",NULL,MB_OK);
		}
	}
}