/** 策略管理类，采用builder模式
*   @FileName  : StagetyManager.h
*   @Author    : Double Sword(doublesword.jack@gmail.com)
*   @date      : 2016-4-7
*   @Copyright : Copyright belong to Author and 9u2,CO.LTD
*   @RefDoc    : --
*/

#ifndef __StagetyManager_H
#define __StagetyManager_H
#include <map>
#include "QueueService/QueueService.h"

/** 管理和处理行为o
*   @ClassName	: 
*   @Purpose	:
*		管理现有的策略数据，以及安排相应的行为处理
*   @Author	: Double Sword
*   @Data	: 2016-4-7
*/

using std::map;
class StagetyManager:public IQueueServiceSink
{
	/*! 策略保存的进程信息 */
	typedef map<DWORD,ProcessInfoStagety*> StagetyProcessMap;
public:
	~StagetyManager();
	static StagetyManager& Instance(){static StagetyManager s;return s;};

public:
	/** 创建策略
	*   @Author   : Double sword
	*   @Params   : none	
			
	*   @Return   :	none			
	*   @Date     :	2016-4-7
	*/
	void BuildAllStagety(IActionResultDelegate* pHandleResultDelegate);

	void ResetData();

	bool HandleAction(UINT type, PVOID lpata, DWORD size_len);

	//异步通知结果，不影响驱动的结果
	void PostProcessResult(NotifyStagety* pNotify);

	//同步通知结果，影响驱动结果
	BOOL SendProcessResult(NotifyStagety* pNotify);

private:
	StagetyManager();

	/** 验证父进程是否是我们官方的进程ID,如果是放行，反之则阻止
	*   @Author   : Double sword
	*   @Params   : 	
			
	*   @Return   :	true-放行 false-阻止			
	*   @Date     :	2016-4-8
	*/
	bool VerifyPrantPidIsOurs(DWORD dwParantPid);

	/** 驱动返回错误处理线程 
	*   @Author   : Double sword
	*   @Params   : 	
			
	*   @Return   :				
	*   @Date     :	2016-4-14
	*/
	void ProcessDriverErrorThread(void * pThis);
	
	//实现接口
public:
	virtual void OnHandleQueueData(LPVOID pData,UINT uDataSize);

private:
	ProcessInfoStagety* GetProcessInfo(DWORD dwProcessId, bool bCreate=false);
	void RemoveProcessInfo(DWORD dwProcessId);
	std::string GetProcessNameByPid(DWORD dwPid);

private:
	IStagety* mStagetySet[enActionTypeMax];
	DWORD mCurProcessId;					//当前进程
	DWORD mClientPid;						//客户端主进程
	DWORD mSuspensionWindowPid;				//客户端悬浮窗进程
	TCHAR szNetServerIp;					//服务器IP
	StagetyProcessMap mProcessMap;			//进程列表
	QueueService mHandleResultService;		//处理结果服务
	IActionResultDelegate* mActionResultDelegate;	//行为结果的代理
};
#endif //__StagetyManager_H