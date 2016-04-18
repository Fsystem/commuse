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

	//�ϱ���������Ϣ
	//ReportManager::Instance().SetWorkServerInfo(netserver_ip.c_str(),netserver_port);
	//ReportManager::Instance().SetKernelServerInfo(coreserver_ip.c_str(),coreserver_port_second);
	ReportManager::Instance().StartService();

	//��ʼ�����Թ�����
	StagetyManager::Instance().BuildAllStagety(this);

	//��������
	if(LoadProtectDriver()==FALSE) 
	{
		LOGEVEN(TEXT("ERROR:������������ʧ��\n"));
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
		LOGEVEN(TEXT("ע��ص�ʧ��\n"));
		FreeLibrary(mHDllModule);
		mHDllModule = NULL;
		return FALSE;
	}

	mOperMsgFilter(TRUE);
	mOperSysFilter(TRUE);

	LOGEVEN(TEXT(">>���������ɹ�\n" ));

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

		LOGEVEN(TEXT(">>���¼�������\n" ));
	}
	catch(...)
	{
		LOGEVEN(TEXT(">>���¼��������쳣[%p],[%p],[%p],[%p]\n" ),
			mRegCallBackFun,mOperMsgFilter,mOperSysFilter,mOperPorcessStatus);
	}

}

void ProtectorEngine::OnHandleResultByStagety(ActionOperateResult* pResult)
{
	//�޲����ı�ʾ��ӡ��־
	//if (pResult->actionType == (WORD)enActionNull || pResult->operateType == (WORD)enOperateNull ) return;

	//1.���ÿͻ��˽�����������
	if(mEngineSink) mEngineSink->OnHandleResultByStagety(pResult);

	//2.�ϱ�
	ReportOper(*pResult);

}

void ProtectorEngine::ReportOper(const ActionOperateResult& pResult)
{
	if (ActionOperateRecord::Instance().NeedReport(pResult))
	{
		//�ϱ�������ϵͳ�����κͳ��̲����Σ������ķ�����,���в��������ɷ�����
		ReportManager::Instance().AddReport(pResult);
	}

}