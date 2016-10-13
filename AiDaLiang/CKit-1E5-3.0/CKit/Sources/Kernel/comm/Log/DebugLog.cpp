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

	//begin *********************************************   ���������Ӵ������ļ�����־����
	//�ν���ɾ��
/*	char log_level[2];
	memset(log_level, 0, 2);
	GetPrivateProfileString("log_level", "level", "3", log_level, 2, ".\\port.ini");
	int level = atoi(log_level);*/
	//�ν���ɾ����ԭ�����ֱ��ʹ��GetPrivateProfileint,����UNICODE����
	m_logLevel = GetPrivateProfileInt(_T("LOG_LEVEL"), _T("LEVEL"), 3, CONFIG_FILE);
	//end ********************************************************************************

	::InitializeCriticalSection(&m_csLock);
}

CDebugLog::~CDebugLog()
{
	::DeleteCriticalSection(&m_csLock);
}

/********************************************************************************************************
  ���ܣ�����һ���ַ������ļ��У��ַ���ǰ������ʱ�䣬��ȷ���룬�ַ�������ӻ��С�
        ����ʱ�ж�д��־�����Ƿ�С�ڵ����趨��д��־��������ǣ���д�룬��֮��д��
  ������(IN) logLevel		д��־�ļ��𣬼�����Խ����ô����ϢԽ����Ҫ
	    (IN) filename	    Ҫ������ļ���(filename==NULLʱ�����ڵ���ʱ���������ļ�����)û�д��ļ����Դ���
	    (IN) fmt			��ʽ���ַ���
		(IN) ...			Ҫ������ַ���(��ʽ������)
  ����ֵ��bool				����ɹ�����true,����ʧ�ܷ���false
*********************************************************************************************************/
bool CDebugLog::SaveLog( const int logLevel, const char * filename, const char *  fmt, ...)
{
	if(logLevel > m_logLevel)	
	{
		return false;
	}
	else	//�����־���𲻴���(��Ҫ�Ը���)����ֵ,���¼��־
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
  ���ܣ�д�ռ�(��SaveLog����)��
  ��Σ�(IN) filename  - ��־�ļ���(filename==NULLʱ�����ڵ���ʱ���������ļ�����)
		(IN) fmt	   - ��ʽ������
		(IN) args	   - Ҫ������ַ���(��ʽ������)
  ���Σ���
  ����: �ɹ�:true  ʧ��:false
********************************************************/
bool CDebugLog::Log( const char * filename, const char * fmt, va_list args )
{ 
	FILE *logfp = NULL;
	time_t tm = time (NULL);
	char strtime[32];
	strftime (strtime, 31, "\r\n%Y-%m-%d %X", localtime (&tm));//ϵͳʱ��
	
	//����ļ���Ϊ��,����ϵͳ���������ļ�����: 20080925.log
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
		if (logfp == NULL)//���Ե��������������ļ�
		{
			//����Ŀ¼���ٴδ�
			strcat(curr_path,"\\log");
			CreateDirectoryA(curr_path,NULL);
			logfp = fopen (defaultFilename, "ab+");
			if (logfp == NULL)//���Ե��������������ļ�
			{
				fprintf (stderr, "%s  ", strtime);
				vfprintf (stderr, fmt, args);
				fprintf (stderr, "Log error error : fopen %s error : %s\r\n", defaultFilename, strerror (errno));
				return false;
			}
		}
	}
	else//���ָ���ļ���
	{
		logfp = fopen (filename, "ab+");
		if (logfp == NULL)
		{
			//����Ŀ¼���ٴδ� 
			fprintf (stderr, "%s  ", strtime);
			vfprintf (stderr, fmt, args);
			fprintf (stderr, "Log error : fopen %s error : %s\r\n", filename, strerror (errno));
			return false; 
		}
	}
	
	//д���ļ���
	fprintf (logfp, "%s ", strtime);
	vfprintf (logfp, fmt, args);
	fclose (logfp);
 

	return true;
}


/*=====================================================================================================
���ܣ�	���ô�ӡ��־�ļ���
����:	(IN) debug_level			��־�ļ�¼����,ֻ����Ҫ�Ը���(��ֵԽСԽ��Ҫ)����������־��¼�Żᱣ������
����ֵ����	
======================================================================================================*/
void CDebugLog::SetRatingLog( int logLevel )
{
	m_logLevel = logLevel;	
}


/*****************************************************************************************
����:	�Ѵ����ת��Ϊ���������ַ���
����:	(IN)nError				����GetLastError�õ��Ĵ���ID
����ֵ:	const char *			���ش����������ʹ�ָ�룬ʧ��Ϊ���ַ���
*****************************************************************************************/
const char * CDebugLog::GetErrorString(DWORD nError)
{
	LPVOID lpMsgBuf;
	memset(m_buffer,0,sizeof(m_buffer));

	sprintf(m_buffer,"�Ҳ��������%d��Ӧ�Ĵ���������",nError);

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
����:	��ȡ��־��ӡ����
------------------------------------------------------------------------------------------
����:	��
����ֵ:	int		��ǰ��־��ӡ����
*****************************************************************************************/
int CDebugLog::GetPrintLevel() const
{
	return m_logLevel;	
}