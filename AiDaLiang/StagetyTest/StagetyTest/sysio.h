#pragma once

#include <map>

enum
{
	LOAD_DRIVER = 1,
	CREATE_TERMINATE_PROCESS,
	OPER_PROTECTED_PROCESS,
	OPER_ERROR = 101
};

 
#define 	OPER_TERMINATE_PROCESS		(0x0001)
#define		OPER_WRITE_PROCESS			(0x0002)
#define		OPER_TERMINATE_MSG			(0x0004)
 
#pragma pack(push, 1)


typedef	struct	_DRIVER_INFO
{
	char	driver_path[260];		//����·��

}DRIVER_INFO,*PDRIVER_INFO;

typedef	struct	_PROCESS_INFO
{
	ULONG		ParentId;		//������
	ULONG		ProcessId;		//�ӽ���
	BOOLEAN		IsCreate;		//�Ƿ񴴽����̣�TRUE���������̣�FALSE��������
}PROCESS_INFO,*PPROCESS_INFO;

typedef	struct	_PROTECTED_PROCESS_INFO
{
	ULONG		ParentId;		//������
	ULONG		ProcessId;		//�ӽ���
	ULONG		oper_type;		//������ʽ OPER_TERMINATE_PROCESS  OPER_WRITE_PROCESS����ϣ��п�����
}PROTECTED_PROCESS_INFO,*PPROTECTED_PROCESS_INFO;

typedef	struct	_OPER_ERROR_INFO
{
	DWORD		error;
	char		error_str[MAX_PATH];
	char		file_name[MAX_PATH];
	DWORD		file_line;
}OPER_ERROR_INFO,*POPER_ERROR_INFO;

#pragma pack(pop)

typedef	struct	_OPER_PROCESS_INFO
{
	ULONG		pid;
	INT64		oper_time;
}OPER_PROCESS_INFO,*POPER_PROCESS_INFO;

typedef	std::map<ULONG,ULONG>		OPER_TERMINATE_PROCESS_MAP;
	
//UINT			��ǰ��������	LOAD_DRIVER	CREATE_TERMINATE_PROCESS	OPER_PROTECTED_PROCESS
//pvoid			�����������ݣ�DRIVER_INFO  PROCESS_INFO  PROCESS_INFO
//Dword			�ڶ������������ݳ���
//����ֵ		�����棬��ʾ����������٣����������

typedef  BOOL   (WINAPI * CALLBACK_FILTERFUN)  (UINT,PVOID,DWORD);//����һ�������͵Ļص�����

BOOL WINAPI	RegCallBackFun(CALLBACK_FILTERFUN fun);	//ע��ص�
BOOL	WINAPI		OperSysFilter(BOOL start);		//���أ�ֹͣ����
BOOL	WINAPI		OperMsgFilter(BOOL start);		//������Ϣ���˱���
BOOL	WINAPI		OperPorcessStatus(BOOL start);	//ֹͣ����������������ģ��
