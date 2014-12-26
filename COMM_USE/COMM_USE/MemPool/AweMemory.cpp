/************************************************************************/
/* 内存池源码                                                            */
/*作者：双刃剑															*/
/*时间:2010-10-21														*/
/************************************************************************/
#pragma once
//#include "stdafx.h"
#include <Windows.h>
#include "AweMemory.h"


/************************************************************************/
/* 变量声明                                                              */
DWORD		dwMemPageSize			= 0;	//默认内存页的大小

DWORD		dwStartingBytes			= 0;	//操作系统64K内存区以后的向量值，具体参考virtualalloc

DWORD		dwNeedPages				= 0;	//所需的页数


DWORD		dwRealPageSize			= 256;	//在一页中分成小页的大小

DWORD		dwRealPages				= 0;	//小页实际所需的页数

DWORD		dwTotalBytes			= 0;	//通过换算得出实际的内存大小



ULONG_PTR*	pUlPfnArray				= NULL;	//页框号数组

void*		pAddrHead				= NULL;	//内存的首地址

void*		pAddrTail				= NULL;	//内存的尾地址

BOOL		bIsActive				= FALSE;//标识内存存活



/************************************************************************/

/************************************************************************/
MemList*	pMemlistHead			= NULL ;
/************************************************************************/

BOOL SetPrivileges(BOOL bActive)
{
	BOOL	result = FALSE;
	HANDLE  hToken;
	//打开进程，取得令牌hToken
	if ( !OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES,&hToken) )
	{
		return result;
	}
	TOKEN_PRIVILEGES tp={0};
	// 获得锁定内存权限的ID
	if( !LookupPrivilegeValue(NULL,SE_LOCK_MEMORY_NAME,&tp.Privileges[0].Luid ) )
	{
		CloseHandle(hToken);
		return result;
	}
	// 设置权限信息
	tp.PrivilegeCount = 1 ;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED * ((DWORD)bActive) ;
	// 调整权限
	AdjustTokenPrivileges(hToken,FALSE,&tp,sizeof(tp),NULL,NULL);
	result = ( ERROR_SUCCESS == GetLastError() );
	CloseHandle( hToken );
	return result;
}

BOOL MemCreate(DWORD dwMemSize)
{
	if (bIsActive)
		return FALSE;

	MEMORYSTATUSEX	ms;
	SYSTEM_INFO		si;
	MemList*		pMemTemp	= NULL ;

	GetSystemInfo(&si);
	ms.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&ms);
	if ( ms.ullTotalPhys-ms.ullAvailPhys<dwMemSize )
	{
		return FALSE;
	}
	//计算计算机的页大小
	dwMemPageSize		= si.dwPageSize;
	//计算实际内存大小(分配粒度)（符合粒度的对齐的实际需要的内存大小）
	dwStartingBytes		= ( dwMemSize/si.dwAllocationGranularity+( (DWORD)(dwMemSize%si.dwAllocationGranularity > 0 ? 1 : 0) ) )
		*si.dwAllocationGranularity;
	//计算页数
	dwNeedPages			= dwStartingBytes/dwMemPageSize + (DWORD)(dwStartingBytes%dwMemPageSize > 0 ? 1 : 0);
	//申请页框号数组
	pUlPfnArray			= new ULONG_PTR[dwNeedPages];

	pAddrHead	= VirtualAlloc(NULL,dwStartingBytes,MEM_RESERVE|MEM_PHYSICAL , PAGE_READWRITE );
	if (!pAddrHead)
	{
		return FALSE;
	}
	pAddrTail	= (char*)pAddrHead+dwStartingBytes;

	//正式开始预订内存页
	BOOL bRet = FALSE;
	SetPrivileges(TRUE);
	bRet = AllocateUserPhysicalPages(GetCurrentProcess(),(PULONG_PTR)&dwNeedPages,pUlPfnArray);
	SetPrivileges(FALSE);
	if (FALSE == bRet)
	{
		VirtualFree(pAddrHead,0,MEM_RELEASE);
		delete[] pUlPfnArray;
		return FALSE;
	}

	if (!MapUserPhysicalPages(pAddrHead,dwNeedPages,pUlPfnArray))
	{
		VirtualFree(pAddrHead,0,MEM_RELEASE);
		FreeUserPhysicalPages(GetCurrentProcess(),(PULONG_PTR)&dwNeedPages,pUlPfnArray );
		delete[] pUlPfnArray;
		return FALSE;
	}
	////分成实际大小的页dwRealPageSize(实际节点的大小，通过SetPageSize函数设置)
	dwRealPages		= dwMemSize / dwRealPageSize;
	dwTotalBytes	= dwRealPages * dwRealPageSize;

	for (DWORD i = 0 ; i<dwRealPages ; i++)
	{
		pMemTemp = new MemList;
		if (!pMemlistHead)
		{
			pMemlistHead = pMemTemp;
			pMemlistHead->_pNext = NULL;
			pMemlistHead->_data = pAddrHead;
			continue;
		}
		pMemTemp->_pNext    = pMemlistHead ;
		pMemlistHead	    = pMemTemp;
		pMemlistHead->_data = ((char*)pAddrHead) + ( i*dwRealPageSize );
	}

	bIsActive = TRUE;

	return TRUE;

}


void MemDestroy(void)
{
	if (!pAddrHead)
	{
		return;
	}
	bIsActive = FALSE;

	MemList* pTemp;
	while(pMemlistHead)
	{
		pTemp = pMemlistHead;
		pMemlistHead = pMemlistHead->_pNext;
		delete pTemp;

	}

	MapUserPhysicalPages(pAddrHead,dwNeedPages,NULL);
	FreeUserPhysicalPages(GetCurrentProcess(),(PULONG_PTR)&dwNeedPages,pUlPfnArray );
	VirtualFree(pAddrHead,0,MEM_RELEASE);

	

	pMemlistHead = NULL;
	pAddrHead = NULL;
	pAddrTail = NULL;
	delete[] pUlPfnArray;
	pUlPfnArray = NULL;

}

void SetPageSize(DWORD dwSize)
{
	dwRealPageSize = dwSize;
}