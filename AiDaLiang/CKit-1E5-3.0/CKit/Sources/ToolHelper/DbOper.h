#ifndef __DbOper_H
#define __DbOper_H

class CDbOper
{
public:
	enum
	{
		SQL_WRITE = 1,
		SQL_READ,
		SQL_NOKNOW,
	};
	enum RESULT
	{
		RESULT_BEGIN = 1,
		RESULT_END = 2,
	};
public:
	CDbOper(void);
	~CDbOper(void);

public:
	typedef	std::map<std::string,DWORD>		FILED_MAP;			//字段列表
	typedef	std::vector<std::string>		RECODE_INFO;		//一条记录
	typedef	std::vector<RECODE_INFO>		RECODE_INFO_VECTOR;	//N条记录

public: 
	std::string GetLastErrorStr();
	BOOL MyConnDB();
	BOOL ConnDB(std::string server_name,std::string user,std::string password,std::string db_name);
	int DisConn();
	BOOL OpenRecode(std::string sql);
	BOOL MoveNext();		//后移一条
	BOOL MovePre();			//前移一条
	BOOL Move(RESULT rs);	//移到最前或最后
	BOOL IsEnd();			//判断是不是最后
	BOOL IsBeing();			//判断是不是最开始

public:
	virtual	DWORD IsWriteSql(std::string sql);
	virtual	BOOL OpenRecodeForWrite(std::string sql);
	virtual	BOOL OpenRecodeForRead(std::string	sql);

public:
	BOOL AddNewRcode(std::string sql);
	int Begin();//事务操作开始
	int Commit();//事务提交
	int Rollback();//事务回滚
public:
	BOOL ReadFieldValue(std::string FiledName,std::string &FiledValue);
	BOOL ReadFieldValue(std::string FiledName,DWORD &FiledValue);
	BOOL ReadFieldValue(DWORD FieldItem,std::string &FieldValue);
	BOOL ReadFieldValue(DWORD FieldItem,DWORD &FieldValue);

private:
	void Clear();
private:
	yCMysql				m_mysql;

	FILED_MAP			m_field_map;		//字段记录
	RECODE_INFO_VECTOR	m_recode_vector;	//记录集
	DWORD				m_max_field;		//字段数量，读
	DWORD				m_max_recode;		//记录数，对应读
	DWORD				m_oper_type;		//最后一次查询的操作方式，读写
	DWORD				m_write_recode;		//写操作，影响的记录数


	DWORD				m_curr_recode;		//当前记录指针
};

#endif //__DbOper_H
