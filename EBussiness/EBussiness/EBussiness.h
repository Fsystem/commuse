
// EBussiness.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
//#include "EBussinessDlg.h"
// CEBussinessApp:
// �йش����ʵ�֣������ EBussiness.cpp
//

class CEBussinessApp : public CWinAppEx
{
public:
	CEBussinessApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();

public:
	inline CEDispath* getDispath(){return m_pDispath;}

protected:
	CEDispath*	m_pDispath;
	//CEBussinessDlg* m_pDlg;
};

extern CEBussinessApp theApp;