/*****************************************************************************
* 文  件：SAFELIST.H
* 功  能：安全链表
* 作  者：双刃剑
* 日  期：2011-1-19 14:31:50
******************************************************************************/

#include <Windows.h>


#include <algorithm>
#include "SafeList.h"

namespace COMMUSE
{
	SafeList::SafeList()
	{

	}


	SafeList:: ~SafeList()
	{

	}

	//in
	void	SafeList::CopyInList(SafeList& List)
	{
		List.CopyOutList(m_list);
	}

	void	SafeList::CopyInList(void** List,int nCount)
	{
		if (List)
		{
			
			LOCK lk(m_cs);
			m_list.clear();
			for (int i = 0 ; i<nCount;i++)
			{
				m_list.push_back(List[i]);
			}
		}
		
	}

	void	SafeList::CopyInList(std::list<void*>& List)
	{
		LOCK lk(m_cs);
		m_list.clear();
		m_list = List;
	}

	//out
	void	SafeList::CopyOutList(SafeList& List)
	{
		List.Clear();

		LOCK lk(m_cs);
		for (LISTVOID::iterator it = m_list.begin() ; it != m_list.end() ; it++ )
		{
			List.Insert(*it);
		}

	}

	int	SafeList::CopyOutList(void* List[])
	{
		int nCount = 0;
		LOCK lk(m_cs);

		for( LISTVOID::iterator it = m_list.begin();it != m_list.end() ; it++)
		{
			List[nCount++] = *it;
		}

		return nCount;
	}

	void	SafeList::CopyOutList(std::list<void*>& List)
	{
		List.clear();
		LOCK lk(m_cs);
		List = m_list;
	}
		


	bool	SafeList::Insert(void* pData)
	{
		LOCK lk(m_cs);
		m_list.push_back(pData);

		return true;
	}

	bool	SafeList::Delete(void* pData)
	{
		LOCK lk(m_cs);
		m_list.remove(pData);

		return true;
	}
	bool	SafeList::IsEmpty()
	{
		return m_list.empty()?true:false;
	}

	void*	SafeList::RemoveHead()
	{
		void* p=NULL;
		
		LOCK lk(m_cs);
		if (!m_list.empty())
		{
			p = m_list.front();
			m_list.pop_front();
		}

		return p;
	}
	void	SafeList::RemoveNull()
	{//删除空 ,remove表示将值=null的单元用不等于null的单元往前移动覆盖，不删除

		LOCK lk(m_cs);
		m_list.erase(std::remove(m_list.begin(),m_list.end(),(void*)NULL),m_list.end());
	}
}

