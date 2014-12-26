#ifndef __COThread_H
#define __COThread_H
/** cocos2dx�̻߳���
*   @FileName  : COThread.h
*   @Author    : Double Sword
*   @date      : 2012-12-14
*   @Copyright : Copyright belong to Author and ZhiZunNet.CO.
*   @RefDoc    : --
*/
#include "AppInc.h"

class CCOThread
{
	//����
public:

protected:
	pthread_t				thread_id;
	pthread_cond_t			thread_cond;
	pthread_mutex_t			mutexSuspend;
	pthread_mutex_t			mutex;
	bool					run;
	bool					suspend;
private:

	//����
public:
	CCOThread();
	virtual ~CCOThread();

	/** �����߳�
	*   @FuncName : CreateThred
	*   @Author   : Double sword
	*   @Params   : 	
	*				bSuspend �Ƿ�����߳�
	*   @Return   :	return true if success anything else 
							
	*   @Date     :	2012-12-14
	*/
	bool CreateThred(bool bSuspend=false);




	/** �����߳�
	*   @FuncName : 
	*   @Author   : Double sword
	*   @Params   : 	
			
	*   @Return   :	return true if success anything else 
							
	*   @Date     :	2012-12-14
	*/
	bool DestroyThread();





	/** �����߳�
	*   @FuncName : SuspendThread
	*   @Author   : Double sword
	*   @Params   : 	
			
	*   @Return   :	return true if success anything else 
							
	*   @Date     :	2012-12-14
	*/
	bool SuspendThread();





	/** �ָ��߳�
	*   @FuncName : ResumeThread
	*   @Author   : Double sword
	*   @Params   : 	
			
	*   @Return   :	return true if success anything else 
							
	*   @Date     :	2012-12-14
	*/
	bool ResumeThread();




	/** �������ʵ�ֵ��̻߳ص�����
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