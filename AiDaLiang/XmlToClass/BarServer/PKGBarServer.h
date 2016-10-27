#ifndef __PKGCommon_H
#define __PKGCommon_H
//#include "globaldef/gloabdef.h"
//-------------------------------------------------------------------------------
// ��ȡ����
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
	std::string sAgent;			//������
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
// �ͻ�������
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
// �ͻ�������
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
// �����֪ͨ�ͻ���ɱ������
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
//������ͻ�������ͻ�����Ϣ
//-------------------------------------------------------------------------------
typedef PKGCommon_Q PKGBarAskClient_Q;

class PKGBarAskClient_A:public NetPackBase
{
public:
	struct PKGClientInfo
	{
		string strHostName;			//�ͻ���������
		DWORD dwLocalIp;			//�ͻ���IP
		string strMac;				//�ͻ���MAC		
		DWORD dwKeyMacIP;			//mac�ͱ���IP������
		DWORD dwKeyMacHostName;		//mac���������Ƶ�����
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
// ��ȡ�ͻ��˽����б�
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
// ���ķ����ֹͣ��ȫ��ʦ
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
// �ͻ��˹ػ�
//-------------------------------------------------------------------------------
typedef PKGCommon_Q PKGBarClientShutDown_Q;
typedef PKGCommon_A PKGBarClientShutDown_A;

//-------------------------------------------------------------------------------
// �ͻ�������
//-------------------------------------------------------------------------------
typedef PKGCommon_Q PKGBarClientReStart_Q;
typedef PKGCommon_A PKGBarClientReStart_A;


//-------------------------------------------------------------------------------
// ���ɷ���֪ͨ�ͻ���ֹͣ
//-------------------------------------------------------------------------------
typedef PKGCommon_Q PKGBarStopClient_Q;
typedef PKGCommon_A PKGBarStopClient_A;
//-------------------------------------------------------------------------------
// �ͻ��˸��²���
//-------------------------------------------------------------------------------
typedef PKGCommon_Q PKGBarClientUpStrage_Q;
typedef PKGCommon_A PKGBarClientUpStrage_A;

//-------------------------------------------------------------------------------
// �ϱ���Ϊ����
//-------------------------------------------------------------------------------
//��Ϊ:0-���� 1-���� 2-�ļ� 3-ע���
//����:
//������0-�������� 1-���Խ�������,�����ļ�,����ע���
//	   2-����ע�����,���ļ�,�޸�ע���
//	   4-��Ŀ����̷����˳���Ϣ,�������ļ�/ע���
//	   8-����Ŀ�����,ɾ���ļ�/ע���
//	   16-��������
struct ProcessInfo
{
	DWORD                   dwKey;						//����Key
	DWORD					dwPid;						//����ID
	char					szTrustDes[64];				//��������
	unsigned int			unCrc;						//�ļ���CRC
	char					szProcessName[1000];	//����·��
	char					szProcessPath[1000];	//����·��
	char					szProcessDescribe[MAX_PATH];//��������
	char					szSign[MAX_PATH];			//�ļ�ǩ��
	char					szMd5[33];					//md5
};

struct ProcessInfoEx
{
	DWORD                   dwKey;						//����Key
	DWORD					dwPid;						//����ID
	DWORD					dwParantPid;				//����ID
	DWORD					dwStartTm;					//����ʱ��
	char					szTrustDes[64];				//��������
	unsigned int			unCrc;						//�ļ���CRC
	char					szProcessPath[1000];	//����·��
	char					szSign[MAX_PATH];			//�ļ�ǩ��
	char					szProcessDescribe[MAX_PATH];//��������
	char					szMd5[33];					//md5
};

//-------------------------------------------------------------------------------
// ���̲���
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
// �ļ�����
//-------------------------------------------------------------------------------
class PKGBarActionFile_Q : public NetPackBase
{
public:
	PKGBarActionFile_Q();
	WORD wAction;
	WORD wOperation;
	DWORD dwTimeStamp;
	//���׽���
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
// ע������
//-------------------------------------------------------------------------------
class PKGBarActionReg_Q : public NetPackBase
{
public:
	PKGBarActionReg_Q();
	WORD wAction;
	WORD wOperation;
	DWORD dwTimeStamp;
	//���׽���
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
// ΢��������Ϣ
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
// ���������б�
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
// �����ɷ���㱨CPU���ڴ���Ϣ
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
// �����־
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
//����֪ͨ��������
//-------------------------------------------------------------------------------
typedef PKGCommon_Q PKGReStartServer_Q;
typedef PKGCommon_A PKGReStartServer_A;
//-------------------------------------------------------------------------------
//����رջ�������
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
//���ɷ�����ͻ����·�������
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
//����֪ͨ���������Ϣ�б�
//-------------------------------------------------------------------------------
typedef PKGCommon_A PKGBarUpdateMessage_Q;

//-------------------------------------------------------------------------------
//����֪ͨ���������־
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
//����֪ͨ������־���ñ��
//-------------------------------------------------------------------------------
typedef PKGCommon_Q PKGBarLogSetChange_Q;
typedef PKGCommon_A PKGBarLogSetChange_A;

#endif //__PKGCommon_H