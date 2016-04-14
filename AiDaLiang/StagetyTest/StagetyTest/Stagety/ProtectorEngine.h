/** 保护引擎
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

#define STAGETY_NOTIFY_PROCESS_RESULT	WM_USER+1212

using std::map;

class ProtectorEngine : public IActionResultDelegate
{
	typedef BOOL (WINAPI *SYS_RegCallBackFun)(CALLBACK_FILTERFUN fun);			//注册回调
	typedef BOOL (WINAPI *SYS_OperSysFilter)(BOOL start);						//加载，停止驱动
	typedef BOOL (WINAPI *SYS_OperMsgFilter)(BOOL start);						//加载消息过滤保护
	typedef BOOL (WINAPI *SYS_OperPorcessStatus)(BOOL start);					//停止或者启动保护进程模块

public:
	static ProtectorEngine& Instance(){static ProtectorEngine e;return e;};

public:
	void StartEngine(IActionResultDelegate* pEngineSink);
	void StopEngine();
	void Block();
	void UnBlock();

	void ReloadProtectDriver();

protected:
	ProtectorEngine();

	//实现接口
protected:
	virtual void OnHandleResultByStagety(ActionOperateResult* pResult);

protected:
	BOOL LoadProtectDriver();
	void ReportOper(const ActionOperateResult& pResult);

protected:
	HMODULE mHDllModule;
	TCHAR mModulePath[MAX_PATH];
	//驱动导出函数
	SYS_RegCallBackFun		mRegCallBackFun;
	SYS_OperSysFilter		mOperSysFilter;
	SYS_OperMsgFilter		mOperMsgFilter;
	SYS_OperPorcessStatus   mOperPorcessStatus;
	//消息
	HANDLE					mEventBlock;
	IActionResultDelegate*	mEngineSink;
};
#endif //__ProtectorEngine_H