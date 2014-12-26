#ifndef _PROCESS_LOCK_EVENT_H_
#define _PROCESS_LOCK_EVENT_H_

#pragma once

namespace COMMUSE
{
	class PROCESS_LOCK
	{
	public:
		PROCESS_LOCK( 
			char* pszWLockName , 
			char* pszRLockName );
		~PROCESS_LOCK();

		//д����ʼ�� 
		BOOL WInit();
		void WUninit();

		//д�� 0 = OK
		BOOL WLock();
		void WUnlock();

		//���� 0 = OK
		BOOL RLock();
		void RUnlock();

	protected:
	private:
		HANDLE  _hWhd;
		HANDLE	_hRhd;
		char	_szWLockName[64];
		char	_szRLockName[64];

	};

}




#endif