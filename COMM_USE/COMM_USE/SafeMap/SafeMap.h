#if !defined(  _SAFE_MAP_H_ )
#define _SAFE_MAP_H_

#include "../SafeList/SafeList.h"

namespace COMMUSE
{
	typedef std::string	STRING;
	typedef std::map <int,	 void*>  MAP_VOIDPTR;
	typedef std::map <STRING, void*>	 MAP_STRING;


	class SafeMap 
	{
	private:

		CRITICAL_SECTION	m_Lock;

		// Attributes
	public:


		// Operations
	public:
		SafeMap();
		virtual ~SafeMap();


		// Overrides
	public:

		int	 GetCountString();
		int	 GetCountInt();

		void CopyOutMap( SafeMap &pMap );

		int  CopyOutTmpList( void* pList[] );
		void CopyOutTmpList( SafeList &pList );
		void StrCopyOutTmpList(SafeList &pList );

		void CopyOutList( SafeList &pList );
		void StrCopyOutList(SafeList &pList );

		void RemoveNULL() ;
		void RemoveStrNULL() ;

		MAP_VOIDPTR		m_SafeMap;
		MAP_STRING		m_SafeMapString;

		SafeList	m_CopyList;

		void DeleteValue( void *pData );

		void Delete(MAP_STRING::iterator& v) ;
		void Delete(MAP_VOIDPTR::iterator& v);

		bool Delete(int pKey);
		bool Delete( STRING pKey);

		void Clear();

		void * Find( int pKey );
		void * Find( STRING pKey);

		bool Insert( int	pKey, void* pData );
		bool Insert( STRING pKey, void *pData );

		void Lock()
		{
			::EnterCriticalSection(&m_Lock);
		};
		void UnLock()
		{
			::LeaveCriticalSection(&m_Lock);
		};


		MAP_VOIDPTR::iterator Begin()
		{
			return m_SafeMap.begin();
		};
		MAP_VOIDPTR::iterator End()
		{
			return m_SafeMap.end();
		};

		MAP_STRING::iterator BeginString()
		{
			return m_SafeMapString.begin();
		};

		MAP_STRING::iterator EndString()
		{
			return m_SafeMapString.end();
		};


		// Implementation
	protected:
	};
}



/////////////////////////////////////////////////////////////////////////////

#endif
