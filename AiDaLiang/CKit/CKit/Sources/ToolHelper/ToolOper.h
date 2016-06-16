// ToolOper.h: interface for the CToolOper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOOLOPER_H__3E321A7C_C8BC_4687_8CEE_A2C390C0F2D5__INCLUDED_)
#define AFX_TOOLOPER_H__3E321A7C_C8BC_4687_8CEE_A2C390C0F2D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <tchar.h>
#include <map>
#include <string>
#include <Psapi.h>
 
enum
{
	FILEDESCRIPTION = 1,
		LEGALTRADEMARKS,
		PRODUCTNAME,

};

typedef	std::map<DWORD,std::string>		VERSION_INFO_LIST;

typedef	struct _PROCESS_INFO_2
{
	std::string	process_name;		//进程名
	std::string	process_path;		//进程路径
}PROCESS_INFO_2,*PPROCESS_INFO_2;

typedef	std::map<DWORD,PROCESS_INFO_2>		SYSTEM_PROCESS_MAP;

class CToolOper  
{
public:
	CToolOper();
	virtual ~CToolOper();

	BOOL	ReadFileVersionInfo(std::string filename,VERSION_INFO_LIST &_list);

	BOOL	GetProcessList(SYSTEM_PROCESS_MAP &process_list);

	void	GetProcessInfo(DWORD pid, std::string &process_name, std::string &process_path);

	//增加获取某个特定进程的内存占用情况
	void    get_memory_info(DWORD pid, PPROCESS_MEMORY_COUNTERS ptr);
	size_t  get_work_size(HANDLE hProcess, SIZE_T pageSize, HINSTANCE hinstance);
	size_t  get_work_size(DWORD pid);

};
BOOL DosPathToNtPath(LPTSTR pszDosPath, LPTSTR pszNtPath);
#endif // !defined(AFX_TOOLOPER_H__3E321A7C_C8BC_4687_8CEE_A2C390C0F2D5__INCLUDED_)
