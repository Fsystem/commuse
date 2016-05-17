/*****************************************************************************
* ��  ����COMMUSE.CPP
* ��  �ܣ������ļ�
* ��  �ߣ�˫�н�
* ��  �ڣ�2010-12-29 9:03:15
******************************************************************************/
#include <stdafx.h>
#include <Windows.h>
#include "SafeCriticalSection.h "

namespace COMMUSE
{
	/*****************************************************************************
	* ��  ����CCriticalSection
	* ��  �ܣ�����
	* ��  �ߣ�˫�н�
	* ��  �ڣ�2010-12-29 9:21:41
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