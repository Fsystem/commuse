#include <stdio.h>
#include <assert.h>
#include "helper.h"
#include <string>
#include <fstream>
#include <direct.h>
#include <stdarg.h>
#include "FuncPerformanceLog.h"
#include "GlobalDef.h"

char * HelperFunc::SafeNCpy( char *strDest,const char *strSource,size_t count )
{
	guardfunc;
	if( !strDest || !strSource )
	{
		assert( false && "HelperFunc::SafeNCpy error, src or dst is null" );
		return "";
	}
	strDest[ count - 1 ] = 0;
	return strncpy( strDest,strSource,count-1 );
	unguard;
}

int HelperFunc::GetIDByConstString( char*szString,char**pszString,int iHowManyString )
{
	guardfunc;
	_strlwr( szString );
	int iLoop;
	int iBlack = ' ';

	//去掉前面和后面的空格
	char *szFind = 0;
	while( szFind = strchr( szString,iBlack ) )
	{
		if( szString == szFind )
			szString ++;
		else
			break;
	}

	szFind = 0;
	//int iLen;
	while( szFind = strrchr( szString,iBlack  ) )
	{	
		if( 1 == strlen( szFind ) )
		{
			szFind[0] = 0;
		}
		else
			break;
	}

	//
	for( iLoop = 0 ; iLoop < iHowManyString ; iLoop ++)
	{
		if( 0 == _stricmp( szString,pszString[iLoop] ) )
			return iLoop;
	}
	return -1;
	unguard;
}

static FILE *g_fpOutput = 0;
//输出
void HelperFunc::Output( char *szFormat,... )
{
	guardfunc;
	if( !g_fpOutput )
		return;
	char szBuf[ 4096];	
	va_list vl;

	//转换参数
	va_start(vl, szFormat);
	vsnprintf(szBuf, sizeof(szBuf)-1, szFormat, vl);
	va_end(vl);
	fprintf( g_fpOutput,szBuf );
	unguard;
}

//设置输出
void HelperFunc::SetOutputFile( FILE *fp )
{
	guardfunc;
	g_fpOutput = fp;
	unguard;
}

static char g_szErrorString[ 256 ];
void HelperFunc::SetErrorString( char *szErrorString,... )
{
	guardfunc;
	va_list vl;	
	//转换参数
	va_start(vl, szErrorString);
	vsnprintf(g_szErrorString, sizeof(g_szErrorString)-1, szErrorString, vl);
	va_end(vl);	
	unguard;
}

char* HelperFunc::GetErrorString(void)
{
	guardfunc;
	return g_szErrorString;
	unguard;
}

char* HelperFunc::GetFileDataString( char*szFileName )
{
	guardfunc;
	FILE *fp = fopen( szFileName,"rb" );
	if( !fp )
		return 0;
	char *szRet = GetFileDataString( fp );
	fclose( fp );
	return szRet;
	unguard;
}

char* HelperFunc::GetFileDataString( FILE *fp )
{
	guardfunc;
	if( !fp )
		return 0;
	char *szRet = 0;
	fseek( fp,0,SEEK_END );
	long lLen = ftell( fp );
	fseek( fp,0,SEEK_SET );
	
	szRet = new char[ lLen + 3 ];
	if( !szRet )
		return 0;
	szRet[ lLen ] = 0;
	if( 1 != fread( szRet,lLen,1,fp ) )
	{
		delete[] szRet;
		return 0;
	}
	strcpy( szRet + lLen, "\r\n" );
	return szRet;
	unguard;
}

void HelperFunc::VerifyString( char *szStr )
{
	guardfunc;	

	int iLoop;
	for( iLoop = 0 ; iLoop < strlen( szStr ) ; iLoop ++ )
	{
		if( '%' == szStr[ iLoop ] )
			szStr[ iLoop ] = '_';
		if( '\'' == szStr[ iLoop ] )
			szStr[ iLoop ] = '_';
	}
	unguard;
}

void HelperFunc::LimitStringSix( char *szStr)
{
	guardfunc;
	//--zizi add  检测名字小于6个中文字符
	if( strlen(szStr) > MAX_NAME_CREATE )
	{
		szStr[MAX_NAME_CREATE] = '\0';
	}
	//--
	unguard;
}

char * HelperFunc::AddNCpy( char *strDest,const char *strSource, size_t dstsize )
{
	guardfunc;
	if( !strDest || !strSource )
	{
		assert( false && "HelperFunc::SafeNCpy error, src or dst is null" );
		return "";
	}

    size_t freesize = dstsize - strlen(strDest);

	return strncat( strDest,strSource, freesize-1);
	unguard;
}

void DebugLogout(char* szFormat, ...)
{
    guardfunc;
#ifdef OPEN_LOG
	char szDate[256] = {0};
	char szTime[256] = {0};
	char szFile[256] = {0};
	char szLog[1024] = {0};

	SYSTEMTIME systime;
	GetLocalTime( &systime );

	_snprintf(szDate, sizeof(szDate)-1, "%04d-%02d-%02d", systime.wYear, systime.wMonth, systime.wDay);
	_snprintf(szTime, sizeof(szTime)-1, "%02d:%02d:%02d", systime.wHour, systime.wMinute, systime.wSecond);
	_snprintf(szFile, sizeof(szTime)-1, "%02d.log", systime.wHour);

	_mkdir("Log");
	chdir("Log");
	_mkdir(szDate);
	chdir(szDate);
	std::ofstream fLog(szFile, std::ios::app);

	if(!fLog.is_open())
	{
		return;
	}
	//转换参数
	va_list vl;
	va_start(vl, szFormat);
	int cnt = vsnprintf(szLog, sizeof(szLog)-1, szFormat, vl);
	va_end(vl);

	fLog << szDate << " " << szTime << "    " << szLog << std::endl;
	fLog.close();
	chdir("../../");
#endif
    unguard;
}
const char* BigInt2String(__int64 nInt)
{
    guardfunc;
    static char szGuid[40];
    //	char *pszGuid;
    _ui64toa(nInt, szGuid, 10);
    //if (pszGuid)
    //{
    //	strncpy(szGuid, (char *)pszGuid, sizeof(szGuid) );
    //	return szGuid;
    //}
    //assert(false);
    return szGuid;
    unguard;
}
