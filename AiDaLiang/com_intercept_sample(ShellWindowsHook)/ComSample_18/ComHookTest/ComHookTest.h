
// ComHookTest.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CComHookTestApp:
// �йش����ʵ�֣������ ComHookTest.cpp
//

class CComHookTestApp : public CWinApp
{
public:
	CComHookTestApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CComHookTestApp theApp;