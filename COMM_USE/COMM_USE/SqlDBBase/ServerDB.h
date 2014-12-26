#ifndef __ServerDB_H
#define __ServerDB_H

/** ���ݿ⴦��

*   FileName  : ServerDB.h

*   Author    : Double Sword

*   date      : 2011-8-12

*   Copyright : Copyright belong to Author and ZhiZunNet.CO.

*   RefDoc    : --

*/

#include <list>
#include <SafeCriticalSection/SafeCriticalSection.h>
using namespace COMMUSE;

//���ݿ��ļ�
#import "c:\program files\common files\system\ado\msado15.dll" no_namespace rename ("EOF", "adoEOF")


typedef struct tagSqlParam
{
	_variant_t	vParam;
	bool		bIsResult;

}tagSqlParam;


/** ���ݿ������
*   @ClassName	: ServerDB
*   @Purpose	:
*				������ݿ���ʵ�ִ�д������̵���
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

	//ִ��sql���ߴ洢����typ�Ǵ洢���̲���û�в���������sql����ʱ�򶼲���Ҫ��pParam
	bool ExecuteSql(LPCTSTR sql,
		_RecordsetPtr& record ,
		CommandTypeEnum typ,
		long nParamCount = 0,
		tagSqlParam* pParam = NULL);



private:
	//��������Ƿ�����
	void CheckConnect(_ConnectionPtr& con);
	//��ȡ����
	_ConnectionPtr* GetConnect();

protected:
private:
	int						_nDBConnectCount;//���ݿ�������
	TCHAR					_defDb[256];
	TCHAR					_tszUser[33];
	TCHAR					_tszPwd[33];
	DWORD					_port;
	bool					_init;
	TCHAR					_tszConnStr[256];
	_ConnectionPtr*			_pCon;		//���ݿ�����
	CCritiSection			_cs_db;
	FREE_DB_CONNECT_LIST	_freeDbConnectList;
	long					_lFreeConnectNum;//����������
};
#endif //__ServerDB_H