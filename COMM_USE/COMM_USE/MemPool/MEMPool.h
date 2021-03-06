/************************************************************************/
/*内存池源码															*/
/*作者：双刃剑															*/
/*时间:2010-10-21														*/
/************************************************************************/
#pragma once

#include <Windows.h>
#include "AweMemory.h"
#include <math.h>
namespace COMMUSE
{
	template <class T>
	class AWEMEMPool
	{
	public:
		AWEMEMPool(unsigned int unNum);
		virtual ~AWEMEMPool();
		T**   Allocate();
		void Free(T** ppData);
		__inline void Lock()
		{
			EnterCriticalSection(&m_cs_memlist);
		}
		__inline void UnLock()
		{
			LeaveCriticalSection(&m_cs_memlist);
		}

	protected:
	private:
		BOOL					m_bInit;
		MemList*				m_pMemListHead;//头永远指向可用，当 == null时表示无节点可用
		CRITICAL_SECTION		m_cs_memlist;
	};

	template <class T >	
	AWEMEMPool<T>::AWEMEMPool(unsigned int unNum)
	{
		unsigned int unMemSize = unNum*sizeof(T);
		SetPageSize(sizeof(T));
		if ( !MemCreate(unMemSize) )
		{
			m_bInit = false;
			return;
		}
		
		InitializeCriticalSection(&m_cs_memlist);
		m_pMemListHead	= pMemlistHead ;
		m_bInit = TRUE;
	}

	template <class T >	
	AWEMEMPool<T>::~AWEMEMPool()
	{
		if ( m_bInit )
		{
			DeleteCriticalSection(&m_cs_memlist);
			MemDestroy();
		}
		
	}

	template <class T >	
	T**   AWEMEMPool<T>::Allocate()
	{
		T**  pRet = NULL;
		if (!m_bInit)
		{
			return pRet;
		}
		Lock();
		if (m_pMemListHead)
		{
			pRet = (T**)( &m_pMemListHead->_data );
			m_pMemListHead = m_pMemListHead->_pNext;
		}
		UnLock();
		return pRet;
	}

	template <class T >	
	void  AWEMEMPool<T>::Free(T** ppData)
	{
		if (!m_bInit)
		{
			return;
		}
		if (ppData)
		{
			Lock();
			MemList* pTem = CONTAINING_RECORD(ppData,MemList,_data);//(MemList*)ppData ;
			pTem->_pNext = m_pMemListHead;
			m_pMemListHead = pTem;
			UnLock();
		}

	}

	//-------------------------------------------------------------------------------


	/**	普通内存池
	*	classname  : MemPool
	*	author	   : double sword
	*	effect     : 普通内存池
	*	datatime   : 2011-2-10
	*/
	template<class T>
	class MEMPool
	{
	public:
		MEMPool(unsigned int nCount);
		~MEMPool();
		T** Allocate();
		void Free(T** pAllocate);
		void Clear();

	protected:
	private:
		HANDLE	 m_hMemPool;
		unsigned m_uNum;		//内存节点个数
		bool	 m_bInit;
		MemList* m_pList;		//内存链表
		MemList* m_pListHead;	//内存链表头

		CRITICAL_SECTION m_cs;

		__inline void Lock()
		{
			EnterCriticalSection(&m_cs);
		}
		__inline void UnLock()
		{
			LeaveCriticalSection(&m_cs);
		}

	};
	//-----------------------------------------------------------------------------
	//cpp==============================================
	template<class T>
	MEMPool<T>::MEMPool(unsigned int nCount)
	{
		m_pListHead				= NULL;
		m_pList					= NULL;
		m_bInit					= false;
		m_hMemPool				= NULL;
		m_uNum					= nCount;
		unsigned uTSize			= sizeof(T);
		unsigned uDefPageSize	= 0;
		unsigned uNeedPage		= 0;
		void*	 pAllocAddr		= NULL;
		unsigned int nMemSize	= 0;
		SYSTEM_INFO si;
		GetSystemInfo(&si);
		uDefPageSize = si.dwPageSize;

		if ( uTSize > uDefPageSize )
		{
			uNeedPage = (unsigned)ceil( (double)(uTSize - uDefPageSize)*nCount/(double)uDefPageSize ) ;
			nMemSize = uDefPageSize*( m_uNum + uNeedPage );
			m_hMemPool = HeapCreate(HEAP_ZERO_MEMORY , nMemSize , 0 );
		}
		else
		{
			nMemSize = uTSize*m_uNum;
			m_hMemPool = HeapCreate(HEAP_ZERO_MEMORY , nMemSize , 0 );
		}

		if (m_hMemPool)
		{
			m_pList		= new MemList[m_uNum];
			if (  m_pList )
			{
				memset( m_pList,0,m_uNum*sizeof(MemList) );
#if (_WIN32_WINNT >= 0x400 )
				InitializeCriticalSectionAndSpinCount(&m_cs , 2000 );
#else
				InitializeCriticalSection(&m_cs);
#endif
				nCount = 0;
				m_pListHead = m_pList;
				for ( unsigned int i = 0 ; i < m_uNum ; i++ )
				{
					pAllocAddr = HeapAlloc(m_hMemPool,HEAP_ZERO_MEMORY,uTSize );
					if (NULL != pAllocAddr)
					{
						nCount++;
						m_pList[i]._data = (T*)pAllocAddr;
					}
					
					if ( i == m_uNum - 1 )
					{
						m_pList[i]._pNext = NULL;
					}
					else
					{
						m_pList[i]._pNext = &m_pList [ i + 1 ];
					}
				}
				m_uNum = nCount;
				m_bInit = true;
			}
		}
		
		
	}
	//-------------------------------------------------------------------------------
	template<class T>
	void MEMPool<T>::Clear()
	{
		m_pListHead = m_pList;
	}
	//-----------------------------------------------------------------------------
	template<class T>
	MEMPool<T>::~MEMPool()
	{
		if ( m_bInit && m_hMemPool )
		{
			Lock();
			if ( m_pList )
			{
				for (unsigned i  = 0 ; i < m_uNum ; i++)
				{
					if ( m_pList[i]._data )
					{
						HeapFree(m_hMemPool , 0 ,m_pList[i]._data );
					}	
				}

				delete[] m_pList;
				m_pList = NULL;
			}
			
			m_bInit = false;
			UnLock();
			DeleteCriticalSection(&m_cs);
			//摧毁内存堆
			HeapDestroy(m_hMemPool);
			m_hMemPool = NULL;
			
		}
	}
	//-------------------------------------------------------------------------------

	/*
	*funcname : Allocate
	*author      : double sword
	*efect        : 分配一个可用节点的指针
	*return       : 返回一个可用节点的指针，NULL为没有
	*args         : 无
	*datetime   : 2011-2-10
	*/
	template<class T>
	T** MEMPool<T>::Allocate()
	{
		T** ppRes = NULL;
		if ( m_bInit )
		{
			Lock();
			if(m_pListHead)
			{
				ppRes = (T**) &m_pListHead->_data ;
				m_pListHead = m_pListHead->_pNext;
			}
			
			UnLock();
		}

		return ppRes;
	}

	/*
	*funcname : Free
	*author      : double sword
	*efect        : 归还节点
	*return	    : 无
	*args        : pAllocate（allocate返回的指针）
	*datetime : 2011-2-10
	*/
	template<class T>
	void MEMPool<T>::Free(T** pAllocate)
	{
		if ( m_bInit )
		{
			Lock();
			if ( pAllocate )
			{
				MemList* pTemp = (MemList*)pAllocate;
				pTemp->_pNext = m_pListHead;
				m_pListHead = pTemp;
			}
			UnLock();
		}
	}
}


