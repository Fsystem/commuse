// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"

class TestPlug : public IPluginInterface
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
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
		LOGEVENA("aaaaaaaaaaaaaaaaaaaa");
		break;
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

IMP_PLUGIN_EXPORT(TestPlug);

