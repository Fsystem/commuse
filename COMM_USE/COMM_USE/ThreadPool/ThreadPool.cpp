#pragma once
//#include "stdafx.h"

#include <process.h>
#include "ThreadPool.h"
#include <assert.h>


namespace COMMUSE
{
	ThreadPool::ThreadPool(unsigned int unMaxThrdNum)
	{
		assert( unMaxThrdNum>0 && unMaxThrdNum <= 1000 );
		m_unMaxThrdNum = unMaxThrdNum ;

		m_bRun = FALSE;

		m_hEvent = ::CreateEvent(NULL,0,0,NULL);//创建是否手动复位无信号状态的事件

		m_Thrd_queue.clear();
		m_tk_queue.clear();

	}

	ThreadPool::~ThreadPool()
	{
		if ( m_bRun )
		{
			m_bRun = FALSE;
			//终止线程
			for (THREADQUEUE::iterator it = m_Thrd_queue.begin() ; it!= m_Thrd_queue.end();it++ )
			{
				::TerminateThread(*it,0);

			}
			m_Thrd_queue.clear();

			//释放内存
// 			for (TASKQUEUE::iterator it = m_tk_queue.begin() ; it!= m_tk_queue.end();it++ )
// 			{
// 				::HeapFree(GetProcessHeap(),0,it->pData );
// 			}
			m_tk_queue.clear();

		}
		
		m_unMaxThrdNum = 0;

		if (m_hEvent)
		{
			SetEvent(m_hEvent);
			CloseHandle(m_hEvent);
			m_hEvent = NULL;
		}

	}

	//-------------------------------------------------------------------------------
	BOOL ThreadPool::NewThread()
	{
		BOOL bRes = FALSE;
		HANDLE h ;
		if ( ( h =(HANDLE) ::_beginthreadex(NULL,0,ThreadPoolFunc,this,0,0) ) )
		{
			LOCK lk(m_cs_thread);
			m_Thrd_queue.push_back(h);
			bRes = TRUE;
		}
		return bRes;
	}
	//-------------------------------------------------------------------------------

	void ThreadPool::DelThread(HANDLE hThread)
	{
		if (hThread)
		{
			LOCK lk(m_cs_thread);
			for (THREADQUEUE::iterator it = m_Thrd_queue.begin() ; 
				it != m_Thrd_queue.end() ; it++)
			{
				if ((*it) == hThread)
				{
					it = m_Thrd_queue.erase(it);
				}
			}
			CloseHandle(hThread);
		}
		
	}
	//-------------------------------------------------------------------------------

	void ThreadPool::Start(PWORKFUNC p)
	{
		if ( !m_bRun )
		{
			m_lvBusyThrdNum = 0;
			m_callback = p;

			m_Thrd_queue.clear();
			m_tk_queue.clear();

			//开启线程
			unsigned int unMaxThrdNum = m_unMaxThrdNum;

			m_bRun = TRUE;

			while( unMaxThrdNum-- > 0 )
			{
				NewThread();
			}
		}
		
	}

	void ThreadPool::Stop()
	{
		if ( m_bRun )
		{
			m_lvBusyThrdNum = 0;
			m_bRun = FALSE;
			//终止线程
			for (THREADQUEUE::iterator it = m_Thrd_queue.begin() ; it!= m_Thrd_queue.end();it++ )
			{
				::TerminateThread(*it,0);

			}
			m_Thrd_queue.clear();
			m_tk_queue.clear();

			m_callback = NULL;
		}
		
	}

	
	//添加任务
	void ThreadPool::AddTask(const void* pTaskInfo , unsigned unTaskInfoSize)
	{
		if ( pTaskInfo && unTaskInfoSize < 0x7ffffff )
		{
			TaskInfo tk= {0};
			tk.pData = (void*)pTaskInfo;
			tk.nDataSize = unTaskInfoSize;

			LOCK lk(m_cs_task);
			m_tk_queue.push_back(tk);
			//通知线程有任务了
			SetEvent(m_hEvent);
		}
	}

	//获取忙线程数
	unsigned int ThreadPool::GetBusyThreadNum()
	{
		return m_lvBusyThrdNum;
	}

	//获取闲线程数
	unsigned int ThreadPool::GetFreeThreadNum()
	{
		return m_unMaxThrdNum-m_lvBusyThrdNum;
	}
		

	//获取当前待处理的任务数
	unsigned int ThreadPool::GetCurrentTaskNum()
	{
		return m_tk_queue.size();
	}

	unsigned int __stdcall ThreadPool::ThreadPoolFunc(void* p)
	{
		ThreadPool* pThis = static_cast<ThreadPool*>(p);
		TaskInfo tk = {0};
		while( pThis && pThis->m_bRun )
		{
			__try
			{
				if ( pThis->m_tk_queue.empty() )
					WaitForSingleObject( pThis->m_hEvent,INFINITE );

				if ( !pThis->m_bRun ) break;

				//busy线程+1
				InterlockedIncrement(&pThis->m_lvBusyThrdNum);

				memset(&tk , 0 , sizeof(tk) );

				pThis->m_cs_task.Enter();
				if ( !pThis->m_tk_queue.empty())
				{

					tk = pThis->m_tk_queue.front();
					pThis->m_tk_queue.pop_front();

					pThis->m_cs_task.Leave();


					if ( pThis->m_callback && pThis->m_bRun )
					{
						pThis->m_callback(tk.pData,tk.nDataSize);
					}

					tk.pData = NULL;

				}
				else
				{	
					pThis->m_cs_task.Leave();
				}

			}//END __try
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				char msg[255];
				sprintf_s(msg,255,"THREADPOOL>>>ThreadPoolFunc[PID:%u]\n",GetProcessId(GetCurrentProcess()));
				OutputDebugStringA(msg);

			}//END __except

			//busy线程-1
			InterlockedDecrement(&pThis->m_lvBusyThrdNum);

		}//END while( pThis && pThis->m_bRun )




		return 0;
	}
}