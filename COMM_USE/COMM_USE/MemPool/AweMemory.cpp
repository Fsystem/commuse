/************************************************************************/
/* �ڴ��Դ��                                                            */
/*���ߣ�˫�н�															*/
/*ʱ��:2010-10-21														*/
/************************************************************************/
#pragma once
//#include "stdafx.h"
#include <Windows.h>
#include "AweMemory.h"


/************************************************************************/
/* ��������                                                              */
DWORD		dwMemPageSize			= 0;	//Ĭ���ڴ�ҳ�Ĵ�С

DWORD		dwStartingBytes			= 0;	//����ϵͳ64K�ڴ����Ժ������ֵ������ο�virtualalloc

DWORD		dwNeedPages				= 0;	//�����ҳ��


DWORD		dwRealPageSize			= 256;	//��һҳ�зֳ�Сҳ�Ĵ�С

DWORD		dwRealPages				= 0;	//Сҳʵ�������ҳ��

DWORD		dwTotalBytes			= 0;	//ͨ������ó�ʵ�ʵ��ڴ��С



ULONG_PTR*	pUlPfnArray				= NULL;	//ҳ�������

void*		pAddrHead				= NULL;	//�ڴ���׵�ַ

void*		pAddrTail				= NULL;	//�ڴ��β��ַ

BOOL		bIsActive				= FALSE;//��ʶ�ڴ���



/************************************************************************/

/************************************************************************/
MemList*	pMemlistHead			= NULL ;
/************************************************************************/

BOOL SetPrivileges(BOOL bActive)
{
	BOOL	result = FALSE;
	HANDLE  hToken;
	//�򿪽��̣�ȡ������hToken
	if ( !OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES,&hToken) )
	{
		return result;
	}
	TOKEN_PRIVILEGES tp={0};
	// ��������ڴ�Ȩ�޵�ID
	if( !LookupPrivilegeValue(NULL,SE_LOCK_MEMORY_NAME,&tp.Privileges[0].Luid ) )
	{
		CloseHandle(hToken);
		return result;
	}
	// ����Ȩ����Ϣ
	tp.PrivilegeCount = 1 ;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED * ((DWORD)bActive) ;
	// ����Ȩ��
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
	//����������ҳ��С
	dwMemPageSize		= si.dwPageSize;
	//����ʵ���ڴ��С(��������)���������ȵĶ����ʵ����Ҫ���ڴ��С��
	dwStartingBytes		= ( dwMemSize/si.dwAllocationGranularity+( (DWORD)(dwMemSize%si.dwAllocationGranularity > 0 ? 1 : 0) ) )
		*si.dwAllocationGranularity;
	//����ҳ��
	dwNeedPages			= dwStartingBytes/dwMemPageSize + (DWORD)(dwStartingBytes%dwMemPageSize > 0 ? 1 : 0);
	//����ҳ�������
	pUlPfnArray			= new ULONG_PTR[dwNeedPages];

	pAddrHead	= VirtualAlloc(NULL,dwStartingBytes,MEM_RESERVE|MEM_PHYSICAL , PAGE_READWRITE );
	if (!pAddrHead)
	{
		return FALSE;
	}
	pAddrTail	= (char*)pAddrHead+dwStartingBytes;

	//��ʽ��ʼԤ���ڴ�ҳ
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
	////�ֳ�ʵ�ʴ�С��ҳdwRealPageSize(ʵ�ʽڵ�Ĵ�С��ͨ��SetPageSize��������)
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