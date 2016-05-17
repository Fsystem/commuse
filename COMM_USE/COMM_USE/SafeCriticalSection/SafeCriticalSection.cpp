/*****************************************************************************
* 文  件：COMMUSE.CPP
* 功  能：常用文件
* 作  者：双刃剑
* 日  期：2010-12-29 9:03:15
******************************************************************************/
#include <stdafx.h>
#include <Windows.h>
#include "SafeCriticalSection.h "

namespace COMMUSE
{
	/*****************************************************************************
	* 类  名：CCriticalSection
	* 功  能：锁类
	* 作  者：双刃剑
	* 日  期：2010-12-29 9:21:41
	******************************************************************************/
	CCritiSection::CCritiSection()
	{

	#if	( _WIN32_WINNT >= 0x400 )

		InitializeCriticalSectionAndSpinCount(&crit,4000);

	#else

		InitializeCriticalSection(&crit);

	#endif

	}

	CCritiSection:: ~CCritiSection()
	{
		DeleteCriticalSection(&crit);
	}




}