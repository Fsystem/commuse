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

class JKThread
{
public:
	
	template<class T>
	static uintptr_t Start(void (T::*func)(void *),T* pDelegateThis)
	{
		union {                                // 联合类，用于转换类成员方法指针到普通函数指针（试过编译器不允许在这两种函数之间强制转换），不知道有没有更好的方法。
			void ( *ThreadProc)(void *);
			void ( T::*MemberProc)(void *);
		} Proc;

		Proc.MemberProc = func;

		return _beginthread(Proc.ThreadProc,0,pDelegateThis);
	}

protected:
	
};

#endif //__JKThread_H