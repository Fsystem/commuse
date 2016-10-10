/*****************************************************************
**
**	版权所有：	CopyRight @ 2006-2010
**
**	作    者：	
**
**	文 件 名：	yCMyseclock.h
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
