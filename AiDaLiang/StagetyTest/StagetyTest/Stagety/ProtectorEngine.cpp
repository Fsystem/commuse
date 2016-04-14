#include <stdafx.h>
#include "ProtectorEngine.h"

#include "StagetyInclude.h"

#include "../resource.h"

//-------------------------------------------------------------------------------

//�����ص�
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
}

void ProtectorEngine::StartEngine()
{
	//��ʼ�����Թ�����
	StagetyManager::Instance().BuildAllStagety(this);

	//��������
	if(LoadProtectDriver()==FALSE) 
	{
		LOGEVEN(TEXT("ERROR:������������ʧ��\n"));
		return;
	}

	//�����ϱ��߳�
	
}

void ProtectorEngine::StopEngine()
{
	StagetyManager::Instance().ResetData();

	if (mHDllModule)
	{
		mOperMsgFilter(FALSE);
		mOperSysFilter(FALSE);
		mOperPorcessStatus(FALSE);

		FreeLibrary(mHDllModule);
		mHDllModule = NULL;
	}
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
		LOGEVEN(TEXT("ע��ص�ʧ��\n"));
	}
	mOperSysFilter(TRUE);
	mOperMsgFilter(TRUE);

	return TRUE;
}

BOOL ProtectorEngine::OnHandleResultByStagety(ActionOperateResult* pResult)
{
	ReportOper(*pResult);

	return TRUE;
}

void ProtectorEngine::ReportOper(const ActionOperateResult& pResult)
{

}