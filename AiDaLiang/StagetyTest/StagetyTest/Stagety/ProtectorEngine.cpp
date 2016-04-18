#include <stdafx.h>
#include "ProtectorEngine.h"

#include "StagetyInclude.h"

#include "../resource.h"

//-------------------------------------------------------------------------------

//驱动回调
static BOOL WINAPI Sys_Filter_fun(UINT type, PVOID lpata, DWORD size_len)
{
	return StagetyManager::Instance().HandleAction(type,lpata,size_len);
}
//-------------------------------------------------------------------------------
ProtectorEngine::ProtectorEngine()
{
	GetModuleFileName(NULL, mModulePath, MAX_PATH);
	_tcsrchr(mModulePath,'\\')[0]=0;

	mHDllModule = NULL;
	mEngineSink = NULL;

}

void ProtectorEngine::StartEngine(IActionResultDelegate* pEngineSink)
{
	mEngineSink = pEngineSink;

	mEventBlock = CreateEvent(NULL,FALSE,FALSE,NULL);
	ResetEvent(mEventBlock);

	//上报服务器信息
	//ReportManager::Instance().SetWorkServerInfo(netserver_ip.c_str(),netserver_port);
	//ReportManager::Instance().SetKernelServerInfo(coreserver_ip.c_str(),coreserver_port_second);
	ReportManager::Instance().StartService();

	//初始化策略管理器
	StagetyManager::Instance().BuildAllStagety(this);

	//加载驱动
	if(LoadProtectDriver()==FALSE) 
	{
		LOGEVEN(TEXT("ERROR:保护驱动加载失败\n"));
		return;
	}
}

void ProtectorEngine::StopEngine()
{
	if (mHDllModule)
	{
		mOperPorcessStatus(FALSE);
		mOperMsgFilter(FALSE);
		mOperSysFilter(FALSE);

		FreeLibrary(mHDllModule);
		mHDllModule = NULL;
	}

	StagetyManager::Instance().ResetData();

	ReportManager::Instance().StopService();

	if(mEventBlock)
	{
		SetEvent(mEventBlock);
		CloseHandle(mEventBlock);
		mEventBlock = NULL;
	}
}

void ProtectorEngine::Block()
{
	WaitForSingleObject(mEventBlock,INFINITE);
}

void ProtectorEngine::UnBlock()
{
	SetEvent(mEventBlock);
}


BOOL ProtectorEngine::LoadProtectDriver()
{
	TCHAR strFile[MAX_PATH] = {0};
	_sntprintf(strFile,MAX_PATH,TEXT("%s/tmpsys.dll"),mModulePath);
	CheckAndExportFile(TEXT("DLL"), strFile, IDR_SYSIO);	
	mHDllModule = LoadLibrary(strFile);
	if( NULL != mHDllModule)
	{
		mRegCallBackFun		= (SYS_RegCallBackFun)GetProcAddress(mHDllModule, "RegCallBackFun");
		mOperMsgFilter		= (SYS_OperMsgFilter)GetProcAddress(mHDllModule, "OperMsgFilter");
		mOperSysFilter		= (SYS_OperSysFilter)GetProcAddress(mHDllModule, "OperSysFilter");
		mOperPorcessStatus	= (SYS_OperSysFilter)GetProcAddress(mHDllModule, "OperPorcessStatus");

		if( NULL == mRegCallBackFun||
			NULL == mOperMsgFilter||
			NULL == mOperSysFilter||
			NULL == mOperPorcessStatus)
		{
			FreeLibrary(mHDllModule);
			mHDllModule = NULL;
			return FALSE;
		}
	}

	if( !mRegCallBackFun(Sys_Filter_fun) )
	{
		LOGEVEN(TEXT("注册回调失败\n"));
		FreeLibrary(mHDllModule);
		mHDllModule = NULL;
		return FALSE;
	}

	mOperMsgFilter(TRUE);
	mOperSysFilter(TRUE);

	LOGEVEN(TEXT(">>加载驱动成功\n" ));

	return TRUE;
}

void ProtectorEngine::ReloadProtectDriver()
{
	try
	{
		if (mHDllModule==NULL)
		{
			LoadProtectDriver();
			return;
		}

		if( NULL == mRegCallBackFun||
			mOperMsgFilter==NULL ||
			mOperSysFilter==NULL ||
			mOperPorcessStatus==NULL) throw 0;

		mOperPorcessStatus(FALSE);
		mRegCallBackFun(NULL);
		mOperMsgFilter(FALSE);
		mOperSysFilter(FALSE);
		Sleep(1000);
		mRegCallBackFun(Sys_Filter_fun);
		mOperPorcessStatus(TRUE);
		mOperMsgFilter(TRUE);
		mOperSysFilter(TRUE);

		LOGEVEN(TEXT(">>重新加载驱动\n" ));
	}
	catch(...)
	{
		LOGEVEN(TEXT(">>重新加载驱动异常[%p],[%p],[%p],[%p]\n" ),
			mRegCallBackFun,mOperMsgFilter,mOperSysFilter,mOperPorcessStatus);
	}

}

void ProtectorEngine::OnHandleResultByStagety(ActionOperateResult* pResult)
{
	//无操作的表示打印日志
	//if (pResult->actionType == (WORD)enActionNull || pResult->operateType == (WORD)enOperateNull ) return;

	//1.调用客户端结果及操作结果
	if(mEngineSink) mEngineSink->OnHandleResultByStagety(pResult);

	//2.上报
	ReportOper(*pResult);

}

void ProtectorEngine::ReportOper(const ActionOperateResult& pResult)
{
	if (ActionOperateRecord::Instance().NeedReport(pResult))
	{
		//上报（操作系统不信任和厂商不信任）到核心服务器,所有操作到网吧服务器
		ReportManager::Instance().AddReport(pResult);
	}

}