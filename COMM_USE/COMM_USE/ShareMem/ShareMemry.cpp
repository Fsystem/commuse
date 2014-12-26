#pragma once
//#include "stdafx.h"
#include "ShareMemry.h"
#include <assert.h>

namespace COMMUSE
{
	ShareMemry::ShareMemry(char* szWLockName, char* szRLockName)
	{
		m_sa.nLength=sizeof(SECURITY_ATTRIBUTES);
		m_sa.bInheritHandle			= TRUE;
		m_sa.lpSecurityDescriptor	= NULL;

		m_hShareMem					= NULL;
		m_pBaseMem					= NULL;

		m_dwShareMemSize			= 0;
		m_lock = NULL;
		m_lock = new PROCESS_LOCK(szWLockName,szRLockName);
	}
	ShareMemry::~ShareMemry()
	{
		if (m_lock)
		{
			delete m_lock;
			m_lock = NULL;
		}

		if (m_pBaseMem)
		{
			UnmapViewOfFile(m_pBaseMem);
		}

		m_hShareMem					= NULL;
		m_pBaseMem					= NULL;

		m_dwShareMemSize			= 0;
		memset(&m_sa , 0 , sizeof(m_sa));
	}


	void ShareMemry::CreateInit()
	{
		m_lock->WInit();
	}

	void ShareMemry::CreateUninit()
	{
		m_lock->WUninit();
	}

	void* ShareMemry::OpenShareMemry( char* szFileName)
	{
		void* ret = NULL;

		if (!m_lock->RLock())
		{
			m_hShareMem = ::OpenFileMappingA(FILE_MAP_ALL_ACCESS ,FALSE , szFileName);

			if (m_hShareMem)
			{
				m_pBaseMem = ret  = ::MapViewOfFile( m_hShareMem,FILE_MAP_ALL_ACCESS,0,0,0 );
			}

		}

		return ret;
	}

	void  ShareMemry::CloseShareMemry()
	{
		if (m_pBaseMem)
		{
			UnmapViewOfFile(m_pBaseMem);
			m_pBaseMem = NULL;
		}

		if (m_hShareMem)
		{
			CloseHandle(m_hShareMem);
		}

		//锁区
		m_lock->RUnlock();
		////////////////////////////////////////////////////

	}

	void* ShareMemry::CreateShareMemry( char* szFileName,DWORD dwMemSize ,BOOL bUpdate )
	{//创建数据者调用
		if ( szFileName )
		{

			//锁区
			if (!m_lock->WLock())
			{
				//数据区
				if (!m_hShareMem)
				{
					m_dwShareMemSize = dwMemSize;
					m_hShareMem = ::CreateFileMappingA( INVALID_HANDLE_VALUE , &m_sa ,PAGE_READWRITE ,0 , dwMemSize,szFileName );
				}
				else if (
					m_dwShareMemSize != dwMemSize
					|| (m_dwShareMemSize == dwMemSize && bUpdate) 
					)
				{
					m_dwShareMemSize = dwMemSize;
					CloseHandle(m_hShareMem);
					m_hShareMem = ::CreateFileMappingA( INVALID_HANDLE_VALUE , &m_sa ,PAGE_READWRITE ,0 , dwMemSize,szFileName );
				}
				else
				{
					return NULL;
				}

				if ( NULL != m_hShareMem )
				{
					return (m_pBaseMem =::MapViewOfFile( m_hShareMem,FILE_MAP_ALL_ACCESS,0,0,0 ) );
				}
			}

		}
		return NULL;
	}

	void  ShareMemry::CreateShareMemryOk()
	{
		if (m_pBaseMem)
		{
			UnmapViewOfFile(m_pBaseMem);
			m_pBaseMem = NULL;
		}

		//锁区
		m_lock->WUnlock();
		////////////////////////////////////////////////////
	}
}

