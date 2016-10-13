/*****************************************************************
**
**	��Ȩ���У�	CopyRight @ 2006-2010
**
**	��    �ߣ�	
**
**	�� �� ����	yCMyseclock.h
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
#ifndef __YCMYSECLOCK_H__
#define __YCMYSECLOCK_H__

//#include <windows.h>

class yCMyseclock  
{
public:
	class Auto
	{
	public:
		explicit Auto(yCMyseclock &crit);
		~Auto();
	private:
		Auto(const Auto &);
		Auto& operator = (const Auto &);
	private:
		yCMyseclock &m_crit;
	};

public:
	yCMyseclock();
	virtual ~yCMyseclock();

public:
	 void Unlock();
	 void Lock();

private:
//	yCMyseclock(const yCMyseclock &);
//	yCMyseclock& operator = (const yCMyseclock &);

private:
	CRITICAL_SECTION m_sect;
};

#endif //__YCMYSECLOCK_H__
