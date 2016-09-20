#ifndef __SystemCommon_H
#define __SystemCommon_H
#include <windows.h>

#define NT_SUCCESS(Status)			((NTSTATUS)(Status) >= 0)
#define STATUS_INFO_LENGTH_MISMATCH ((NTSTATUS)0xC0000004L)
#define STATUS_ACCESS_DENIED        ((NTSTATUS)0xC0000022L)

typedef	long	LONG;
typedef	unsigned long ULONG;
//#if _MSC_VER < 1500
typedef ULONG	NTSTATUSEX;
//#endif
typedef ULONG	ACCESS_MASK;

typedef struct _IO_STATUS_BLOCK
{
	NTSTATUSEX	Status;
	ULONG		Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

typedef struct _UNICODE_STRING
{
	USHORT    Length;
	USHORT    MaximumLength;
#ifdef MIDL_PASS
	[size_is(MaximumLength / 2), length_is((Length) / 2) ] USHORT * Buffer;
#else // MIDL_PASS
	PWSTR Buffer;
#endif // MIDL_PASS
} UNICODE_STRING, *PUNICODE_STRING;

#define OBJ_INHERIT       0x00000002L
#define OBJ_PERMANENT      0x00000010L
#define OBJ_EXCLUSIVE      0x00000020L
#define OBJ_CASE_INSENSITIVE  0x00000040L
#define OBJ_OPENIF       0x00000080L
#define OBJ_OPENLINK      0x00000100L
#define OBJ_KERNEL_HANDLE    0x00000200L
#define OBJ_VALID_ATTRIBUTES  0x000003F2L

#define RVATOVA(base,offset) ((PVOID)((DWORD)(base)+(DWORD)(offset)))

typedef struct _IMAGE_FIXUP_ENTRY
{
	WORD	offset:12;
	WORD	type:4;
} IMAGE_FIXUP_ENTRY, *PIMAGE_FIXUP_ENTRY;

typedef enum _SYSTEM_INFORMATION_CLASS
{
	SystemBasicInformation,                 //  0 Y N
	SystemProcessorInformation,             //  1 Y N
	SystemPerformanceInformation,           //  2 Y N
	SystemTimeOfDayInformation,             //  3 Y N
	SystemNotImplemented1,                  //  4 Y N
	SystemProcessesAndThreadsInformation,   //  5 Y N
	SystemCallCounts,                       //  6 Y N
	SystemConfigurationInformation,         //  7 Y N
	SystemProcessorTimes,                   //  8 Y N
	SystemGlobalFlag,                       //  9 Y Y
	SystemNotImplemented2,                  // 10 Y N
	SystemModuleInformation,                // 11 Y N
	SystemLockInformation,                  // 12 Y N
	SystemNotImplemented3,                  // 13 Y N
	SystemNotImplemented4,                  // 14 Y N
	SystemNotImplemented5,                  // 15 Y N
	SystemHandleInformation,                // 16 Y N
	SystemObjectInformation,                // 17 Y N
	SystemPagefileInformation,              // 18 Y N
	SystemInstructionEmulationCounts,       // 19 Y N
	SystemInvalidInfoClass1,                // 20
	SystemCacheInformation,                 // 21 Y Y
	SystemPoolTagInformation,               // 22 Y N
	SystemProcessorStatistics,              // 23 Y N
	SystemDpcInformation,                   // 24 Y Y
	SystemNotImplemented6,                  // 25 Y N
	SystemLoadImage,                        // 26 N Y
	SystemUnloadImage,                      // 27 N Y
	SystemTimeAdjustment,                   // 28 Y Y
	SystemNotImplemented7,                  // 29 Y N
	SystemNotImplemented8,                  // 30 Y N
	SystemNotImplemented9,                  // 31 Y N
	SystemCrashDumpInformation,             // 32 Y N
	SystemExceptionInformation,             // 33 Y N
	SystemCrashDumpStateInformation,        // 34 Y Y/N
	SystemKernelDebuggerInformation,        // 35 Y N
	SystemContextSwitchInformation,         // 36 Y N
	SystemRegistryQuotaInformation,         // 37 Y Y
	SystemLoadAndCallImage,                 // 38 N Y
	SystemPrioritySeparation,               // 39 N Y
	SystemNotImplemented10,                 // 40 Y N
	SystemNotImplemented11,                 // 41 Y N
	SystemInvalidInfoClass2,                // 42
	SystemInvalidInfoClass3,                // 43
	SystemTimeZoneInformation,              // 44 Y N
	SystemLookasideInformation,             // 45 Y N
	SystemSetTimeSlipEvent,                 // 46 N Y
	SystemCreateSession,                    // 47 N Y
	SystemDeleteSession,                    // 48 N Y
	SystemInvalidInfoClass4,                // 49
	SystemRangeStartInformation,            // 50 Y N
	SystemVerifierInformation,              // 51 Y Y
	SystemAddVerifier,                      // 52 N Y
	SystemSessionProcessesInformation       // 53 Y N
} SYSTEM_INFORMATION_CLASS, *PSYSTEM_INFORMATION_CLASS;

typedef struct _SYSTEM_MODULE_INFORMATION
{// Information Class 11
	ULONG Reserved[2];
	PVOID Base;
	ULONG Size;
	ULONG Flags;
	USHORT Index;
	USHORT Unknown;
	USHORT LoadCount;
	USHORT ModuleNameOffset;
	CHAR ImageName[256];
} SYSTEM_MODULE_INFORMATION, *PSYSTEM_MODULE_INFORMATION; 

typedef struct _SYSTEM_HANDLE_INFORMATION
{// Information Class 16
	ULONG ProcessId;
	UCHAR ObjectTypeNumber;
	UCHAR Flags;
	USHORT Handle;
	PVOID Object;
	ACCESS_MASK GrantedAccess;
} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;

typedef struct _OBJECT_ATTRIBUTES
{
	ULONG Length;
	HANDLE RootDirectory;
	PUNICODE_STRING ObjectName;
	ULONG Attributes;
	PVOID SecurityDescriptor;
	PVOID SecurityQualityOfService;
} OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;
#define InitializeObjectAttributes( p, n, a, r, s ) { (p)->Length = sizeof( OBJECT_ATTRIBUTES ); (p)->RootDirectory = r; (p)->Attributes = a; (p)->ObjectName = n; (p)->SecurityDescriptor = s; (p)->SecurityQualityOfService = NULL; }

typedef enum _SYSDBG_COMMAND 
{ 
	//以下5个在Windows NT各个版本上都有 
	SysDbgGetTraceInformation = 1, 
	SysDbgSetInternalBreakpoint = 2, 
	SysDbgSetSpecialCall = 3, 
	SysDbgClearSpecialCalls = 4, 
	SysDbgQuerySpecialCalls = 5, 
	// 以下是NT 5.1 新增的 
	SysDbgDbgBreakPointWithStatus = 6, 
	//获取KdVersionBlock 
	SysDbgSysGetVersion = 7, 
	//从内核空间拷贝到用户空间，或者从用户空间拷贝到用户空间 
	//但是不能从用户空间拷贝到内核空间 
	SysDbgCopyMemoryChunks_0 = 8, 
	SysDbgReadVirtualMemory = 8, 

	//从用户空间拷贝到内核空间，或者从用户空间拷贝到用户空间 
	//但是不能从内核空间拷贝到用户空间 
	SysDbgCopyMemoryChunks_1 = 9, 
	SysDbgWriteVirtualMemory = 9, 

	//从物理地址拷贝到用户空间，不能写到内核空间 
	SysDbgCopyMemoryChunks_2 = 10, 
	SysDbgReadPhysicalMemory = 10, 

	//从用户空间拷贝到物理地址，不能读取内核空间 
	SysDbgCopyMemoryChunks_3 = 11, 
	SysDbgWritePhysicalMemory = 11, 
	//读写处理器相关控制块 
	SysDbgSysReadControlSpace = 12, 
	SysDbgSysWriteControlSpace = 13, 
	//读写端口 
	SysDbgSysReadIoSpace = 14, 
	SysDbgSysWriteIoSpace = 15, 
	//分别调用RDMSR@4和_WRMSR@12 
	SysDbgSysReadMsr = 16, 
	SysDbgSysWriteMsr = 17, 
	//读写总线数据 
	SysDbgSysReadBusData = 18, 
	SysDbgSysWriteBusData = 19, 
	SysDbgSysCheckLowMemory = 20, 
	// 以下是NT 5.2 新增的 
	//分别调用_KdEnableDebugger@0和_KdDisableDebugger@0 
	SysDbgEnableDebugger = 21, 
	SysDbgDisableDebugger = 22, 
	//获取和设置一些调试相关的变量 
	SysDbgGetAutoEnableOnEvent = 23, 
	SysDbgSetAutoEnableOnEvent = 24, 
	SysDbgGetPitchDebugger = 25, 
	SysDbgSetDbgPrintBufferSize = 26, 
	SysDbgGetIgnoreUmExceptions = 27, 
	SysDbgSetIgnoreUmExceptions = 28 
}SYSDBG_COMMAND, *PSYSDBG_COMMAND;

typedef struct _MEMORY_CHUNKS 
{
	DWORD Address;
	PVOID Data;
	ULONG Length;
}MEMORY_CHUNKS, *PMEMORY_CHUNKS;

const ULONG IA32_SYSENTER_CS = 0x174;
const ULONG IA32_SYSENTER_ESP = 0x175;
const ULONG IA32_SYSENTER_EIP = 0x176;
const ULONG SelCodeKernel = 0x8;
const ULONG CmosIndx = 0x0E;        // CMOS Diagnostic Status
const ULONG RdWrIoPort = 0x80;

typedef struct _MSR_STRUCT
{   
	DWORD MsrNum;            // MSR number   
	DWORD NotUsed;            // Never accessed by the kernel   
	DWORD MsrLo;            // IN (write) or OUT (read): Low 32 bits of MSR   
	DWORD MsrHi;            // IN (write) or OUT (read): High 32 bits of MSR   
} MSR_STRUCT, *PMSR_STRUCT;   

typedef enum _BUS_DATA_TYPE
{
	ConfigurationSpaceUndefined = -1,
	Cmos,
	EisaConfiguration,
	BDT_Pos,
	CbusConfiguration,
	PCIConfiguration,
	VMEConfiguration,
	NuBusConfiguration,
	PCMCIAConfiguration,
	MPIConfiguration,
	MPSAConfiguration,
	PNPISAConfiguration,
	SgiInternalConfiguration,
	MaximumBusDataType
} BUS_DATA_TYPE, *PBUS_DATA_TYPE;
// See HalGetBusDataByOffset()/HalSetBusDataByOffset() for explanations of
struct MyCallGate
{
	WORD      OFFSETL;
	WORD      SELECTOR;
	BYTE      DCOUNT;
	BYTE      GTYPE;
	WORD      OFFSETH;
	DWORD    CodeLimit;
	DWORD    CodeBase;
};

typedef struct _BUS_STRUCT {
	ULONG  Offset;
	PVOID  Buffer;
	ULONG  Length;
	BUS_DATA_TYPE  BusDataType;
	ULONG  BusNumber;
	ULONG  SlotNumber;
} BUS_STRUCT, *PBUS_STRUCT;

//<<Windows NT/2000 Native API Reference>> - Gary Nebbett

typedef ULONG (NTAPI * RTLNTSTATUSTODOSERROR)(
	IN NTSTATUSEX Status);

typedef NTSTATUSEX (NTAPI * ZWQUERYSYSTEMINFORMATION )(
	IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
	IN OUT PVOID SystemInformation,
	IN ULONG SystemInformationLength,
	OUT PULONG ReturnLength OPTIONAL);

typedef VOID (NTAPI * RTLINITUNICODESTRING)(
	IN OUT PUNICODE_STRING DestinationString,
	IN PCWSTR SourceString);

typedef NTSTATUSEX (NTAPI * NTOPENSECTION)(
	OUT PHANDLE            SectionHandle,
	IN  ACCESS_MASK        DesiredAccess,
	IN  POBJECT_ATTRIBUTES ObjectAttributes);

typedef NTSTATUSEX (NTAPI * ZWOPENSECTION)(
	OUT PHANDLE SectionHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes);

typedef	NTSTATUSEX (NTAPI * ZWSYSTEMDEBUGCONTROL)(
	IN SYSDBG_COMMAND ControlCode,
	IN PVOID InputBuffer,
	IN ULONG InputBufferLength,
	OUT PVOID OutputBuffer,
	IN ULONG OutputBufferLength,
	OUT PULONG ReturnLength OPTIONAL);

typedef NTSTATUSEX (NTAPI * NTREADVIRTUALMEMORY)(
	IN HANDLE ProcessHandle, 
	IN PVOID BaseAddress, 
	OUT PVOID Buffer, 
	IN ULONG NumberOfBytesToRead, 
	OUT PULONG NumberOfBytesRead OPTIONAL);


#endif //__SystemCommon_H