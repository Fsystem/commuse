/** �߳�
*   @FileName  : JKThread.h
*   @Author    : Double Sword(doublesword.jack@gmail.com)
*   @date      : 2016-4-12
*   @Copyright : Copyright belong to Author and 9u2,CO.LTD
*   @RefDoc    : --
*/

#ifndef __JKThread_H
#define __JKThread_H

#include <process.h>

class JKThread
{
public:
	
	template<class T>
	static uintptr_t Start(void (T::*func)(void *),T* pDelegateThis)
	{
		union {                                // �����࣬����ת�����Ա����ָ�뵽��ͨ����ָ�루�Թ��������������������ֺ���֮��ǿ��ת��������֪����û�и��õķ�����
			void ( *ThreadProc)(void *);
			void ( T::*MemberProc)(void *);
		} Proc;

		Proc.MemberProc = func;

		return _beginthread(Proc.ThreadProc,0,pDelegateThis);
	}

protected:
	
};

#endif //__JKThread_H