/** ���ܽ����ļ�
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
	/** ��¼Լ�������㷨
	*   FuncName : 	LoginEncypt
	*   Author   : 	Double sword
	*   Params   : 	
					pBuf����ǰ��������������
					npBufLen����ǰ��ĳ���
	*   Return   :	
	void		
	*   Date     :	2011-3-24
	*/
	void LoginEncypt( char* pBuf , int npBufLen );

	//-------------------------------------------------------------------------------
	/** ��¼Լ�������㷨
	*   FuncName : 	LoginEncypt
	*   Author   : 	Double sword
	*   Params   : 	
					pBuf����ǰ��������������
					npBufLen����ǰ��ĳ���
	*   Return   :	
	void		
	*   Date     :	2011-3-24
	*/
	void LoginDecrypt( char* pBuf , int npBufLen );

	//-------------------------------------------------------------------------------
	/** ��ͨ�����㷨
	*   FuncName : 	LoginEncypt
	*   Author   : 	Double sword
	*   Params   : 	
					pBuf����ǰ��������������
					npBufLen����ǰ��ĳ���
					pKey����Ҫ�õ�KEY
	*   Return   :	void
	void		
	*   Date     :	2011-3-24
	*/
	void Encrypt( char* pBuf , int npBufLen , 
		const char* pKey  );
	//-------------------------------------------------------------------------------
	/** ��ͨ�����㷨
	*   FuncName : 	LoginEncypt
	*   Author   : 	Double sword
	*   Params   : 	
					pBuf����ǰ��������������
					npBufLen����ǰ��ĳ���
					pKey����Ҫ�õ�Key
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