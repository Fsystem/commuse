#ifndef __SearchIpAndPort_H
#define __SearchIpAndPort_H


class SearchIpAndPort
{
public:
	SearchIpAndPort();
	~SearchIpAndPort();
	std::vector<std::string> GetIpAndport(LPCSTR szFile,LPCSTR szFileInnerFile,LPCSTR szFileData);
	std::vector<std::string> GetIpAndportRegex(LPCSTR szFile,LPCSTR szFileInnerFile,LPCSTR szFileData);
	void AddKey(LPCSTR szKey);
	bool ExistKey(LPCSTR szContent);

	LPCSTR GetRetFileName(){return szFileName;};
	int GetRetCnt(){return nResultCnt;}
protected:
	void WriteStringFile(LPCSTR format,...);
	bool IsUrl(std::string sTxtUrl);
	void ParseUrlRegex(std::string szContent, std::vector<std::string>& sAhrefPatterns
		,std::string& sUrlPartten
		,std::map<std::string,int>& mapAnalysis
		,std::map<std::string,int>& mapRes);
private:
	//std::string sFileName;
	FILE* fp;
	static char szAppPath[MAX_PATH];
	static char szErrorPath[MAX_PATH];
	char szFileName[MAX_PATH];
	std::vector<std::string> sKeys;
	int nResultCnt;
	//int nErrorFileCount;
};
#endif //__SearchIpAndPort_H