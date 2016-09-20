/*****************************************************************
**
**	版权所有：	CopyRight @ 2006-2010
**
**	作    者：	
**
**	文 件 名：	yCMyseclock.cpp
**
**	功能描述：	
**
**	创建时间：	
**
**	完成时间：	
**
**	数据结构：	
**
**	修改记录：	
**
*****************************************************************/
#include "stdafx.h"
#include "yCMyseclock.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

yCMyseclock::yCMyseclock()
{
	::InitializeCriticalSection(&m_sect);
}

yCMyseclock::~yCMyseclock()
{
	::DeleteCriticalSection(&m_sect);
}

 void yCMyseclock::Lock()
{
	::EnterCriticalSection(&m_sect);
}

 void yCMyseclock::Unlock()
{
	::LeaveCriticalSection(&m_sect);
}

yCMyseclock::Auto::Auto(yCMyseclock &crit):m_crit(crit)
{
	m_crit.Lock();
}

yCMyseclock::Auto::~Auto()
{
	m_crit.Unlock();
}