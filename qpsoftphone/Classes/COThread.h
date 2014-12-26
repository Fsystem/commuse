#ifndef __COThread_H
#define __COThread_H
/** cocos2dx线程基类
*   @FileName  : COThread.h
*   @Author    : Double Sword
*   @date      : 2012-12-14
*   @Copyright : Copyright belong to Author and ZhiZunNet.CO.
*   @RefDoc    : --
*/
#include "AppInc.h"

class CCOThread
{
	//变量
public:

protected:
	pthread_t				thread_id;
	pthread_cond_t			thread_cond;
	pthread_mutex_t			mutexSuspend;
	pthread_mutex_t			mutex;
	bool					run;
	bool					suspend;
private:

	//函数
public:
	CCOThread();
	virtual ~CCOThread();

	/** 创建线程
	*   @FuncName : CreateThred
	*   @Author   : Double sword
	*   @Params   : 	
	*				bSuspend 是否挂起线程
	*   @Return   :	return true if success anything else 
							
	*   @Date     :	2012-12-14
	*/
	bool CreateThred(bool bSuspend=false);




	/** 销毁线程
	*   @FuncName : 
	*   @Author   : Double sword
	*   @Params   : 	
			
	*   @Return   :	return true if success anything else 
							
	*   @Date     :	2012-12-14
	*/
	bool DestroyThread();





	/** 挂起线程
	*   @FuncName : SuspendThread
	*   @Author   : Double sword
	*   @Params   : 	
			
	*   @Return   :	return true if success anything else 
							
	*   @Date     :	2012-12-14
	*/
	bool SuspendThread();





	/** 恢复线程
	*   @FuncName : ResumeThread
	*   @Author   : Double sword
	*   @Params   : 	
			
	*   @Return   :	return true if success anything else 
							
	*   @Date     :	2012-12-14
	*/
	bool ResumeThread();




	/** 子类必须实现的线程回调过程
	*   @FuncName : ThreadFunc
	*   @Author   : Double sword
	*   @Params   : 	
			
	*   @Return   :	
							
	*   @Date     :	2012-12-14
	*/
	virtual void* ThreadWork(void* p)=0;

protected:

private:
	static void* BaseThreadFunc(void* p);
	
};
#endif //__COThread_H