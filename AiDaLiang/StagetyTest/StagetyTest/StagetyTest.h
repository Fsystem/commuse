
// StagetyTest.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CStagetyTestApp:
// �йش����ʵ�֣������ StagetyTest.cpp
//

class CStagetyTestApp : public CWinApp
{
public:
	CStagetyTestApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CStagetyTestApp theApp;