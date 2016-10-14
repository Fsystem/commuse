#ifndef __CKInterface_H
#define __CKInterface_H

//-------------------------------------------------------------------------------
// 网络管理接口
//-------------------------------------------------------------------------------
struct INetDelegate
{
	virtual void OnSocketRecv(enNetMethod method,DWORD dwCMD,const char* pData,int nDataSize) = 0;
	virtual void OnSocketErr(enNetMethod method,DWORD dwCMD,const char* pData,int nDataSize) = 0;
};

//-------------------------------------------------------------------------------
// 插件接口
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
// 业务插件接口
//-------------------------------------------------------------------------------
#ifndef INOUT
#define INOUT
#endif
typedef DWORD (WINAPI * REG_P2P_FUN) (IN DWORD crc,IN const char * file_data,IN DWORD len);		//上传一个文件到内网
typedef DWORD (WINAPI * DOWNLOAD_P2P_FUN) (IN DWORD crc,OUT char * file_data,INOUT DWORD *len);	//从内网找一个文件，根据CRC值
class CPlug_Proxy  
{
public:
	CPlug_Proxy(){};
	virtual ~CPlug_Proxy(){};

	//初始化服务器，路径，渠道ID
	virtual	DWORD InitPlug(const char * server,const char * path_dir,const char *agent_id){return NO_ERROR;};	
	//设置WEB下载服务器地址；插件存放自己的网络配置文件
	virtual	void  SetWebServer(const char * web_server){};										
	//P2P部分上传，下载
	virtual	DWORD PlugP2p(REG_P2P_FUN	_reg_fun,DOWNLOAD_P2P_FUN _down_fun){return NO_ERROR;};	
	//初始化完成，不管有多少个初始化，这个函数最后调用
	virtual	void  StartPlug(){};																	

};

#endif //__CKInterface_H