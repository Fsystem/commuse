// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"

#include "resource.h"

class TestPlug : public IPluginInterface,public SingletonBase<TestPlug>
{
public:
	virtual BOOL OnInitPlugin(IPluginParant* pDelegate)
	{
		PLUGIN_BIND_PARANT;

		mParantDelegate->SendMessageToParant(123,"asd",3);

		return TRUE;
	}

	virtual void OnClosePlugin()
	{

	}

	virtual BOOL OnHandleData(DWORD dwCmd,PVOID pData,int nDataLen)
	{
		LOGEVENA("plug:[%u]\n",dwCmd);
		return TRUE;
	}

protected:
private:
};

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	TCHAR szPath[MAX_PATH]={0};
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		LOGEVENA("ccccccccccccccccccccccccccccc");
		
		GetModuleFileName(ThisModuleHandle(),szPath,MAX_PATH);
		//_tcsrchr(szPath,'\\')[1] = 0;
		_tcscat(szPath,TEXT("asdasd.txt"));
		LOGEVEN(szPath);
		CheckAndExportFile(TEXT("TXT"),szPath,IDR_TXT1);
		break;
	case DLL_THREAD_ATTACH:
		LOGEVENA("aaaaaaaaaaaaaaaaaaaa");
		
		break;
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

IMP_PLUGIN_EXPORT_EX(&TestPlug::Instance());

