/*********************************************************
*   ��Ȩ����(C) 2000-2050, �����а�Ĭ�ؿƼ����޹�˾.
*
*   �� �� �� : yCMysql.h
*             
*   ��    �� : ��   ��
*
*   ������� : 2005-06-25
*
*   ��    �� : yCMysql�࣬�Է������ݿ�����ķ�װ��
*
*   �޸ļ�¼ : 2005-07-18
*               
*   ���ݽṹ :
*       
*   ��ע:   
*
*       
***********************************************************/
#ifndef __YCMYSQL_H__
#define __YCMYSQL_H__

#ifdef WIN32 
#include <winsock2.h>
#endif

/**********************
**yCMysql��Ķ���
**********************/

class yCMysql
{
private:
	MYSQL *mydata;//���ݿ�����
	MYSQL_RES *m_pResult;//��ѯ�������
	int m_irowNum;//�����е�����
	int m_ifieldNum;//ÿһ�е��ֶ���
	char	m_error_str[2048];
public:	
	MYSQL_ROW row;//ÿһ�м�¼
public:
	yCMysql ();//���캯��
	virtual ~yCMysql ();//��������

public:	
	char * GetLastErrorStr();	//�õ������¼
	int Begin();//���������ʼ
	int Commit();//�����ύ
	int Rollback();//����ع�
	void FreeResult();//�ͷŲ�ѯ�������
	int GetFieldNum();//��ȡ����
	int GetFieldName(char * name,int name_len,int FieldItem);	//��ȡ�ֶ�����
	int GetRowNum();//��ȡÿ�е��ֶ���
	MYSQL_FIELD * GetFile(unsigned int item);//�õ��ֶε���ϸ��Ϣ
	int GetRow();//��ÿ����¼д��row�ṹ
	int QueryRead(const char *query); //�����ݿ��в�ѯ����,��д��m_pResult��
													  
	int QueryWrite(const char *query);//����SQL���д����

	int Login(const char *HostName,	  //�������ݿ�
		const char *UserName, 
		const char *PassWord,		
		const char *Db, 
		const char *char_set = "",
		unsigned int Port=0,
		const char *U_Sock="",
		unsigned int Flag=0);
	
	int CheckConnect();				//������ݿ������Ƿ���Ч

	int Logout();					  //�Ͽ����ݿ�����
};
#endif
