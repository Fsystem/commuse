
// WinCrc32.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CWinCrc32App:
// �йش����ʵ�֣������ WinCrc32.cpp
//

class CWinCrc32App : public CWinAppEx
{
public:
	CWinCrc32App();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CWinCrc32App theApp;