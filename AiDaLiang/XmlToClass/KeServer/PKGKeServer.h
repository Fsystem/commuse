#ifndef __PKGKeServer_H
#define __PKGKeServer_H
//-------------------------------------------------------------------------------
// �ͻ�������
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
// �ͻ�������
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
// �ͻ��˴Ӻ��ķ�������ȡ����url
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
// �ͻ�����ɱ�ϴ������ķ�����
//-------------------------------------------------------------------------------
typedef PKGCommon_Q PKGKeCltKillSelf_Q;
typedef PKGCommon_A PKGKeCltKillSelf_A;

//////////////////////////////////////////////////////////////////////////
// ���ɷ�������ķ���ͨ�Ż���
class KeNetPackBase : public NetPackBase
{
public:
	std::string		sOSVersion;						//ϵͳ�汾
	std::string		sIEVersion;						//IE�汾
	std::string		sFlashVersion;					//�ڴ�汾
	std::string		sDiskId;						//Ӳ�����к�

	std::string     sServerVersion;                 //���ɷ���汾��
	int				nClientCount;					//���ɷ����µĿͻ�������
protected:
	virtual std::string MakeBodyData();
	virtual void ParseBodyData(std::string szXml);
};
//////////////////////////////////////////////////////////////////////////
//���ɷ�������ķ�����������ID
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
//���ɷ��񿪻��ϱ�
typedef KeNetPackBase PKGKeStartServer_Q;
typedef PKGCommon_A PKGKeStartServer_A;
//////////////////////////////////////////////////////////////////////////
//���ɷ����������������
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
//���ɷ����ϱ�ʵʱ����
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
//���ɷ���������������
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
//���ɷ������������ѭ����Ϣ�б�
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
//���ɷ��������ѯ���Ƿ���Ҫֹͣ�Ŀͻ���
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
//����ķ������������
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