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
	char	driver_path[260];		//驱动路径

}DRIVER_INFO,*PDRIVER_INFO;

typedef	struct	_PROCESS_INFO
{
	ULONG		ParentId;		//父进程
	ULONG		ProcessId;		//子进程
	BOOLEAN		IsCreate;		//是否创建进程，TRUE，创建进程，FALSE结束进程
}PROCESS_INFO,*PPROCESS_INFO;

typedef	struct	_PROTECTED_PROCESS_INFO
{
	ULONG		ParentId;		//父进程
	ULONG		ProcessId;		//子进程
	ULONG		oper_type;		//操作方式 OPER_TERMINATE_PROCESS  OPER_WRITE_PROCESS，组合，有可能是
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
	
//UINT			当前操作类型	LOAD_DRIVER	CREATE_TERMINATE_PROCESS	OPER_PROTECTED_PROCESS
//pvoid			操作具体数据，DRIVER_INFO  PROCESS_INFO  PROCESS_INFO
//Dword			第二个参数的数据长度
//返回值		返回真，表示允许操作，假，不允许操作

typedef  BOOL   (WINAPI * CALLBACK_FILTERFUN)  (UINT,PVOID,DWORD);//定义一个此类型的回调函数

BOOL WINAPI	RegCallBackFun(CALLBACK_FILTERFUN fun);	//注册回调
BOOL	WINAPI		OperSysFilter(BOOL start);		//加载，停止驱动
BOOL	WINAPI		OperMsgFilter(BOOL start);		//加载消息过滤保护
BOOL	WINAPI		OperPorcessStatus(BOOL start);	//停止或者启动保护进程模块
