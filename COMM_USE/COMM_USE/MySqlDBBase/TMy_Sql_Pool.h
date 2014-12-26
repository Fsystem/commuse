#pragma once

#include <Windows.h>
#include "mysql_include/mysql.h"
#include <queue>


namespace COMMUSE
{
	//������Ϣ
	typedef struct _ROWCOL
	{
		char	szName[64];
		char*	szContent;
		int		nConLen;
	}ROWCOL;

	//ִ�д洢���̷��ؼ�¼��Ϣ
	typedef struct _RESOURCE_INFO
	{	
		unsigned int		nRow;
		unsigned int		nCol;
		ROWCOL **			res;
	}RESOURCE_INFO,*PRESOURCE_INFO;

	typedef struct tagSqlParam
	{
		tagSqlParam(){vParam=CString();bIsResult=false;};
		CString		vParam;
		bool		bIsResult;

	}tagSqlParam;

	class TMySql
	{
	public:
		explicit TMySql(){m_mysql=NULL;m_res=NULL;m_conv_res=NULL;};
		explicit TMySql(char *szHost, 
			char *szDBUserName, 
			char *szDBPwd, 
			unsigned int unDBPort,
			char *szDefDB );
		virtual ~TMySql();
		BOOL	ExecuteProcedrue(char* szProc,int nProcLen);
		BOOL	ExecuteSql(char* szSql,bool bProc,int nParamCnt,tagSqlParam * pParam);
		void	Free();
		inline void	Ping()
		{
			if (m_mysql)
			{
				mysql_ping(m_mysql);
			}
		};
		char*	GetResult(unsigned int unRow,unsigned int unCol);
		char*	GetResult(unsigned int unRow,char * szColName);
		inline BOOL ChangeDefDb(char* szDbName);
		inline unsigned int GetRowCount()
		{
			if (!m_conv_res) return NULL;
			return m_conv_res->nRow;
		};
		inline unsigned int GetColCount()
		{
			if (!m_conv_res) return NULL;
			return m_conv_res->nCol;
		}
		inline void MoveFirst()
		{
			m_nCurRow=0;
		}
		inline void MoveNext()
		{
			m_nCurRow++;
		}
		inline bool IsEndRecord()
		{
			return !(m_nCurRow<m_conv_res->nRow);
		}
		inline char* GetFieldValue(char * szName)
		{
			return GetResult(m_nCurRow,szName);
		}
		unsigned int GetMemSize();
		inline char * GetErrDescribe(){return m_szErrDescribe;};

	private:
		//����ʵ��
		MYSQL*			m_mysql;
		//��¼��
		MYSQL_RES*		m_res;
		//ת����ļ�¼��
		PRESOURCE_INFO	m_conv_res;
		//��ǰ��
		int				m_nCurRow;
		//����
		char			m_szErrDescribe[512];
	};


	class TMySqlPool
	{
	public:
		typedef std::queue<TMySql*> TMySqlQueue;
		explicit TMySqlPool(
			char *szHost, 
			char *szDBUserName, 
			char *szDBPwd, 
			unsigned int unDBPort,
			char *szDefDB,
			int nConNum = 1
			);
		virtual ~TMySqlPool();

		TMySql* GetConnect(char* szDBName);
		void	FreeConnect(TMySql* my);
	protected:
	private:
		//֪ͨ�������ˣ�û���ӵȴ��Ķ�����Լ���ִ��
		HANDLE m_hNoticeHasCon;
		TMySqlQueue	m_mysql_queue;
		CRITICAL_SECTION m_cs_queue;

		inline void lock(){EnterCriticalSection(&m_cs_queue);};
		inline void unlock(){LeaveCriticalSection(&m_cs_queue);};
	};
}

