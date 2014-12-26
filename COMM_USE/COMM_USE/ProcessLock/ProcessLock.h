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

		//Ð´Ëø³õÊ¼»¯ 
		BOOL WInit();
		void WUninit();

		//Ð´Ëø 0 = OK
		BOOL WLock();
		void WUnlock();

		//¶ÁËø 0 = OK
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