/*****************************************************************************
* 文  件：SAFELIST.H
* 功  能：安全链表
* 作  者：双刃剑
* 日  期：2011-1-19 14:31:50
******************************************************************************/

#ifndef _SAFE_LIST_H_
#define _SAFE_LIST_H_
#include <list>
#include "../SafeCriticalSection/SafeCriticalSection.h"
namespace COMMUSE
{

	class SafeList
	{
	public:
		typedef std::list<void*> LISTVOID;

		SafeList();
		virtual ~SafeList();

		//in
		void	CopyInList(SafeList& List);
		void	CopyInList(void** List,int nCount);
		void	CopyInList(std::list<void*>& List);

		//out
		void	CopyOutList(SafeList& List);
		int		CopyOutList(void* List[]);
		void	CopyOutList(std::list<void*>& List);


		bool	Insert(void* pData);
		bool	Delete(void* pData);
		bool	IsEmpty();

		void*	RemoveHead();
		void	RemoveNull();

		LISTVOID::iterator Begin()
		{
			return m_list.begin();
		};
		LISTVOID::iterator End()
		{
			return m_list.end();
		};

		void Clear()
		{	
			LOCK lk(m_cs);

			m_list.clear();


		};

		void Lock()
		{
			m_cs.Enter();
		}

		void UnLock()
		{
			m_cs.Leave();
		}


		int GetCount()
		{
			return m_list.size();
		};




	protected:
	private:
		LISTVOID	m_list;
		CCritiSection m_cs;

	};
}




#endif