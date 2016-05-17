#pragma once
#include "stdafx.h"
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
		_bOwnerWLock = false;
		_bOwnerRLock = false;
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

		Init();
	}

	PROCESS_LOCK::~PROCESS_LOCK(  )
	{
		Uninit();
	}

	//写锁初始化 只能被写进程调用
	BOOL PROCESS_LOCK::Init()
	{
		BOOL bRes = 0;
		_hWhd = ::CreateEventA(0,0,0,_szWLockName);
		if (_hWhd)
		{
			if ( ERROR_ALREADY_EXISTS != GetLastError() )
			{
				_bOwnerWLock = true;
			}
		}
		
		_hRhd = ::CreateEventA(0,0,0,_szRLockName);
		if (_hRhd)
		{
			if ( ERROR_ALREADY_EXISTS != GetLastError() )
			{
				_bOwnerRLock = true;
			}
		}

		if (_bOwnerRLock) RNotify();
		if (_bOwnerWLock) WNotify();

		return bRes;
	}


	void PROCESS_LOCK::Uninit()
	{
		if (_hWhd && _bOwnerWLock)
		{
			::SetEvent(_hWhd);
			::CloseHandle(_hWhd);
			_hWhd = NULL;
		}

		if (_hRhd && _bOwnerRLock)
		{
			::SetEvent(_hRhd);
			::CloseHandle(_hRhd);
			_hRhd = NULL;
		}
	}


	BOOL PROCESS_LOCK::WLock()
	{
		BOOL bRes = -1;
		//不在允许新的线程进程进入
		if (_hWhd)
		{
			//等锁
			if (WAIT_OBJECT_0 == ::WaitForSingleObject(_hWhd,5000))
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


	void PROCESS_LOCK::WNotify()
	{
		//解读锁,下一次写解锁
		if(_hWhd) ::SetEvent(_hWhd);
	}


	BOOL PROCESS_LOCK::RLock()
	{
		BOOL bRes = -1;

		if (_hRhd)
		{
			//等锁
			if (WAIT_OBJECT_0 == ::WaitForSingleObject(_hRhd,5000))
			{
				bRes = 0;
			}
			else
			{
				bRes = 2;
			}
		}

		return bRes;
	}


	void PROCESS_LOCK::RNotify()
	{
		if (_hRhd) ::SetEvent(_hRhd);
	}

}


