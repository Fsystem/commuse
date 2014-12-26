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
		//�����ڴ湲��
		void  CloseShareMemry(char* szFileName);
	protected:
	private:
		HANDLE					m_hShareMem;	//�����ڴ�ľ��
		HANDLE					m_hLock;		//�����ڴ�������

		BOOL					m_bCreator;		//���ݴ�����?
		void*					m_pBaseMem;		//�ڴ����ַ
		void*					m_plock;		//���׵�ַ
		DWORD					m_dwShareMemSize;//�����ڴ�Ĵ�С

		SECURITY_ATTRIBUTES		m_sa;			//��ȫ����
	};
}


#endif