
// md5_password_xor_encrypt_decrypt.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// Cmd5_password_xor_encrypt_decryptApp:
// 有关此类的实现，请参阅 md5_password_xor_encrypt_decrypt.cpp
//

class Cmd5_password_xor_encrypt_decryptApp : public CWinAppEx
{
public:
	Cmd5_password_xor_encrypt_decryptApp();

// 重写
	public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern Cmd5_password_xor_encrypt_decryptApp theApp;