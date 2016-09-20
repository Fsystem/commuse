#pragma once 
#include <string>
#include "wtypes.h"
#include "tchar.h"
using namespace std;

//服务的结构定义
typedef	struct	_CKIT_SERVICE_INFO
{
	string SZAPPNAME;
	string SZSERVICENAME;
	string SZSERVICEDISPLAYNAME;
	string SZDEPENDENCIES;
	string SZSERVICEEXEPATH;//可执行文件路径
	int SPORT;//服务端口号
	string SIP_STR;//ip地址
	int SITHREADCOUNT;//线程数量

}CKITSERVICE_INFO,*PCKITSERVICE_INFO;
enum SEV_STATUS
{
	SEV_ERROR	= 0x00,
	SEV_NO		= 0x01,
	SEV_HAVE	= 0x02,
	SEV_RUNING	= 0x03,
	SEV_STOPED	= 0x04
};

class CService
{
public:
	CService(void);
public:
	~CService(void);
public:
	static VOID GetServiceInfo(CKITSERVICE_INFO *serviceInfo);
	static BYTE CheckServiceStatus(const char*pServiceName);
	static BOOL StartSevice(const char*pServiceName);
	static BOOL AddService(/*const char*pSourceName,*/const char*pServiceName,const char*pDisName,const char*pPath);
	static BOOL RemoveService(const char*pServiceName);
	static BOOL StopSevice(const char*pServiceName);
	//BOOL GetSevicePath(const char*pServiceName,CString &strServicePath);
	//BOOL GetCurPath(CString &strCurPath);
public:

	static CKITSERVICE_INFO m_ServiceInfo;
};

