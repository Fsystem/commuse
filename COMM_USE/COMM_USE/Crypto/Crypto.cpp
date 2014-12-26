#include "../derypto/XXTEA.h"
#include "Crypto.h"
#include <string.h>
//-------------------------------------------------------------------------------
//constructor
Crypto::Crypto()
{
	strcpy_s(_LoginSec , sizeof(_LoginSec) , LOGINSEC);
}
//-------------------------------------------------------------------------------
void Crypto::LoginEncypt( char* pBuf , int npBufLen )
{
	int nKeyLen = strlen(LOGINSEC);
	for (int i = 0 ; i < npBufLen ; )
	{
		for (int j = 0 ; j<nKeyLen && i < npBufLen ; j++)
		{
			pBuf[i] = pBuf[i]^_LoginSec[j];
			i++;
		}
	}
}
//-------------------------------------------------------------------------------
void Crypto::LoginDecrypt( char* pBuf , int npBufLen )
{
	int nKeyLen = strlen(LOGINSEC);
	for (int i = 0 ; i < npBufLen ; )
	{
		for (int j = 0 ; j<nKeyLen && i < npBufLen ; j++)
		{
			pBuf[i] = pBuf[i]^_LoginSec[j];
			i++;
		}
	}
}
//-------------------------------------------------------------------------------
void Crypto::Encrypt( char* pBuf , int npBufLen , 
	const char* pKey )
{
	XXTEA::EncryptBuffer(pBuf,npBufLen,pKey);
}
//-------------------------------------------------------------------------------
void Crypto::Decrypt( char* pBuf , int npBufLen , 
	const char* pKey  )
{
	XXTEA::DecryptBuffer(pBuf,npBufLen,pKey);
}
//-------------------------------------------------------------------------------