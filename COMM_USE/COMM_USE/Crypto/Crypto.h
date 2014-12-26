/** 加密解密文件
*   FileName  : Crypto.h
*   Author    : Double Sword
*   date      : 2011-3-24
*   Copyright : Copyright belong to Author and ZhiZunNet.CO.
*   RefDoc    : --
*/
#ifndef __Crypto_H
#define __Crypto_H


#define  LOGINSEC	"ZHIZUN\0"
class Crypto
{
public:
	Crypto();
	//-------------------------------------------------------------------------------
	/** 登录约定加密算法
	*   FuncName : 	LoginEncypt
	*   Author   : 	Double sword
	*   Params   : 	
					pBuf加密前后的输入输出参数
					npBufLen加密前后的长度
	*   Return   :	
	void		
	*   Date     :	2011-3-24
	*/
	void LoginEncypt( char* pBuf , int npBufLen );

	//-------------------------------------------------------------------------------
	/** 登录约定解密算法
	*   FuncName : 	LoginEncypt
	*   Author   : 	Double sword
	*   Params   : 	
					pBuf解密前后的输入输出参数
					npBufLen解密前后的长度
	*   Return   :	
	void		
	*   Date     :	2011-3-24
	*/
	void LoginDecrypt( char* pBuf , int npBufLen );

	//-------------------------------------------------------------------------------
	/** 普通加密算法
	*   FuncName : 	LoginEncypt
	*   Author   : 	Double sword
	*   Params   : 	
					pBuf加密前后的输入输出参数
					npBufLen加密前后的长度
					pKey加密要用的KEY
	*   Return   :	void
	void		
	*   Date     :	2011-3-24
	*/
	void Encrypt( char* pBuf , int npBufLen , 
		const char* pKey  );
	//-------------------------------------------------------------------------------
	/** 普通解密算法
	*   FuncName : 	LoginEncypt
	*   Author   : 	Double sword
	*   Params   : 	
					pBuf解密前后的输入输出参数
					npBufLen解密前后的长度
					pKey解密要用的Key
	*   Return   :	void
	void		
	*   Date     :	2011-3-24
	*/
	void Decrypt( char* pBuf , int npBufLen , 
		const char* pKey );
protected:
private:
	char _LoginSec[8];
};


#endif //__Crypto_H