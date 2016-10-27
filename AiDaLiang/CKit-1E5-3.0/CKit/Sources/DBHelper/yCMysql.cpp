/*********************************************************
*   ��Ȩ����(C) 2000-2050, �����а�Ĭ�ؿƼ����޹�˾.
*
*   �� �� �� : yCMysql .cpp
*             
*   ��    �� : ��   ��
*
*   ������� : ��ʼʱ�䣺2005-06-27
*
*   ��    �� : yCMysql ���Ա�������Է������ݿ�����ӿڵĶ��塣
*
*   �޸ļ�¼ : 2005-07-18
*               
*   ���ݽṹ :
*       
*   ��ע: 
*       
*********************************************************/
#include "stdafx.h"
#include "yCMysql.h" 

//#ifdef WIN32 
//
//#ifdef _WIN64
//
//#pragma comment(lib,"libmySQL_64.lib")
//
//#else
//
//#pragma comment(lib,"libmySQL_32.lib") 
//
//#endif // X64
//
//#endif // WIN32

#ifndef WIN32
#include <string.h>
//#define	NULL 0
#endif
/****************************************************
**	���캯�� 
****************************************************/

yCMysql::yCMysql()
{
	mydata			= NULL;   
	m_pResult		= NULL;
	m_irowNum	=0;		
	m_ifieldNum	=0;
}

/****************************************************
**	�������� 
****************************************************/
yCMysql::~yCMysql()
{
	if( mydata )
	{	
		mysql_close(mydata);
		mydata = NULL;
	}

	if(m_pResult)
	{	
		mysql_free_result( m_pResult );
		m_pResult = NULL;
	}
}

/*******************�ⲿ�ӿ�************************/		

/****************************************************
**	������ yCMysql::Login()
**	���ԣ� public
**	���ܣ� ��ʼ�����ݿ�����
	��Σ�
		HostName �� ��������IP��ַ
		UserName �� ���ݿ��û���
		PassWord �� ���ݿ����
		DbName �� ���ݿ���
		char_set �� �ַ�������
		Port �� ���ݿ�����Ӷ˿�
		U_Sock �� ���ݿ�������׽���
		Flag �� ��־
**	����ֵ��int
**			1  �ɹ�;
**			0  ʧ��;       
****************************************************/
int yCMysql::Login(                     const char		*HostName, 
					const char		*UserName,  
					const char		*PassWord, 
					const char		*DbName, 
					const char		*char_set,
					unsigned int	        Port,
					const char		*U_Sock,
					unsigned int	        Flag)
{	
	if(mydata)
	{		
		if(!mysql_ping(mydata))
		{		
			return 1;
		}
	}			

	mydata = mysql_init((MYSQL *)NULL);
	
	if(strlen(char_set) != 0)
	{
		mysql_options (mydata , MYSQL_SET_CHARSET_NAME , char_set);
	}

	if(mydata)
	{	
		if(mysql_real_connect(mydata,  HostName,  UserName, PassWord,  DbName,  
							   Port,   U_Sock,  Flag))
		{
			return 1;
		}
        else
		{
		//	printf( "real_connect fail:\t" );
		//	printf( "%s\n",mysql_error(mydata) );
			memset(m_error_str,0,sizeof(m_error_str));
			strncpy_s(m_error_str,2048,mysql_error(mydata),2047);

			mysql_close( mydata );
			mydata = NULL;
			return 0;
		}    
	}
    
	return 0;
}

char * yCMysql::GetLastErrorStr()
{
	return m_error_str;
}

/****************************************************
**	������ int yCMysql::Begin()
**	���ԣ� public
**	���ܣ� ������ʼ��־
**	����ֵ��int  �����ɹ�����1������ʧ�ܷ���0��      
****************************************************/
int yCMysql::Begin()
{
	if(!QueryWrite ("SET AUTOCOMMIT=0"))
	{
		return 0;
	}

	if(!QueryWrite ("START TRANSACTION"))
	{
		return 0;
	}
	return 1;
}

/****************************************************
**	������ int yCMysql::Commit()
**	���ԣ� public
**	���ܣ� �ύ����
**	����ֵ��int  �����ɹ�����1������ʧ�ܷ���0��      
****************************************************/
int yCMysql::Commit()
{
	if(!QueryWrite ("COMMIT"))
	{
		return 0;
	}

	return 1;
}

/****************************************************
**	������ int yCMysql::Rollback()
**	���ԣ� public
**	���ܣ� �ع����ݿ����
**	����ֵ��int  �����ɹ�����1������ʧ�ܷ���0��      
****************************************************/
int yCMysql::Rollback()
{
	if(!QueryWrite ("ROLLBACK"))
	{
		return 0;
	}
	
	return 1;
}

/****************************************************
**	������ int yCMysql::CheckConnect()
**	���ԣ� public
**	���ܣ� ������ݿ������Ƿ���Ч�����ʧ���������������
**	����ֵ��int  �����ɹ�����1������ʧ�ܷ���0��      
****************************************************/
int yCMysql::CheckConnect()
{
	if (!mydata)
	{
		return 0;
	}
	
	if(mysql_ping(mydata))
	{
		return 0;
	}
	
	return 1;
}

/****************************************************
**	������ int yCMysql::Logout()
**	���ԣ� public
**	���ܣ� �˳����ݿ������
**	����ֵ��int  �����ɹ�����1������ʧ�ܷ���0��      
****************************************************/
int yCMysql::Logout()
{
	if(mydata != NULL)
	{
		mysql_close(mydata);
		mydata = NULL;
	}
	
	if(m_pResult)
	{
		mysql_free_result(m_pResult);
		m_pResult = NULL;
	}

	return 1;
}

/****************************************************
**	������ int yCMysql::QueryWrite(const char *query)
**	���ԣ� public
**	���ܣ� �����ݿ�д����
**  ������ (IN)query - ��Ӧ������SQL��ѯ��䣻
**	����ֵ�������ɹ�����1��ʧ�ܷ���0��
****************************************************/
int yCMysql::QueryWrite (const char *query)
{
	if (mydata == NULL) return 0;
	FreeResult ();

	//ִ��д��SLQ����
	int iWrite=mysql_send_query(mydata, query, (unsigned long)strlen(query));
	if(iWrite==0)//�ɹ�
	{
		int mCheck=mysql_read_query_result(mydata);
		if(mCheck!=0)//ʧ��
		{
//			printf("write_real_query fail:\t");
//			printf("%s\n",mysql_error(mydata));
			return 0;				
		}
		else//�ɹ�
		{  
			m_irowNum = (int)mysql_affected_rows(mydata);		//���ѯ����е�����
			return 1;	
		}
	}
	else//ʧ��
	{
// 		printf("write_real_query fail:\t");
//		printf("%s\n",mysql_error(mydata));
		return 0;
	} 
}

/****************************************************
**	������ int yCMysql::QueryRead(const char *query)
**	���ԣ� public
**	���ܣ� �����ݿ������
**  ������ (IN)query - ��Ӧ������SQL��ѯ���,�������з���ֵ�ģ�
**	����ֵ��int(O)  �����ɹ�����1��ʧ�ܷ���0��
****************************************************/
int yCMysql::QueryRead(const char *query)
{
	if (mydata == NULL) return 0;
	FreeResult ();

	//ִ�ж�SQL������
	int iError=mysql_real_query(mydata, query, (unsigned long)strlen(query));
	if(iError!=0)//ʧ��
	{
//		printf("read_real_query fail:\t");
//		printf("%s\n",mysql_error(mydata));
		return 0;
	}
	
	//�ɹ���Ҫ�ѽ����ѯ��MYSQL_RES�ṹ��ȥ��������Ӧ�Ĵ���
	m_pResult = mysql_store_result(mydata);

	if(!m_pResult)//����Ƿ��д�����
	{
		if( !mysql_field_count(mydata) )//�Ƿ�SELECT����ѯ
		{
//			printf("No error here,just a non-SELECT statment...\n");
			return 0;
		}
		else
		{
//			printf("An error has occurred! The error is:\t");
//			printf("%s\n",mysql_error(mydata));
			return 0;
		}
	}

	//�޴����������д���
	m_irowNum = (int)mysql_num_rows(m_pResult);		//���ѯ����е�����
	m_ifieldNum = (int)mysql_num_fields(m_pResult);	//��ÿ���е��ֶ���


	return 1;
}

/****************************************************
**	������ int yCMysql::GetRow(STRU& myStru)
**	���ԣ� public
**	���ܣ� ��ȡ����
**  ������ ��
**	����ֵ�������ɹ�����1��ʧ�ܷ���0��
****************************************************/
int yCMysql::GetRow()
{
	row = mysql_fetch_row(m_pResult);	
	return 1;
}

/****************************************************
**	������ int yCMysql::GetRowNum()
**	���ԣ� public
**	���ܣ� ��ȡ��ѯ��������е�����
**  ������ ��
**	����ֵ��������
****************************************************/
int yCMysql::GetRowNum()
{
	return m_irowNum;
}

/****************************************************
**	������ int yCMysql::GetFieldNum()
**	���ԣ� public
**	���ܣ� ��ȡ��ѯ���������ÿ�е��ֶ���
**  ������ ��
**	����ֵ���ֶ�����
****************************************************/
int yCMysql::GetFieldNum()
{
	return m_ifieldNum;
}

/************************************************************************
  ������  int	yCMysql::GetFieldName(char * name,int name_len,int FieldItem)
  ���ԣ�public
  ���ܣ���ȡ��¼���е��ֶ���
  ��������
  ����ֵ��1��ʾ�ɹ���0��ʾʧ��
************************************************************************/
int	yCMysql::GetFieldName(char * name,int name_len,int FieldItem)
{ 
	
	MYSQL_FIELD * lpfield = mysql_fetch_field_direct(m_pResult,FieldItem);

	if (lpfield->name && name_len >= (int)lpfield->name_length)
	{
		memcpy(name,lpfield->name,lpfield->name_length);

		return 1;
	}

	return 0;
}

/****************************************************
**	������ void yCMysql::FreeResult()
**	���ԣ� public
**	���ܣ� �ͷŲ�ѯ���
**  ������ ��
**	����ֵ:�ޡ�
****************************************************/
void yCMysql::FreeResult()
{
	if(m_pResult)
	{
		mysql_free_result(m_pResult);
		m_pResult = NULL;
	}
	if(row)
	{
		row = 0;
	}
	m_irowNum	=0;		
	m_ifieldNum	=0;
}

/************************************************************************ 
**		������ MYSQL_FIELD * yCMysql::GetFile(int item)
**		���ԣ�pbuilc
**		���ܣ�ȡ�ֶ����Ľṹ����
**		�������ֶ���˳��
**		����ֵ����
**		��ע��2010��5��27�նν�������                                                                    
 ************************************************************************/
MYSQL_FIELD * yCMysql::GetFile(unsigned int item)
{
	if(!m_pResult)
		return NULL;

	if (item > m_pResult->field_count)
	{
		return NULL;
	}

	return &m_pResult->fields[item];

}
/*
/************************************************************************ 
**		������ int yCMysql::GetFileCount()
**		���ԣ�pbuilc
**		���ܣ�ȡ���β�ѯ�ֶ�������
**		��������
**		����ֵ����        
**		��ע��2010��5��27�նν�������                                                              
 ************************************************************************/
 /*int yCMysql::GetFileCount()
 {
	 return m_ifieldNum;
 }*/
 

