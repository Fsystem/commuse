// VersionControl.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// ������


// CVersionControlApp:
// �йش����ʵ�֣������ VersionControl.cpp
//

class CVersionControlApp : public CWinApp
{
public:
	CVersionControlApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CVersionControlApp theApp;
