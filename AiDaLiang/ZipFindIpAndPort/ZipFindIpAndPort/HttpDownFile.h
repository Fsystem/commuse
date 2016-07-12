#ifndef __HttpDownFile_H
#define __HttpDownFile_H
class HttpDownFile
{
public:
	const std::vector<char>& OpentUrl(std::string sUrl);
protected:
private:
	std::vector<char> mRecievData;
};
#endif //__HttpDownFile_H