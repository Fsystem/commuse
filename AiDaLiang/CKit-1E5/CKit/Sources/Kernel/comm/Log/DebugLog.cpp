#include "stdafx.h"
#include "DebugLog.h"
#include <string>
#include <ctime>
#include <tchar.h>
#define CONFIG_FILE	_T(".\\config.ini")

#pragma warning(disable:4996)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDebugLog::CDebugLog()
{
//	m_logLevel = PRINTF_THIRD_LOG;

	//begin *********************************************   鲍立良增加从配置文件读日志级别
	//段建均删除
/*	char log_level[2];
	memset(log_level, 0, 2);
	GetPrivateProfileString("log_level", "level", "3", log_level, 2, ".\\port.ini");
	int level = atoi(log_level);*/
	//段建均删除，原因可以直接使用GetPrivateProfileint,兼容UNICODE编码
	m_logLevel = GetPrivateProfileInt(_T("LOG_LEVEL"), _T("LEVEL"), 3, CONFIG_FILE);
	//end ********************************************************************************

	::InitializeCriticalSection(&m_csLock);
}

CDebugLog::~CDebugLog()
{
	::DeleteCriticalSection(&m_csLock);
}

/********************************************************************************************************
  功能：保存一个字符串到文件中，字符串前增加上时间，精确到秒，字符串后面加换行。
        保存时判断写日志级别是否小于等于设定的写日志级别，如果是，就写入，反之不写入
  参数：(IN) logLevel		写日志的级别，级别数越大，那么此信息越不重要
	    (IN) filename	    要保存的文件名(filename==NULL时保存在当天时间命名的文件名中)没有此文件尝试创建
	    (IN) fmt			格式化字符串
		(IN) ...			要保存的字符串(格式化输入)
  返回值：bool				保存成功返回true,保存失败返回false
*********************************************************************************************************/
bool CDebugLog::SaveLog( const int logLevel, const char * filename, const char *  fmt, ...)
{
	if(logLevel > m_logLevel)	
	{
		return false;
	}
	else	//如果日志级别不大于(重要性高于)设置值,则记录日志
	{
		__try
		{ 
			::EnterCriticalSection(&m_csLock);
			va_list args;
			va_start (args, fmt);

			return Log(filename, fmt, args);

			va_end (args);
		}
		__finally
		{ 
			::LeaveCriticalSection(&m_csLock);
		} 
	}
	return true;
}

/********************************************************
  功能：写日记(供SaveLog调用)。
  入参：(IN) filename  - 日志文件名(filename==NULL时保存在当天时间命名的文件名中)
		(IN) fmt	   - 格式化内容
		(IN) args	   - 要保存的字符串(格式化输入)
  出参：无
  返回: 成功:true  失败:false
********************************************************/
bool CDebugLog::Log( const char * filename, const char * fmt, va_list args )
{ 
	FILE *logfp = NULL;
	time_t tm = time (NULL);
	char strtime[32];
	strftime (strtime, 31, "\r\n%Y-%m-%d %X", localtime (&tm));//系统时间
	
	//如果文件名为空,则以系统日期命名文件名如: 20080925.log
	if(NULL == filename)
	{
		time_t tmFilename = time (NULL);
		char defaultFilename[MAX_PATH];
		char curr_path[MAX_PATH];
		memset(curr_path,0,sizeof(curr_path));
		GetModuleFileNameA(NULL,curr_path,MAX_PATH);
		for (size_t i = strlen(curr_path); i >= 0; i--)
		{
			if (curr_path[i] == '\\')
			{
				curr_path[i] = '\0';
				break;
			}
		}
		struct tm my_time;
		memcpy(&my_time,localtime(&tmFilename),sizeof(my_time));
	    sprintf(defaultFilename,  "%s\\log\\%04d%02d%02d.log",
			curr_path,my_time.tm_year + 1900,my_time.tm_mon + 1,
			my_time.tm_mday); 

		logfp = fopen (defaultFilename, "ab+");
		if (logfp == NULL)//打开以当天日期命名的文件
		{
			//创建目录，再次打开
			strcat(curr_path,"\\log");
			CreateDirectoryA(curr_path,NULL);
			logfp = fopen (defaultFilename, "ab+");
			if (logfp == NULL)//打开以当天日期命名的文件
			{
				fprintf (stderr, "%s  ", strtime);
				vfprintf (stderr, fmt, args);
				fprintf (stderr, "Log error error : fopen %s error : %s\r\n", defaultFilename, strerror (errno));
				return false;
			}
		}
	}
	else//如果指定文件名
	{
		logfp = fopen (filename, "ab+");
		if (logfp == NULL)
		{
			//创建目录，再次打开 
			fprintf (stderr, "%s  ", strtime);
			vfprintf (stderr, fmt, args);
			fprintf (stderr, "Log error : fopen %s error : %s\r\n", filename, strerror (errno));
			return false; 
		}
	}
	
	//写入文件中
	fprintf (logfp, "%s ", strtime);
	vfprintf (logfp, fmt, args);
	fclose (logfp);
 

	return true;
}


/*=====================================================================================================
功能：	设置打印日志的级别
参数:	(IN) debug_level			日志的记录级别,只有重要性高于(数值越小越重要)这个级别的日志记录才会保存下来
返回值：无	
======================================================================================================*/
void CDebugLog::SetRatingLog( int logLevel )
{
	m_logLevel = logLevel;	
}


/*****************************************************************************************
功能:	把错误号转化为错误描述字符串
参数:	(IN)nError				利用GetLastError得到的错误ID
返回值:	const char *			返回错误描述符客串指针，失败为空字符串
*****************************************************************************************/
const char * CDebugLog::GetErrorString(DWORD nError)
{
	LPVOID lpMsgBuf;
	memset(m_buffer,0,sizeof(m_buffer));

	sprintf(m_buffer,"找不到错误号%d对应的错误描述！",nError);

	if(!FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		nError,
		0, // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
		))
	{
		return m_buffer;
	}
	
	strcpy(m_buffer,(char *)lpMsgBuf);
	LocalFree( lpMsgBuf );
	
	return m_buffer;
}


/*****************************************************************************************
功能:	获取日志打印级别
------------------------------------------------------------------------------------------
参数:	无
返回值:	int		当前日志打印级别
*****************************************************************************************/
int CDebugLog::GetPrintLevel() const
{
	return m_logLevel;	
}