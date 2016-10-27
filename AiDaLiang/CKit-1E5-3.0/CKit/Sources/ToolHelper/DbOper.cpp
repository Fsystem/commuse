#include "stdafx.h"
#include "DbOper.h"

CDbOper::CDbOper( void )
{
	Clear();

}

CDbOper::~CDbOper( void )
{

}

void	CDbOper::Clear()
{
	m_write_recode = 0;
	m_oper_type = SQL_NOKNOW;
	m_max_recode = 0;
	m_max_field = 0;
	m_curr_recode = 0;
	m_recode_vector.clear();
	m_field_map.clear();
}


std::string	CDbOper::GetLastErrorStr()
{
	return m_mysql.GetLastErrorStr();
}

//连接数据库
BOOL CDbOper::ConnDB(std::string server_name,std::string user,std::string password,std::string db_name)
{
	if(!m_mysql.Login(server_name.c_str(), user.c_str(), password.c_str(), db_name.c_str(),"gbk"))
	{ 
		return FALSE;
	} 
	return TRUE;
}

int CDbOper::DisConn()//断开连接
{ 
	m_mysql.Logout();
	return 0;
}


BOOL CDbOper::MyConnDB()
{
	char	buffer[MAX_PATH];
	char	path[MAX_PATH];

	std::string	user,password,dbname,server;

	memset(buffer,0,sizeof(buffer));
	memset(path,0,sizeof(path));

	GetModuleFileNameA(NULL,path,MAX_PATH);
	char	*lpstr = strrchr(path,'\\');
	*lpstr = '\0';

	strcat_s(path,MAX_PATH,"\\db.ini");

	GetPrivateProfileStringA("DB_CONFIG","DBSERVER","127.0.0.1",buffer,MAX_PATH - 1,path);
	server = buffer;

	GetPrivateProfileStringA("DB_CONFIG","DBUSER","root",buffer,MAX_PATH - 1,path);
	user = buffer;

	GetPrivateProfileStringA("DB_CONFIG","DBPASSWORD","",buffer,MAX_PATH - 1,path);
	password = buffer;

	GetPrivateProfileStringA("DB_CONFIG","DBNAME","addata",buffer,MAX_PATH - 1,path);
	dbname = buffer;


	return ConnDB(server,user,password,dbname);
}

DWORD	CDbOper::IsWriteSql(std::string sql)
{
	if (_strnicmp(sql.c_str(),"select ",strlen("select ")) == 0)
	{
		return SQL_READ;
	}

	if (_strnicmp(sql.c_str(),"insert ",strlen("insert ")) == 0)
	{
		return SQL_WRITE;
	}

	if (_strnicmp(sql.c_str(),"delete ",strlen("delete ")) == 0)
	{
		return SQL_WRITE;
	}

	if (_strnicmp(sql.c_str(),"update ",strlen("update ")) == 0)
	{
		return SQL_WRITE;
	}

	if (_strnicmp(sql.c_str(),"create ",strlen("create ")) == 0)
	{
		return SQL_WRITE;
	}

	return SQL_NOKNOW;
}

BOOL CDbOper::OpenRecodeForWrite(std::string	sql)
{
	if(m_mysql.QueryWrite(sql.c_str()) == 0)
	{
		m_mysql.FreeResult();
		return FALSE;
	}

	//读取影响记录数
	m_write_recode = m_mysql.GetRowNum();

	return TRUE;
}

BOOL CDbOper::OpenRecodeForRead(std::string	sql)
{
	if(m_mysql.QueryRead(sql.c_str()) == 0)
	{
		m_mysql.FreeResult();
		return FALSE;
	}
	//先读字段数
	m_max_field = m_mysql.GetFieldNum();
	char		field[MAX_PATH];

	for (int i = 0; i != m_max_field; i++)
	{
		memset(field,0,sizeof(field));

		if (m_mysql.GetFieldName(field,MAX_PATH - 1,i))
		{
			m_field_map[field] = i;
		}
		else
		{
			Clear();
			m_mysql.FreeResult();
			return FALSE;
		}
	}

	//读取记录
	m_max_recode = m_mysql.GetRowNum();

	RECODE_INFO		recode_info;
	std::string		value_str;

	for ( int j = 0; j != m_max_recode; j++)
	{
		recode_info.clear();
		m_mysql.GetRow();

		for(int i = 0; i != m_max_field; i++)
		{
			if (m_mysql.row[i] != NULL)
			{
				value_str = m_mysql.row[i];
			}
			else
			{
				value_str = "";
			}

			recode_info.push_back(value_str);
		}

		m_recode_vector.push_back(recode_info);
	}

	m_mysql.FreeResult();	//释放结果集
	return TRUE;

}

//打开记录集
BOOL CDbOper::OpenRecode(std::string	sql)
{
	Clear();

	if (!m_mysql.CheckConnect())
	{
		return FALSE;
	}

	m_oper_type = IsWriteSql(sql); 

	switch(m_oper_type)
	{
	case SQL_READ:
		return OpenRecodeForRead(sql);
		break;
	case SQL_WRITE:
		return OpenRecodeForWrite(sql);
		break;
	case SQL_NOKNOW:
		return FALSE;
		break;
	} 

	return TRUE;
}

//增加一个写记录到数据库中，可以开启事务，加快速度
BOOL	CDbOper::AddNewRcode(std::string	sql)
{
	if(m_mysql.QueryWrite(sql.c_str()) == 0)
	{
		m_mysql.FreeResult();
		return FALSE;
	}

	m_mysql.FreeResult();

	return TRUE;
}

BOOL CDbOper::ReadFieldValue(std::string FieldName,std::string &FieldValue)
{
	FILED_MAP::iterator iter = m_field_map.find(FieldName);

	if (iter == m_field_map.end())
	{
		return FALSE;
	}

	DWORD	FieldItem = iter->second;


	return ReadFieldValue(FieldItem,FieldValue);
}

BOOL CDbOper::ReadFieldValue(std::string FieldName,DWORD &FieldValue)
{
	FILED_MAP::iterator iter = m_field_map.find(FieldName);

	if (iter == m_field_map.end())
	{
		return FALSE;
	}

	DWORD	FieldItem = iter->second;


	return ReadFieldValue(FieldItem,FieldValue);
}


BOOL CDbOper::ReadFieldValue(DWORD FieldItem,std::string &FieldValue)
{
	if (FieldItem >= m_max_field)
	{
		return FALSE;
	}

	if (IsEnd())
	{
		return FALSE;		//指向最后一条记录了
	}

	FieldValue = m_recode_vector.at(m_curr_recode).at(FieldItem);

	return TRUE;
}

BOOL CDbOper::ReadFieldValue(DWORD FieldItem,DWORD &FieldValue)
{
	if (FieldItem >= m_max_field)
	{
		return FALSE;
	}

	if (IsEnd())
	{
		return FALSE;		//指向最后一条记录了
	}

	std::string	data_str = m_recode_vector.at(m_curr_recode).at(FieldItem);

	FieldValue = strtoul(data_str.c_str(),0,10);

	return TRUE;
}

int CDbOper::Begin()
{
	return m_mysql.Begin();
}

int CDbOper::Commit()
{
	return m_mysql.Commit();
}

int CDbOper::Rollback()
{
	return m_mysql.Rollback();
}

BOOL CDbOper::MoveNext()
{
	if (IsEnd())
	{
		return FALSE;
	}

	m_curr_recode ++;

	return TRUE;
}

BOOL CDbOper::MovePre()
{
	if (IsBeing())
	{
		return FALSE;
	}

	m_curr_recode --;
	return TRUE;
}

BOOL CDbOper::Move( RESULT rs )
{
	switch(rs)
	{
	case RESULT_BEGIN:
		m_curr_recode = 0;
		break;

	case RESULT_END:
		m_curr_recode = m_max_recode;
		break;
	}

	return TRUE;

}

BOOL CDbOper::IsEnd()
{
	if (m_curr_recode == m_max_recode)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CDbOper::IsBeing()
{
	if (m_curr_recode == 0)
	{
		return TRUE;
	}

	return FALSE;
}

 