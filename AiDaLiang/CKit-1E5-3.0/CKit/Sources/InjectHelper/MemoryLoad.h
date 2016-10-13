// MemoryLoad.h: interface for the CMemoryLoad class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMORYLOAD_H__FA56EDD8_59D3_4C15_9EE7_D8E2DBBAE7E5__INCLUDED_)
#define AFX_MEMORYLOAD_H__FA56EDD8_59D3_4C15_9EE7_D8E2DBBAE7E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/************************************************************************/
/* 函数是从  MemoryModule.h 发展而来
主要是实现加载DLL后，如果有依赖项，从本地目录也使用内存加载进来
                                                                   */
/************************************************************************/
#include "MemoryModule.h"
#include <map>

namespace MEM
{
	
	typedef	std::map<HCUSTOMMODULE,DWORD>		FILE_HCUSTOMMODULE_MAP;
	
	void MyCustomFreeLibraryFunc(HCUSTOMMODULE hdll, void * userdata);
	FARPROC MyCustomGetProcAddressFunc(HCUSTOMMODULE hdll, LPCSTR name, void * userdata);
	HCUSTOMMODULE MyCustomLoadLibraryFunc(LPCSTR name, void * userdata);
 
	HMEMORYMODULE MyMemoryLoadLibraryEx(const void *buffer,void * userdata);
}

#endif // !defined(AFX_MEMORYLOAD_H__FA56EDD8_59D3_4C15_9EE7_D8E2DBBAE7E5__INCLUDED_)
