#ifndef __CKInterface_H
#define __CKInterface_H

//-------------------------------------------------------------------------------
// �������ӿ�
//-------------------------------------------------------------------------------
struct INetDelegate
{
	virtual void OnSocketRecv(enNetMethod method,DWORD dwCMD,const char* pData,int nDataSize) = 0;
	virtual void OnSocketErr(enNetMethod method,DWORD dwCMD,const char* pData,int nDataSize) = 0;
};

//-------------------------------------------------------------------------------
// ����ӿ�
//-------------------------------------------------------------------------------
struct IPluginParant
{
	virtual void SendMessageToParant(DWORD dwCmd,PVOID pData,int nLen) = 0;
};

struct IPluginInterface
{
	#define PLUGIN_BIND_PARANT mParantDelegate = pDelegate;

	IPluginParant* mParantDelegate;
	virtual BOOL OnInitPlugin(IPluginParant* pDelegate) {PLUGIN_BIND_PARANT;return TRUE;};
	virtual void OnClosePlugin(){};
	virtual BOOL OnHandleData(DWORD dwCmd,PVOID pData,int nDataLen){return TRUE;};
};

//-------------------------------------------------------------------------------
// ҵ�����ӿ�
//-------------------------------------------------------------------------------
#ifndef INOUT
#define INOUT
#endif
typedef DWORD (WINAPI * REG_P2P_FUN) (IN DWORD crc,IN const char * file_data,IN DWORD len);		//�ϴ�һ���ļ�������
typedef DWORD (WINAPI * DOWNLOAD_P2P_FUN) (IN DWORD crc,OUT char * file_data,INOUT DWORD *len);	//��������һ���ļ�������CRCֵ
//class  CPlug_Proxy
//{
//public:
//	CPlug_Proxy(){};
//	//ռλ�麯��
//	virtual ~CPlug_Proxy(){};
//	//��ʼ����������·��������ID
//	virtual	DWORD InitPlug(const char * server,const char * path_dir,const char *agent_id){return NO_ERROR;};	
//	//����WEB���ط�������ַ���������Լ������������ļ�
//	//virtual	void  SetWebServer(const char * web_server) = 0;										
//	//P2P�����ϴ�������
//	virtual	DWORD PlugP2p(REG_P2P_FUN	_reg_fun,DOWNLOAD_P2P_FUN _down_fun){return NO_ERROR;};	
//	//��ʼ����ɣ������ж��ٸ���ʼ�����������������
//	virtual	void  StartPlug(){};
//};

class CPlug_Proxy
{
	//
	PropertyMember(BOOL,MemLoaded);
public:
	CPlug_Proxy(){};
	virtual ~CPlug_Proxy(){};

	//��ʼ����������·��������ID
	virtual	DWORD InitPlug(const char * server,const char * path_dir,const char *agent_id){
		setMemLoaded(TRUE);

		mServerInfo = server;
		mMainPath = path_dir;
		mAgentId = agent_id;

		//G_COM_PRINT_LOG.SetLogPath(mMainPath.c_str());

		return NO_ERROR;
	};	
	//����WEB���ط�������ַ���������Լ������������ļ�
// 	virtual	void  SetWebServer(const char * web_server){
// 		mWebServer = web_server;
// 	};										
	//P2P�����ϴ�������
	virtual	DWORD PlugP2p(REG_P2P_FUN	_reg_fun,DOWNLOAD_P2P_FUN _down_fun){
		mpUploadFun = _reg_fun;
		mpDownloadFun = _down_fun;
		return NO_ERROR;
	};	
	//��ʼ����ɣ������ж��ٸ���ʼ�����������������
	virtual	void  StartPlug(){};																	

	//������Ϣ
protected:
	std::string					mServerInfo;
	std::string					mWebServer;
	std::string					mMainPath;
	std::string					mAgentId;
	REG_P2P_FUN					mpUploadFun;
	DOWNLOAD_P2P_FUN			mpDownloadFun;
};

#endif //__CKInterface_H