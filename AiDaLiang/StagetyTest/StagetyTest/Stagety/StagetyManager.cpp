#include <StdAfx.h>
#include "StagetyInclude.h"
#include "StagetyManager.h"

#include <atlconv.h>

//-------------------------------------------------------------------------------

StagetyManager::StagetyManager()
{
	mCurProcessId = ::GetCurrentProcessId();
	memset(mStagetySet,0,sizeof mStagetySet);
	LPCTSTR szCmdLine = ::GetCommandLine();
	_tscanf(szCmdLine, TEXT("%s %d %d"), szNetServerIp, &mSuspensionWindowPid, &mClientPid);

	mActionResultDelegate = NULL;
}

StagetyManager::~StagetyManager()
{
	ResetData();

	StagetyProcessMap::iterator it = mProcessMap.begin();
	while(it != mProcessMap.end())
	{
		if (it->second)
		{
			delete it->second;
			it->second = NULL;
		}

		it++;
	}

	mProcessMap.clear();
}

void StagetyManager::ResetData()
{
	for(int i = 0; i<enActionTypeMax; ++i)
	{
		if (mStagetySet[i])
		{
			delete mStagetySet[i];
			mStagetySet[i] = NULL;
		}
	}

	mGuardObject.StopGuard();

	mActionResultDelegate = NULL;
	mHandleResultService.SetHandleSink(NULL);
	mHandleResultService.StopService();
}

void StagetyManager::BuildAllStagety(IActionResultDelegate* pHandleResultDelegate)
{
	ResetData();

	//����app����
	mStagetySet[enActionDriver] = new StagetyDriver();

	//����Company����
	mStagetySet[enActioProcess] = new StagetyProcess();

	//�����������߳�
	mActionResultDelegate = pHandleResultDelegate;
	mHandleResultService.SetOwnMem(false);
	mHandleResultService.SetHandleSink(this);
	mHandleResultService.StartService();

	//�����ػ����� 
	mGuardObject.StartGuard();

	//nThreadProcessResult = JKThread::Start(&StagetyManager::ProcessResultThread,this);
}

bool StagetyManager::HandleAction(UINT type, PVOID lpata, DWORD size_len)
{
	bool bRet = true;
	bool isNeedVerifyStagety = false;	//�Ƿ���Ҫ��֤��ȫ����
	ProcessInfoStagety* pProcessParent = NULL;	//������
	ProcessInfoStagety* pProcessChild = NULL;	//�ӽ���
	std::string szOldDriverPath="";		//����ԭ·��
	std::string szNewDriverPath="";		//ת���������·��
	WORD wOperation = enOperateNull;	//��������
	WORD wAction = (type==LOAD_DRIVER)?enActionDriver:enActioProcess;//��Ϊ����

	mGuardObject.IntoDriver();

	USES_CONVERSION;

	switch(type)
	{
	case LOAD_DRIVER:
		{
			//ASSERT(size_len == sizeof(PDRIVER_INFO));
			//if(size_len != sizeof(PDRIVER_INFO)) break;
			PDRIVER_INFO pdrver_info = (PDRIVER_INFO)lpata;
			szOldDriverPath = pdrver_info->driver_path;
			szNewDriverPath = change_dirver_path(pdrver_info->driver_path);
			isNeedVerifyStagety = true;

			wAction = enActionDriver;
			wOperation = enLoadDriver;

			
			ActionOperateResult Result;
			Result.parantProcess.szProcessPath = szNewDriverPath;
			Result.actionType	= enActionNull ;
			Result.operateType	= enOperateNull;

			_sntprintf(Result.szDescriber,1024,TEXT("��������[·��:%s][��·��:%s]\r\n"),
				A2T((LPSTR)szOldDriverPath.c_str()),A2T((LPSTR)szNewDriverPath.c_str()) );

			SendProcessResult( NotifyStagety::Builder(Result,Result.actionType,Result.operateType) );

			break;
		}
	case CREATE_TERMINATE_PROCESS://��������ֹ����,������ֹ
		{
			PPROCESS_INFO pprocess_info = (PROCESS_INFO*)lpata;
			if(pprocess_info->IsCreate) wOperation = enCreateProcess;
			else wOperation = enTerminateProcess;

			//���ӵ�ǰ���̿������κβ���
			if( VerifyPrantPidIsOurs(pprocess_info->ParentId) ) break;

			isNeedVerifyStagety = false;
			pProcessParent = GetProcessInfo(pprocess_info->ParentId);
			pProcessChild = GetProcessInfo(pprocess_info->ProcessId,wOperation==enCreateProcess);

			//LOGEVEN(TEXT("CREATE_TERMINATE_PROCESS[%d][%p][%p]"),type,pProcessParent,pProcessChild);

			//���ڲ����ص����һ�� Ϊʲô���
			if (pProcessParent==NULL || pProcessChild == NULL) break;

			ActionOperateResult Result;
			Result.parantProcess	= *pProcessParent;
			Result.childProcess		= *pProcessChild;
			Result.actionType		= enActionNull ;
			Result.operateType		= enOperateNull;

			_sntprintf(Result.szDescriber,1024,TEXT("[%d:%s]%s[%d%s]\r\n"),
				pprocess_info->ParentId, pProcessParent?A2T((LPSTR)pProcessParent->szProcessPath.c_str()):TEXT(""),
				wOperation==enCreateProcess?TEXT("��������"):TEXT("��������"),
				pprocess_info->ProcessId, pProcessChild?A2T((LPSTR)pProcessChild->szProcessPath.c_str()):TEXT("")
				);

			
			SendProcessResult(NotifyStagety::Builder(Result,Result.actionType,Result.operateType));


			if(wOperation == enTerminateProcess) RemoveProcessInfo(pprocess_info->ProcessId);

			break;
		}
	case OPER_PROTECTED_PROCESS://���Բ�������
		{
			PPROTECTED_PROCESS_INFO pprotected_info	= (PPROTECTED_PROCESS_INFO)lpata;
			if(VerifyPrantPidIsOurs(pprotected_info->ParentId)) break;

			isNeedVerifyStagety = true;
			pProcessParent = GetProcessInfo(pprotected_info->ParentId);
			pProcessChild = GetProcessInfo(pprotected_info->ProcessId);

			WORD wOpt = 0;
			TCHAR szOptItemDes[128]={0};
			if(pprotected_info->oper_type & OPER_TERMINATE_PROCESS)
			{
				wOpt |= enTryTerminateProcess;					//���Խ�������
				_sntprintf(szOptItemDes,128,TEXT("%s"),TEXT("���Խ�������"));
			}
			if(pprotected_info->oper_type & OPER_WRITE_PROCESS)
			{
				wOpt |= enWritrProcess;							//����ע�����
				_sntprintf(szOptItemDes,128,TEXT("%s %s"),szOptItemDes,TEXT("����ע�����"));
			}
			if(pprotected_info->oper_type & OPER_TERMINATE_MSG)
			{
				wOpt |= enMessageExit;							//������Ϣ�˳�����
				_sntprintf(szOptItemDes,128,TEXT("%s %s"),szOptItemDes,TEXT("������Ϣ�˳�����"));
			}

			if(wOpt == 0) wOpt = enOperateNull;

			wOperation = wOpt;

			if(mGuardObject.InjectionCheck(pprotected_info->ParentId,pprotected_info->ProcessId)) break;

			//���ڲ����ص����һ�� Ϊʲô���
			if (pProcessParent==NULL || pProcessChild == NULL) {isNeedVerifyStagety = false;break;}

			ActionOperateResult Result;
			Result.parantProcess	= *pProcessParent;
			Result.childProcess		= *pProcessChild;
			Result.actionType		= enActionNull ;
			Result.operateType		= enOperateNull;

			_sntprintf(Result.szDescriber,1024,TEXT("[%d:%s]%s[%d%s]\r\n"),
				pprotected_info->ParentId, pProcessParent?A2T((LPSTR)pProcessParent->szProcessPath.c_str()):TEXT(""),
				szOptItemDes,
				pprotected_info->ProcessId, pProcessChild?A2T((LPSTR)pProcessChild->szProcessPath.c_str()):TEXT("")
				);
			
			SendProcessResult(NotifyStagety::Builder(Result,Result.actionType,Result.operateType));
			
			break;
		}
	case OPER_ERROR://����|δ֪����
		{
			POPER_ERROR_INFO perror_info	= (POPER_ERROR_INFO)lpata;

			USES_CONVERSION;
			LOGEVEN(TEXT("error:�������[%d]�ļ���[%d],%s[%s]"),perror_info->error,perror_info->file_line,A2T(perror_info->error_str),A2T(perror_info->file_name) );

			//JKThread::Start(&StagetyManager::ProcessDriverErrorThread,this);

			break;
		}
	default:
		return true;
	}

	//�Ƿ�����Ҫ��������ж�
	if ( isNeedVerifyStagety )
	{
		for(int i = 0;i < enActionTypeMax; ++i)
		{
			if (wAction == enActionDriver)
			{
				bRet &= (mStagetySet[i]->HandleDriver(szOldDriverPath.c_str(),szNewDriverPath.c_str())==0);
			}
			else
			{
				bRet &= (mStagetySet[i]->HandleProcess((ActionType)wAction,(OperateType)wOperation,pProcessParent,pProcessChild)==0);
			}
			
			if(!bRet) break;
		}
	}

	ActionOperateResult Result;
	Result.actionType = enActionNull;
	Result.operateType = enOperateNull;
	_sntprintf(Result.szDescriber,1024,TEXT("=================================================\r\n"));

	SendProcessResult(NotifyStagety::Builder(Result,Result.actionType,Result.operateType));

	mGuardObject.OutDriver();

	return bRet;
}


bool StagetyManager::VerifyPrantPidIsOurs(DWORD dwParantPid)
{
	return ( dwParantPid==mCurProcessId || dwParantPid == mClientPid || dwParantPid == mSuspensionWindowPid );
}

void StagetyManager::ProcessDriverErrorThread(void * pThis)
{
	PROTECTOR_ENGINE.ReloadProtectDriver();
}

void StagetyManager::PostProcessResult(NotifyStagety* pNotify)
{
	LOGEVEN(TEXT("POST\r\n%s"),pNotify->GetActionResult().szDescriber);

	mHandleResultService.AddData(pNotify,sizeof(NotifyStagety));
}

//ͬ��֪ͨ�����Ӱ���������
BOOL StagetyManager::SendProcessResult(NotifyStagety* pNotify)
{
	LOGEVEN(TEXT("SEND\r\n%s"),pNotify->GetActionResult().szDescriber);

	//��֪��Ҫ�������ĵ�����
	mActionResultDelegate->OnHandleResultByStagety(&pNotify->GetActionResult());
	//��¼����
	ActionOperateRecord::Instance().RecordProcessOperate(pNotify->GetActionResult());

	pNotify->Release();

	return TRUE;
}

ProcessInfoStagety* StagetyManager::GetProcessInfo(DWORD dwProcessId, bool bCreate)
{
	ProcessInfoStagety* processInfo = NULL;
	
	if(0 < dwProcessId)
	{
		StagetyProcessMap::iterator itFind = mProcessMap.find(dwProcessId);

		if (itFind == mProcessMap.end())//����һ���µĽ���
		{
			std::string szProcessName = GetProcessNameByPid(dwProcessId);
			if (!szProcessName.empty())
			{
				processInfo = new ProcessInfoStagety();
				processInfo->szProcessPath = szProcessName;
				processInfo->dwPid = dwProcessId;
				processInfo->cbVerifyResult = enTrustNull;
				processInfo->szMd5="";
				processInfo->szSign="";
				processInfo->unCrc = 0;

				mProcessMap[dwProcessId]=processInfo;
			}
			else
			{
				LOGEVEN(TEXT("error:get process(%u) name fail!"),dwProcessId);
			}
		}
		else
		{
			processInfo = itFind->second;

			if(bCreate)
			{
				std::string szProcessName = GetProcessNameByPid(dwProcessId);
				if (!szProcessName.empty() )
				{
					if (stricmp(szProcessName.c_str(),processInfo->szProcessPath.c_str()) != 0)
					{
						processInfo->szProcessPath = szProcessName;
						processInfo->cbVerifyResult = enTrustNull;
						processInfo->szMd5="";
						processInfo->szSign="";
						processInfo->unCrc = 0;
					}
				}
				else
				{
					LOGEVEN(TEXT("create : get process(%u) name fail!"),dwProcessId);
				}
			}

		}
		//end if (processInfo == NULL)
	}

	return processInfo;
}

void StagetyManager::RemoveProcessInfo(DWORD dwProcessId)
{
	if(0 < dwProcessId)
	{
		StagetyProcessMap::iterator itFind = mProcessMap.find(dwProcessId);
		if(itFind != mProcessMap.end())
		{
			delete itFind->second;
			mProcessMap.erase(dwProcessId);
		}
		
	}
}

std::string StagetyManager::GetProcessNameByPid(DWORD dwPid)
{
	std::string szProcessName = "";
	if(0 == dwPid) return szProcessName;

	HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPid);

	if (hProcess)
	{
		OSVERSIONINFO osvi;//����OSVERSIONINFO���ݽṹ����
		memset(&osvi, 0, sizeof(OSVERSIONINFO));//���ռ� 
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);//�����С 
		GetVersionEx (&osvi);//��ð汾��Ϣ 

		if (osvi.dwMajorVersion >= 6)//os vista above
		{
			szProcessName = GetWin7ProcessName(hProcess);
		}
		else
		{
			szProcessName = GetXPProcessName(hProcess);
		}

		CloseHandle(hProcess);
	}

	return szProcessName;
}

void StagetyManager::OnHandleQueueData(LPVOID pData,UINT uDataSize)
{
	if(mActionResultDelegate==NULL) 
	{
		LOGEVEN(TEXT("error:��Ϊ�������ΪNULL\n"));
		return;
	}

	NotifyStagety* pNotify = (NotifyStagety*)pData;
	if(pNotify == NULL) 
	{
		LOGEVEN(TEXT("error:pNotifyΪNULL\n"));
		return;
	}
	
	//��֪��Ҫ�������ĵ�����
	mActionResultDelegate->OnHandleResultByStagety(&pNotify->GetActionResult());
	//��¼����
	BOOL bReport = ActionOperateRecord::Instance().RecordProcessOperate(pNotify->GetActionResult());

	pNotify->Release();
}



//#pragma endregion