#include <tchar.h>
#include <stdio.h>
#include <Windows.h>
#include <psapi.h>

#include <TlHelp32.h>

#include <atlconv.h>

#include "jkInjectDll.h"

#pragma comment(lib,"Psapi.lib")

//-------------------------------------------------------------------------------
static BOOL gInitialized = FALSE;

//-------------------------------------------------------------------------------
//函数库
//-------------------------------------------------------------------------------
static DWORD GetProcessIdByName(LPCTSTR procname)
{
	if(procname == NULL) return 0;

	HANDLE SnapShot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(SnapShot==NULL) return 0;

	PROCESSENTRY32 ProcessInfo;//声明进程信息变量
	ProcessInfo.dwSize=sizeof(ProcessInfo);//设置ProcessInfo的大小
	//返回系统中第一个进程的信息
	BOOL Status=Process32First(SnapShot,&ProcessInfo); 

	while(Status)
	{    
		if (_tcsnicmp(procname,ProcessInfo.szExeFile,MAX_PATH)==0)
		{
			CloseHandle(SnapShot);
			return ProcessInfo.th32ProcessID;
		}

		Status=Process32Next(SnapShot,&ProcessInfo);
	}
	CloseHandle(SnapShot);
	return 0;
}

//系统是否是64位0S
static BOOL IsWindows64()
{
	SYSTEM_INFO si={0};
	typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);
	PGNSI fnGetNativeSystemInfo = (PGNSI)GetProcAddress( GetModuleHandleA("kernel32"), "GetNativeSystemInfo");
	if (NULL != fnGetNativeSystemInfo) fnGetNativeSystemInfo(&si);
	else GetSystemInfo(&si);

	if(si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
		si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64) 
	{
		return TRUE;
	}
	else return FALSE;
}

/*判断是否是x64进程 
参  数:进程句柄 
返回值:是x64进程返回TRUE,否则返回FALSE 
*/  
BOOL IsWow64ProcessEx(HANDLE hProcess)  
{  
	if (!IsWindows64()) return FALSE;

	typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);   
	LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(TEXT("kernel32")), "IsWow64Process");  
  
	//如果没有导出则判定为32位 
	if(!fnIsWow64Process) return FALSE; 

	BOOL bX64=FALSE;  
	if(!fnIsWow64Process(hProcess,&bX64)) return FALSE;  

	return !bX64;  
}  

BOOL IsWow64ProcessEx(DWORD dwPid)  
{  
	BOOL bRet = FALSE;
	if(dwPid == 0) return bRet;

	HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_QUERY_LIMITED_INFORMATION ,FALSE,dwPid);
	if (hProcess)
	{
		bRet = IsWow64ProcessEx(hProcess);
		CloseHandle(hProcess);
	}
	return bRet;
}

BOOL IsWow64ProcessEx(LPCTSTR szExeName)
{
	return IsWow64ProcessEx(GetProcessIdByName(szExeName));
}

static BOOL hasDll(DWORD processId,LPCTSTR szDllName) 
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processId);
	if (hSnapshot == NULL)
	{
		return FALSE;
	}
	MODULEENTRY32 me;
	me.dwSize = sizeof(MODULEENTRY32);
	Module32First(hSnapshot, &me);
	do {
		if (_tcsncmp(me.szModule, szDllName , MAX_MODULE_NAME32) == 0) 
		{
			CloseHandle(hSnapshot);
			return TRUE;
		}
	} while (Module32Next(hSnapshot, &me));

	CloseHandle(hSnapshot);

	return FALSE;
}
static BOOL EnableDebugPrivilege()
{
	///提升权限
	TOKEN_PRIVILEGES tkp;
	HANDLE hToken;
	if (!OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
	{
		DWORD err = GetLastError();
		return false;
	}
	if(! LookupPrivilegeValue(NULL, SE_DEBUG_NAME,&tkp.Privileges[0].Luid))//获得本地机唯一的标识
	{ 
		DWORD err = GetLastError();
		CloseHandle(hToken);
		return false;
	}
	tkp.PrivilegeCount = 1;  
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
	AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp),(PTOKEN_PRIVILEGES) NULL, 0); //调整获得的权限
	if (GetLastError() != ERROR_SUCCESS) 
	{
		DWORD err = GetLastError();
		CloseHandle(hToken);
		return false;
	}

	CloseHandle(hToken);

	return true;
}

static BOOL IsVistaOrLater()  
{  
	OSVERSIONINFO osvi;  
	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));  
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);  
	GetVersionEx(&osvi);  
	if( osvi.dwMajorVersion >= 6 )  
		return TRUE;  
	return FALSE;  
} 

static HANDLE jkCreateRemoteThread(HANDLE hProcess, LPTHREAD_START_ROUTINE pThreadProc, LPVOID pRemoteBuf)  
{  
	typedef void* POBJECT_ATTRIBUTES;
	typedef void* PNT_PROC_THREAD_ATTRIBUTE_LIST;

	typedef NTSTATUS (__stdcall *PFNTCREATETHREADEX)(
		OUT PHANDLE ThreadHandle,
		IN ACCESS_MASK DesiredAccess,
		IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
		IN HANDLE ProcessHandle,
		IN PTHREAD_START_ROUTINE StartRoutine,
		IN PVOID StartContext,
		IN BOOL CreateSuspended,
		IN SIZE_T ZeroBits OPTIONAL,
		IN SIZE_T StackSize OPTIONAL,
		IN SIZE_T MaximumStackSize OPTIONAL,
		IN PNT_PROC_THREAD_ATTRIBUTE_LIST AttributeList
		);

	HANDLE      hThread = NULL;  
	PFNTCREATETHREADEX     pFunc = NULL;  
	if( IsVistaOrLater() )    // Vista, 7, Server2008  
	{  
		pFunc = (PFNTCREATETHREADEX)GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtCreateThreadEx");  
		if( pFunc == NULL )  
		{  
			printf("MyCreateRemoteThread() : GetProcAddress(\"NtCreateThreadEx\") 调用失败！错误代码: [%d]/n",  
				GetLastError());  
			return FALSE;  
		}  
		pFunc(
			&hThread, 
			0x1FFFFF,  
			NULL,  
			hProcess,  
			pThreadProc,  
			pRemoteBuf,  
			FALSE,  
			NULL,  
			NULL,  
			NULL,  
			NULL);
		
		if( hThread == NULL )  
		{  
			printf("MyCreateRemoteThread() : NtCreateThreadEx() 调用失败！错误代码: [%d]/n", GetLastError());  
			return FALSE;  
		}  
	}  
	else                    // 2000, XP, Server2003  
	{  
		hThread = CreateRemoteThread(hProcess,   
			NULL,   
			0,   
			pThreadProc,   
			pRemoteBuf,   
			0,   
			NULL);  
		if( hThread == NULL )  
		{  
			printf("MyCreateRemoteThread() : CreateRemoteThread() 调用失败！错误代码: [%d]/n", GetLastError());  
			return FALSE;  
		}  
	}

	return hThread;  
} 

//-------------------------------------------------------------------------------
//64位进程注入
#include "wow64ext.h"
#pragma comment(lib,"jkInjectDll/wow64ext-new.lib")

typedef struct _UNICODE_STRING64  
{  
	USHORT Length;  
	USHORT MaximumLength;  
	LPWSTR Buffer;  
} UNICODE_STRING, *PUNICODE_STRING; 

typedef _CLIENT_ID<HANDLE> CLIENT_ID,*PCLIENT_ID;

NTSTATUS
	NTAPI
	RtlCreateUserThread(
	_In_ HANDLE processHandle,
	_In_ SECURITY_DESCRIPTOR* securityDescriptor,
	_In_ BOOLEAN createSuspended,
	_In_ ULONG stackZeroBits,
	_Inout_opt_ size_t* stackReserved,
	_Inout_opt_ size_t* stackCommit,
	_In_ const void* startAddress,
	_In_ void* startParameter,
	_Inout_ HANDLE* threadHandle,
	_Inout_opt_ CLIENT_ID* clientID
	);

NTSTATUS
	NTAPI
	LdrLoadDll(
	_In_opt_ PWSTR SearchPath,
	_In_opt_ PULONG LoadFlags,
	_In_ PUNICODE_STRING Name,
	_Out_opt_ PVOID *BaseAddress
	);

VOID
	NTAPI
	RtlExitUserThread(
	_In_ NTSTATUS Status
	);

template<typename Res, typename Deleter>
class ScopeResource {
	Res res;
	Deleter deleter;
	ScopeResource(const ScopeResource&) {}
public:
	Res get() const {
		return this->res;
	}
	ScopeResource(Res res, Deleter deleter) : res(res), deleter(deleter) {}
	~ScopeResource() {
		this->deleter(this->res);
	}
};

InjectResult Wow64InjectWin64(HANDLE hProcess, LPCWSTR filename, int nameLen)
{
	DWORD dwDesiredAccess = PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ;
	auto closeProcessHandle = [](HANDLE hProcess) {
		if(hProcess != NULL) CloseHandle(hProcess);
	};

	unsigned char injectCode[] = {
		0x48, 0x89, 0x4c, 0x24, 0x08,                               // mov       qword ptr [rsp+8],rcx
		0x57,                                                       // push      rdi
		0x48, 0x83, 0xec, 0x20,                                     // sub       rsp,20h
		0x48, 0x8b, 0xfc,                                           // mov       rdi,rsp
		0xb9, 0x08, 0x00, 0x00, 0x00,                               // mov       ecx,8
		0xb8, 0xcc, 0xcc, 0xcc, 0xcc,                               // mov       eac,0CCCCCCCCh
		0xf3, 0xab,                                                 // rep stos  dword ptr [rdi]
		0x48, 0x8b, 0x4c, 0x24, 0x30,                               // mov       rcx,qword ptr [__formal]
		0x49, 0xb9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov       r9,0
		0x49, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov       r8,0
		0x48, 0xba, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov       rdx,0
		0x48, 0xb9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov       rcx,0
		0x48, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov       rax,0
		0xff, 0xd0,                                                 // call      rax
		0x48, 0xb9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov       rcx,0
		0x48, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov       rax,0
		0xff, 0xd0                                                  // call      rax
	};

	size_t parametersMemSize = sizeof(DWORD64) + sizeof(_UNICODE_STRING_T<DWORD64>) + nameLen + sizeof(wchar_t);
	auto freeInjectCodeMem = [&hProcess, &injectCode](DWORD64 address) {
		if(address != 0) VirtualFreeEx64(hProcess, address, sizeof(injectCode), MEM_COMMIT | MEM_RESERVE);
	};
	ScopeResource<DWORD64, decltype(freeInjectCodeMem)> injectCodeMem(VirtualAllocEx64(hProcess, NULL, sizeof(injectCode), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE), freeInjectCodeMem);
	auto freeParametersMem = [&hProcess, parametersMemSize](DWORD64 address) {
		if(address != 0) VirtualFreeEx64(hProcess, address, parametersMemSize, MEM_COMMIT | MEM_RESERVE);
	};
	ScopeResource<DWORD64, decltype(freeParametersMem)> parametersMem(VirtualAllocEx64(hProcess, NULL, parametersMemSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE), freeParametersMem);
	if (injectCodeMem.get() == 0 || parametersMem.get() == 0) {
		return InjectResult::Error_VirtualAllocEx;
	}
	DWORD64 ntdll64 = GetModuleHandle64(L"ntdll.dll");
	DWORD64 ntdll_LdrLoadDll = GetProcAddress64(ntdll64, "LdrLoadDll");
	DWORD64 ntdll_RtlExitUserThread = GetProcAddress64(ntdll64, "RtlExitUserThread");
	DWORD64 ntdll_RtlCreateUserThread = GetProcAddress64(ntdll64, "RtlCreateUserThread");
	if(ntdll_LdrLoadDll == 0 || ntdll_RtlExitUserThread == 0 || ntdll_RtlCreateUserThread == 0) {
		return InjectResult::Error_GetProcAddress;
	}
	unsigned char* parameters(new unsigned char[parametersMemSize]);
	memset(parameters, 0, parametersMemSize);
	_UNICODE_STRING_T<DWORD64>* upath = reinterpret_cast<_UNICODE_STRING_T<DWORD64>*>(parameters + sizeof(DWORD64));
	upath->Length = nameLen ;
	upath->MaximumLength = nameLen +  sizeof(wchar_t);
	wchar_t* path = reinterpret_cast<wchar_t*>(parameters + sizeof(DWORD64) + sizeof(_UNICODE_STRING_T<DWORD64>));
	memcpy(path,filename,nameLen );
	upath->Buffer = parametersMem.get() + sizeof(DWORD64) + sizeof(_UNICODE_STRING_T<DWORD64>);

	union {
		DWORD64 from;
		unsigned char to[8];
	} cvt;

	// r9
	cvt.from = parametersMem.get();
	memcpy(injectCode + 32, cvt.to, sizeof(cvt.to));

	// r8
	cvt.from = parametersMem.get() + sizeof(DWORD64);
	memcpy(injectCode + 42, cvt.to, sizeof(cvt.to));

	// rax = LdrLoadDll
	cvt.from = ntdll_LdrLoadDll;
	memcpy(injectCode + 72, cvt.to, sizeof(cvt.to));

	// rax = RtlExitUserThread
	cvt.from = ntdll_RtlExitUserThread;
	memcpy(injectCode + 94, cvt.to, sizeof(cvt.to));

	if(FALSE == WriteProcessMemory64(hProcess, injectCodeMem.get(), injectCode, sizeof(injectCode), NULL)
		|| FALSE == WriteProcessMemory64(hProcess, parametersMem.get(), parameters, parametersMemSize, NULL)) {
			return InjectResult::Error_WriteProcessMemory;
	}

	DWORD64 hRemoteThread = 0;
	struct {
		DWORD64 UniqueProcess;
		DWORD64 UniqueThread;
	} client_id;

	X64Call(ntdll_RtlCreateUserThread, 10,
		(DWORD64)hProcess,					// ProcessHandle
		(DWORD64)NULL,                      // SecurityDescriptor
		(DWORD64)FALSE,                     // CreateSuspended
		(DWORD64)0,                         // StackZeroBits
		(DWORD64)NULL,                      // StackReserved
		(DWORD64)NULL,                      // StackCommit
		injectCodeMem.get(),                // StartAddress
		(DWORD64)NULL,                      // StartParameter
		(DWORD64)&hRemoteThread,            // ThreadHandle
		(DWORD64)&client_id);               // ClientID
	if(hRemoteThread != 0) {
		CloseHandle((HANDLE)hRemoteThread);
		return InjectResult::OK;
	}
	return InjectResult::Error_CreateRemoteThread;
}

//-------------------------------------------------------------------------------
// 32位注入(NtCreateThreadEx + LdrLoadDll)
//-------------------------------------------------------------------------------

typedef VOID (WINAPI *fRtlInitUnicodeString)(PUNICODE_STRING DestinationString,PCWSTR ourceString);

typedef NTSTATUS (WINAPI *fLdrLoadDll)(IN PWCHAR PathToFile OPTIONAL, IN ULONG Flags OPTIONAL, IN PUNICODE_STRING  ModuleFileName, OUT PHANDLE ModuleHandle);

typedef struct _NTCREATE_THREAD_EX_DATA
{
	PVOID fnRtlInitUnicodeString;
	PVOID fnLdrLoadDll;
	UNICODE_STRING UnicodeString;
	WCHAR DllName[260];
	PWCHAR DllPath;
	ULONG Flags;
	HANDLE ModuleHandle;
}NTCREATE_THREAD_EX_DATA,*PNTCREATE_THREAD_EX_DATA;

static HANDLE WINAPI NTCreateThreadExProc(PNTCREATE_THREAD_EX_DATA data)
{
	((fRtlInitUnicodeString)data->fnRtlInitUnicodeString)(&data->UnicodeString,data->DllName);
	((fLdrLoadDll)data->fnLdrLoadDll)(data->DllPath,data->Flags,&data->UnicodeString,&data->ModuleHandle);
	return data->ModuleHandle;
}

static DWORD WINAPI NTCreateThreadExProcEnd()
{
	return 0;
}


static HANDLE CreateThread32(HANDLE ProcessHandle,void*ThreadProc,void*Parameter,int Suspended)  
{  
	if(ThreadProc == NULL) return NULL;

	typedef LONG NTSTATUS, *PNTSTATUS, **PPNTSTATUS; 

	//#define STATUS_SUCCESS                      ((NTSTATUS) 0x00000000) 
	#define STATUS_INFO_LENGTH_MISMATCH			((NTSTATUS) 0xC0000004) 
	#define STATUS_IO_DEVICE_ERROR              ((NTSTATUS) 0xC0000185) 

	typedef PVOID PUSER_THREAD_START_ROUTINE; 

	typedef NTSTATUS (__stdcall *PRtlCreateThread)(HANDLE
		,PSECURITY_DESCRIPTOR
		,BOOLEAN,ULONG
		,SIZE_T
		,SIZE_T
		,PUSER_THREAD_START_ROUTINE
		,PVOID
		,PHANDLE
		,PCLIENT_ID); 


	PRtlCreateThread fnRtlCreateThread = (PRtlCreateThread)GetProcAddress64(GetModuleHandle64(L"Kernel32.dll"),"RtlCreateUserThread");

	HANDLE hThread = NULL;

	if (ProcessHandle!=NULL && ProcessHandle != INVALID_HANDLE_VALUE)
	{
		CLIENT_ID cid;
		NTSTATUS state = fnRtlCreateThread(ProcessHandle
						  ,NULL
						  ,true
						  ,0,0,0
						  ,ThreadProc
						  ,Parameter
						  ,&hThread
						  ,&cid);
		
		if (state == STATUS_SUCCESS)
		{
			::ResumeThread(hThread);
		}
	}

	return hThread;
} 

//32位进程注入
static InjectResult InjectLibrary32(HANDLE hProcess, const wchar_t*Name, unsigned short NameLength)
{
	InjectResult bRet = Error_Normal;

	if(hProcess == NULL || hProcess == INVALID_HANDLE_VALUE) return bRet;

	PVOID pParamData = NULL,pCodeData = NULL;

	pParamData = ::VirtualAllocEx(hProcess,0,4096,MEM_COMMIT | MEM_RESERVE,PAGE_READWRITE);
	if(!pParamData) goto RELEASE_DATA;

	DWORD dwCodeSize = DWORD(NTCreateThreadExProcEnd) - DWORD(NTCreateThreadExProc);
	pCodeData = ::VirtualAllocEx(hProcess, 0, dwCodeSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if(!pCodeData) goto RELEASE_DATA;

	//先写入参数内存
	NTCREATE_THREAD_EX_DATA data;
	HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
	data.fnRtlInitUnicodeString = GetProcAddress(hNtdll,"RtlInitUnicodeString");
	data.fnLdrLoadDll = GetProcAddress(hNtdll,"LdrLoadDll");
	memcpy(data.DllName, Name, NameLength+sizeof(wchar_t));
	data.DllPath = NULL;
	data.Flags = 0;
	data.ModuleHandle = INVALID_HANDLE_VALUE;

	if(!::WriteProcessMemory(hProcess,pParamData,&data,sizeof(data),NULL)) goto RELEASE_DATA;
	//写入代码
	if(!::WriteProcessMemory(hProcess,pCodeData,(PVOID)NTCreateThreadExProc,dwCodeSize,NULL)) goto RELEASE_DATA;

	//创建远程线程注入
	HANDLE hThread = CreateThread32(hProcess,(LPTHREAD_START_ROUTINE)pCodeData,pParamData,0);

	if (hThread)
	{
		WaitForSingleObject(hThread,INFINITE);
		DWORD ec;  
		GetExitCodeThread(hThread,&ec); 
		if (ec == STILL_ACTIVE)
		{
			::TerminateThread(hThread,1);
		}

		CloseHandle(hThread);

		bRet = InjectResult::OK;
	}


RELEASE_DATA:
	if(pParamData)
	{
		::VirtualFreeEx(hProcess,pParamData,0,MEM_RELEASE);
	}

	if (pCodeData)
	{
		::VirtualFreeEx(hProcess,pCodeData,0,MEM_RELEASE);
	}

	return bRet;
}

static InjectResult InjectLibraryByRemoteThread(HANDLE hProcess, const wchar_t*Name, unsigned short NameLength)
{
	InjectResult bRet = Error_Normal;

	if(hProcess == NULL || hProcess == INVALID_HANDLE_VALUE) return bRet;

	PVOID pParamData = NULL,pCodeData = NULL;

	pParamData = ::VirtualAllocEx(hProcess,0,4096,MEM_COMMIT | MEM_RESERVE,PAGE_READWRITE);
	if(!pParamData) goto RELEASE_DATA;

	DWORD dwCodeSize = DWORD(NTCreateThreadExProcEnd) - DWORD(NTCreateThreadExProc);
	pCodeData = ::VirtualAllocEx(hProcess, NULL, dwCodeSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if(!pCodeData) goto RELEASE_DATA;

	//先写入参数内存
	NTCREATE_THREAD_EX_DATA data;
	HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
	data.fnRtlInitUnicodeString = GetProcAddress(hNtdll,"RtlInitUnicodeString");
	data.fnLdrLoadDll = GetProcAddress(hNtdll,"LdrLoadDll");
	memcpy(data.DllName, Name, NameLength+sizeof(wchar_t));
	memset(&data.UnicodeString,0,sizeof(data.UnicodeString)); 
	data.DllPath = NULL;
	data.Flags = 0;
	data.ModuleHandle = INVALID_HANDLE_VALUE;

	if(!::WriteProcessMemory(hProcess,pParamData,&data,sizeof(data),NULL)) goto RELEASE_DATA;
	//写入代码
	if(!::WriteProcessMemory(hProcess,pCodeData,(PVOID)NTCreateThreadExProc,dwCodeSize,NULL)) goto RELEASE_DATA;

	//创建远程线程注入
	HANDLE hThread = jkCreateRemoteThread(hProcess,(LPTHREAD_START_ROUTINE)pCodeData,pParamData);

	if (hThread)
	{
		WaitForSingleObject(hThread,INFINITE);
		DWORD ec;  
		GetExitCodeThread(hThread,&ec); 
		if (ec == STILL_ACTIVE)
		{
			::TerminateThread(hThread,1);
		}

		CloseHandle(hThread);

		bRet = InjectResult::OK;
	}
	

RELEASE_DATA:
	if(pParamData)
	{
		::VirtualFreeEx(hProcess,pParamData,0,MEM_RELEASE);
	}

	if (pCodeData)
	{
		::VirtualFreeEx(hProcess,pCodeData,0,MEM_RELEASE);
	}

	return bRet;
}

static InjectResult InjectLibraryByAPC(DWORD dwPid, const wchar_t*Name, unsigned short NameLength)
{
	InjectResult bRet = Error_Normal;

	if(dwPid == 0 ) return bRet;

	HANDLE hProcess = ::OpenProcess(2035711,FALSE,dwPid);

	if( hProcess == NULL || hProcess == INVALID_HANDLE_VALUE) return bRet;

	PVOID buf = ::VirtualAllocEx(hProcess,0,NameLength+2,MEM_COMMIT,PAGE_EXECUTE_READWRITE);

	if (buf)
	{
		SIZE_T byteWritten = 0;
		::WriteProcessMemory(hProcess,buf,Name,NameLength,&byteWritten);

		//APC注入
		HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD,0);
		if (hSnap != INVALID_HANDLE_VALUE)
		{
			THREADENTRY32 entry={sizeof(entry)};
			BOOL state = Thread32First(hSnap,&entry);
			while(state)
			{
				if (entry.th32OwnerProcessID == dwPid)
				{
					HANDLE hThread = OpenThread(THREAD_ALL_ACCESS,FALSE,entry.th32ThreadID);
					if (hThread)
					{
						QueueUserAPC((PAPCFUNC)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryW"), hThread, (DWORD)buf ); 

						CloseHandle(hThread);
						bRet = InjectResult::OK;
						//break;
					}
				}

				state = Thread32Next(hSnap,&entry);
			}
			CloseHandle(hSnap);
		}

		::VirtualFreeEx(hProcess,buf,0,MEM_RELEASE);
	}

	CloseHandle(hProcess);

	return bRet;
}

//-------------------------------------------------------------------------------
// 注入主函数(导出函数)
//-------------------------------------------------------------------------------
InjectResult jkInjectDllA(const char* szPath,const char* szInjectExe)
{
#ifdef _WIN64 
	return Error_NotSuport64Process;
#endif

	USES_CONVERSION;

	if(szPath==NULL || szInjectExe==NULL) return Error_Normal;

	wchar_t wszPath[MAX_PATH]={0},wszInjectExe[MAX_PATH]={0};

	memcpy(wszPath,A2W(szPath),strnlen(szPath,MAX_PATH)*sizeof(wchar_t));
	memcpy(wszInjectExe,A2W(szInjectExe),strnlen(szInjectExe,MAX_PATH)*sizeof(wchar_t));

	return jkInjectDllW(wszPath,wszInjectExe);
}

InjectResult jkInjectDllW(const wchar_t* szPath,const wchar_t* szInjectExe)
{
#ifdef _WIN64 
	return Error_NotSuport64Process;
#endif

	EnableDebugPrivilege();

	if(gInitialized==FALSE) gInitialized = InitWow64Ext();
	

	USES_CONVERSION;

	InjectResult bRet = Error_Normal;

	if(szPath==NULL || szInjectExe==NULL) return Error_Normal;

	DWORD dwPid = GetProcessIdByName(W2T((LPWSTR)szInjectExe));
	if (dwPid != 0)
	{
		HANDLE hProcess = ::OpenProcess(2035711,FALSE,dwPid);
		if (hProcess == NULL) return bRet;

		//判断进程是32位还是64位
		if( IsWow64ProcessEx(hProcess) )
		{
			bRet = Wow64InjectWin64(hProcess,szPath,wcsnlen(szPath,MAX_PATH)*sizeof(wchar_t));
		}
		else
		{
			bRet = InjectLibraryByRemoteThread(hProcess,szPath,wcsnlen(szPath,MAX_PATH)*sizeof(wchar_t));
/*
			bRet = InjectLibrary32(hProcess,szPath,wcsnlen(szPath,MAX_PATH)*sizeof(wchar_t));
			const wchar_t* pNameDll = wcsrchr(szPath,'\\')+1;
			if (!hasDll(dwPid,W2T((LPWSTR)pNameDll)) )
			{	
				bRet = InjectLibraryByRemoteThread(hProcess,szPath,wcsnlen(szPath,MAX_PATH)*sizeof(wchar_t));
				if (!hasDll(dwPid,W2T((LPWSTR)pNameDll)) ) 
				{
					bRet = InjectLibraryByAPC(dwPid,szPath,wcsnlen(szPath,MAX_PATH)*sizeof(wchar_t) );
					if(hasDll(dwPid,W2T((LPWSTR)pNameDll)) ) printf("InjectLibraryByAPC ok\n");
				}
				else printf("InjectLibraryByRemoteThread ok\n");
			}
			else printf("InjectLibrary32 ok\n");*/

		}

		CloseHandle(hProcess);
		return bRet;
	}

	return bRet;
}

//-------------------------------------------------------------------------------