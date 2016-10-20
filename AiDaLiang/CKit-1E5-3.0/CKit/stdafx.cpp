// stdafx.cpp : 只包括标准包含文件的源文件
// CommLib.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

// TODO: 在 STDAFX.H 中
// 引用任何所需的附加头文件，而不是在此文件中引用


///************************************************************************/
///* 库加载                                                               */
///************************************************************************/
//#if defined(_MT) && defined(_DLL)	//[MD]
//#	ifdef _DEBUG			//MDD
//#		ifdef _UNICODE 
//#			pragma comment(lib,"lib/ZipLibud.lib")
//#		else
//#			pragma comment(lib,"lib/ZipLibd.lib")
//#		endif
//#	else					//MD
//#		ifdef _UNICODE 
//#			pragma comment(lib,"lib/ZipLibu.lib")
//#		else
//#			pragma comment(lib,"lib/ZipLib.lib")
//#		endif
//#	endif
//#elif defined(_MT) && !defined(_DLL) //[MT]
//#	ifdef _DEBUG			//MTD
//#		ifdef _UNICODE 
//#			pragma comment(lib,"lib/ZipLibumtd.lib")
//#		else
//#			pragma comment(lib,"lib/ZipLibmtd.lib")
//#		endif
//#	else					//MT
//#		ifdef _UNICODE 
//#			pragma comment(lib,"lib/ZipLibumt.lib")
//#		else
//#			pragma comment(lib,"lib/ZipLibmt.lib")
//#		endif
//#	endif
//#endif