// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// CatchSocket.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"

// TODO: �� STDAFX.H ��
// �����κ�����ĸ���ͷ�ļ����������ڴ��ļ�������
DWORD GetPidBySocketLink(DWORD dwSrcIp,DWORD dwSrcPort,DWORD dwDesIp,DWORD dwDesPort)
{
	DWORD dwSize = 0;
	DWORD dwPid = 0;
	if( GetExtendedTcpTable(NULL, &dwSize, TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0)==ERROR_INSUFFICIENT_BUFFER )
	{
		PMIB_TCPTABLE_OWNER_PID pTcpTab = (PMIB_TCPTABLE_OWNER_PID)malloc(dwSize);
		if ( GetExtendedTcpTable(pTcpTab, &dwSize, TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0)==NO_ERROR )
		{
			for (DWORD i=0;i<pTcpTab->dwNumEntries;i++)
			{
				if (dwSrcIp == pTcpTab->table[i].dwLocalAddr && dwSrcPort == pTcpTab->table[i].dwLocalPort
					&& dwDesIp == pTcpTab->table[i].dwRemoteAddr && dwDesPort == pTcpTab->table[i].dwRemotePort)
				{
					dwPid = pTcpTab->table[i].dwOwningPid;
					break;
				}
			}
		}

		free(pTcpTab);
	}

	return dwPid;
}

//CheckSum:����У��͵��Ӻ���(����Ϊ��tcpα�ײ�+tcp�ײ���Ϊ������)
USHORT ChechSum(USHORT *buffer, int size)
{
	unsigned long cksum=0;
	while(size >1)
	{
		cksum+=*buffer++;
		size -=sizeof(USHORT);
	}
	if(size )
	{
		cksum += *(UCHAR*)buffer;
	}

	cksum = (cksum >> 16) + (cksum & 0xffff);
	cksum += (cksum >>16);
	return (USHORT)(~cksum);
}

// BKDR Hash Function
unsigned int BKDRHash(char *str)
{
	unsigned int seed = 1313131313; // 31 131 1313 13131 131313 etc..
	unsigned int hash = 0;

	while (*str)
	{
		hash = hash * seed + (*str++);
	}

	return (hash & 0x7FFFFFFF);
}

HMODULE GetThisModule()
{
	static HMODULE h = NULL;
	if (h == NULL)
	{
		MEMORY_BASIC_INFORMATION mbi;
		::VirtualQuery(GetThisModule,&mbi,sizeof mbi);
		h = (HMODULE)mbi.AllocationBase;
	}
	return h;
}