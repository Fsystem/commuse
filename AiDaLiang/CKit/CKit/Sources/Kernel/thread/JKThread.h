/** 线程
*   @FileName  : JKThread.h
*   @Author    : Double Sword(doublesword.jack@gmail.com)
*   @date      : 2016-4-12
*   @Copyright : Copyright belong to Author and 9u2,CO.LTD
*   @RefDoc    : --
*/

#ifndef __JKThread_H
#define __JKThread_H

#include <process.h>
template<class T>
class JKThread
{
	template<class T>
	struct JK_THREAD_INFO
	{
		void (T::*func)();
		T* pOwner;
	};

	static void thread_func_private(void* pData)
	{
		try
		{
			JK_THREAD_INFO<T>* pFuncInfo = (JK_THREAD_INFO<T>*)pData;
			if (pFuncInfo)
			{
				(pFuncInfo->pOwner->*(pFuncInfo->func))();

				delete pFuncInfo;
			}
			else throw 0;
		}
		catch (...)
		{
			
		}
		
		Stop();
	}
public:
	//需自己调用Stop，释放线程数据
	static uintptr_t Start(void (T::*func)(void *),void* pDelegateThis)
	{
		union {
			void ( *ThreadProc)(void *);
			void ( T::*MemberProc)(void *);
		} Proc;

		Proc.MemberProc = func;

		return _beginthread(Proc.ThreadProc,0,pDelegateThis);
	}

	//不需调用Stop，自己释放释放线程数据
	static uintptr_t Start(void (T::*func)(),T* pDelegateThis)
	{
		JK_THREAD_INFO<T> * pData = new JK_THREAD_INFO<T>;
		pData->func = func;
		pData->pOwner = pDelegateThis;
		return _beginthread(thread_func_private,0,pData);
	}

	//需自己调用Stop，释放线程数据
	static uintptr_t Start(void ( *ThreadProc)(void *),void* pDelegateThis)
	{
		return _beginthread(ThreadProc,0,pDelegateThis);
	}

	static void Stop()
	{
		_endthread();
	}

protected:

};

#endif //__JKThread_H