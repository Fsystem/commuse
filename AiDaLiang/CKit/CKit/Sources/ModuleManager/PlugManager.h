#ifndef _PLUG_H
#define _PLUG_H

#include <string>
#include <vector>
#include <map>



//�ص�����


/*
* !brief ���������Ϣ
*/
typedef struct PlugInfo
{
	char				szName[MAX_PATH];			//�������
	char				szRelativePath[MAX_PATH];	//������·��
	char				szPlugDescribe[64];			//�������
	HINSTANCE			hModule;					//������
	IPluginInterface*	pPlugDelegate;				//�������
}PlugInfo,*PPlugInfo;

/*
* !brief ���������
*/
class PlugManager : public SingletonBase<PlugManager>,public IPluginParant
{
public:
	PlugManager();
	virtual ~PlugManager();

public:
	void LoadAllPlugin(IPluginParant* pSink = NULL);
	void StopAllPlugin();

	//����Ƿ�����������
	bool IsPlugRunning();

protected:
	BOOL LoadCfg();
	BOOL LoadPlugin(LPCSTR szPlugName);
	BOOL LoadPlugin(PlugInfo& plugInfo);
	void FreePlugin(LPCSTR szPlugName);
	void FreePlugin(PlugInfo& plugInfo);

protected:
	void LoadPluginThread();
	void FreePluginThread();

public:
	//��������ϢͶ�ݸ����
	void OnHandleMessage(DWORD dwCmd,PVOID pData,int nLen);
	//�������ϢͶ�ݸ�����
	virtual void SendMessageToParant(DWORD dwCmd,PVOID pData,int nLen);

private:
	int											mPlugCnt;
	char										mPlugDir[MAX_PATH];
	char										mPlugIni[MAX_PATH];

private:
	//static HANDLE   thread_handle_load_;
	HANDLE mThreadLoad;
	bool mLoadedAll;

	IPluginParant* mSink;
};


#endif