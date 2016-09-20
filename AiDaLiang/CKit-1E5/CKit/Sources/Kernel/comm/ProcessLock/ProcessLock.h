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
 
		BOOL Init();
		void Uninit();

		//Ð´Ëø 0 = OK
		BOOL WLock();
		void WNotify();

		//¶ÁËø 0 = OK
		BOOL RLock();
		void RNotify();

	protected:
	private:
		HANDLE  _hWhd;
		HANDLE	_hRhd;
		char	_szWLockName[64];
		char	_szRLockName[64];
		bool	_bOwnerWLock;
		bool	_bOwnerRLock;

	};

}




#endif