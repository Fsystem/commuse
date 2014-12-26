#ifndef __ServerDB_H
#define __ServerDB_H

/** 数据库处理

*   FileName  : ServerDB.h

*   Author    : Double Sword

*   date      : 2011-8-12

*   Copyright : Copyright belong to Author and ZhiZunNet.CO.

*   RefDoc    : --

*/

#include <list>
#include <SafeCriticalSection/SafeCriticalSection.h>
using namespace COMMUSE;

//数据库文件
#import "c:\program files\common files\system\ado\msado15.dll" no_namespace rename ("EOF", "adoEOF")


typedef struct tagSqlParam
{
	_variant_t	vParam;
	bool		bIsResult;

}tagSqlParam;


/** 数据库服务类
*   @ClassName	: ServerDB
*   @Purpose	:
*				解决数据库访问的执行存贮过程的类
*
*
*
*   @Author	: Double Sword
*   @Data	: 2011-8-28
*/
class ServerDB
{
	typedef std::list<_ConnectionPtr*>	FREE_DB_CONNECT_LIST;
public:
	ServerDB(int nDBConnectCont,LPCTSTR defDB,LPCTSTR dbUser,LPCTSTR dbPwd,DWORD dbPort);
	~ServerDB();

	//执行sql或者存储过程typ是存储过程并且没有参数或者是sql语句的时候都不需要传pParam
	bool ExecuteSql(LPCTSTR sql,
		_RecordsetPtr& record ,
		CommandTypeEnum typ,
		long nParamCount = 0,
		tagSqlParam* pParam = NULL);



private:
	//检测连接是否正常
	void CheckConnect(_ConnectionPtr& con);
	//获取连接
	_ConnectionPtr* GetConnect();

protected:
private:
	int						_nDBConnectCount;//数据库连接数
	TCHAR					_defDb[256];
	TCHAR					_tszUser[33];
	TCHAR					_tszPwd[33];
	DWORD					_port;
	bool					_init;
	TCHAR					_tszConnStr[256];
	_ConnectionPtr*			_pCon;		//数据库连接
	CCritiSection			_cs_db;
	FREE_DB_CONNECT_LIST	_freeDbConnectList;
	long					_lFreeConnectNum;//空闲连接数
};
#endif //__ServerDB_H