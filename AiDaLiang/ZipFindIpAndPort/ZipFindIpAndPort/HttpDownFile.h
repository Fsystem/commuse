#ifndef __HttpDownFile_H
#define __HttpDownFile_H
class HttpDownFile
{
public:
	HttpDownFile();
	static void __cdecl OnExitInstance();
	const std::vector<char>& OpentUrl(std::string sUrl);
	const std::vector<char>& OpentUrlByCURL(std::string sUrl);
	std::string DownFile(std::string sUrl);
protected:
private:
	std::vector<char> mRecievData;
};
#endif //__HttpDownFile_H