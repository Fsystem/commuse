#pragma once
//#include "stdafx.h"
#include "TMy_Sql_Pool.h"
#include "../Log/CommLog.h"


#pragma comment(lib, "MySqlDBBase/mysql_bin/libmysql.lib")

namespace COMMUSE
{



	TMySql::TMySql(char *szHost, 
		char *szDBUserName, 
		char *szDBPwd, 
		unsigned int unDBPort,
		char *szDefDB )
	{
		m_res = NULL;
		m_conv_res = NULL;
		m_mysql = new MYSQL;
		if (NULL == mysql_init(m_mysql))
		{//创建失败
			LOGEVEN(TEXT("MYSQL>>>创建数据库连接失败\n"));
		}
		else
		{
			//设置字符集
			mysql_options(m_mysql, MYSQL_SET_CHARSET_NAME, "gb2312");
			//自动重连
			my_bool my_true=true;
			mysql_options(m_mysql, MYSQL_OPT_RECONNECT, &my_true);
			if (NULL == mysql_real_connect(m_mysql, szHost,szDBUserName, szDBPwd, szDefDB, unDBPort, NULL, CLIENT_MULTI_STATEMENTS))
			{
				char szTemp[512] = {0};
				sprintf_s(szTemp,512,"MYSQL>>>连接数据库失败(%s)\n",mysql_error(m_mysql));

				//LOGEVEN(szTemp);
				mysql_close(m_mysql);
				if (m_mysql)
				{
					delete m_mysql;
					m_mysql = NULL;
				}
				return;
			}

			mysql_query(m_mysql,"set names 'gbk';\0");
			LOGEVEN(TEXT("MYSQL>>>连接数据库成功,设置中文字符\n"));
		}

	}

	void	TMySql::Free()
	{
		LOGEVEN(TEXT("MYSQL>>>free mysql_array\n"));
		if (m_conv_res)
		{
			for (unsigned int i=0;i<m_conv_res->nRow;i++)
			{
				for (unsigned int j = 0;j<m_conv_res->nCol;j++)
				{
					if (m_conv_res->res[i][j].szContent)
					{
						delete[] m_conv_res->res[i][j].szContent;
						m_conv_res->res[i][j].szContent = NULL;
					}
				}
				if (m_conv_res->res[i])
				{
					delete[] m_conv_res->res[i];
					m_conv_res->res[i] = NULL;
				}
			}
			if (m_conv_res->res)
			{
				delete[] m_conv_res->res;
				m_conv_res->res = NULL;
			}
			if (m_conv_res)
			{
				delete m_conv_res;
				m_conv_res = NULL;
			}

		}
	}

	volatile long icount = 0;
	TMySql::~TMySql()
	{
		char s[50] = {0};
		InterlockedIncrement(&icount);
		if (m_mysql)
		{
			mysql_close(m_mysql);
			delete m_mysql;
			m_mysql = NULL;
		}
		// 	_ltoa_s(icount,s,10);
		// 	LOGEVEN(s);
		LOGEVEN(TEXT("MYSQL>>>free mysql\n"));
		Free();
		/*if (m_conv_res)
		{
			for (unsigned int i=0;i<m_conv_res->nRow;i++)
			{
				for (unsigned int j = 0;j<m_conv_res->nCol;j++)
				{
					if (m_conv_res->res[i][j].szContent)
					{
						delete[] m_conv_res->res[i][j].szContent;
						m_conv_res->res[i][j].szContent = NULL;
					}
				}
				if (m_conv_res->res[i])
				{
					delete[] m_conv_res->res[i];
					m_conv_res->res[i] = NULL;
				}
			}
			if (m_conv_res->res)
			{
				delete[] m_conv_res->res;
				m_conv_res->res = NULL;
			}
			if (m_conv_res)
			{
				delete m_conv_res;
				m_conv_res = NULL;
			}
		}*/

	}

	BOOL TMySql::ExecuteProcedrue(char* szProc,int nProcLen)
	{
		if (m_mysql)
		{
			m_nCurRow=0;
			if (mysql_real_query(m_mysql,szProc,nProcLen)!=NULL)
			{
				//LOGEVEN(mysql_error(m_mysql));
				return FALSE;
			}
			else
			{
				m_res = mysql_store_result(m_mysql);
				if (!m_res)
				{
					//LOGEVEN(mysql_error(m_mysql));
					LOGEVEN(TEXT("MYSQL>>>执行存储过程OK,结果集NULL\n"));
					return TRUE;
				}
				m_conv_res =new RESOURCE_INFO;
				m_conv_res->nCol = mysql_field_count(m_mysql);
				m_conv_res->nRow = (unsigned)m_res->row_count;
				//行
				m_conv_res->res = new ROWCOL*[m_conv_res->nRow];
				for (unsigned int i=0;i<m_conv_res->nRow;i++)
				{
					//列
					m_conv_res->res[i] = new ROWCOL[m_conv_res->nCol];
				}
				//转义数据
				unsigned int nCurRow = 0;
				unsigned int nCurCol = 0;
				unsigned int nConLen = 0;
				unsigned long*	lens = NULL;	
				MYSQL_ROW mRow;
				MYSQL_FIELD* mCol = NULL;
				for (;nCurRow < m_conv_res->nRow;nCurRow++)
				{
					mRow = mysql_fetch_row(m_res);
					//功能    返回结果集内当前行的列的长度。如果打算复制字段值，该长度信息有助于优化，这是因为，
					//你能避免调用strlen()。此外，如果结果集包含二进制数据，必须使用该函数来确定数据的大小，
					//原因在于，对于包含Null字符的任何字段，strlen()将返回错误的结果。
					lens = mysql_fetch_lengths(m_res);
					for (; nCurCol<m_conv_res->nCol; nCurCol++)
					{
						mCol = mysql_fetch_field(m_res);
						nConLen	 = lens[nCurCol] + 1;
						m_conv_res->res[nCurRow][nCurCol].nConLen = nConLen -1;
						m_conv_res->res[nCurRow][nCurCol].szContent = new char[nConLen];
						memset(m_conv_res->res[nCurRow][nCurCol].szContent,0,nConLen);
						strcpy_s(m_conv_res->res[nCurRow][nCurCol].szName,_countof(m_conv_res->res[nCurRow][nCurCol].szName),mCol->name);
						if (nConLen == 1)
							memcpy(m_conv_res->res[nCurRow][nCurCol].szContent,"\0",1);
						else
							memcpy(m_conv_res->res[nCurRow][nCurCol].szContent,mRow[nCurCol],nConLen - 1 );
					}

					//将mysql光标移动到下一行的第一列
					mysql_field_seek(m_res, 0);
					nCurCol = 0;
				}

				do
				{
					mysql_free_result(m_res);
				}while( !mysql_next_result(m_mysql) , m_res = mysql_store_result( m_mysql ));
				LOGEVEN(TEXT("MYSQL>>>执行存储过程OK\n"));
				return TRUE;
			}
		}
		return FALSE;
	}

	BOOL	TMySql::ExecuteSql(char* szSql,bool bProc,int nParamCnt,tagSqlParam * pParam)
	{
		if (m_mysql)
		{
			m_nCurRow=0;

			char szTemp[1024]={0};
			strcpy_s(szTemp,_countof(szTemp),szSql);
			if (bProc)
			{
				sprintf_s(szTemp,1024,"call %s(",szSql);
				for (int i=0;i<nParamCnt;i++)
				{
					if(pParam[i].bIsResult)sprintf(szTemp+strlen(szTemp),"%s",pParam[i].vParam);
					else sprintf(szTemp+strlen(szTemp),"'%s'",pParam[i].vParam);
					if(i==nParamCnt-1)strcat(szTemp+strlen(szTemp),");");
					else strcat(szTemp+strlen(szTemp),",");
				}
			}

			if (mysql_real_query(m_mysql,szTemp,strlen(szTemp))!=NULL)
			{
				//LOGEVEN(mysql_error(m_mysql));
				strcpy_s(m_szErrDescribe,mysql_error(m_mysql));
				return FALSE;
			}
			else
			{
				m_res = mysql_store_result(m_mysql);
				if (!m_res)
				{
					//LOGEVEN(mysql_error(m_mysql));
					//LOGEVEN(TEXT("MYSQL>>>执行存储过程OK,结果集NULL\n"));
					//return TRUE;
					goto OUTPUT;
				}
				m_conv_res =new RESOURCE_INFO;
				m_conv_res->nCol = mysql_field_count(m_mysql);
				m_conv_res->nRow = (unsigned)m_res->row_count;
				//行
				m_conv_res->res = new ROWCOL*[m_conv_res->nRow];
				for (unsigned int i=0;i<m_conv_res->nRow;i++)
				{
					//列
					m_conv_res->res[i] = new ROWCOL[m_conv_res->nCol];
				}
				//转义数据
				unsigned int nCurRow = 0;
				unsigned int nCurCol = 0;
				unsigned int nConLen = 0;
				unsigned long*	lens = NULL;	
				MYSQL_ROW mRow;
				MYSQL_FIELD* mCol = NULL;
				for (;nCurRow < m_conv_res->nRow;nCurRow++)
				{
					mRow = mysql_fetch_row(m_res);
					//功能    返回结果集内当前行的列的长度。如果打算复制字段值，该长度信息有助于优化，这是因为，
					//你能避免调用strlen()。此外，如果结果集包含二进制数据，必须使用该函数来确定数据的大小，
					//原因在于，对于包含Null字符的任何字段，strlen()将返回错误的结果。
					lens = mysql_fetch_lengths(m_res);
					for (; nCurCol<m_conv_res->nCol; nCurCol++)
					{
						mCol = mysql_fetch_field(m_res);
						nConLen	 = lens[nCurCol] + 1;
						m_conv_res->res[nCurRow][nCurCol].nConLen = nConLen -1;
						m_conv_res->res[nCurRow][nCurCol].szContent = new char[nConLen];
						memset(m_conv_res->res[nCurRow][nCurCol].szContent,0,nConLen);
						strcpy_s(m_conv_res->res[nCurRow][nCurCol].szName,_countof(m_conv_res->res[nCurRow][nCurCol].szName),mCol->name);
						if (nConLen == 1)
							memcpy(m_conv_res->res[nCurRow][nCurCol].szContent,"\0",1);
						else
							memcpy(m_conv_res->res[nCurRow][nCurCol].szContent,mRow[nCurCol],nConLen - 1 );
					}

					//将mysql光标移动到下一行的第一列
					mysql_field_seek(m_res, 0);
					nCurCol = 0;
				}

				do
				{
					mysql_free_result(m_res);
				}while( !mysql_next_result(m_mysql) , m_res = mysql_store_result( m_mysql ));

OUTPUT:
				//取输出参数
				bool bReturnParam=false;
				sprintf(szTemp,"select ");
				std::vector<int> returnArr;
				for (int i=0;i<nParamCnt;i++)
				{
					if (pParam[i].bIsResult)
					{
						sprintf(szTemp+strlen(szTemp),"%s",pParam[i].vParam);
						if(i!=nParamCnt-1)strcat(szTemp+strlen(szTemp),",");
						else strcat(szTemp+strlen(szTemp),";");
						returnArr.push_back(i);
						bReturnParam=true;
					}
				}

				if (bReturnParam)
				{
					if( mysql_query(m_mysql,szTemp)==0 )
					{
						int nFieldCnt=mysql_field_count(m_mysql);
						MYSQL_RES * pRes = mysql_store_result(m_mysql);
						if (pRes)
						{
							MYSQL_ROW mRow=mysql_fetch_row(pRes);
							int nReturnArrIdx=0;
							for (int i=0;i<nFieldCnt;i++)
							{
								if (mRow) pParam[returnArr[nReturnArrIdx++]].vParam=mRow[i];
							}
						}
						
						mysql_free_result(pRes);
					}
				}


				LOGEVEN(TEXT("MYSQL>>>执行存储过程OK\n"));
				return TRUE;
			}
		}
		return FALSE;
	}

	char*	TMySql::GetResult(unsigned int unRow,unsigned int unCol)
	{
		if (!m_conv_res)
			return NULL;
		return m_conv_res->res[unRow][unCol].szContent;
	}

	char*	TMySql::GetResult(unsigned int unRow,char * szColName)
	{
		if (!m_conv_res || !szColName) return NULL;

		for (unsigned int j = 0 ; j<m_conv_res->nCol ;j++)
		{
			if (stricmp(m_conv_res->res[unRow][j].szName,szColName)==0)
			{
				return m_conv_res->res[unRow][j].szContent;
			}
		}

		return NULL;
	}

	inline BOOL TMySql::ChangeDefDb(char* szDbName)
	{
		return (BOOL)mysql_select_db(m_mysql,szDbName);
	}

	unsigned int TMySql::GetMemSize()
	{
		unsigned int nRet = 0;
		if (!m_conv_res)

			return nRet;

		for(unsigned int i = 0 ; i<m_conv_res->nRow ;i++)
		{
			for (unsigned int j = 0 ; j<m_conv_res->nCol ;j++)
			{
				if (m_conv_res->res)
				{
					nRet += m_conv_res->res[i][j].nConLen;
				}

			}
		}
		return nRet;
	}




	//MYSQLPOOL========================================================================================================================
	TMySqlPool::TMySqlPool(
		char *szHost, 
		char *szDBUserName, 
		char *szDBPwd, 
		unsigned int unDBPort,
		char *szDefDB,
		int nConNum 
		)
	{
		m_hNoticeHasCon		= ::CreateEvent(NULL,1,1,NULL);
		SetEvent(m_hNoticeHasCon);
		InitializeCriticalSection(&m_cs_queue);
		TMySql* my			= NULL;
		for (int i= 0;i<nConNum;i++)
		{
			my = new TMySql(szHost,szDBUserName,szDBPwd,unDBPort,szDefDB);
			m_mysql_queue.push(my);
		}
	}
	TMySqlPool:: ~TMySqlPool()
	{
		TMySql* Temp = NULL;
		lock();
		while (!m_mysql_queue.empty())
		{
			Temp = m_mysql_queue.front();
			m_mysql_queue.pop();
			if (Temp)
			{
				delete Temp;
				Temp = NULL;
			}
		}
		unlock();
		DeleteCriticalSection(&m_cs_queue);
		CloseHandle(m_hNoticeHasCon);
	}

	TMySql* TMySqlPool::GetConnect(char* szDBName)
	{
		if (!szDBName)
		{
			return NULL;
		}
		TMySql* Temp = NULL;
		lock();


		if ( !m_mysql_queue.empty() )
		{
			Temp = m_mysql_queue.front();
			m_mysql_queue.pop();

			Temp->Ping();
			Temp->ChangeDefDb(szDBName);
		}


		unlock();
		return Temp;
	}
	void	TMySqlPool::FreeConnect(TMySql* my)
	{
		if (!my)
		{
			return;
		}
		my->Free();
		lock();
		m_mysql_queue.push(my);	
		unlock();


	}


}
