#pragma once
#include <string>

using namespace std;
class XXTEA
{
public:
	XXTEA(void);
	~XXTEA(void);
	int toIntArray(int ** ppresult, const unsigned char * pData, unsigned nDataLen, int includeLength);
	int toByteArray(unsigned char ** ppresult, int * pData, int nDataLen, int includeLength);
	int *raw_encrypt(int * v, int vlen, int * k, int klen);
	int * raw_decrypt(int * v, int vlen, int * k, int klen);
	int xxtea_encrypt(unsigned char ** ppresult, const unsigned char* pData, int nDataLen, const unsigned char* pKey, int nKeyLen);
	int xxtea_decrypt(unsigned char ** ppresult, const unsigned char* pData, int nDataLen, const unsigned char* pKey, int nKeyLen);
	char *hex2hexstr(unsigned char * pSrc, int nSrcLen);
	char intToHexChar(int x);
	string bytesToHexString(const unsigned char *in, size_t size);
	char *hexstr2hex(unsigned char * pSrc,int nSrcLen);

	int HexToStr(const char *hex, char *str);
	int StrToHex(const char *str, char *hex , unsigned int nLength);
};
