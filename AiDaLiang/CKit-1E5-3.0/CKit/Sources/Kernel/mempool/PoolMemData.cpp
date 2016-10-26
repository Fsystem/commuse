#include "stdafx.h"
#include "PoolMemData.h"


CPoolMemData::CPoolMemData(void)
{ 
}

CPoolMemData::~CPoolMemData(void)
{
} 

PVOID	CPoolMemData::GetBuffer(DWORD size)
{
	m_data_lock.Lock();

	PVOID		lpret = NULL;

	IO_KEY_LIST::iterator iter = m_io_map.find(size);

	//分配内存，如果内存充足，就直接退出
	if (iter == m_io_map.end())
	{
		IO_LIST	io_list;

		for (int i = 0; i != 10; i++)
		{
			PVOID p = (PVOID)GlobalAlloc(GPTR, size);

			if (p == NULL)
			{
				break;
			}

			io_list.push_back(p);
		}

		m_io_map[size] = io_list;

	}
	else
	{
		if (iter->second.size() == 0)
		{
			for (int i = 0; i != 10; i++)
			{
				PVOID p = (PVOID)GlobalAlloc(GPTR, size);

				if (p == NULL)
				{ 
					break;
				}

				iter->second.push_back(p);
			}
		}
	}

	iter = m_io_map.find(size);

	if (iter != m_io_map.end() && iter->second.size())
	{
		lpret = iter->second.front();
		iter->second.pop_front();
	}
	
	m_data_lock.Unlock();

	return lpret;
}


void	CPoolMemData::FreeBuffer(PVOID p,DWORD size)
{
	m_data_lock.Lock();

	PVOID		lpret = NULL;

	IO_KEY_LIST::iterator iter = m_io_map.find(size);

	if (iter != m_io_map.end())
	{
		iter->second.push_back(p);
	}
	else
	{
		printf("奇怪的错误，一个找不到归属的变量！！！\r\n");
	}

	m_data_lock.Unlock();
}


void	CPoolMemData::FreeAll()
{
	m_data_lock.Lock();
	 
	for (IO_KEY_LIST::iterator map_iter = m_io_map.begin(); map_iter != m_io_map.end(); map_iter++)
	{
		for(IO_LIST::iterator iter = map_iter->second.begin(); iter != map_iter->second.end(); iter++)
		{
				GlobalFree((PVOID)*iter);
		}

		map_iter->second.clear();
	} 

	m_io_map.clear();
	m_data_lock.Unlock();
}