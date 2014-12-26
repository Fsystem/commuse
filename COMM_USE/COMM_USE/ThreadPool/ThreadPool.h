#pragma once
#include <list>
#include "../SafeCriticalSection/SafeCriticalSection.h"


namespace COMMUSE
{
	typedef struct _TaskInfo
	{
		void*		pData;
		unsigned	nDataSize;
	}TaskInfo;

	//任务信息队列
	typedef std::list<TaskInfo> TASKQUEUE;

	//空闲线程队列
	typedef std::list<HANDLE> THREADQUEUE;

	//任务的工作函数
	typedef void (_cdecl * PWORKFUNC)(void* , unsigned int );

	class ThreadPool
	{
	public:
		ThreadPool(unsigned int unMaxThrdNum);
		~ThreadPool();

		void Start(PWORKFUNC p);

		void Stop();

		//添加任务
		void AddTask(const void* pTaskInfo , unsigned unTaskInfoSize);

		//获取忙线程数
		unsigned int GetBusyThreadNum();

		//获取闲线程数
		unsigned int GetFreeThreadNum();

		//获取当前待处理的任务数
		unsigned int GetCurrentTaskNum();

	protected:
	private:
		//当前重在工作的线程数
		volatile long			m_lvBusyThrdNum;

		BOOL					m_bRun;
		//线程最大数
		unsigned int			m_unMaxThrdNum;

		//空闲线程队列
		THREADQUEUE				m_Thrd_queue;

		//任务队列
		TASKQUEUE				m_tk_queue;

		//任务回调函数
		PWORKFUNC				m_callback;

		HANDLE					m_hEvent;

		//锁
		CCritiSection			m_cs_task;
		CCritiSection			m_cs_thread;
		////尾锁
		//CCriticalSection		m_cs_Tail;

	private:
		BOOL NewThread();
		void DelThread(HANDLE hThread);
		static  unsigned int __stdcall ThreadPoolFunc(void* p);
	};
}