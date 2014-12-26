#pragma once
//#include "stdafx.h"
#include <Windows.h>
#include "ProcessLock.h"
#include <assert.h>

namespace COMMUSE
{
	PROCESS_LOCK::PROCESS_LOCK( 
		char* pszWLockName , 
		char* pszRLockName 
		)
	{
		_hWhd = NULL;
		_hRhd = NULL;
		if (pszWLockName && pszRLockName)
		{
			strcpy_s(_szWLockName,sizeof(_szWLockName),pszWLockName);
			strcpy_s(_szRLockName,sizeof(_szWLockName),pszRLockName);
		}
		else
		{
			memset(_szWLockName,0,sizeof(_szWLockName));
			memset(_szRLockName,0,sizeof(_szRLockName));
		}

	}

	PROCESS_LOCK::~PROCESS_LOCK(  )
	{

	}

	//写锁初始化 只能被写进程调用
	BOOL PROCESS_LOCK::WInit()
	{
		BOOL bRes = 0;
		_hWhd = ::CreateMutexA(0,0,_szWLockName);
		if (_hWhd)
		{
			if ( ERROR_ALREADY_EXISTS == GetLastError() )
			{
				::CloseHandle(_hWhd);
				_hWhd = NULL;
				bRes = 1;
			}
		}
		else
		{
			bRes = 2;
			_hWhd = NULL;
		}

		if (0 == bRes )
		{
			_hRhd = ::CreateEventA(0,0,1,_szRLockName);
			if (_hRhd)
			{
				if ( ERROR_ALREADY_EXISTS == GetLastError() )
				{
					::CloseHandle(_hRhd);
					_hRhd = NULL;
					bRes = 3;
				}
			}
			else
			{
				bRes = 4;
			}
		}
		else
		{
			_hRhd = NULL;
		}


		return bRes;


	}

	//只能被写进程调用
	void PROCESS_LOCK::WUninit()
	{
		if (_hWhd)
		{
			::CloseHandle(_hWhd);
		}

		if (_hRhd)
		{
			::SetEvent(_hRhd);
			::CloseHandle(_hRhd);
		}
	}


	BOOL PROCESS_LOCK::WLock()
	{
		BOOL bRes = -1;
		//不在允许新的线程进程进入
		if (_hWhd && _hRhd)
		{
			//ReleaseMutex(_hWhd);
			::CloseHandle(_hWhd);

			//等读锁
			if (WAIT_OBJECT_0 == ::WaitForSingleObject(_hRhd,INFINITE))
			{
				bRes = 0;
			}
			else
			{
				bRes = 2;
			}
		}
		else
		{
			bRes = 1 ;
		}

		return bRes;

	}


	void PROCESS_LOCK::WUnlock()
	{

		//1.允许其他线程进程进入
		_hWhd = ::CreateMutexA(0,0,_szWLockName);
		if ( 0 == GetLastError())
		{
			;
		}
		//解读锁,下一次写解锁
		::SetEvent(_hRhd);

	}


	BOOL PROCESS_LOCK::RLock()
	{
		BOOL bRes = -1;

		_hWhd = ::OpenMutexA(MUTEX_ALL_ACCESS,0,_szWLockName);
		if (_hWhd)
		{
			::CloseHandle(_hWhd);
			_hWhd = NULL;
			_hRhd = ::OpenEventA(EVENT_ALL_ACCESS,0,_szRLockName);
			if (_hRhd)
			{
				::ResetEvent(_hRhd);
				bRes = 0;
			}
			else
			{
				//printf("/ ");
				bRes =2;
			}

		}
		else
		{
			//printf("; ");
			bRes = 1;
		}

		return bRes;
	}


	void PROCESS_LOCK::RUnlock()
	{

		if (_hRhd)
		{
			//printf("- ");
			::SetEvent(_hRhd);
			::CloseHandle(_hRhd);
			_hRhd = NULL;
		}
	}

}


