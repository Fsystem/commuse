#ifndef ENCRYPT_HEAD_FILE
#define ENCRYPT_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//MD5 ������
class CMD5Encrypt
{
	//��������
private:
	//���캯��
	CMD5Encrypt() {}

	//���ܺ���
public:
	//��������
	static void EncryptData(LPCTSTR pszSrcData, TCHAR szMD5Result[33]);
};

//////////////////////////////////////////////////////////////////////////

//��������
class CXOREncrypt
{
	//��������
private:
	//���캯��
	CXOREncrypt() {}

	//���ܺ���
public:
	//��������
	static WORD EncryptData(LPCTSTR pszSrcData, LPTSTR pszEncrypData, WORD wSize);
	//�⿪����
	static WORD CrevasseData(LPCTSTR pszEncrypData, LPTSTR pszSrcData, WORD wSize);
};


#define SCRAMBLE_LENGTH 20
#define SCRAMBLED_PASSWORD_CHAR_LENGTH (SCRAMBLE_LENGTH*2+1)

//mysql password ������
class CMySqlPWEncrypt
{
	//��������
private:
	//���캯��
	CMySqlPWEncrypt() {}

	//���ܺ���
public:
	//��������
	static void EncryptData(LPCTSTR pszSrcData, TCHAR pszEncrypData[SCRAMBLED_PASSWORD_CHAR_LENGTH]);
};


//////////////////////////////////////////////////////////////////////////

#endif