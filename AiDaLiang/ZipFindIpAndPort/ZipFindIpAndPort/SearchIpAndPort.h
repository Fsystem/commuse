#ifndef __SearchIpAndPort_H
#define __SearchIpAndPort_H
class SearchIpAndPort
{
public:
	SearchIpAndPort();
	~SearchIpAndPort();
	std::vector<std::string> GetIpAndport(LPCSTR szFile,LPCSTR szFileInnerFile,LPCSTR szFileData,LPCSTR szKey);
protected:
	void WriteStringFile(LPCSTR format,...);
private:
	//std::string sFileName;
	FILE* fp;
};
#endif //__SearchIpAndPort_H