#ifndef _SHARE_MEMRY_H
#define _SHARE_MEMRY_H

#include <Windows.h>
#include "../ProcessLock/ProcessLock.h"

//#define g_sharemem  (ShareMemry::getInstance())

namespace COMMUSE
{
	class ShareMemry
	{
	public:
		ShareMemry(char* szWLockName, char* szRLockName);
		ShareMemry();
		~ShareMemry();
	public:

		//write

		void* CreateShareMemry(char* szFileName,DWORD dwMemSize  ,BOOL bUpdate = FALSE);

		void  CreateShareMemryOk();

		/////////////////////////////////////////////////////////////////

		//read

		void* OpenShareMemry( char* szFileName);
		//销毁内存共享
		void  CloseShareMemry();
	protected:
		void InitData(char* szWLockName, char* szRLockName);
	private:
		HANDLE					m_hShareMem;	//共享内存的句柄
		PROCESS_LOCK*			m_lock;

		void*					m_pBaseMem;		//内存基地址

		DWORD					m_dwShareMemSize;//共享内存的大小

		SECURITY_ATTRIBUTES		m_sa;			//安全属性
	};
}


#endif