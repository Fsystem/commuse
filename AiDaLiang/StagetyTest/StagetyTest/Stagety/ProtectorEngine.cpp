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
	mEngineSink = NULL;
	
}

void ProtectorEngine::StartEngine(IActionResultDelegate* pEngineSink)
{
	mEngineSink = pEngineSink;

	mEventBlock = CreateEvent(NULL,FALSE,FALSE,NULL);
	ResetEvent(mEventBlock);

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
		LOGEVEN(TEXT("ע��ص�ʧ��\n"));
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
	LOGEVEN(TEXT("���¼�������" ));
}

void ProtectorEngine::OnHandleResultByStagety(ActionOperateResult* pResult)
{
	//���еĲ���Ҫ�ͻ��˸�Ԥ
	if (pResult->nTrusted==0) return;

	//�޲����ı�ʾ��ӡ��־
	if (pResult->actionType == enActionNull || pResult->operateType == enOperateNull ) return;

	//1.���ÿͻ��˽�����������
	if(mEngineSink) mEngineSink->OnHandleResultByStagety(pResult);

	//2.�ϱ�
	ReportOper(*pResult);

}

void ProtectorEngine::ReportOper(const ActionOperateResult& pResult)
{

}