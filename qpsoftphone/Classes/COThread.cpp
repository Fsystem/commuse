#include "COThread.h"



CCOThread::CCOThread()
{
	//pthread_mutex_init(&mutex);
	//pthread_mutex_init(&mutexSuspend);
	mutex = PTHREAD_MUTEX_INITIALIZER;
	mutexSuspend = PTHREAD_MUTEX_INITIALIZER;
	thread_cond = PTHREAD_COND_INITIALIZER;
	run = false;
}

CCOThread::~CCOThread()
{
	DestroyThread();
	pthread_mutex_destroy(&mutex);
	pthread_mutex_destroy(&mutexSuspend);
}

bool CCOThread::CreateThred(bool bSuspend)
{
	int nRes = 0;
	suspend = bSuspend;
	run = true;

	pthread_attr_t attr;

	pthread_attr_init(&attr);
	pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
	//pthread_setcancelstate();

	nRes = pthread_create(&thread_id,&attr,BaseThreadFunc,this);
	return (nRes!=0?true:false);
}

bool CCOThread::DestroyThread()
{
	if(run)
	{
		run = false;
		pthread_join(thread_id,NULL);
	}
	
	return true;
}

bool CCOThread::SuspendThread()
{
	suspend = true;
	return true;
}

bool CCOThread::ResumeThread()
{
	suspend = false;
	pthread_cond_signal(&thread_cond);
	return true;
}

//////////////////////////////////////////////////////////////////////////
//pravite:

void* CCOThread::BaseThreadFunc(void* p)
{
	CCOThread* pThis = (CCOThread*)p;
	while(pThis->run&&pThis)
	{
		pthread_mutex_lock(&pThis->mutexSuspend);
		if (pThis->suspend)
		{
			pthread_cond_wait(&pThis->thread_cond,&pThis->mutexSuspend);
		}
		pthread_mutex_unlock(&pThis->mutexSuspend);

		//work code
		pThis->ThreadWork(p);

	}

	return NULL;
}

