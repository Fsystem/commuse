// AutoPatch.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// ������


// CAutoPatchApp:
// �йش����ʵ�֣������ AutoPatch.cpp
//

class CAutoPatchApp : public CWinApp
{
public:
	enum
	{
		eCloneFile = 1<<0,
	};
public:
	CAutoPatchApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CAutoPatchApp theApp;
