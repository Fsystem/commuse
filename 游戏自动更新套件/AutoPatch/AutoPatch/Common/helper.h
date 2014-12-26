#ifndef HELPER_20031114_INC

#define HELPER_20031114_INC
#include <stdio.h>

// 是否打开日志记录
#define OPEN_LOG

namespace HelperFunc
{
	//copy string...when strlen(strSounrce) >= count, strDest[count-1] = 0;
	char *SafeNCpy( char *strDest,const char *strSource,size_t count );
	//从一个字符串数组匹配字符串，返回序号
	int GetIDByConstString( char*szString,char**pszString,int iHowManyString );

	//输出
	void Output( char *szFormat,... );
	//设置输出
	void SetOutputFile( FILE *fp );
	void SetErrorString( char *szErrorString,... );
	char* GetErrorString(void);

	//取得文件字符串
	//读取一个文本格式的文件，分配并且返回一个
	//数据字符串，strlen( szRet ) = filelen + 1;
	char* GetFileDataString( FILE *fp );
	char* GetFileDataString( char*szFileName );

	//去掉字符串中的百分号
	void VerifyString( char *szStr );

	char *AddNCpy( char *strDest,const char *strSource, size_t dstsize );

	void LimitStringSix( char *szStr);

}

void DebugLogout(char* szFormat, ...);
const char* BigInt2String(__int64 nInt);

#endif