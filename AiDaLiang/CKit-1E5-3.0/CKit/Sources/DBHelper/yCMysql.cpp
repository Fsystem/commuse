/*********************************************************
*   版权所有(C) 2000-2050, 深圳市艾默特科技有限公司.
*
*   文 件 名 : yCMysql .cpp
*             
*   作    者 : 王   礼
*
*   完成日期 : 开始时间：2005-06-27
*
*   描    述 : yCMysql 类成员函数，对访问数据库操作接口的定义。
*
*   修改记录 : 2005-07-18
*               
*   数据结构 :
*       
*   备注: 
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
**	构造函数 
****************************************************/

yCMysql::yCMysql()
{
	mydata			= NULL;   
	m_pResult		= NULL;
	m_irowNum	=0;		
	m_ifieldNum	=0;
}

/****************************************************
**	析构函数 
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

/*******************外部接口************************/		

/****************************************************
**	函数： yCMysql::Login()
**	属性： public
**	功能： 初始化数据库连接
	入参：
		HostName － 主机名或IP地址
		UserName － 数据库用户名
		PassWord － 数据库口令
		DbName － 数据库名
		char_set － 字符集名称
		Port － 数据库的连接端口
		U_Sock － 数据库的连接套接字
		Flag － 标志
**	返回值：int
**			1  成功;
**			0  失败;       
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
**	函数： int yCMysql::Begin()
**	属性： public
**	功能： 事务处理开始标志
**	返回值：int  操作成功返回1，操作失败返回0。      
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
**	函数： int yCMysql::Commit()
**	属性： public
**	功能： 提交操作
**	返回值：int  操作成功返回1，操作失败返回0。      
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
**	函数： int yCMysql::Rollback()
**	属性： public
**	功能： 回滚数据库操作
**	返回值：int  操作成功返回1，操作失败返回0。      
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
**	函数： int yCMysql::CheckConnect()
**	属性： public
**	功能： 检查数据库连接是否有效，如果失败则进行重新连接
**	返回值：int  操作成功返回1，操作失败返回0。      
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
**	函数： int yCMysql::Logout()
**	属性： public
**	功能： 退出数据库的连接
**	返回值：int  操作成功返回1，操作失败返回0。      
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
**	函数： int yCMysql::QueryWrite(const char *query)
**	属性： public
**	功能： 对数据库写操作
**  参数： (IN)query - 相应操作的SQL查询语句；
**	返回值：操作成功返回1，失败返回0。
****************************************************/
int yCMysql::QueryWrite (const char *query)
{
	if (mydata == NULL) return 0;
	FreeResult ();

	//执行写入SLQ操作
	int iWrite=mysql_send_query(mydata, query, (unsigned long)strlen(query));
	if(iWrite==0)//成功
	{
		int mCheck=mysql_read_query_result(mydata);
		if(mCheck!=0)//失败
		{
//			printf("write_real_query fail:\t");
//			printf("%s\n",mysql_error(mydata));
			return 0;				
		}
		else//成功
		{  
			m_irowNum = (int)mysql_affected_rows(mydata);		//求查询结果中的行数
			return 1;	
		}
	}
	else//失败
	{
// 		printf("write_real_query fail:\t");
//		printf("%s\n",mysql_error(mydata));
		return 0;
	} 
}

/****************************************************
**	函数： int yCMysql::QueryRead(const char *query)
**	属性： public
**	功能： 对数据库读操作
**  参数： (IN)query - 相应操作的SQL查询语句,必须是有返回值的；
**	返回值：int(O)  操作成功返回1，失败返回0。
****************************************************/
int yCMysql::QueryRead(const char *query)
{
	if (mydata == NULL) return 0;
	FreeResult ();

	//执行读SQL语句操作
	int iError=mysql_real_query(mydata, query, (unsigned long)strlen(query));
	if(iError!=0)//失败
	{
//		printf("read_real_query fail:\t");
//		printf("%s\n",mysql_error(mydata));
		return 0;
	}
	
	//成功后要把结果查询到MYSQL_RES结构中去并做出相应的处理。
	m_pResult = mysql_store_result(mydata);

	if(!m_pResult)//检查是否有错误发生
	{
		if( !mysql_field_count(mydata) )//是非SELECT语句查询
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

	//无错误发生，进行处理
	m_irowNum = (int)mysql_num_rows(m_pResult);		//求查询结果中的行数
	m_ifieldNum = (int)mysql_num_fields(m_pResult);	//求每行中的字段数


	return 1;
}

/****************************************************
**	函数： int yCMysql::GetRow(STRU& myStru)
**	属性： public
**	功能： 获取数据
**  参数： 无
**	返回值：操作成功返回1，失败返回0。
****************************************************/
int yCMysql::GetRow()
{
	row = mysql_fetch_row(m_pResult);	
	return 1;
}

/****************************************************
**	函数： int yCMysql::GetRowNum()
**	属性： public
**	功能： 获取查询结果集合中的行数
**  参数： 无
**	返回值：行数。
****************************************************/
int yCMysql::GetRowNum()
{
	return m_irowNum;
}

/****************************************************
**	函数： int yCMysql::GetFieldNum()
**	属性： public
**	功能： 获取查询结果集合中每行的字段数
**  参数： 无
**	返回值：字段数。
****************************************************/
int yCMysql::GetFieldNum()
{
	return m_ifieldNum;
}

/************************************************************************
  函数：  int	yCMysql::GetFieldName(char * name,int name_len,int FieldItem)
  属性：public
  功能：获取记录信中的字段名
  参数：无
  返回值：1表示成功，0表示失败
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
**	函数： void yCMysql::FreeResult()
**	属性： public
**	功能： 释放查询结果
**  参数： 无
**	返回值:无。
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
**		函数： MYSQL_FIELD * yCMysql::GetFile(int item)
**		属性：pbuilc
**		功能：取字段名的结构对像
**		参数：字段名顺序
**		返回值：无
**		备注：2010年5月27日段建均新增                                                                    
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
**		函数： int yCMysql::GetFileCount()
**		属性：pbuilc
**		功能：取本次查询字段名个数
**		参数：无
**		返回值：无        
**		备注：2010年5月27日段建均新增                                                              
 ************************************************************************/
 /*int yCMysql::GetFileCount()
 {
	 return m_ifieldNum;
 }*/
 

