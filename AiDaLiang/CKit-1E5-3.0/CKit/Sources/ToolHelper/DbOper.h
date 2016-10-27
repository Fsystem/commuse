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
	typedef	std::map<std::string,DWORD>		FILED_MAP;			//�ֶ��б�
	typedef	std::vector<std::string>		RECODE_INFO;		//һ����¼
	typedef	std::vector<RECODE_INFO>		RECODE_INFO_VECTOR;	//N����¼

public: 
	std::string GetLastErrorStr();
	BOOL MyConnDB();
	BOOL ConnDB(std::string server_name,std::string user,std::string password,std::string db_name);
	int DisConn();
	BOOL OpenRecode(std::string sql);
	BOOL MoveNext();		//����һ��
	BOOL MovePre();			//ǰ��һ��
	BOOL Move(RESULT rs);	//�Ƶ���ǰ�����
	BOOL IsEnd();			//�ж��ǲ������
	BOOL IsBeing();			//�ж��ǲ����ʼ

public:
	virtual	DWORD IsWriteSql(std::string sql);
	virtual	BOOL OpenRecodeForWrite(std::string sql);
	virtual	BOOL OpenRecodeForRead(std::string	sql);

public:
	BOOL AddNewRcode(std::string sql);
	int Begin();//���������ʼ
	int Commit();//�����ύ
	int Rollback();//����ع�
public:
	BOOL ReadFieldValue(std::string FiledName,std::string &FiledValue);
	BOOL ReadFieldValue(std::string FiledName,DWORD &FiledValue);
	BOOL ReadFieldValue(DWORD FieldItem,std::string &FieldValue);
	BOOL ReadFieldValue(DWORD FieldItem,DWORD &FieldValue);

private:
	void Clear();
private:
	yCMysql				m_mysql;

	FILED_MAP			m_field_map;		//�ֶμ�¼
	RECODE_INFO_VECTOR	m_recode_vector;	//��¼��
	DWORD				m_max_field;		//�ֶ���������
	DWORD				m_max_recode;		//��¼������Ӧ��
	DWORD				m_oper_type;		//���һ�β�ѯ�Ĳ�����ʽ����д
	DWORD				m_write_recode;		//д������Ӱ��ļ�¼��


	DWORD				m_curr_recode;		//��ǰ��¼ָ��
};

#endif //__DbOper_H
