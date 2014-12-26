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

	//������Ϣ����
	typedef std::list<TaskInfo> TASKQUEUE;

	//�����̶߳���
	typedef std::list<HANDLE> THREADQUEUE;

	//����Ĺ�������
	typedef void (_cdecl * PWORKFUNC)(void* , unsigned int );

	class ThreadPool
	{
	public:
		ThreadPool(unsigned int unMaxThrdNum);
		~ThreadPool();

		void Start(PWORKFUNC p);

		void Stop();

		//�������
		void AddTask(const void* pTaskInfo , unsigned unTaskInfoSize);

		//��ȡæ�߳���
		unsigned int GetBusyThreadNum();

		//��ȡ���߳���
		unsigned int GetFreeThreadNum();

		//��ȡ��ǰ�������������
		unsigned int GetCurrentTaskNum();

	protected:
	private:
		//��ǰ���ڹ������߳���
		volatile long			m_lvBusyThrdNum;

		BOOL					m_bRun;
		//�߳������
		unsigned int			m_unMaxThrdNum;

		//�����̶߳���
		THREADQUEUE				m_Thrd_queue;

		//�������
		TASKQUEUE				m_tk_queue;

		//����ص�����
		PWORKFUNC				m_callback;

		HANDLE					m_hEvent;

		//��
		CCritiSection			m_cs_task;
		CCritiSection			m_cs_thread;
		////β��
		//CCriticalSection		m_cs_Tail;

	private:
		BOOL NewThread();
		void DelThread(HANDLE hThread);
		static  unsigned int __stdcall ThreadPoolFunc(void* p);
	};
}