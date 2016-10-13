#pragma once

#include <string>
#include <map>
#include <windows.h>

#ifdef _UNICODE

#define mystring	std::wstring	

#else

#define mystring	std::string		

#endif


#define DFP_GET_VERSION 0x00074080
#define DFP_RECEIVE_DRIVE_DATA 0x0007c088
// values for IDEREGS.bCommandReg
#define IDE_ATAPI_IDENTIFY 0xA1
#define IDE_ATA_IDENTIFY 0xEC
#define IDENTIFY_BUFFER_SIZE 512

//save disk information

typedef struct _GETVERSIONOUTPARAMS
{
	BYTE bVersion;
	BYTE bRevision;
	BYTE bReserved;
	BYTE bIDEDeviceMap;
	DWORD fCapabilities;
	DWORD dwReserved[4];
} GETVERSIONOUTPARAMS, *PGETVERSIONOUTPARAMS, *LPGETVERSIONOUTPARAMS;
/*
// IDE register information
typedef struct _IDEREGS
{
	BYTE bFeaturesReg;
	BYTE bSectorCountReg;
	BYTE bSectorNumberReg;
	BYTE bCylLowReg;
	BYTE bCylHighReg;
	BYTE bDriveHeadReg;
	BYTE bCommandReg;
	BYTE bReserved;
} IDEREGS, *PIDEREGS, *LPIDEREGS;
// disk command input parameter
typedef struct _SENDCMDINPARAMS
{
	DWORD cBufferSize;
	IDEREGS irDriveRegs;
	BYTE bDriveNumber;
	BYTE bReserved[3];
	DWORD dwReserved[4];
	BYTE bBuffer[1];
} SENDCMDINPARAMS, *PSENDCMDINPARAMS, *LPSENDCMDINPARAMS;
// disk status
typedef struct _DRIVERSTATUS
{
	BYTE bDriverError;
	BYTE bIDEStatus;
	BYTE bReserved[2];
	DWORD dwReserved[2];
} DRIVERSTATUS, *PDRIVERSTATUS, *LPDRIVERSTATUS;
// sent param for disk commands
typedef struct _SENDCMDOUTPARAMS
{
	DWORD cBufferSize;
	DRIVERSTATUS DriverStatus;
	BYTE bBuffer[1];
} SENDCMDOUTPARAMS, *PSENDCMDOUTPARAMS, *LPSENDCMDOUTPARAMS;*/


typedef	std::map<std::string,DWORD>				PROCESS_NAME_COUNT_MAP;

class CDeviceOper
{
public:
	CDeviceOper(void);
	~CDeviceOper(void);

public:

	DWORD		GetProcessName(PROCESS_NAME_COUNT_MAP &process_map);

	mystring	GetDiskSnId();
	mystring    GetMac(std::string &ip_str);

	mystring	GetIeVer();
	mystring	GetFlashVer();

	mystring	GetSystemVer();
};
