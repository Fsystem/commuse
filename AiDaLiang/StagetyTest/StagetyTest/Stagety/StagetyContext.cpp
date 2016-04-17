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

int StagetyProcess::HandleDriver(LPCSTR pOldPath, LPCSTR pDriverName) //�������� 0-���� ����Ϊ������
{
	//LOGEVEN(TEXT("������δʵ��"));
	return 0;
}

//������� 0-���� ����Ϊ������
int StagetyProcess::HandleProcess(ActionType action,OperateType optType, ProcessInfoStagety* parant, ProcessInfoStagety* child)
{
	int nRet = 0;
	mActionType = action;
	mOperateType = optType;

	TCHAR szSerial[1024]={0};			//���к�
	TCHAR szIssUser[1024]={0};			//�䷢��
	TCHAR szSigner[1024]={0};			//ǩ����

	TCHAR szChildSerial[1024]={0};		//���к�
	TCHAR szChildIssUser[1024]={0};		//�䷢��
	TCHAR szChildSigner[1024]={0};		//ǩ����	

	USES_CONVERSION;

	//LOGEVEN(TEXT("StagetyProcess::HandleProcess begin\n"));
	/************************************************************************/
	/*! ����֤���Ľ��� */
	/************************************************************************/
	//1.�����̿��ţ�����
	if(VerifyTrust(parant->cbVerifyResult) && child->cbVerifyResult != enTrustNull) {nRet = 0; goto PROCESS_RESULT;}

	//2.�ӽ��̿����Ҹ������Ѿ���֤������ֹ
	if(parant->cbVerifyResult != enTrustNull && VerifyTrust(child->cbVerifyResult)) {nRet = 1; goto PROCESS_RESULT;}

	/************************************************************************/
	/*! δ��֤���Ľ���,����Ҫ��Ϣ */
	/************************************************************************/
	get_sign_info(parant->szProcessPath.c_str(),szSerial,szIssUser,szSigner);
	parant->szSign = string(T2A(szSigner));
	
	get_sign_info(child->szProcessPath.c_str(),szChildSerial,szChildIssUser,szChildSigner);
	child->szSign = string(T2A(szChildSigner));

	//��ȡMD5
	parant->szMd5=GetFileMd5(parant->szProcessPath.c_str());
	child->szMd5=GetFileMd5(child->szProcessPath.c_str());

	//��ȡCRC
	parant->unCrc = GetFileCrc(parant->szProcessPath.c_str());
	child->unCrc = GetFileCrc(child->szProcessPath.c_str());
	
	//-------------------------------------------------------------------------------
	//��ȡ��������֤��Ϣ
	if(parant->cbVerifyResult == enTrustNull) VerifyProcess(parant);
	
	//-------------------------------------------------------------------------------
	//��ȡ�ӽ�����֤��Ϣ
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

	//��ӡ������Ϣ
	_sntprintf(Result.szDescriber,1024,
		TEXT("\t��������[PID:%u]-%s\r\n\t\t��md5:[%s]\r\n\t\t��CRC:[%u]\r\n\t\t����˾ǩ��:[%s]\r\n\t\t�����к�:[%s]\r\n\t\t���䷢��:[%s]\r\n\t\t������:[%s]\r\n")
		TEXT("\t���ӽ���[PID:%u]-%s\r\n\t\t��md5:[%s]\r\n\t\t��CRC:[%u]\r\n\t\t����˾ǩ��:[%s]\r\n\t\t�����к�:[%s]\r\n\t\t���䷢��:[%s]\r\n\t\t������:[%s]\r\n")
		TEXT("\t����Ϊ���:[%s]\r\n"),
		parant->dwPid,A2T((LPSTR)parant->szProcessPath.c_str()) , A2T((LPSTR)parant->szMd5.c_str()),
		parant->unCrc,A2T((LPSTR)parant->szSign.c_str()),szSerial,szIssUser,
		gszTrustDescrier[parant->cbVerifyResult],
		child->dwPid,A2T((LPSTR)child->szProcessPath.c_str()) , A2T((LPSTR)child->szMd5.c_str()),
		child->unCrc,A2T((LPSTR)child->szSign.c_str()),szSerial,szIssUser,
		gszTrustDescrier[child->cbVerifyResult],
		nRet==0?TEXT("����"):TEXT("��ֹ")
		);
	
	StagetyManager::Instance().SendProcessResult(NotifyStagety::Builder(Result,Result.actionType,Result.operateType));	
	//LOGEVEN(TEXT("StagetyProcess::HandleProcess end\n"));

	//-------------------------------------------------------------------------------
	//����������Σ�����
	nRet = (Result.nTrusted == 0 || Result.nTrusted == 2)?0:1;
	return nRet;
}

//-------------------------------------------------------------------------------
// ��������
//-------------------------------------------------------------------------------
ActionType StagetyDriver::GetActionType()
{
	return mActionType;
}

OperateType StagetyDriver::GetOperateType()
{
	return mOperateType;
}

int StagetyDriver::HandleDriver(LPCSTR pOldPath, LPCSTR pDriverName) //�������� 0-���� ����Ϊ������
{
	//LOGEVEN(TEXT("������δʵ��"));
	return 0;
}

int StagetyDriver::HandleProcess(ActionType action,OperateType optType, ProcessInfoStagety* parant, ProcessInfoStagety* child)//������� 0-���� ����Ϊ������
{
	mActionType = action;
	mOperateType = optType;

	return 0;
}