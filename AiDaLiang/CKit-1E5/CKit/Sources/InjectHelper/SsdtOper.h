#ifndef __SsdtOper_H
#define __SsdtOper_H
#include "SystemCommon.h"
class CSsdtOper
{
public:
	CSsdtOper(void);
	~CSsdtOper(void);

	RTLNTSTATUSTODOSERROR		RtlNtStatusToDosError;
	ZWQUERYSYSTEMINFORMATION	ZwQuerySystemInformation;
	RTLINITUNICODESTRING		RtlInitUnicodeString;
	ZWOPENSECTION				ZwOpenSection;
	NTOPENSECTION				NtOpenSection;
	ZWSYSTEMDEBUGCONTROL		ZwSystemDebugControl;

protected:
	HMODULE		m_hNtDLL; 
	BOOL		m_bIsSupport;

	HANDLE		m_hMutex;
	SYSTEM_HANDLE_INFORMATION	m_tSystemHandleInformation;
	DWORD m_dwAddressPosForward, m_dwAddressPosBackward;
	DWORD m_dwDataForward, m_dwDataBackward;
	// 	DWORD m_dwDataForwardOld, m_dwDataBackwardOld;

	DWORD    m_dwKiServiceTable;    // rva of KiServiceTable
	DWORD    m_dwKernelBase;
	typedef struct _SSDT_LIST_ENTRY
	{
		CHAR fname[64];
		ULONG Org;
		ULONG Now;
	} SSDT_LIST_ENTRY, *PSSDT_LIST_ENTRY;
	PSSDT_LIST_ENTRY m_pListSSDT;
	DWORD m_dwServices;

	PDWORD m_pdwTableOrg;
	PDWORD m_pdwTableNow;

	PVOID m_pMapPhysicalMemory;
	HANDLE m_hMPM;

	typedef struct _PROCESS_INFO
	{
		DWORD	dwID;
		DWORD	dwParentID;
		TCHAR	szImagePath[MAX_PATH + 1];
		TCHAR	*pszExeFile;
		TCHAR	szCmdLine[MAX_PATH + 1];
		TCHAR	szOwner[MAX_PATH + 1];
		TCHAR	*pszOwnerName;
	}PROCESS_INFO, *PPROCESS_INFO;
	PPROCESS_INFO m_pProcessInfo;

	typedef struct
	{
		DWORD Filler[4];
		DWORD InfoBlockAddress;
	} __PEB;

	typedef struct 
	{
		DWORD Filler[17];
		DWORD wszCmdLineAddress;
	} __INFOBLOCK;

public:
	BOOL GetSSDT();
	BOOL ResumeSSDT();
	BOOL SetData(DWORD dwAddress, PVOID pData, DWORD dwLength);
	BOOL CleanSSDT();
	BOOL EnablePrivilege(LPCTSTR pPrivName, BOOL bEnable);
	BOOL Hide(DWORD dwProcessID);

private:
	void CloseNTDLL();
	BOOL GetEprocessFromPid(DWORD dwProcessId);
	BOOL InitNTDLL();
	DWORD FindKiServiceTable(HMODULE hModule, DWORD dwKSDT);
	DWORD GetHeaders(PCHAR ibase, PIMAGE_FILE_HEADER *pfh, PIMAGE_OPTIONAL_HEADER *poh, PIMAGE_SECTION_HEADER *psh);
	void  FindExport();
	BOOL  GetData(DWORD dwAddress, PVOID pData, DWORD dwLength);
	BOOL  OperateData(SYSDBG_COMMAND eCmd, DWORD dwAddress, PVOID pData, DWORD dwLength);
};

#endif //__SsdtOper_H