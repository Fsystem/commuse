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


#endif //__CKInterface_H