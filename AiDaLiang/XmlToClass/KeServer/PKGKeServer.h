#ifndef __PKGKeServer_H
#define __PKGKeServer_H
//-------------------------------------------------------------------------------
// 客户端启动
//-------------------------------------------------------------------------------
class PKGKeStartClient_Q : public NetPackBase
{
public:
	std::string sOSVersion;
	std::string sIEVersion;
	std::string sFlashVersion;
	std::string sDiskId;
	std::string sCltVersion;

	DWORD dwCrcMacHostIpstr;

protected:
	virtual std::string MakeBodyData();
	virtual void ParseBodyData(std::string szXml);
private:
};
//-------------------------------------------------------------------------------
// 客户端在线
//-------------------------------------------------------------------------------
typedef PKGKeStartClient_Q PKGKeClinetOnline_Q;

class PKGKeClinetOnline_A : public PKGCommon_A
{
public:
	PKGKeClinetOnline_A();
	DWORD dwNextTime;
protected:
	virtual std::string MakeBodyData();
	virtual void ParseBodyData(std::string szXml);
private:
};
//-------------------------------------------------------------------------------
// 客户端从核心服务器获取下载url
//-------------------------------------------------------------------------------
typedef PKGKeStartClient_Q PKGKeUpDateUrl_Q;

class PKGKeUpDateUrl_A : public NetPackBase
{
public:
	PKGKeUpDateUrl_A();
	BOOL bStatus;
	std::string sUrl;
protected:
	virtual std::string MakeBodyData();
	virtual void ParseBodyData(std::string szXml);
private:
};
//-------------------------------------------------------------------------------
// 客户端自杀上传到核心服务器
//-------------------------------------------------------------------------------
typedef PKGCommon_Q PKGKeCltKillSelf_Q;
typedef PKGCommon_A PKGKeCltKillSelf_A;

//////////////////////////////////////////////////////////////////////////
// 网吧服务与核心服务通信基类
class KeNetPackBase : public NetPackBase
{
public:
	std::string		sOSVersion;						//系统版本
	std::string		sIEVersion;						//IE版本
	std::string		sFlashVersion;					//内存版本
	std::string		sDiskId;						//硬盘序列号

	std::string     sServerVersion;                 //网吧服务版本号
	int				nClientCount;					//网吧服务下的客户端数量
protected:
	virtual std::string MakeBodyData();
	virtual void ParseBodyData(std::string szXml);
};
//////////////////////////////////////////////////////////////////////////
//网吧服务向核心服务请求网吧ID
typedef KeNetPackBase PKGKeBarIDServer_Q;

class PKGKeBarIDServer_A : public KeNetPackBase
{
public:
	PKGKeBarIDServer_A();
	BOOL bStatus;
	int nBarId;
protected:
	virtual std::string MakeBodyData();
	virtual void ParseBodyData(std::string szXml);
private:
};
//////////////////////////////////////////////////////////////////////////
//网吧服务开机上报
typedef KeNetPackBase PKGKeStartServer_Q;
typedef PKGCommon_A PKGKeStartServer_A;
//////////////////////////////////////////////////////////////////////////
//网吧服务向核心请求升级
typedef KeNetPackBase PKGKeUpdateServer_Q;

class PKGKeUpdateServer_A : public KeNetPackBase
{
public:
	PKGKeUpdateServer_A();
	BOOL bStatus;
	std::string strServUrl;
	std::string strClientUrl;
	std::string strServVer;
	std::string strClientVer;
protected:
	virtual std::string MakeBodyData();
	virtual void ParseBodyData(std::string szXml);
private:
};
//////////////////////////////////////////////////////////////////////////
//网吧服务上报实时在线
typedef KeNetPackBase PKGKeServerOnline_Q;

class PKGKeServerOnline_A : public PKGCommon_A
{
public:
	PKGKeServerOnline_A();
	DWORD dwNextTime;
	//BOOL bWhiteList;
	BOOL bUpdate;
	BOOL bStopClient;
	BOOL bMsgList;
protected:
	virtual std::string MakeBodyData();
	virtual void ParseBodyData(std::string szXml);
private:
};
//////////////////////////////////////////////////////////////////////////
//网吧服务向核心请求策略
typedef KeNetPackBase PKGKeGetRuleServer_Q;

class PKGKeGetRuleServer_A : public KeNetPackBase
{
public:
	PKGKeGetRuleServer_A();
	BOOL bStatus;
	std::string strConfigUrl;
protected:
	virtual std::string MakeBodyData();
	virtual void ParseBodyData(std::string szXml);
private:
};
//////////////////////////////////////////////////////////////////////////
//网吧服务向核心请求循环消息列表
typedef KeNetPackBase PKGKeGetMsgServer_Q;

class PKGKeGetMsgServer_A : public KeNetPackBase
{
public:
	struct PKGMsgInfo
	{
		std::string strTitle;
		std::string strUrl;
	};
public:
	PKGKeGetMsgServer_A();
	std::list<PKGMsgInfo> msglist;
protected:
	virtual std::string MakeBodyData();
	virtual void ParseBodyData(std::string szXml);
private:
};
//////////////////////////////////////////////////////////////////////////
//网吧服务向核心询问是否有要停止的客户端
typedef KeNetPackBase PKGKeStopClientServer_Q;

class PKGKeStopClientServer_A : public KeNetPackBase
{
public:
	PKGKeStopClientServer_A();
	int iType;
protected:
	virtual std::string MakeBodyData();
	virtual void ParseBodyData(std::string szXml);
private:
};

typedef PKGCommon_A PKGKeStopClientAnser;
//////////////////////////////////////////////////////////////////////////
//向核心服务请求白名单
typedef KeNetPackBase PKGKeGetWhiteList_Q;

class PKGKeGetWhiteList_A : public KeNetPackBase
{
public:
// 	struct PKGWhiteInfo
// 	{
// 		std::string strProcName;
// 		DWORD dwProcMD5;
// 		DWORD dwProcCRC;
// 	};
public:
	PKGKeGetWhiteList_A();
/*	std::list<PKGWhiteInfo> whitelist;*/
	BOOL bStatus;
	string strWhiteUrl;
protected:
	virtual std::string MakeBodyData();
	virtual void ParseBodyData(std::string szXml);
private:
};
//////////////////////////////////////////////////////////////////////////

#endif //__PKGKeServer_H