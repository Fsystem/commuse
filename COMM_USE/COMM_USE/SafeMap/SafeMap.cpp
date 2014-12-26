////////////////////////////////////////////////////////////////////////////////////////
//	SafeMap.cpp : implementation file
//
//
//
//
//
////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <string>
#include <map>
#include "../SafeCriticalSection/SafeCriticalSection.h"
#include "SafeMap.h"

namespace COMMUSE
{
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

	////////////////////////////////////////////////////////////////////////////////////////
	// SafeMap
	////////////////////////////////////////////////////////////////////////////////////////
	SafeMap::SafeMap()
	{
		m_SafeMap.clear();
		m_SafeMapString.clear();
		m_CopyList.Clear();

		::InitializeCriticalSectionAndSpinCount( &m_Lock, 2000 );
	}

	SafeMap::~SafeMap()
	{
		::DeleteCriticalSection( &m_Lock );
	}


	/*****************************************************************************
	* �� �� ����CopyOutMap
	* ��    �ܣ���map����������mapȫ��������
	* ���������pMap
	* ���������pMap
	* �� �� ֵ��
	* ��    �ߣ�˫�н�
	* ��    �ڣ�2011-1-25 9:28:27
	******************************************************************************/
	void SafeMap::CopyOutMap(SafeMap &pMap)
	{
		pMap.Clear() ;

		Lock();

		pMap.m_SafeMap			= m_SafeMap;
		pMap.m_SafeMapString	= m_SafeMapString;

		UnLock();
	}


	/*****************************************************************************
	* �� �� ����CopyOutTmpList
	* ��    �ܣ�int map�����ݴ�
	* ���������pList
	* ���������pList
	* �� �� ֵ��
	* ��    �ߣ�˫�н�
	* ��    �ڣ�2011-1-25 9:32:04
	******************************************************************************/
	int SafeMap::CopyOutTmpList( void*	pList[])
	{

		int nCount = 0;

		Lock();

		MAP_VOIDPTR::iterator itEnd = m_SafeMap.end() ;
		for (MAP_VOIDPTR::iterator it = m_SafeMap.begin(); it != itEnd; it++)
		{

			if( it->second != NULL)
			{
				pList[nCount++] = it->second;
			}
		}	

		UnLock();

		return	nCount;
	}


	void SafeMap::CopyOutTmpList(SafeList &pList)
	{	
		pList.Clear() ;

		Lock();

		MAP_VOIDPTR::iterator itEnd = m_SafeMap.end() ;
		for (MAP_VOIDPTR::iterator it = m_SafeMap.begin(); it != itEnd; ++it)
		{
			if( it->second != NULL )
			{
				pList.Insert( it->second );
			}
		}	

		UnLock();
	}

	/*****************************************************************************
	* �� �� ����Insert
	* ��    �ܣ�key��data�������
	* ���������pKey, pData
	* ���������
	* �� �� ֵ��true,false
	* ��    �ߣ�˫�н�
	* ��    �ڣ�2011-1-24 17:13:24
	******************************************************************************/
	bool SafeMap::Insert(int pKey, void *pData)
	{
		if( m_SafeMap.find(pKey) != m_SafeMap.end() )
			return false;

		Lock();
		m_SafeMap.insert( MAP_VOIDPTR::value_type(pKey, pData) );
		UnLock();

		return true;
	}


	/*****************************************************************************
	* �� �� ����Insert
	* ��    �ܣ���string map�������
	* ���������pKey, pData
	* ���������
	* �� �� ֵ��true,false
	* ��    �ߣ�˫�н�
	* ��    �ڣ�2011-1-24 17:22:22
	******************************************************************************/
	bool SafeMap::Insert(STRING pKey, void *pData)
	{
		if ((MAP_STRING::iterator)m_SafeMapString.find( pKey ) != m_SafeMapString.end() )
			return false ;

		Lock();

		m_SafeMapString.insert( MAP_STRING::value_type(pKey, pData) );

		UnLock();

		return true;
	}


	/*****************************************************************************
	* �� �� ����Delete
	* ��    �ܣ�ͨ��int key��ɾ������
	* ���������pKey
	* ���������
	* �� �� ֵ��bool(1,0)
	* ��    �ߣ�˫�н�
	* ��    �ڣ�2011-1-25 8:50:13
	******************************************************************************/
	bool SafeMap::Delete(int pKey)
	{
		bool	bReturn;

		Lock();

		MAP_VOIDPTR::iterator v = m_SafeMap.find( pKey );

		if (v != m_SafeMap.end() )
		{
			m_SafeMap.erase( v );

			bReturn	=	true;
		}
		else
		{
			bReturn	=	false;
		}	

		UnLock();


		return bReturn;	
	}


	/*****************************************************************************
	* �� �� ����Delete
	* ��    �ܣ�ͨ��string key��ɾ������
	* ���������pKey
	* ���������
	* �� �� ֵ��bool(1,0)
	* ��    �ߣ�˫�н�
	* ��    �ڣ�2011-1-25 8:50:13
	******************************************************************************/
	bool SafeMap::Delete(STRING pKey)
	{
		bool	bReturn;

		Lock();

		MAP_STRING::iterator v = m_SafeMapString.find( pKey );

		if (v != m_SafeMapString.end() )
		{
			m_SafeMapString.erase( v );

			bReturn	=	true;
		}
		else
		{
			bReturn	=	false;
		}	
		UnLock();	


		return bReturn;	
	}




	/*****************************************************************************
	* �� �� ����Delete
	* ��    �ܣ�ͨ��MAP_STRING::iterator&��ɾ������
	* ���������v
	* ���������v(��һ�����ݵĵ�����)
	* �� �� ֵ����
	* ��    �ߣ�˫�н�
	* ��    �ڣ�2011-1-25 8:50:13
	******************************************************************************/
	void SafeMap::Delete(MAP_STRING::iterator& v)
	{
		Lock();

		if (v != m_SafeMapString.end())
		{
			m_SafeMapString.erase( v++ );
		}

		UnLock();

		return ;	
	}

	/*****************************************************************************
	* �� �� ����Delete
	* ��    �ܣ�ͨ��MAP_VOIDPTR::iterator&��ɾ������
	* ���������v
	* ���������v(��һ�����ݵĵ�����)
	* �� �� ֵ����
	* ��    �ߣ�˫�н�
	* ��    �ڣ�2011-1-25 8:50:13
	******************************************************************************/
	void SafeMap::Delete(MAP_VOIDPTR::iterator& v)
	{

		Lock();

		if ( v != m_SafeMap.end() )
		{
			m_SafeMap.erase( v++ );
		}

		UnLock();

		return ;	
	}

	/*****************************************************************************
	* �� �� ����Find
	* ��    �ܣ�ͨ��int key��������
	* ���������pKey
	* ���������
	* �� �� ֵ��void* ���ݲ���
	* ��    �ߣ�˫�н�
	* ��    �ڣ�2011-1-25 8:59:41
	******************************************************************************/
	void* SafeMap::Find(int pKey)
	{
		MAP_VOIDPTR::iterator v;
		MAP_VOIDPTR::iterator v_end;
		Lock();

		v		= m_SafeMap.find( pKey );	
		v_end	= m_SafeMap.end();		

		UnLock();

		if ( v == v_end )
		{
			return NULL;	
		}

		return v->second;
	}

	/*****************************************************************************
	* �� �� ����Find
	* ��    �ܣ�ͨ��string key��������
	* ���������pKey
	* ���������
	* �� �� ֵ��void* ���ݲ���
	* ��    �ߣ�˫�н�
	* ��    �ڣ�2011-1-25 8:59:41
	******************************************************************************/
	void * SafeMap::Find( STRING pKey )
	{
		MAP_STRING::iterator v;
		MAP_STRING::iterator v_end;

		Lock();

		v		= m_SafeMapString.find( pKey );	//	
		v_end	= m_SafeMapString.end();		//	

		UnLock();

		// Data
		if (v == v_end )
		{
			return NULL;	
		}

		return v->second;
	}

	/*****************************************************************************
	* �� �� ����CopyOutList
	* ��    �ܣ�����Int Map���ݵ�SafeList
	* ���������
	* ���������pList
	* �� �� ֵ��
	* ��    �ߣ�˫�н�
	* ��    �ڣ�2011-1-25 9:12:19
	******************************************************************************/
	void SafeMap::CopyOutList( SafeList &pList )
	{
		pList.Clear();

		Lock();

		MAP_VOIDPTR::iterator itEnd = m_SafeMap.end() ;
		for (MAP_VOIDPTR::iterator it = m_SafeMap.begin(); it != itEnd; it++)
		{
			if( it->second != NULL )
			{
				pList.Insert( it->second );
			}
		}

		UnLock();
	}



	/*****************************************************************************
	* �� �� ����CopyOutList
	* ��    �ܣ�����String Map���ݵ�SafeList
	* ���������
	* ���������pList
	* �� �� ֵ��
	* ��    �ߣ�˫�н�
	* ��    �ڣ�2011-1-25 9:12:19
	******************************************************************************/
	void SafeMap::StrCopyOutList( SafeList & pList )
	{
		pList.Clear();

		Lock();

		MAP_STRING::iterator itEnd = m_SafeMapString.end() ;
		for (MAP_STRING::iterator it = m_SafeMapString.begin(); it != itEnd; it++)
		{
			if( it->second != NULL )
			{
				pList.Insert( it->second );
			}
		}

		UnLock();
	}

	void SafeMap::Clear()
	{	
		Lock();

		m_SafeMap.clear();
		m_SafeMapString.clear();

		UnLock();
	}

	////////////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////////////
	void SafeMap::DeleteValue( void *pData )
	{
		Lock();
		MAP_STRING::iterator itEnd = m_SafeMapString.end() ;
		for ( MAP_STRING::iterator it = m_SafeMapString.begin(); it != itEnd;)
		{
			if( it->second != NULL && it->second == pData )
			{
				m_SafeMapString.erase(it++);
			}
			else
			{
				++it ;
			}

		}
		UnLock();
	}



	////////////////////////////////////////////////////////////////////////////////////////
	//ɾ��second==null��ֵ
	////////////////////////////////////////////////////////////////////////////////////////
	void SafeMap:: RemoveNULL()
	{
		Lock();
		MAP_VOIDPTR::iterator itEnd = m_SafeMap.end();
		for ( MAP_VOIDPTR::iterator it = m_SafeMap.begin(); it != itEnd;)
		{
			if( it->second == NULL )
			{

				m_SafeMap.erase(it++);
			}
			else
			{
				++it;
			}
		}
		UnLock();	

		return ;
	}

	////////////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////////////
	void SafeMap:: RemoveStrNULL()
	{
		Lock();
		MAP_STRING::iterator itEnd = m_SafeMapString.end();
		for ( MAP_STRING::iterator it = m_SafeMapString.begin(); it != itEnd; )
		{
			if( it->second == NULL )
			{
				m_SafeMapString.erase(it++);
			}
			else
				it++ ;

		}
		UnLock();	

		return ;
	}

	////////////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////////////



	////////////////////////////////////////////////////////////////////////////////////////	
	//	INT KEY ������ ��ȯ ��Ŵ 
	////////////////////////////////////////////////////////////////////////////////////////	
	int SafeMap::GetCountInt()
	{
		return m_SafeMap.size();
	}

	////////////////////////////////////////////////////////////////////////////////////////	
	//	STRING KEY ������ ��ȯ ��Ŵ 
	////////////////////////////////////////////////////////////////////////////////////////	
	int SafeMap::GetCountString()
	{
		return m_SafeMapString.size();
	}

}

