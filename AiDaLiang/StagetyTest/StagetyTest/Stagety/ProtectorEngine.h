/** ��������
*   @FileName  : ProtectorEngine.h
*   @Author    : Double Sword(doublesword.jack@gmail.com)
*   @date      : 2016-4-13
*   @Copyright : Copyright belong to Author and 9u2,CO.LTD
*   @RefDoc    : --
*/

#ifndef __ProtectorEngine_H
#define __ProtectorEngine_H

#include <map>

#define PROTECTOR_ENGINE (ProtectorEngine::Instance())

using std::map;

class ProtectorEngine : public IActionResultDelegate
{
	typedef BOOL (WINAPI *SYS_RegCallBackFun)(CALLBACK_FILTERFUN fun);			//ע��ص�
	typedef BOOL (WINAPI *SYS_OperSysFilter)(BOOL start);						//���أ�ֹͣ����
	typedef BOOL (WINAPI *SYS_OperMsgFilter)(BOOL start);						//������Ϣ���˱���
	typedef BOOL (WINAPI *SYS_OperPorcessStatus)(BOOL start);					//ֹͣ����������������ģ��

public:
	static ProtectorEngine& Instance(){static ProtectorEngine e;return e;};

public:
	void StartEngine();
	void StopEngine();

protected:
	ProtectorEngine();

	//ʵ�ֽӿ�
protected:
	virtual BOOL OnHandleResultByStagety(const ActionOperateResult* pResult, BOOL bReport);

protected:
	BOOL LoadProtectDriver();
	void ReportOper(const ActionOperateResult& pResult);

protected:
	HMODULE mHDllModule;
	TCHAR mModulePath[MAX_PATH];
	//������������
	SYS_RegCallBackFun		mRegCallBackFun;
	SYS_OperSysFilter		mOperSysFilter;
	SYS_OperMsgFilter		mOperMsgFilter;
	SYS_OperPorcessStatus   mOperPorcessStatus;
};
#endif //__ProtectorEngine_H