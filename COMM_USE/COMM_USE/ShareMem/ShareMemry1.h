#ifndef _SHARE_MEMRY_H
#define _SHARE_MEMRY_H

#include <Windows.h>

//#define g_sharemem  (ShareMemry::getInstance())

namespace COMMUSE
{
	class ShareMemry
	{
	public:
		ShareMemry();
		~ShareMemry();
	public:
		static ShareMemry& getInstance();

		void* CreateShareMemry(char* szLockName,char* szFileName,DWORD dwMemSize  ,BOOL bUpdate = FALSE);

		void  CreateShareMemryOk(char* szLockName);

		void* OpenShareMemry(char* szLockName, char* szFileName);
		//销毁内存共享
		void  CloseShareMemry(char* szFileName);
	protected:
	private:
		HANDLE					m_hShareMem;	//共享内存的句柄
		HANDLE					m_hLock;		//共享内存的锁句柄

		BOOL					m_bCreator;		//数据创建者?
		void*					m_pBaseMem;		//内存基地址
		void*					m_plock;		//锁首地址
		DWORD					m_dwShareMemSize;//共享内存的大小

		SECURITY_ATTRIBUTES		m_sa;			//安全属性
	};
}


#endif