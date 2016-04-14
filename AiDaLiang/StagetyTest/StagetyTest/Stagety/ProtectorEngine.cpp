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

	//初始化策略管理器
	StagetyManager::Instance().BuildAllStagety(this);

	//加载驱动
	if(LoadProtectDriver()==FALSE) 
	{
		LOGEVEN(TEXT("ERROR:保护驱动加载失败\n"));
		return;
	}

	//启动上报线程
	
}

void ProtectorEngine::StopEngine()
{
	StagetyManager::Instance().ResetData();

	SetEvent(mEventBlock);
	CloseHandle(mEventBlock);

	if (mHDllModule)
	{
		mOperMsgFilter(FALSE);
		mOperSysFilter(FALSE);
		mOperPorcessStatus(FALSE);

		FreeLibrary(mHDllModule);
		mHDllModule = NULL;
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
	}
	mOperSysFilter(TRUE);
	mOperMsgFilter(TRUE);

	return TRUE;
}

void ProtectorEngine::ReloadProtectDriver()
{
	if( NULL == mRegCallBackFun) return;

	mRegCallBackFun(NULL);
	mOperMsgFilter(FALSE);
	mOperSysFilter(FALSE);
	mOperPorcessStatus(FALSE);
	Sleep(1000);
	mRegCallBackFun(Sys_Filter_fun);
	mOperPorcessStatus(TRUE);
	mOperMsgFilter(TRUE);
	mOperSysFilter(TRUE);
	LOGEVEN(TEXT("重新加载驱动" ));
}

void ProtectorEngine::OnHandleResultByStagety(ActionOperateResult* pResult)
{
	//放行的不需要客户端干预
	if (pResult->nTrusted==0) return;

	//无操作的表示打印日志
	if (pResult->actionType == enActionNull || pResult->operateType == enOperateNull ) return;

	//1.调用客户端结果及操作结果
	if(mEngineSink) mEngineSink->OnHandleResultByStagety(pResult);

	//2.上报
	ReportOper(*pResult);

}

void ProtectorEngine::ReportOper(const ActionOperateResult& pResult)
{

}