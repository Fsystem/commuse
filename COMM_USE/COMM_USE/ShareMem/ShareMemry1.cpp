#pragma once
//#include "stdafx.h"
#include "ShareMemry.h"
#include <assert.h>

namespace COMMUSE
{
	ShareMemry::ShareMemry()
	{
		m_sa.nLength=sizeof(SECURITY_ATTRIBUTES);
		m_sa.bInheritHandle			= TRUE;
		m_sa.lpSecurityDescriptor	= NULL;

		m_hShareMem					= NULL;
		m_pBaseMem					= NULL;

		m_bCreator					= FALSE;

		m_hLock						= NULL;

		m_plock						= NULL;

		m_dwShareMemSize			= 0;
	}
	ShareMemry::~ShareMemry()
	{
		if (m_plock)
		{
			UnmapViewOfFile(m_plock);
		}
		if (m_pBaseMem)
		{
			UnmapViewOfFile(m_pBaseMem);
		}
		// 	if (m_hShareMem)
		// 	{
		// 		CloseHandle(m_hShareMem);
		// 	}
		// 	if (m_hLock)
		// 	{
		// 		CloseHandle(m_hLock);
		// 	}

		m_hShareMem					= NULL;
		m_pBaseMem					= NULL;

		m_bCreator					= FALSE;

		m_hLock						= NULL;

		m_plock						= NULL;

		m_dwShareMemSize			= 0;
		memset(&m_sa , 0 , sizeof(m_sa));
	}

	ShareMemry& ShareMemry::getInstance()
	{
		static ShareMemry sm;
		return sm;
	}

	void* ShareMemry::OpenShareMemry(char* szLockName, char* szFileName)
	{
		void* ret = NULL;

		m_hLock = ::OpenFileMappingA(FILE_MAP_ALL_ACCESS ,FALSE , szLockName);
		if (m_hLock)
		{
			m_plock = ::MapViewOfFile( m_hLock,FILE_MAP_ALL_ACCESS,0,0,0 );
			if (m_plock)
			{
				while(*((char*)m_plock) ==1 )
					Sleep(10);//释放一会cpu
			}

			m_hShareMem = ::OpenFileMappingA(FILE_MAP_ALL_ACCESS ,FALSE , szFileName);

			if (m_hShareMem)
			{
				m_pBaseMem = ret  = ::MapViewOfFile( m_hShareMem,FILE_MAP_ALL_ACCESS,0,0,0 );
			}
		}


		return ret;
	}

	void  ShareMemry::CloseShareMemry(char* szFileName)
	{
		if (m_plock)
		{
			UnmapViewOfFile(m_plock);
			m_plock = NULL;
		}

		if (m_pBaseMem)
		{
			UnmapViewOfFile(m_pBaseMem);
			m_pBaseMem = NULL;
		}

		if (m_hLock)
		{
			CloseHandle(m_hLock);
		}

		if (m_hShareMem)
		{
			CloseHandle(m_hShareMem);
		}

	}

	void* ShareMemry::CreateShareMemry(char* szLockName,char* szFileName,DWORD dwMemSize ,BOOL bUpdate )
	{//创建数据者调用
		if (szLockName && szFileName )
		{

			//锁区
			if (!m_hLock)
			{
				m_hLock = CreateFileMappingA( INVALID_HANDLE_VALUE , &m_sa , PAGE_READWRITE ,0,1,szLockName );
				if (m_hLock)
				{
					char* pLock = (char*)::MapViewOfFile( m_hLock,FILE_MAP_ALL_ACCESS,0,0,0 );
					if (pLock)
					{
						m_plock = pLock;
						*pLock = 1 ;
					}
				}

			}
			else
			{
				m_hLock = ::OpenFileMappingA(FILE_MAP_ALL_ACCESS ,FALSE , szLockName);
				//int nErr = GetLastError();
				if (m_hLock)
				{
					char* pLock = (char*)::MapViewOfFile( m_hLock,FILE_MAP_ALL_ACCESS,0,0,0 );
					int nErr = GetLastError();
					if (pLock)
					{
						m_plock = pLock;
						*pLock = 1 ;
					}
				}
			}

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
				m_bCreator = TRUE;
				return (m_pBaseMem =::MapViewOfFile( m_hShareMem,FILE_MAP_ALL_ACCESS,0,0,0 ) );
			}
		}
		return NULL;
	}

	void  ShareMemry::CreateShareMemryOk(char* szLockName)
	{


		if (m_plock)
		{
			*((char*)m_plock) = 0;
			UnmapViewOfFile(m_plock);
			m_plock = NULL;
		}

		if (!m_bCreator)
		{
			return;
		}
		m_bCreator = FALSE;

		if (m_pBaseMem)
		{
			UnmapViewOfFile(m_pBaseMem);
			m_pBaseMem = NULL;
		}

		// 	if (m_hLock)
		// 	{
		// 		CloseHandle(m_hLock);
		// 	}
		// 
		// 	if (m_hShareMem)
		// 	{
		// 		CloseHandle(m_hShareMem);
		// 	}

	}
}

