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

	std::string PostDataToServer(std::string url,void* data,int len);
private:
	

private:
	std::vector<char> HttpRequest(const char* url,DWORD dwBegin=0,DWORD dwEnd=0,const char* method="GET");
	std::vector<char> HttpUploadByPost(const char* url,void* pData,int nLen);

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