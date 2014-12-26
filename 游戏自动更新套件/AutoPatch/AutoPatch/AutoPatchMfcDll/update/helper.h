#ifndef HELPER_20031114_INC

#define HELPER_20031114_INC
#include <stdio.h>

// �Ƿ����־��¼
#define OPEN_LOG

namespace HelperFunc
{
	//copy string...when strlen(strSounrce) >= count, strDest[count-1] = 0;
	char *SafeNCpy( char *strDest,const char *strSource,size_t count );
	//��һ���ַ�������ƥ���ַ������������
	int GetIDByConstString( char*szString,char**pszString,int iHowManyString );

	//���
	void Output( char *szFormat,... );
	//�������
	void SetOutputFile( FILE *fp );
	void SetErrorString( char *szErrorString,... );
	char* GetErrorString(void);

	//ȡ���ļ��ַ���
	//��ȡһ���ı���ʽ���ļ������䲢�ҷ���һ��
	//�����ַ�����strlen( szRet ) = filelen + 1;
	char* GetFileDataString( FILE *fp );
	char* GetFileDataString( char*szFileName );

	//ȥ���ַ����еİٷֺ�
	void VerifyString( char *szStr );

	char *AddNCpy( char *strDest,const char *strSource, size_t dstsize );

	void LimitStringSix( char *szStr);

}

void DebugLogout(char* szFormat, ...);
const char* BigInt2String(__int64 nInt);

#endif