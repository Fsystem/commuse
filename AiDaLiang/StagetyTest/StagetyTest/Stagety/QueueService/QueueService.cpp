#include <stdafx.h>
#include <Windows.h>
#include "QueueService.h"
#include <process.h>


QueueService::QueueService()
{
	pSink = NULL;
	bRun = false;
	hEventData = NULL;
	isOwnMem = true;
}
QueueService::~QueueService()
{
	StopService();
}
void QueueService::StartService()
{
	//创建事件
	hEventData = ::CreateEvent(NULL,FALSE,FALSE,NULL);
	::ResetEvent(hEventData);

	//创建线程
	bRun = true;
	JKThread::Start(&QueueService::ThreadProcess,this);
}
void QueueService::StopService()
{
	if (bRun)
	{
		bRun = false;
		::SetEvent(hEventData);
		::WaitForSingleObject(hEventData,300);

		::CloseHandle(hEventData);
		hEventData = NULL;

		if (isOwnMem)
		{
			for (list<QueueData>::iterator it = listQueue.begin(); it != listQueue.end();++it)
			{
				if (it->pData)
				{
					free(it->pData);
					it->pData = NULL;
				}
			}
		}
		
		listQueue.clear();
	}
}

void QueueService::AddData(LPVOID pData,UINT uDataSize)
{
	if(uDataSize == 0 || pData == NULL) return;

	QueueData QData;
	QData.uDataSize = uDataSize;
	QData.pData = pData;
	if (isOwnMem)
	{
		QData.pData = new char[uDataSize];
		memcpy(QData.pData,pData,uDataSize);
	}
	
	{
		LOCK lk(lkData);
		listQueue.push_back(QData);
	}

	::SetEvent(hEventData);
}
void QueueService::ThreadProcess(void * pThis)
{
	QueueService* mThis = (QueueService*)pThis;
	while (mThis->bRun)
	{
		::WaitForSingleObject(mThis->hEventData,INFINITE);
		if(mThis->listQueue.size()==0) continue;

		QueueData QData = mThis->listQueue.front();
		{
			LOCK lk(mThis->lkData);
			mThis->listQueue.pop_front();
		}

		if(mThis->pSink) mThis->pSink->OnHandleQueueData(QData.pData,QData.uDataSize);

		if (mThis->isOwnMem) delete [] QData.pData;
	}

	::_endthread();
}


void QueueService::SetOwnMem(bool isOwn)
{
	isOwnMem = isOwn;
}