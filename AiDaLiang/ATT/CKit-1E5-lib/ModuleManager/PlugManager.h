#ifndef _PLUG_H
#define _PLUG_H

#include <string>
#include <vector>
#include <map>



//回调函数


/*
* !brief 插件描述信息
*/
typedef struct PlugInfo
{
	char				szName[MAX_PATH];			//插件名字
	char				szRelativePath[MAX_PATH];	//插件相对路径
	char				szPlugDescribe[64];			//插件描述
	HINSTANCE			hModule;					//插件句柄
	IPluginInterface*	pPlugDelegate;				//插件代理
}PlugInfo,*PPlugInfo;

/*
* !brief 插件管理类
*/
class PlugManager : public SingletonBase<PlugManager>,public IPluginParant
{
public:
	PlugManager();
	virtual ~PlugManager();

public:
	void LoadAllPlugin(IPluginParant* pSink = NULL);
	void StopAllPlugin();

	//插件是否都正常运行了
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
	//宿主将消息投递给插件
	void OnHandleMessage(DWORD dwCmd,PVOID pData,int nLen);
	//插件将消息投递给宿主
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