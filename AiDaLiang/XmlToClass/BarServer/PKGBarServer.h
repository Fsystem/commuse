#ifndef __PKGCommon_H
#define __PKGCommon_H
//#include "globaldef/gloabdef.h"
//-------------------------------------------------------------------------------
// 获取配置
//-------------------------------------------------------------------------------
class PKGCltConfig_Q : public NetPackBase
{
public:
	std::vector<std::string> vecProperNames;

	virtual std::string MakeBodyData();
	virtual void ParseBodyData(std::string szXml);
};

class PKGCltConfig_A : public NetPackBase
{
public:
	std::string sAgent;			//代理商
	std::string sBarId;
	BOOL bAllowTipNoTrust;
	BOOL bAllowTipUnKnow;
	BOOL bAllowTipAccount;

	//BOOL bAllowStartClt;
	BOOL bSafeScan;
	BOOL bShowTrayIcon;
	BOOL bStartAccelerateBall;
	BOOL bShowStartPage;
	BOOL bAllowCloseClt;


	virtual std::string MakeBodyData();
	virtual void ParseBodyData(std::string szXml);
};
//-------------------------------------------------------------------------------
// 客户端启动
//-------------------------------------------------------------------------------
class PKGBarStartClient_Q : public NetPackBase
{
public:
	std::string sOSVersion;
	std::string sIEVersion;
	std::string sFlashVersion;
	std::string sDiskId;
	std::string sCltVersion;
	DWORD       dwStartSysTm;

protected:
	virtual std::string MakeBodyData();
	virtual void ParseBodyData(std::string szXml);
private:
};

typedef PKGCommon_A PKGBarStartClient_A;
//-------------------------------------------------------------------------------
// 客户端握手
//-------------------------------------------------------------------------------
class PKGBarClientShakeHand_Q : public NetPackBase
{
public:
	PKGBarClientShakeHand_Q();
	BOOL bStatus;
	BOOL bClientService;
protected:
	virtual std::string MakeBodyData();
	virtual void ParseBodyData(std::string szXml);
private:
};

class PKGBarClientShakeHand_A : public NetPackBase
{
public:
	PKGBarClientShakeHand_A();
	BOOL bStatus;
protected:
	virtual std::string MakeBodyData();
	virtual void ParseBodyData(std::string szXml);
private:
};
// ------------------------------------------------------------------------------
// 服务端通知客户端杀死进程
// ------------------------------------------------------------------------------
typedef  PKGCommon_A  PKGBarClientKillProc_A;

class PKGBarClientKillProc_Q : public PKGCommon_Q
{
public:
	PKGBarClientKillProc_Q();
	unsigned int pid;
protected:
	virtual std::string MakeBodyData();
	virtual void ParseBodyData(std::string szXml);
private:
};
//-------------------------------------------------------------------------------
//界面向客户端请求客户端信息
//-------------------------------------------------------------------------------
typedef PKGCommon_Q PKGBarAskClient_Q;

class PKGBarAskClient_A:public NetPackBase
{
public:
	struct PKGClientInfo
	{
		string strHostName;			//客户端主机名
		DWORD dwLocalIp;			//客户端IP
		string strMac;				//客户端MAC		
		DWORD dwKeyMacIP;			//mac和本地IP的主键
		DWORD dwKeyMacHostName;		//mac和主机名称的主键
		int nState;
	};
public:
	PKGBarAskClient_A();
	std::list<PKGClientInfo> clientlist;

protected:
	virtual std::string MakeBodyData();
	virtual void ParseBodyData(std::string szXml);
private:
};
//-------------------------------------------------------------------------------
// 获取客户端进程列表
//-------------------------------------------------------------------------------
typedef PKGCommon_A PKGBarGetClientProcListFALSE_A;

class PKGBarGetClientProcListTRUE_A:public NetPackBase
{
public:
	struct PKGProcInfo
	{
		std::string name;
		DWORD		pid;
		std::string Desc;
		std::string Path;
	};
public:
	PKGBarGetClientProcListTRUE_A();
	std::list<PKGProcInfo> proclist;

protected:
	virtual std::string MakeBodyData();
	virtual void ParseBodyData(std::string szXml);
private:
};
//-------------------------------------------------------------------------------
// 核心服务端停止安全大师
//-------------------------------------------------------------------------------
typedef  PKGCommon_A PKGBarStopSafeSoft_A;

class PKGBarStopSafeSoft_Q : public PKGCommon_Q
{
public:
	PKGBarStopSafeSoft_Q();
	BOOL optype;
protected:
	virtual std::string MakeBodyData();
	virtual void ParseBodyData(std::string szXml);
private:
};
//-------------------------------------------------------------------------------
// 客户端关机
//-------------------------------------------------------------------------------
typedef PKGCommon_Q PKGBarClientShutDown_Q;
typedef PKGCommon_A PKGBarClientShutDown_A;

//-------------------------------------------------------------------------------
// 客户端重启
//-------------------------------------------------------------------------------
typedef PKGCommon_Q PKGBarClientReStart_Q;
typedef PKGCommon_A PKGBarClientReStart_A;


//-------------------------------------------------------------------------------
// 网吧服务通知客户端停止
//-------------------------------------------------------------------------------
typedef PKGCommon_Q PKGBarStopClient_Q;
typedef PKGCommon_A PKGBarStopClient_A;
//-------------------------------------------------------------------------------
// 客户端更新策略
//-------------------------------------------------------------------------------
typedef PKGCommon_Q PKGBarClientUpStrage_Q;
typedef PKGCommon_A PKGBarClientUpStrage_A;

//-------------------------------------------------------------------------------
// 上报行为定义
//-------------------------------------------------------------------------------
//行为:0-驱动 1-进程 2-文件 3-注册表
//操作:
//驱动：0-加载驱动 1-尝试结束进程,创建文件,创建注册表
//	   2-尝试注入进程,读文件,修改注册表
//	   4-向目标进程发送退出消息,重命名文件/注册表
//	   8-创建目标进程,删除文件/注册表
//	   16-结束进程
struct ProcessInfo
{
	DWORD                   dwKey;						//进程Key
	DWORD					dwPid;						//进程ID
	char					szTrustDes[64];				//信任描述
	unsigned int			unCrc;						//文件的CRC
	char					szProcessName[1000];	//进程路径
	char					szProcessPath[1000];	//进程路径
	char					szProcessDescribe[MAX_PATH];//进程描述
	char					szSign[MAX_PATH];			//文件签名
	char					szMd5[33];					//md5
};

struct ProcessInfoEx
{
	DWORD                   dwKey;						//进程Key
	DWORD					dwPid;						//进程ID
	DWORD					dwParantPid;				//进程ID
	DWORD					dwStartTm;					//启动时间
	char					szTrustDes[64];				//信任描述
	unsigned int			unCrc;						//文件的CRC
	char					szProcessPath[1000];	//进程路径
	char					szSign[MAX_PATH];			//文件签名
	char					szProcessDescribe[MAX_PATH];//进程描述
	char					szMd5[33];					//md5
};

//-------------------------------------------------------------------------------
// 进程操作
//-------------------------------------------------------------------------------
class PKGBarActionProcess_Q : public NetPackBase
{
public:
	PKGBarActionProcess_Q();
	WORD wAction;			
	WORD wOperation;		
	DWORD dwTimeStamp;
	ProcessInfo parant;
	ProcessInfo child;
public:
	virtual std::string MakeBodyData();
	virtual void ParseBodyData(std::string szXml);
protected:
private:
};

typedef PKGCommon_A PKGBarActionProcess_A;
//-------------------------------------------------------------------------------
// 文件操作
//-------------------------------------------------------------------------------
class PKGBarActionFile_Q : public NetPackBase
{
public:
	PKGBarActionFile_Q();
	WORD wAction;
	WORD wOperation;
	DWORD dwTimeStamp;
	//父亲进程
	ProcessInfo parant;
	std::string child;
public:
	virtual std::string MakeBodyData();
	virtual void ParseBodyData(std::string szXml);
protected:
private:
};

typedef PKGCommon_A PKGBarActionFile_A;
//-------------------------------------------------------------------------------
// 注册表操作
//-------------------------------------------------------------------------------
class PKGBarActionReg_Q : public NetPackBase
{
public:
	PKGBarActionReg_Q();
	WORD wAction;
	WORD wOperation;
	DWORD dwTimeStamp;
	//父亲进程
	ProcessInfo parant;
	std::string child;
public:
	virtual std::string MakeBodyData();
	virtual void ParseBodyData(std::string szXml);
protected:
private:
};

typedef PKGCommon_A PKGBarActionReg_A;
//-------------------------------------------------------------------------------
// 微软信任信息
//-------------------------------------------------------------------------------
class PKGBarADLFileTrustInfo_Q : public NetPackBase
{
public:
	PKGBarADLFileTrustInfo_Q();
	DWORD dwFileCrc;
	DWORD dwFileNameCrc;
	std::string sFilePath;
	BYTE cbTrustType;
	std::string sSigner;
public:
	virtual std::string MakeBodyData();
	virtual void ParseBodyData(std::string szXml);
protected:
private:
};

typedef PKGCommon_A PKGBarADLFileTrustInfo_A;

//-------------------------------------------------------------------------------
// 开机进程列表
//-------------------------------------------------------------------------------
class PKGBarProcessListOnSysStart_Q : public NetPackBase
{
public:
	std::list<ProcessInfoEx> processList;
public:
	virtual std::string MakeBodyData();
	virtual void ParseBodyData(std::string szXml);
protected:
private:
};

typedef PKGCommon_A PKGBarProcessListOnSysStart_A;

//-------------------------------------------------------------------------------
// 向网吧服务汇报CPU，内存信息
//-------------------------------------------------------------------------------
class PKGBarReportCpuInfo_Q : public NetPackBase
{
public:
	struct PKGBarCpuInfo
	{
		std::string pname;
		DWORD pid;
		DWORD cpu;
		DWORD mem;
	};
public:
	PKGBarReportCpuInfo_Q();

	
	std::list<PKGBarCpuInfo> cpuinfo_list;

protected:
	virtual std::string MakeBodyData();
	virtual void ParseBodyData(std::string szXml);
};

typedef PKGCommon_A  PKGBarReportCpuInfo_A;

//-------------------------------------------------------------------------------
// 插件日志
//-------------------------------------------------------------------------------
class PKGBarPluginReport_Q : public NetPackBase
{
public:
	int nModuleId;
	std::string sLog;
protected:
	virtual std::string MakeBodyData();
	virtual void ParseBodyData(std::string szXml);
private:
};

typedef PKGCommon_A  PKGBarPluginReport_A;

//-------------------------------------------------------------------------------
//界面通知服务重启
//-------------------------------------------------------------------------------
typedef PKGCommon_Q PKGReStartServer_Q;
typedef PKGCommon_A PKGReStartServer_A;
//-------------------------------------------------------------------------------
//界面关闭或开启服务
//-------------------------------------------------------------------------------
typedef PKGCommon_A PKGCloseOrOpenServer_Q;

class PKGCloseOrOpenServer_A : public PKGCommon_A
{
public:
	PKGCloseOrOpenServer_A(PKGCommon_A* pDependObj = NULL);
	int nOperType;
protected:
	virtual std::string MakeBodyData();
	virtual void ParseBodyData(std::string szXml);
private:
};
//-------------------------------------------------------------------------------
//网吧服务向客户端下发白名单
//-------------------------------------------------------------------------------
typedef PKGCommon_A PKGBarSendWhiteList_A;

class PKGBarSendWhiteList_Q : public NetPackBase
{
public:
	struct PKGWhiteInfo
	{
		std::string strProcName;
		DWORD dwProcMD5;
		DWORD dwProcCRC;
	};
public:
	PKGBarSendWhiteList_Q();
	std::list<PKGWhiteInfo> whitelist;
protected:
	virtual std::string MakeBodyData();
	virtual void ParseBodyData(std::string szXml);
private:
};

//-------------------------------------------------------------------------------
//服务通知界面更新消息列表
//-------------------------------------------------------------------------------
typedef PKGCommon_A PKGBarUpdateMessage_Q;

//-------------------------------------------------------------------------------
//界面通知服务清空日志
//-------------------------------------------------------------------------------
typedef  PKGCommon_A  PKGBarClearLog_A;

class PKGBarClearLog_Q : public PKGCommon_Q
{
public:
	PKGBarClearLog_Q();
	string strLogPath;
protected:
	virtual std::string MakeBodyData();
	virtual void ParseBodyData(std::string szXml);
private:
};
//-------------------------------------------------------------------------------
//界面通知服务日志设置变更
//-------------------------------------------------------------------------------
typedef PKGCommon_Q PKGBarLogSetChange_Q;
typedef PKGCommon_A PKGBarLogSetChange_A;

#endif //__PKGCommon_H