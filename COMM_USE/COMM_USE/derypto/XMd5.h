#pragma once



class XMD5
{
public:
	XMD5(void);
	~XMD5(void);
	int HexToStr(const char *hex, char *str);
	int StrToHex(const char *str, char *hex , unsigned int nLength);
	char* XMD5String( char* string ); 
private:
	char	Md5Key[33];
};


