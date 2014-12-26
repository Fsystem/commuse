//#include "stdafx.h"
#include "UXTimerQueue.h"
#include <process.h>
 
CUXTimer* CUXTimer::m_pThis = NULL;
//--------------------------------------------------------------------------------------------------------
CUXTimer::CUXTimer():m_longIdEvent(0)
{
	m_hTimerQueue = CreateTimerQueue();
	InitializeCriticalSection(&m_cs_timer_queue);
	InitializeCriticalSection(&m_cs_del_timer_queue);
	m_pThis = this;
	m_hGuardThd = (HANDLE)_beginthreadex(NULL,0,GuardThd,0,0,0);
}
//--------------------------------------------------------------------------------------------------------
CUXTimer:: ~CUXTimer()
{
	DeleteTimerQueue(m_hTimerQueue);
	if (m_hGuardThd)
	{
		::TerminateThread(m_hGuardThd , 0);
	}
	DeleteCriticalSection(&m_cs_timer_queue);
	DeleteCriticalSection(&m_cs_del_timer_queue);


	for ( TIMERQUEUE::iterator it = m_timer_queue.begin() ; it != m_timer_queue.end() ; it++ )
	{
		if (it->second)
		{
			delete it->second;
		}
	}
	m_timer_queue.clear();


	for ( TIMERQUEUEDELETE::iterator it = m_del_timer_queue.begin() ; it != m_del_timer_queue.end() ; it++ )
	{
		if (*it)
		{
			delete *it;
		}
	}
	m_timer_queue.clear();

}
//--------------------------------------------------------------------------------------------------------
void CUXTimer::SetTimer(DWORD dwIdEvent,DWORD dwTime,UXTIMERCALLBACK callBack,void* p)
{
	TIMERINFO* t=new TIMERINFO;
	t->dwEventID = dwIdEvent;
	t->callback = callBack;
	t->pEvent = p;
	t->pThis = this;
	//t->hCmpEvent = CreateEvent(NULL,1,1,0);
	//ResetEvent(t->hCmpEvent);
	if(!CreateTimerQueueTimer(&t->hEvent,m_hTimerQueue,(WAITORTIMERCALLBACK)TimerFunc,(void*)t->dwEventID,dwTime,dwTime,WT_EXECUTEINIOTHREAD))
	{
		if (!t->hEvent)
		{
			if (t)
			{
				delete t;
				t = NULL;
			}
			return;
		}
	}

	TIMERQUEUE::iterator it;
	EnterCriticalSection(&m_cs_timer_queue);
	//预防在维护线程没有清空原有的timer，这时候这个timer重复利用，需要释放先前一个timer
	if ( (it = m_timer_queue.find(dwIdEvent) ) != m_timer_queue.end())
	{
		if (it->second)
		{
			DeleteTimerQueueTimer(m_hTimerQueue,it->second->hEvent,NULL);
			delete it->second;
			it->second = NULL;
		}
		m_timer_queue.erase(it);
	}
	//将timer添加到map中用于保存
	m_timer_queue.insert(std::make_pair(dwIdEvent,t));
	LeaveCriticalSection(&m_cs_timer_queue);
}
//--------------------------------------------------------------------------------------------------------
void CUXTimer::KillTimer(DWORD dwIdEvent)
{
	EnterCriticalSection(&m_cs_timer_queue);
	
	
	TIMERQUEUE::iterator it = m_timer_queue.find(dwIdEvent);
	if (it!=m_timer_queue.end())
	{
		it->second->pThis = NULL;
		if (it->second->hEvent)
		{
			DeleteTimerQueueTimer(m_hTimerQueue,it->second->hEvent,NULL);
			//LOGEVEN("KILLTIMER\n");
			EnterCriticalSection(&m_cs_del_timer_queue);
			m_del_timer_queue.push_back(it->second);
			LeaveCriticalSection(&m_cs_del_timer_queue);

			m_timer_queue.erase(it);
		}

	}
	LeaveCriticalSection(&m_cs_timer_queue);

}
//--------------------------------------------------------------------------------------------------------
void  CUXTimer::TimerFunc(void* p,bool b)
{
		DWORD dwEventID = (DWORD)p;
		TIMERINFO TimerInfo = {0} ;
		EnterCriticalSection(&G_UXTimerQueue.m_cs_timer_queue);
		TIMERQUEUE::iterator it1 = m_pThis->m_timer_queue.find(dwEventID);
		if (it1 != m_pThis->m_timer_queue.end()) 
		{
			if (it1->second)
			{
				TimerInfo = *it1->second;
			}
		}
		LeaveCriticalSection(&G_UXTimerQueue.m_cs_timer_queue);
		if (TimerInfo.pThis)
		{
			TimerInfo.callback(dwEventID,TimerInfo.pEvent);
		}
		
}
//--------------------------------------------------------------------------------------------------------
//优先级不必太高只是用于清除工作
unsigned int __stdcall CUXTimer::GuardThd(PVOID p)
 {
	 while(1)
	 {
// 		 if (m_pThis->m_timer_queue.size()>0)
// 		 {
// 			 EnterCriticalSection(&G_UXTimerQueue.m_cs_timer_queue);
// 			 for (TIMERQUEUE::iterator it = m_pThis->m_timer_queue.begin();it != m_pThis->m_timer_queue.end();)
// 			 {
// 				 if(WaitForSingleObject(it->second->hCmpEvent,0) == WAIT_OBJECT_0)
// 				 {
// 					 CloseHandle(it->second->hCmpEvent);
// 					 it->second->hCmpEvent = NULL;
// 
// 					 if (it->second)
// 					 {
// 						 delete it->second;
// 						 it->second = NULL;
// 					 }
// 					 
// 					 m_pThis->m_timer_queue.erase(it++);
// 					
// 					 //LOGEVEN("REAL_KILLTIMER\n");
// 				 }
// 				 else
// 					it++;
// 			 }
// 			 LeaveCriticalSection(&G_UXTimerQueue.m_cs_timer_queue);
// 		 }

		 if (m_pThis->m_del_timer_queue.size()>0)
		 {
			EnterCriticalSection(&m_pThis->m_cs_del_timer_queue);
			for (TIMERQUEUEDELETE::iterator it = m_pThis->m_del_timer_queue.begin();
				it != m_pThis->m_del_timer_queue.end() ; )
			{
				if (*it)
				{
					delete *it;
					it = m_pThis->m_del_timer_queue.erase(it);
				}
				else
					it++;
			}
			LeaveCriticalSection(&m_pThis->m_cs_del_timer_queue);
		 }

		 Sleep(500);
	 }
	 
	return 0;
 }

 //--------------------------------------------------------------------------------------------------------