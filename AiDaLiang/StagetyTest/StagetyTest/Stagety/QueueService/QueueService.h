/** 队列服务
*   @FileName  : QueueService.h
*   @Author    : Double Sword(doublesword.jack@gmail.com)
*   @date      : 2016-4-12
*   @Copyright : Copyright belong to Author and 9u2,CO.LTD
*   @RefDoc    : --
*/

#ifndef __QueueService_H
#define __QueueService_H
#include "../SafeCriticalSection/SafeCriticalSection.h"
#include <list>

using namespace COMMUSE;
using std::list;
/*! 队列数据处理的代理接口 */
struct IQueueServiceSink
{
	virtual void OnHandleQueueData(LPVOID pData,UINT uDataSize) = 0;
};

/*! 队列服务 */
class QueueService
{
	struct QueueData
	{
		LPVOID pData;
		UINT uDataSize;
	};

public:
	QueueService();
	virtual ~QueueService();

	static QueueService& Instance(){static QueueService s;return s;};

public:
	inline void SetHandleSink(IQueueServiceSink* sink){pSink = sink;}
	void StartService();
	void StopService();
	void AddData(LPVOID pData,UINT uDataSize);
	void SetOwnMem(bool isOwn);//设置独立内存，自己创建和释放
protected:
	void ThreadProcess(void* pThis);
protected:
	IQueueServiceSink*			pSink;
	bool						bRun;
	CCritiSection				lkData;
	list<QueueData>				listQueue;
	HANDLE						hEventData;
	bool						isOwnMem;
private:
};
#endif //__QueueService_H