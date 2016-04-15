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
	VerifyProcess(parant);
	
	//-------------------------------------------------------------------------------
	//��ȡ�ӽ�����֤��Ϣ
	VerifyProcess(child);

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
	TCHAR szDes[1024] = {0};

	//������
	_sntprintf(szDes,1024,TEXT("  ��������[PID:%u]-%s"), parant->dwPid,A2T((LPSTR)parant->szProcessPath.c_str()) );
	_sntprintf(szDes,1024,TEXT("%s\r\n    ��md5:[%s]"),szDes, A2T((LPSTR)parant->szMd5.c_str()) );
	_sntprintf(szDes,1024,TEXT("%s\r\n    ��CRC:[%u]"),szDes, parant->unCrc );
	_sntprintf(szDes,1024,TEXT("%s\r\n    ����˾ǩ��:[%s]"),szDes, A2T((LPSTR)parant->szSign.c_str()) );
	_sntprintf(szDes,1024,TEXT("%s\r\n    �����к�:[%s]"),szDes, szSerial);
	_sntprintf(szDes,1024,TEXT("%s\r\n    ���䷢��:[%s]"),szDes, szIssUser);
	_sntprintf(szDes,1024,TEXT("%s\r\n    ������:[%s]"),szDes,gszTrustDescrier[parant->cbVerifyResult]);
	//�ӽ���
	_sntprintf(szDes,1024,TEXT("%s\r\n  ���ӽ���[PID:%u]-%s"),szDes, child->dwPid, A2T((LPSTR)child->szProcessPath.c_str()) );
	_sntprintf(szDes,1024,TEXT("%s\r\n    ��md5:[%s]"),szDes, A2T((LPSTR)child->szMd5.c_str()) );
	_sntprintf(szDes,1024,TEXT("%s\r\n    ��CRC:[%u]"),szDes, child->unCrc );
	_sntprintf(szDes,1024,TEXT("%s\r\n    ����˾ǩ��:[%s]"),szDes, A2T((LPSTR)child->szSign.c_str()) );
	_sntprintf(szDes,1024,TEXT("%s\r\n    �����к�:[%s]"),szDes,szChildSerial);
	_sntprintf(szDes,1024,TEXT("%s\r\n    ���䷢��:[%s]"),szDes,szChildIssUser);
	_sntprintf(szDes,1024,TEXT("%s\r\n    ������:[%s]"),szDes,gszTrustDescrier[child->cbVerifyResult]);
	_sntprintf(szDes,1024,TEXT("%s\r\n  ����Ϊ���:[%s]\r\n"),szDes,nRet==0?TEXT("����"):TEXT("��ֹ"));

	_tcsncpy(Result.szDescriber,szDes,1024);
	
	StagetyManager::Instance().SendProcessResult(NotifyStagety::Builder(Result,Result.actionType,Result.operateType));	
	//LOGEVEN(TEXT("StagetyProcess::HandleProcess end\n"));

	//-------------------------------------------------------------------------------
	//����������Σ�����
	nRet = (Result.nTrusted == 0 || Result.nTrusted == 2)?0:1;
	return nRet;
}

void StagetyProcess::VerifyThread(void* p)
{
	//-------------------------------------------------------------------------------
	//��ȡ��������֤��Ϣ
	VerifyProcess(mParant);

	//-------------------------------------------------------------------------------
	//��ȡ�ӽ�����֤��Ϣ
	VerifyProcess(mChild);
}

void StagetyProcess::StartVerify(ProcessInfoStagety* parant, ProcessInfoStagety* child)
{
	mParant = parant;
	mChild = child;

	JKThread::Start(&StagetyProcess::VerifyThread,this);
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