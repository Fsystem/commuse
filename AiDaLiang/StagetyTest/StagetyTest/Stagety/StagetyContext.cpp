#include <StdAfx.h>
#include "StagetyInclude.h"
#include "StagetyContext.h"
#include <atlconv.h>



using std::string;
using std::wstring;

ActionType StagetyProcess::GetActionType()
{
	return mActionType;
}

OperateType StagetyProcess::GetOperateType()
{
	return mOperateType;
}

int StagetyProcess::HandleDriver(LPCSTR pOldPath, LPCSTR pDriverName) //处理驱动 0-信任 其他为不信任
{
	//LOGEVEN(TEXT("驱动还未实现"));
	return 0;
}

//处理进程 0-信任 其他为不信任
int StagetyProcess::HandleProcess(ActionType action,OperateType optType, ProcessInfoStagety* parant, ProcessInfoStagety* child)
{
	int nRet = 0;
	mActionType = action;
	mOperateType = optType;

	TCHAR szSerial[1024]={0};			//序列号
	TCHAR szIssUser[1024]={0};			//颁发者
	TCHAR szSigner[1024]={0};			//签名者

	TCHAR szChildSerial[1024]={0};		//序列号
	TCHAR szChildIssUser[1024]={0};		//颁发者
	TCHAR szChildSigner[1024]={0};		//签名者	

	USES_CONVERSION;

	//LOGEVEN(TEXT("StagetyProcess::HandleProcess begin\n"));
	/************************************************************************/
	/*! 已验证过的进程 */
	/************************************************************************/
	//1.父进程可信，放行
	if(VerifyTrust(parant->cbVerifyResult) && child->cbVerifyResult != enTrustNull) {nRet = 0; goto PROCESS_RESULT;}

	//2.子进程可信且父进程已经验证过，阻止
	if(parant->cbVerifyResult != enTrustNull && VerifyTrust(child->cbVerifyResult)) {nRet = 1; goto PROCESS_RESULT;}

	/************************************************************************/
	/*! 未验证过的进程,填充必要信息 */
	/************************************************************************/
	get_sign_info(parant->szProcessPath.c_str(),szSerial,szIssUser,szSigner);
	parant->szSign = string(T2A(szSigner));
	
	get_sign_info(child->szProcessPath.c_str(),szChildSerial,szChildIssUser,szChildSigner);
	child->szSign = string(T2A(szChildSigner));

	//获取MD5
	parant->szMd5=GetFileMd5(parant->szProcessPath.c_str());
	child->szMd5=GetFileMd5(child->szProcessPath.c_str());

	//获取CRC
	parant->unCrc = GetFileCrc(parant->szProcessPath.c_str());
	child->unCrc = GetFileCrc(child->szProcessPath.c_str());
	
	//-------------------------------------------------------------------------------
	//获取父进程验证信息
	if(parant->cbVerifyResult == enTrustNull) VerifyProcess(parant);
	
	//-------------------------------------------------------------------------------
	//获取子进程验证信息
	if(child->cbVerifyResult == enTrustNull) VerifyProcess(child);

	if ( VerifyTrust(parant->cbVerifyResult) ) nRet = 0;
	else if ( VerifyTrust(child->cbVerifyResult) ) nRet = 1;
	else nRet = 0;
	
	//-------------------------------------------------------------------------------
PROCESS_RESULT:
	ActionOperateResult Result;
	Result.actionType = action;
	Result.operateType = optType;
	Result.parantProcess = *parant;
	Result.childProcess = *child;
	Result.nTrusted = nRet;

	//打印基础信息
	_sntprintf(Result.szDescriber,1024,
		TEXT("\t┗父进程[PID:%u]-%s\r\n\t\t┗md5:[%s]\r\n\t\t┗CRC:[%u]\r\n\t\t┗公司签名:[%s]\r\n\t\t┗序列号:[%s]\r\n\t\t┗颁发者:[%s]\r\n\t\t┗可信:[%s]\r\n")
		TEXT("\t┗子进程[PID:%u]-%s\r\n\t\t┗md5:[%s]\r\n\t\t┗CRC:[%u]\r\n\t\t┗公司签名:[%s]\r\n\t\t┗序列号:[%s]\r\n\t\t┗颁发者:[%s]\r\n\t\t┗可信:[%s]\r\n")
		TEXT("\t┗行为结果:[%s]\r\n"),
		parant->dwPid,A2T((LPSTR)parant->szProcessPath.c_str()) , A2T((LPSTR)parant->szMd5.c_str()),
		parant->unCrc,A2T((LPSTR)parant->szSign.c_str()),szSerial,szIssUser,
		gszTrustDescrier[parant->cbVerifyResult],
		child->dwPid,A2T((LPSTR)child->szProcessPath.c_str()) , A2T((LPSTR)child->szMd5.c_str()),
		child->unCrc,A2T((LPSTR)child->szSign.c_str()),szSerial,szIssUser,
		gszTrustDescrier[child->cbVerifyResult],
		nRet==0?TEXT("放行"):TEXT("阻止")
		);
	
	StagetyManager::Instance().SendProcessResult(NotifyStagety::Builder(Result,Result.actionType,Result.operateType));	
	//LOGEVEN(TEXT("StagetyProcess::HandleProcess end\n"));

	//-------------------------------------------------------------------------------
	//如果都不信任，放行
	nRet = (Result.nTrusted == 0 || Result.nTrusted == 2)?0:1;
	return nRet;
}

//-------------------------------------------------------------------------------
// 驱动策略
//-------------------------------------------------------------------------------
ActionType StagetyDriver::GetActionType()
{
	return mActionType;
}

OperateType StagetyDriver::GetOperateType()
{
	return mOperateType;
}

int StagetyDriver::HandleDriver(LPCSTR pOldPath, LPCSTR pDriverName) //处理驱动 0-信任 其他为不信任
{
	//LOGEVEN(TEXT("驱动还未实现"));
	return 0;
}

int StagetyDriver::HandleProcess(ActionType action,OperateType optType, ProcessInfoStagety* parant, ProcessInfoStagety* child)//处理进程 0-信任 其他为不信任
{
	mActionType = action;
	mOperateType = optType;

	return 0;
}