/*****************************************************************
**
**	��Ȩ���У�	CopyRight @ 2006-2010
**
**	��    �ߣ�	
**
**	�� �� ����	yCMyseclock.cpp
**
**	����������	
**
**	����ʱ�䣺	
**
**	���ʱ�䣺	
**
**	���ݽṹ��	
**
**	�޸ļ�¼��	
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