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

	//生成app策略
	mStagetySet[enActionDriver] = new StagetyDriver();

	//生成Company策略
	mStagetySet[enActioProcess] = new StagetyProcess();

	//创建处理结果线程
	mActionResultDelegate = pHandleResultDelegate;
	mHandleResultService.SetOwnMem(false);
	mHandleResultService.SetHandleSink(this);
	mHandleResultService.StartService();

	//开启守护驱动 
	mGuardObject.StartGuard();

	//nThreadProcessResult = JKThread::Start(&StagetyManager::ProcessResultThread,this);
}

bool StagetyManager::HandleAction(UINT type, PVOID lpata, DWORD size_len)
{
	bool bRet = true;
	bool isNeedVerifyStagety = false;	//是否需要验证安全策略
	ProcessInfoStagety* pProcessParent = NULL;	//父进程
	ProcessInfoStagety* pProcessChild = NULL;	//子进程
	std::string szOldDriverPath="";		//驱动原路径
	std::string szNewDriverPath="";		//转换后的驱动路径
	WORD wOperation = enOperateNull;	//操作类型
	WORD wAction = (type==LOAD_DRIVER)?enActionDriver:enActioProcess;//行为类型

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

			_sntprintf(Result.szDescriber,1024,TEXT("加载驱动[路径:%s][新路径:%s]\r\n"),
				A2T((LPSTR)szOldDriverPath.c_str()),A2T((LPSTR)szNewDriverPath.c_str()) );

			SendProcessResult( NotifyStagety::Builder(Result,Result.actionType,Result.operateType) );

			break;
		}
	case CREATE_TERMINATE_PROCESS://创建和终止进程,都不阻止
		{
			PPROCESS_INFO pprocess_info = (PROCESS_INFO*)lpata;
			if(pprocess_info->IsCreate) wOperation = enCreateProcess;
			else wOperation = enTerminateProcess;

			//增加当前进程可以做任何操作
			if( VerifyPrantPidIsOurs(pprocess_info->ParentId) ) break;

			isNeedVerifyStagety = false;
			pProcessParent = GetProcessInfo(pprocess_info->ParentId);
			pProcessChild = GetProcessInfo(pprocess_info->ProcessId,wOperation==enCreateProcess);

			//LOGEVEN(TEXT("CREATE_TERMINATE_PROCESS[%d][%p][%p]"),type,pProcessParent,pProcessChild);

			//后期测试重点测试一下 为什么会空
			if (pProcessParent==NULL || pProcessChild == NULL) break;

			ActionOperateResult Result;
			Result.parantProcess	= *pProcessParent;
			Result.childProcess		= *pProcessChild;
			Result.actionType		= enActionNull ;
			Result.operateType		= enOperateNull;

			_sntprintf(Result.szDescriber,1024,TEXT("[%d:%s]%s[%d%s]\r\n"),
				pprocess_info->ParentId, pProcessParent?A2T((LPSTR)pProcessParent->szProcessPath.c_str()):TEXT(""),
				wOperation==enCreateProcess?TEXT("创建进程"):TEXT("结束进程"),
				pprocess_info->ProcessId, pProcessChild?A2T((LPSTR)pProcessChild->szProcessPath.c_str()):TEXT("")
				);

			
			SendProcessResult(NotifyStagety::Builder(Result,Result.actionType,Result.operateType));


			if(wOperation == enTerminateProcess) RemoveProcessInfo(pprocess_info->ProcessId);

			break;
		}
	case OPER_PROTECTED_PROCESS://尝试操作进程
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
				wOpt |= enTryTerminateProcess;					//尝试结束进程
				_sntprintf(szOptItemDes,128,TEXT("%s"),TEXT("尝试结束进程"));
			}
			if(pprotected_info->oper_type & OPER_WRITE_PROCESS)
			{
				wOpt |= enWritrProcess;							//尝试注入进程
				_sntprintf(szOptItemDes,128,TEXT("%s %s"),szOptItemDes,TEXT("尝试注入进程"));
			}
			if(pprotected_info->oper_type & OPER_TERMINATE_MSG)
			{
				wOpt |= enMessageExit;							//发送消息退出进程
				_sntprintf(szOptItemDes,128,TEXT("%s %s"),szOptItemDes,TEXT("发送消息退出进程"));
			}

			if(wOpt == 0) wOpt = enOperateNull;

			wOperation = wOpt;

			if(mGuardObject.InjectionCheck(pprotected_info->ParentId,pprotected_info->ProcessId)) break;

			//后期测试重点测试一下 为什么会空
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
	case OPER_ERROR://错误|未知操作
		{
			POPER_ERROR_INFO perror_info	= (POPER_ERROR_INFO)lpata;

			USES_CONVERSION;
			LOGEVEN(TEXT("error:错误代码[%d]文件行[%d],%s[%s]"),perror_info->error,perror_info->file_line,A2T(perror_info->error_str),A2T(perror_info->file_name) );

			//JKThread::Start(&StagetyManager::ProcessDriverErrorThread,this);

			break;
		}
	default:
		return true;
	}

	//是否再需要进入策略判断
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

//同步通知结果，影响驱动结果
BOOL StagetyManager::SendProcessResult(NotifyStagety* pNotify)
{
	LOGEVEN(TEXT("SEND\r\n%s"),pNotify->GetActionResult().szDescriber);

	//告知需要处理结果的调用者
	mActionResultDelegate->OnHandleResultByStagety(&pNotify->GetActionResult());
	//记录操作
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

		if (itFind == mProcessMap.end())//保存一个新的进程
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
		OSVERSIONINFO osvi;//定义OSVERSIONINFO数据结构对象
		memset(&osvi, 0, sizeof(OSVERSIONINFO));//开空间 
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);//定义大小 
		GetVersionEx (&osvi);//获得版本信息 

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
		LOGEVEN(TEXT("error:行为结果代理为NULL\n"));
		return;
	}

	NotifyStagety* pNotify = (NotifyStagety*)pData;
	if(pNotify == NULL) 
	{
		LOGEVEN(TEXT("error:pNotify为NULL\n"));
		return;
	}
	
	//告知需要处理结果的调用者
	mActionResultDelegate->OnHandleResultByStagety(&pNotify->GetActionResult());
	//记录操作
	BOOL bReport = ActionOperateRecord::Instance().RecordProcessOperate(pNotify->GetActionResult());

	pNotify->Release();
}



//#pragma endregion