#ifndef ENCRYPT_HEAD_FILE
#define ENCRYPT_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//MD5 加密类
class CMD5Encrypt
{
	//函数定义
private:
	//构造函数
	CMD5Encrypt() {}

	//功能函数
public:
	//生成密文
	static void EncryptData(LPCTSTR pszSrcData, TCHAR szMD5Result[33]);
};

//////////////////////////////////////////////////////////////////////////

//异或加密类
class CXOREncrypt
{
	//函数定义
private:
	//构造函数
	CXOREncrypt() {}

	//功能函数
public:
	//生成密文
	static WORD EncryptData(LPCTSTR pszSrcData, LPTSTR pszEncrypData, WORD wSize);
	//解开密文
	static WORD CrevasseData(LPCTSTR pszEncrypData, LPTSTR pszSrcData, WORD wSize);
};


#define SCRAMBLE_LENGTH 20
#define SCRAMBLED_PASSWORD_CHAR_LENGTH (SCRAMBLE_LENGTH*2+1)

//mysql password 加密类
class CMySqlPWEncrypt
{
	//函数定义
private:
	//构造函数
	CMySqlPWEncrypt() {}

	//功能函数
public:
	//生成密文
	static void EncryptData(LPCTSTR pszSrcData, TCHAR pszEncrypData[SCRAMBLED_PASSWORD_CHAR_LENGTH]);
};


//////////////////////////////////////////////////////////////////////////

#endif