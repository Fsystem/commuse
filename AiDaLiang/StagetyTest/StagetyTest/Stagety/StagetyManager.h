/** ���Թ����࣬����builderģʽ
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

/** ����ʹ�����Ϊo
*   @ClassName	: 
*   @Purpose	:
*		�������еĲ������ݣ��Լ�������Ӧ����Ϊ����
*   @Author	: Double Sword
*   @Data	: 2016-4-7
*/

using std::map;
class StagetyManager:public IQueueServiceSink
{
	/*! ���Ա���Ľ�����Ϣ */
	typedef map<DWORD,ProcessInfoStagety*> StagetyProcessMap;
public:
	~StagetyManager();
	static StagetyManager& Instance(){static StagetyManager s;return s;};

public:
	/** ��������
	*   @Author   : Double sword
	*   @Params   : none	
			
	*   @Return   :	none			
	*   @Date     :	2016-4-7
	*/
	void BuildAllStagety(IActionResultDelegate* pHandleResultDelegate);

	void ResetData();

	bool HandleAction(UINT type, PVOID lpata, DWORD size_len);

	//�첽֪ͨ�������Ӱ�������Ľ��
	void PostProcessResult(NotifyStagety* pNotify);

	//ͬ��֪ͨ�����Ӱ���������
	BOOL SendProcessResult(NotifyStagety* pNotify);

private:
	StagetyManager();

	/** ��֤�������Ƿ������ǹٷ��Ľ���ID,����Ƿ��У���֮����ֹ
	*   @Author   : Double sword
	*   @Params   : 	
			
	*   @Return   :	true-���� false-��ֹ			
	*   @Date     :	2016-4-8
	*/
	bool VerifyPrantPidIsOurs(DWORD dwParantPid);

	/** �������ش������߳� 
	*   @Author   : Double sword
	*   @Params   : 	
			
	*   @Return   :				
	*   @Date     :	2016-4-14
	*/
	void ProcessDriverErrorThread(void * pThis);
	
	//ʵ�ֽӿ�
public:
	virtual void OnHandleQueueData(LPVOID pData,UINT uDataSize);

private:
	ProcessInfoStagety* GetProcessInfo(DWORD dwProcessId, bool bCreate=false);
	void RemoveProcessInfo(DWORD dwProcessId);
	std::string GetProcessNameByPid(DWORD dwPid);

private:
	IStagety* mStagetySet[enActionTypeMax];
	DWORD mCurProcessId;					//��ǰ����
	DWORD mClientPid;						//�ͻ���������
	DWORD mSuspensionWindowPid;				//�ͻ�������������
	TCHAR szNetServerIp;					//������IP
	StagetyProcessMap mProcessMap;			//�����б�
	QueueService mHandleResultService;		//����������
	IActionResultDelegate* mActionResultDelegate;	//��Ϊ����Ĵ���
};
#endif //__StagetyManager_H