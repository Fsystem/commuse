#include <tchar.h>
#include <stdio.h>
#include <Windows.h>
#include <psapi.h>

#include <TlHelp32.h>

#include <atlconv.h>

#include "jkInjectDll.h"

#pragma comment(lib,"Psapi.lib")



//-------------------------------------------------------------------------------
//函数申明
BOOL AddToken();

DWORD GetProcessIdByName(LPCTSTR procname);
//系统是否是64位0S
BOOL IsWindows64();
//是否是64位进程
BOOL IsWow64ProcessEx(HANDLE hProcess);
BOOL IsWow64ProcessEx(DWORD dwPid);
//向64位程序注入
HMODULE InjectLibrary64(HANDLE ProcessHandle, const wchar_t*Name,unsigned short NameLength);
//向32位程序注入
BOOL InjectLibrary32(HANDLE hProcess, const wchar_t*Name, unsigned short NameLength);

BOOL InjectLibraryByAPC(DWORD dwPid, const wchar_t*Name, unsigned short NameLength);

static BOOL InjectLibraryByRemoteThread(HANDLE hProcess, const wchar_t*Name, unsigned short NameLength);

//是否已经注入
BOOL hasDll(DWORD processId,LPCTSTR szDllName);

HANDLE jkCreateRemoteThread(HANDLE hProcess, LPTHREAD_START_ROUTINE pThreadProc, LPVOID pRemoteBuf) ;

//-------------------------------------------------------------------------------

//导出函数
BOOL jkInjectDllA(const char* szPath,const char* szInjectExe)
{
#ifdef _WIN64 
	return FALSE;
#endif

	USES_CONVERSION;

	if(szPath==NULL || szInjectExe==NULL) return FALSE;

	wchar_t wszPath[MAX_PATH]={0},wszInjectExe[MAX_PATH]={0};

	memcpy(wszPath,A2W(szPath),strnlen(szPath,MAX_PATH)*sizeof(wchar_t));
	memcpy(wszInjectExe,A2W(szInjectExe),strnlen(szInjectExe,MAX_PATH)*sizeof(wchar_t));

	return jkInjectDllW(wszPath,wszInjectExe);
}

BOOL jkInjectDllW(const wchar_t* szPath,const wchar_t* szInjectExe)
{
#ifdef _WIN64 
	return FALSE;
#endif

	AddToken();

	USES_CONVERSION;

	BOOL bRet = FALSE;

	if(szPath==NULL || szInjectExe==NULL) return FALSE;

	DWORD dwPid = GetProcessIdByName(W2T((LPWSTR)szInjectExe));
	if (dwPid != 0)
	{
		HANDLE hProcess = ::OpenProcess(2035711,FALSE,dwPid);
		if (hProcess == NULL) return bRet;

		//判断进程是32位还是64位
		if( IsWow64ProcessEx(hProcess) )
		{
			bRet = (InjectLibrary64(hProcess,szPath,wcsnlen(szPath,MAX_PATH)*sizeof(wchar_t))==0);
		}
		else
		{
			//bRet = InjectLibrary32(hProcess,szPath,wcsnlen(szPath,MAX_PATH)*sizeof(wchar_t));
			const wchar_t* pNameDll = wcsrchr(szPath,'\\')+1;
			//if (!hasDll(dwPid,W2T((LPWSTR)pNameDll)) )
			{	
				bRet = InjectLibraryByRemoteThread(hProcess,szPath,wcsnlen(szPath,MAX_PATH)*sizeof(wchar_t));
				if (!hasDll(dwPid,W2T((LPWSTR)pNameDll)) ) 
				{
					bRet = InjectLibraryByAPC(dwPid,szPath,wcsnlen(szPath,MAX_PATH)*sizeof(wchar_t) );
					if(hasDll(dwPid,W2T((LPWSTR)pNameDll)) ) printf("InjectLibraryByAPC ok\n");
				}
				else printf("InjectLibraryByRemoteThread ok\n");
			}
			//else printf("InjectLibrary32 ok\n");
			
		}

		CloseHandle(hProcess);
		return bRet;
	}

	return bRet;
}

//-------------------------------------------------------------------------------
//64位进程注入
#define MakeREX(W,R,X,B) __asm __emit 0100##W##R##X##B##b  
#define rep __asm __emit 0xf3  

#pragma pack(push,1)  
typedef struct _STUBARGS  
{  
	unsigned __int64 addr;  
	unsigned int argc;  
	__int64 argv[1];  
}STUBARGS,*PSTUBARGS;  
#pragma pack(pop)  

__declspec(naked) static  void __stdcall stubto64(void)  
{  
	__asm   
	{  
		push esi  
			push edi  
			push ebp  
			lea ebp,[esp+44]  


		mov eax,[ebp+STUBARGS.argc]  
		neg eax  
			lea esp,[esp+eax*8]  
		neg eax  
			and esp,not 15  


			lea esi,[ebp+STUBARGS.argv]  
		mov edi,esp  
			mov ecx,eax  
			cld  


			rep  
			MakeREX(1,0,0,0)  
			movsd  


			dec eax  
			jl callproc  
			MakeREX(1,0,0,0)  
			mov ecx,dword ptr[ebp+STUBARGS.argv+8*0]  
		dec eax  
			jl callproc  
			MakeREX(1,0,0,0)  
			mov edx,dword ptr[ebp+STUBARGS.argv+8*1]  
		dec eax  
			jl callproc  
			MakeREX(1,1,0,0)  
			mov eax,dword ptr[ebp+STUBARGS.argv+8*2]  
		dec eax  
			jl callproc  
			MakeREX(1,1,0,0)  
			mov ecx,dword ptr[ebp+STUBARGS.argv+8*3]  



callproc:  
		MakeREX(1,0,0,0)  
			call dword ptr[ebp+STUBARGS.addr]  

		MakeREX(1,0,0,0)  
			mov edx,eax  
			MakeREX(1,0,0,0)  
			shr edx,32  


			lea esp,[ebp-44]  
		pop ebp  
			pop edi  
			pop esi  
			retf  
	}  
}  


__declspec(naked) static  __int64 __cdecl lcall(void*off,unsigned short seg,...)//__int64 func,unsigned argc,...)  
{  
	__asm call Far ptr[esp+4]  
	__asm ret  
}  


#define Call64Proc(addr,argc,...) lcall(stubto64,0x33,(unsigned __int64)addr,argc,__VA_ARGS__)  
#define To64(x) ((__int64)(x)) 

static HMODULE GetNativeNtdll(void)  
{  
	MEMORY_BASIC_INFORMATION mbi;static HMODULE ntdll=0;char*lastone=0;  
	if(ntdll)return ntdll;  
	for(char*p=0;size_t(p)<0x80000000;p+=mbi.RegionSize)  
	{  
		if(!VirtualQueryEx((HANDLE)-1,p,&mbi,sizeof mbi))break;  
		if(mbi.AllocationBase==lastone||mbi.State!=MEM_COMMIT||mbi.Type!=MEM_IMAGE)continue;  
		wchar_t name[256];  
		const wchar_t ntdllname[]=L"\\windows\\system32\\ntdll.dll";  
		const unsigned int ntdllnamelen=sizeof ntdllname/sizeof(wchar_t)-1;  
		DWORD namelen;  
		if((namelen=GetMappedFileNameW((HANDLE)-1,mbi.AllocationBase,name,256))>ntdllnamelen)  
		{  

			if(wcsicmp(ntdllname,name+namelen-ntdllnamelen)==0)  
			{  
				PIMAGE_DOS_HEADER dosheader=PIMAGE_DOS_HEADER(mbi.AllocationBase);  
				PIMAGE_NT_HEADERS64 ntheaders=PIMAGE_NT_HEADERS64(size_t(dosheader)+dosheader->e_lfanew);  
				if(ntheaders->FileHeader.Machine==IMAGE_FILE_MACHINE_AMD64)return (HMODULE)mbi.AllocationBase;  
			}  
		}  
		lastone=(char*)mbi.AllocationBase;  
	}  
	return ntdll;  
}  


static void* GetProcAddress64(const void*BaseAddress,const char*Name)  
{  
	UINT_PTR base=reinterpret_cast<UINT_PTR>(BaseAddress);  
	PIMAGE_DOS_HEADER DosHeader=PIMAGE_DOS_HEADER(BaseAddress);  
	PIMAGE_NT_HEADERS64 NtHeaders=PIMAGE_NT_HEADERS64(size_t(DosHeader)+DosHeader->e_lfanew);;  
	DWORD ExpRVA=NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;  
	DWORD ExpSize=NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;  
	if(ExpRVA&&ExpSize)  
	{  
		PIMAGE_EXPORT_DIRECTORY ExpDir=reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(base+ExpRVA);  
		PDWORD NameList=reinterpret_cast<PDWORD>(base+ExpDir->AddressOfNames);  
		PWORD OrdinalList=reinterpret_cast<PWORD>(base+ExpDir->AddressOfNameOrdinals);  
		PDWORD FunctionList=reinterpret_cast<PDWORD>(base+ExpDir->AddressOfFunctions);  
		for(DWORD i=0;i<ExpDir->NumberOfNames;++i)  
		{  
			const char*ExpName=reinterpret_cast<const char*>(base+NameList[i]);  
			if(strcmp(Name,ExpName)==0)  
			{  
				WORD Ordinal=OrdinalList[i];  
				return (char*)base+FunctionList[Ordinal];  
			}  
		}  
	}  
	return 0;  
}  


static HANDLE CreateThread64(HANDLE ProcessHandle,void*ThreadProc,void*Parameter,int Suspended)  
{  
	__int64 hThread=0,cid[2];  
	static void*RtlCreateUserThread64=0;  
	if(!RtlCreateUserThread64)  
		RtlCreateUserThread64=GetProcAddress64(GetNativeNtdll(),"RtlCreateUserThread");  
	Call64Proc(RtlCreateUserThread64,10,To64(ProcessHandle),To64(0),To64(Suspended),To64(0),To64(0),To64(0),To64(ThreadProc),To64(Parameter),To64(&hThread),To64(cid));  
	return (HANDLE)hThread;  
}  


typedef struct _UNICODE_STRING64  
{  
	USHORT Length;  
	USHORT MaximumLength;  
	ULONGLONG  Buffer;  
} UNICODE_STRING64, *PUNICODE_STRING64;  

static HMODULE InjectLibrary64(HANDLE ProcessHandle,const wchar_t*Name,unsigned short NameLength)  
{  
	void*stub,*buf;unsigned stublen;  
	__asm  
	{  
		mov stub,offset stubstart  
		mov stublen,offset stubend  
		sub stublen,offset stubstart  
		jmp stubend  ;
stubstart:  
		MakeREX(1,1,0,0)  ;
		lea eax,[ecx+8]  
		MakeREX(1,1,0,0)  
		lea ecx,[esp-8]  
		MakeREX(1,0,0,0)  
		sub esp,48  
		xor ecx,ecx  
		xor edx,edx  

		MakeREX(1,0,0,1)  
		call [eax-8]  
		MakeREX(1,0,0,0)  
		add esp,48  
		MakeREX(1,0,0,0)  
		mov eax,[esp-8]  
		MakeREX(1,0,0,0)  
		mov edx,eax  
		MakeREX(1,0,0,0)  
		shr edx,32  
		ret  
stubend:  
	}  
	static struct   
	{  
		__int64 LdrLoadDll;  
		UNICODE_STRING64 us;  
	}pre={0};  
	buf=VirtualAllocEx(ProcessHandle,0,NameLength+sizeof pre+stublen,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE);  
	if(buf)  
	{  
		if(!pre.LdrLoadDll)pre.LdrLoadDll=(__int64)GetProcAddress64(GetNativeNtdll(),"LdrLoadDll");  
		pre.us.Length=pre.us.MaximumLength=NameLength;  
		pre.us.Buffer=(ULONGLONG)buf+sizeof pre;  
		WriteProcessMemory(ProcessHandle,buf,&pre,sizeof pre,0);  
		WriteProcessMemory(ProcessHandle,(void*)pre.us.Buffer,Name,NameLength,0);  
		void*code=(char*)buf+sizeof pre+NameLength;  
		WriteProcessMemory(ProcessHandle,code,stub,stublen,0);  
		HANDLE hThread=CreateThread64(ProcessHandle,code,buf,0);  
		if(hThread)  
		{  
			if(WaitForSingleObject(hThread,-1))  
			{  
				DWORD ec;  
				GetExitCodeThread(hThread,&ec);  
				CloseHandle(hThread);  
				return (HMODULE)ec;  
			}  
			CloseHandle(hThread);  
		}  
		VirtualFreeEx(ProcessHandle,buf,0,MEM_RELEASE);  
	}  
	return 0;  
}  

//-------------------------------------------------------------------------------
// 32位注入(NtCreateThreadEx + LdrLoadDll)
//-------------------------------------------------------------------------------
typedef struct _LSA_UNICODE_STRING{
	USHORT Length;
	USHORT MaximumLength;
	PWSTR Buffer;
} LSA_UNICODE_STRING, *PLSA_UNICODE_STRING, UNICODE_STRING, *PUNICODE_STRING;

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

	#define STATUS_SUCCESS                      ((NTSTATUS) 0x00000000) 
	#define STATUS_INFO_LENGTH_MISMATCH			((NTSTATUS) 0xC0000004) 
	#define STATUS_IO_DEVICE_ERROR              ((NTSTATUS) 0xC0000185) 

	typedef struct _CLIENT_ID { 
		HANDLE UniqueProcess; 
		HANDLE UniqueThread; 
	} CLIENT_ID; 
	typedef CLIENT_ID *PCLIENT_ID; 

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


	PRtlCreateThread fnRtlCreateThread = (PRtlCreateThread)GetProcAddress(GetModuleHandleA("Kernel32.dll"),"RtlCreateUserThread");

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
static BOOL InjectLibrary32(HANDLE hProcess, const wchar_t*Name, unsigned short NameLength)
{
	BOOL bRet = FALSE;

	if(hProcess == NULL || hProcess == INVALID_HANDLE_VALUE) return bRet;

	PVOID buf = ::VirtualAllocEx(hProcess,0,NameLength+200,MEM_COMMIT,PAGE_EXECUTE_READWRITE);

	if (buf)
	{
		SIZE_T byteWritten = 0;
		::WriteProcessMemory(hProcess,buf,Name,NameLength,&byteWritten);

		HMODULE hM = GetModuleHandleA("kernel32.dll");
		HANDLE hThread = CreateThread32(hProcess,GetProcAddress(hM,"LoadLibraryW"),buf,0);
		if (hThread)
		{
			WaitForSingleObject(hThread,INFINITE); 
			::TerminateThread(hThread,1);
			CloseHandle(hThread);

			bRet = TRUE;
		}

		::VirtualFreeEx(hProcess,buf,0,MEM_RELEASE);
	}

	return bRet;
}

//将函数地址转换为真实地址
static DWORD getFunRealAddr(LPVOID fun)
{
	DWORD realaddr=(DWORD)fun;//虚拟函数地址
	// 计算函数真实地址
	unsigned char* funaddr= (unsigned char*)fun;
	if(funaddr[0]==0xE9)// 判断是否为虚拟函数地址，E9为jmp指令
	{
		DWORD disp=*(DWORD*)(funaddr+1);//获取跳转指令的偏移量
		realaddr+=5+disp;//修正为真实函数地址
	}
	return realaddr;
}

static BOOL InjectLibraryByRemoteThread(HANDLE hProcess, const wchar_t*Name, unsigned short NameLength)
{
	BOOL bRet = FALSE;

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

		bRet = TRUE;
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

static BOOL InjectLibraryByAPC(DWORD dwPid, const wchar_t*Name, unsigned short NameLength)
{
	BOOL bRet = FALSE;

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
					HANDLE hThread = OpenThread(THREAD_TERMINATE,FALSE,entry.th32ThreadID);
					if (hThread)
					{
						QueueUserAPC((PAPCFUNC)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryW"), hThread, (DWORD)buf ); 

						CloseHandle(hThread);
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
//函数库
//-------------------------------------------------------------------------------
static DWORD GetProcessIdByName(LPCTSTR procname)
{
	if(procname == NULL) return 0;

	HANDLE SnapShot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(SnapShot==NULL)
	{ 
		return 0;
	}

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
	if (NULL != fnGetNativeSystemInfo)
	{
		fnGetNativeSystemInfo(&si);
	}
	else
	{
		GetSystemInfo(&si);
	}

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
static BOOL IsWow64ProcessEx(HANDLE hProcess)  
{  
	if (!IsWindows64()) return FALSE;

	typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);   
	LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(TEXT("kernel32")), "IsWow64Process");  
  
	//如果没有导出则判定为32位 
	if(!fnIsWow64Process) return FALSE; 

	BOOL bX64;  
	if(!fnIsWow64Process(hProcess,&bX64)) return FALSE;  

	return !bX64;  
}  

static BOOL IsWow64ProcessEx(DWORD dwPid)  
{  
	HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_QUERY_LIMITED_INFORMATION ,FALSE,dwPid);
	if (hProcess)
	{
		return IsWow64ProcessEx(hProcess);
		CloseHandle(hProcess);
	}
	return FALSE;
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
static BOOL AddToken()
{
	///提升权限
	TOKEN_PRIVILEGES tkp;
	HANDLE hToken;
	if (!OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
	{
		//	MessageBox("OpenProcessToken failed!");
		DWORD err = GetLastError();
		return false;
	}
	if(! LookupPrivilegeValue(NULL, SE_DEBUG_NAME,&tkp.Privileges[0].Luid))//获得本地机唯一的标识
	{ 
		//	AfxMessageBox("从本地计算机提取权限失败！");
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
		//	MessageBox("AdjustTokenPrivileges enable failed!");
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
	/*	if( WAIT_FAILED == WaitForSingleObject(hThread, INFINITE) )  
	{  
	printf("MyCreateRemoteThread() : WaitForSingleObject() 调用失败！错误代码: [%d]/n", GetLastError());  
	return FALSE;  
	} */ 
	return hThread;  
} 

