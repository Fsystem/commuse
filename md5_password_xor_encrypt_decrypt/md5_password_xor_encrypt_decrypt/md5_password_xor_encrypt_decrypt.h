
// md5_password_xor_encrypt_decrypt.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// Cmd5_password_xor_encrypt_decryptApp:
// �йش����ʵ�֣������ md5_password_xor_encrypt_decrypt.cpp
//

class Cmd5_password_xor_encrypt_decryptApp : public CWinAppEx
{
public:
	Cmd5_password_xor_encrypt_decryptApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern Cmd5_password_xor_encrypt_decryptApp theApp;