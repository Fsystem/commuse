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
		//�����ڴ湲��
		void  CloseShareMemry();
	protected:
		void InitData(char* szWLockName, char* szRLockName);
	private:
		HANDLE					m_hShareMem;	//�����ڴ�ľ��
		PROCESS_LOCK*			m_lock;

		void*					m_pBaseMem;		//�ڴ����ַ

		DWORD					m_dwShareMemSize;//�����ڴ�Ĵ�С

		SECURITY_ATTRIBUTES		m_sa;			//��ȫ����
	};
}


#endif