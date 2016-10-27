/*********************************************************
*   版权所有(C) 2000-2050, 深圳市艾默特科技有限公司.
*
*   文 件 名 : yCMysql.h
*             
*   作    者 : 王   礼
*
*   完成日期 : 2005-06-25
*
*   描    述 : yCMysql类，对访问数据库操作的封装。
*
*   修改记录 : 2005-07-18
*               
*   数据结构 :
*       
*   备注:   
*
*       
***********************************************************/
#ifndef __YCMYSQL_H__
#define __YCMYSQL_H__

#ifdef WIN32 
#include <winsock2.h>
#endif

/**********************
**yCMysql类的定义
**********************/

class yCMysql
{
private:
	MYSQL *mydata;//数据库连接
	MYSQL_RES *m_pResult;//查询结果集合
	int m_irowNum;//集合中的行数
	int m_ifieldNum;//每一行的字段数
	char	m_error_str[2048];
public:	
	MYSQL_ROW row;//每一行记录
public:
	yCMysql ();//构造函数
	virtual ~yCMysql ();//析构函数

public:	
	char * GetLastErrorStr();	//得到错误记录
	int Begin();//事务操作开始
	int Commit();//事务提交
	int Rollback();//事务回滚
	void FreeResult();//释放查询结果集合
	int GetFieldNum();//获取行数
	int GetFieldName(char * name,int name_len,int FieldItem);	//获取字段名称
	int GetRowNum();//获取每行的字段数
	MYSQL_FIELD * GetFile(unsigned int item);//得到字段的详细信息
	int GetRow();//将每条记录写入row结构
	int QueryRead(const char *query); //从数据库中查询数据,并写到m_pResult中
													  
	int QueryWrite(const char *query);//根据SQL语句写操作

	int Login(const char *HostName,	  //连接数据库
		const char *UserName, 
		const char *PassWord,		
		const char *Db, 
		const char *char_set = "",
		unsigned int Port=0,
		const char *U_Sock="",
		unsigned int Flag=0);
	
	int CheckConnect();				//检查数据库连接是否有效

	int Logout();					  //断开数据库连接
};
#endif
