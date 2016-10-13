#ifndef __HttpDownFile_H
#define __HttpDownFile_H
#include <WinInet.h>
class HttpDownFile
{
public:
	HttpDownFile();
	virtual ~HttpDownFile();
public:
	DWORD TryUrlDownFile(std::string url,std::string localFile,int nTryCnt=3);
public:
	std::string UrlDownFile(std::string url);
	DWORD UrlDownFile(std::string url,std::string localFile);

	DWORD UrlDownFileForData(std::string url,char* buffer,DWORD buffer_len,DWORD &file_len,DWORD dwbgein,DWORD dwend);
private:
	

private:
	std::vector<char> DownServerFile(const char* serverUrlPath,DWORD dwBegin=0,DWORD dwEnd=0);

	// //设置服务器IP和端口
	int SetServer(char * ServerIp, INTERNET_PORT ServerPort);

protected:
	HINTERNET			mInet;
	HINTERNET			mHinetConn;
	DWORD				mHttpRequestFlags;
	DWORD				mHttpsRequestFlags;
	DWORD				mLastErr;
	
private:
};
#endif //__HttpDownFile_H