
// MakeUpdateList.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMakeUpdateListApp:
// �йش����ʵ�֣������ MakeUpdateList.cpp
//

class CMakeUpdateListApp : public CWinAppEx
{
public:
	CMakeUpdateListApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMakeUpdateListApp theApp;