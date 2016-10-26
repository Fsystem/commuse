#include "StdAfx.h"
#include "TcpClient.h"
#include <mswsock.h>
 

static	GUID g_GuidConnectEx=WSAID_CONNECTEX;
 

CTcpClient::CTcpClient(void)
{
	for (int i = 0; i < MAXTHREAD_COUNT; i++)
	{
		m_hThreadArray[i] = INVALID_HANDLE_VALUE;
	}
 

	//ȡ��ϵͳ��CPU����Ŀ,ָ��SysInfo.dwNumberOfProcessors*2Ϊͨ�ŷ���Ĺ����߳�����
	SYSTEM_INFO SysInfo;
	GetSystemInfo(&SysInfo);
	m_ThreadNum = SysInfo.dwNumberOfProcessors*2+2;	
	if (MAXTHREAD_COUNT < m_ThreadNum)
	{
		m_ThreadNum = MAXTHREAD_COUNT;
	} 
}

CTcpClient::~CTcpClient(void)
{
	//�ͷ���ɶ˿���Դ
	if(m_hCOP)
		CloseHandle(m_hCOP);

	m_share_mem.FreeAll();
}

void CTcpClient::CloseThreadHandle()
{
	for (DWORD i = 0; i < m_ThreadNum; i++)
	{
		if (INVALID_HANDLE_VALUE != m_hThreadArray[i])
		{
			CloseHandle(m_hThreadArray[i]);
			m_hThreadArray[i] = INVALID_HANDLE_VALUE;
		}
	}	
}

void CTcpClient::NoteWorkThreadExit()
{
	for(DWORD i = 0; i < m_ThreadNum; i++)
	{
		PostQueuedCompletionStatus(m_hCOP, 0, NULL, NULL);
		Sleep(500);
	}	
}

BOOL CTcpClient::InitWinsock()
{	 

	

	return TRUE;
}

DWORD WINAPI CTcpClient::CompletionWorkThread(LPVOID Param)
{
	CTcpClient	*lpthis = (CTcpClient*)Param;

	lpthis->CompletionWork();

	return 0;
}
 
 

BOOL CTcpClient::StartWork()
{ 
	WORD version;
	WSAData data;
	version = MAKEWORD(2,2);
	WSAStartup(version,&data);
	//����ͨ�ŷ�����ɶ˿�
	m_hCOP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	if (NULL == m_hCOP)
	{
		m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"[CServerCommuTCPServer]������ɶ˿�ʧ��\r\n");
		return FALSE;
	}

	//�����߳������������߳�
	for (DWORD i = 0; i < m_ThreadNum; i++)
	{
		m_hThreadArray[i] = CreateThread(NULL, 0, CompletionWorkThread, (LPVOID)this,0,NULL);
		if (NULL == m_hThreadArray[i])
		{
			while (i > 0)
			{
				CloseHandle(m_hThreadArray[i-1]);
				m_hThreadArray[i-1] = INVALID_HANDLE_VALUE;
				i--;
			}
			CloseHandle(m_hCOP);
			m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"[CServerCommuTCPServer]���������߳�ʧ��\r\n");
			return FALSE;
		}
	}

	//�������׽���ͬ��ɶ˿ڹ�����������ȡAcceptExָ�롣
	BOOL bSuccess = InitWinsock();
	if (!bSuccess)
	{
		//����ɶ˿��̷߳�����Ϣ��ָʾ�߳��˳���
		NoteWorkThreadExit();
		CloseThreadHandle();
		CloseHandle(m_hCOP);		
		return FALSE;
	}
	return TRUE;
}


BOOL CTcpClient::StopWork()
{
	//֪ͨ���й����߳��˳�,δ�˳���ǿ�ƽ���.
	NoteWorkThreadExit();
	for (DWORD i = 0; i < m_ThreadNum; i++)
	{
		if (INVALID_HANDLE_VALUE == m_hThreadArray[i])
		{
			continue;
		}

		DWORD dwResult = WaitForSingleObject(m_hThreadArray[i], 1000);
		if (WAIT_OBJECT_0 != dwResult)
		{
			TerminateThread(m_hThreadArray[i], 0); 
		}

		CloseHandle(m_hThreadArray[i]);
		m_hThreadArray[i] = INVALID_HANDLE_VALUE;
	}
	return true;
}

BOOL CTcpClient::MyConnect(const char * ip,int	port,PVOID Ext_Data)
{ 
	struct hostent *phe = NULL;
	SOCKET ConnectSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP,
		NULL, 0, WSA_FLAG_OVERLAPPED); 	
	if (INVALID_SOCKET == ConnectSocket)
	{
		m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"[CServerCommuTCPServer]PostOneClient�����׽���ʧ��:%d\r\n",WSAGetLastError());
		return FALSE;
	}
 
	sockaddr_in addr;
	if ((addr.sin_addr.s_addr = inet_addr (ip)) == (unsigned long int) -1)
	{
		if ((phe = gethostbyname (ip)) == NULL)
		{
			return 0;
		}

		memcpy ((char *)&addr.sin_addr, phe->h_addr, phe->h_length);
	}

	addr.sin_family=AF_INET;
	addr.sin_port=htons(port);

	SOCKADDR_IN local;
	local.sin_family = AF_INET;
	local.sin_addr.S_un.S_addr = INADDR_ANY;
	local.sin_port = 0;

	if(SOCKET_ERROR == bind(ConnectSocket, (LPSOCKADDR)&local, sizeof(local)))
	{
		m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"���׽���ʧ��!\r\n");
		return FALSE;
	}

	PPER_IO_DATA lpConnectExIoContext = (PPER_IO_DATA)m_share_mem.GetBuffer(sizeof(PER_IO_DATA));
	if (lpConnectExIoContext == NULL)
	{
		closesocket(ConnectSocket);
		return FALSE;
	}

	lpConnectExIoContext->clear();
	lpConnectExIoContext->sClient = ConnectSocket;
	lpConnectExIoContext->nOperationType = OP_CONNECT;
	lpConnectExIoContext->ext_data = Ext_Data;

	//��ȡ΢��SOCKET��չGUIDAcceptEx����ָ��
	DWORD dwResult;

	LPFN_CONNECTEX  lpConnectEx;				//������չ

	int nResult = WSAIoctl(ConnectSocket,
		SIO_GET_EXTENSION_FUNCTION_POINTER,
		&g_GuidConnectEx,
		sizeof(g_GuidConnectEx),
		&lpConnectEx,
		sizeof(lpConnectEx),
		&dwResult,
		NULL,
		NULL
		);

	if (SOCKET_ERROR == nResult)
	{ 
		m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"[CServerCommuTCPServer]��ȡ��չGUIDConnectEx����ָ��ʧ��: %d", WSAGetLastError());
		closesocket(ConnectSocket);
		return FALSE;
	}

	//���½������׽��ֹ�������ɶ˿�
	HANDLE hResult = CreateIoCompletionPort(
		(HANDLE)lpConnectExIoContext->sClient,
		m_hCOP,
		(DWORD_PTR)lpConnectExIoContext,
		0);

	if(NULL == hResult)
	{ 
		closesocket(lpConnectExIoContext->sClient);		//�رյ�ǰSOCKET
		m_share_mem.FreeBuffer(lpConnectExIoContext,sizeof(PER_IO_DATA));	//�黹��Դ
		return FALSE;
	}

	DWORD dwBytes = 0;
	if(!lpConnectEx(ConnectSocket,(const sockaddr*)&addr,sizeof(addr),NULL,0,&dwBytes,&lpConnectExIoContext->ol))
	{

		DWORD dwError=WSAGetLastError();
		if(ERROR_IO_PENDING != dwError)
		{
			m_share_mem.FreeBuffer(lpConnectExIoContext,sizeof(PER_IO_DATA));
			return FALSE;
		}
	}
 
	return	TRUE;
} 

BOOL	CTcpClient::RecvData(PPER_IO_DATA lpPerIoContext)
{
	WSABUF buf;
	DWORD dwTrans,nFlags = 0;

	buf.buf = (char *)lpPerIoContext->m_buffer + lpPerIoContext->m_oper_success_num;
	buf.len = lpPerIoContext->m_oper_try_num - lpPerIoContext->m_oper_success_num; //���ճ��ȵ���û�н�����ɵĳ���
	lpPerIoContext->nOperationType = OP_READ;  

	int ret = WSARecv(lpPerIoContext->sClient, &buf, 1, &dwTrans, &nFlags, &lpPerIoContext->ol, NULL);
	if(SOCKET_ERROR == ret)
	{
		int error = WSAGetLastError();
		if( error != WSA_IO_PENDING)
		{  
			m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"Ͷ��ʧ�ܣ������:%d ��%s %d",error,__FILE__,__LINE__);
			return FALSE;
		}
	}

	return TRUE;
}


BOOL	CTcpClient::SendData(PPER_IO_DATA lpPerIoContext)
{
	WSABUF buf;
	int ret; 
	DWORD dwRecv;
	DWORD dwFlags = 0; 

	//Ͷ�ݰ�ͷ��β��һ����� 
	lpPerIoContext->nOperationType = OP_WRIER;
	buf.buf = (char *)lpPerIoContext->m_buffer + lpPerIoContext->m_oper_success_num;       //Ͷ�ݰ�β������ 
	buf.len = lpPerIoContext->m_oper_try_num - lpPerIoContext->m_oper_success_num;

	ret = WSASend(lpPerIoContext->sClient, &buf, 1, &dwRecv, dwFlags, &(lpPerIoContext->ol), NULL);
	if( SOCKET_ERROR == ret)
	{
		int error = WSAGetLastError();
		if( error != WSA_IO_PENDING)
		{    
			m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"Ͷ�ݷ���ʧ�ܣ������:%d ��%s %d",
						error,__FILE__,__LINE__);
			return FALSE;
		}
	}
	return TRUE;
}

//���ⲿ���÷��ͽӿڣ�Ϊ��ʵ�֣��ҷ����ڴ棬���ͷ��ڴ�
BOOL CTcpClient::SendData( const void * data,DWORD data_len,SOCKET s)
{
	PPER_IO_DATA	lpper_io_data = (PPER_IO_DATA)m_share_mem.GetBuffer(sizeof(PER_IO_DATA));

	if (lpper_io_data == NULL)
	{
		return FALSE;
	}

	lpper_io_data->clear();
	lpper_io_data->nOperationType = OP_WRIER;
	memcpy(lpper_io_data->m_buffer,data,data_len);
	lpper_io_data->m_oper_try_num = data_len;
	lpper_io_data->m_oper_success_num = 0;
	lpper_io_data->sClient = s;

	BOOL	ret = SendData(lpper_io_data);

	if (ret == FALSE)
	{
		m_share_mem.FreeBuffer(lpper_io_data,sizeof(PER_IO_DATA));
	}
	else
	{
		lpper_io_data->m_buffer[lpper_io_data->m_oper_try_num] = '\0';
		m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"�ͻ��ˣ��������ݣ���%s��\r\n\r\n",lpper_io_data->m_buffer);
	}

	return	ret;
}

BOOL CTcpClient::SendDataForClose(SOCKET s)
{ 

	shutdown(s,2);
	closesocket(s);

	return	TRUE;
}

void CTcpClient::GetClientIPPort(SOCKET clientSock, char* pszIPAddr,int len, DWORD &dPeerport)
{ 
	SOCKADDR_IN addr = {0};
	int sock_size = sizeof(SOCKADDR);

	if (-1 == getpeername(clientSock, (SOCKADDR *)&addr, &sock_size))
	{ 
		return;
	}

	sprintf_s(pszIPAddr,len, 
		"%u.%u.%u.%u", 
		addr.sin_addr.S_un.S_un_b.s_b1,
		addr.sin_addr.S_un.S_un_b.s_b2,
		addr.sin_addr.S_un.S_un_b.s_b3,
		addr.sin_addr.S_un.S_un_b.s_b4);

	dPeerport = ntohs(addr.sin_port);
}

//���������Դ����Ҫ�ǰѻ����У���Ӧ���������������Դ��գ�������ͻ��˷��͵İ��У���һ���Ͽ����Ӱ�
DWORD	CTcpClient::HandDataClose(PPER_IO_DATA lpPerIoContext)
{

	return 0;
}

DWORD	CTcpClient::CompletionWork()
{
	DWORD dwNumberBytes = 0;
	ULONG_PTR	 HandleContext = NULL;
	PPER_IO_DATA lpPerIoContext = NULL;
	DWORD		dwRet = 0;

	int i = sizeof(PER_IO_DATA);
	while (true)
	{
		try
		{
			lpPerIoContext = NULL;

			BOOL bSuccess = GetQueuedCompletionStatus(m_hCOP,
				&dwNumberBytes,
				(PULONG_PTR )&HandleContext,
				(LPOVERLAPPED *)&lpPerIoContext,
				WSA_INFINITE); 

			if (NULL == HandleContext)
			{
				m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"[CServerCommuTCPServer]�߳̽����˳�֪ͨ\r\n");
				//PostQueuedCompletionStatus������һ���յĵ�������ݣ���ʾ�߳�Ҫ�˳��ˡ�
				return 0;
			}

			if (FALSE == bSuccess)//�˴�����Ϊ�ͻ����쳣�رգ�����׽���I/Oֹͣ���������ڲ���I/O���߳��˳���Ҳ�����false
			{
				//�ͻ����ѹر�				
				switch(lpPerIoContext->nOperationType)
				{
				case OP_WRIER://���ͳ����黹�������ݵ���������Ϣ
					m_share_mem.FreeBuffer(lpPerIoContext,sizeof(PER_IO_DATA));
					break;
				case OP_CONNECT:
					{
						DWORD err = WSAGetLastError();
						printf("����ʧ�ܣ�����ţ�%d \r\n",err);
					}
				case OP_READ:
				default://�ͷŽ������ݵ���������Ϣ
					HandDataClose(lpPerIoContext);
					closesocket(lpPerIoContext->sClient);
					m_share_mem.FreeBuffer(lpPerIoContext,sizeof(PER_IO_DATA));
					break;
				}
				continue;
			}

			int	   nResult = 0;
			HANDLE hResult = NULL;

			switch(lpPerIoContext->nOperationType)
			{
			case OP_WRIER://������ɣ��黹�������ݵ���������Ϣ
				m_share_mem.FreeBuffer(lpPerIoContext,sizeof(PER_IO_DATA));
				break;

			case OP_CONNECT:
				 
				//��ʼ���յ�һ����
				memset(lpPerIoContext->m_buffer,0,sizeof(lpPerIoContext->m_buffer));
				lpPerIoContext->m_oper_try_num = MAX_LEN;
				lpPerIoContext->m_oper_success_num = 0;

				if(RecvData(lpPerIoContext) == FALSE)		//�ӷ���������
				{
					HandDataClose(lpPerIoContext);
					closesocket(lpPerIoContext->sClient);		//�رյ�ǰSOCKET
					m_share_mem.FreeBuffer(lpPerIoContext,sizeof(PER_IO_DATA));	//�黹��Դ
					continue;
				}

				ConnectSuccess(lpPerIoContext);		//֪ͨӦ�ó������ӳɹ��ˣ����Է���������

				break;
			case OP_READ:
				if (dwNumberBytes == 0)
				{
					HandDataClose(lpPerIoContext);
					closesocket(lpPerIoContext->sClient);		//�رյ�ǰSOCKET
					m_share_mem.FreeBuffer(lpPerIoContext,sizeof(PER_IO_DATA));	//�黹��Դ
					continue;
				}

				lpPerIoContext->m_oper_success_num += dwNumberBytes;

				dwRet = HandRecvData(lpPerIoContext);

				if(dwRet == RECV_MORE)
				{
					//���ݲ�������������
					lpPerIoContext->m_oper_try_num = MAX_LEN;
					if(RecvData(lpPerIoContext) == FALSE)		//�ӷ���������
					{
						HandDataClose(lpPerIoContext);
						closesocket(lpPerIoContext->sClient);		//�رյ�ǰSOCKET
						m_share_mem.FreeBuffer(lpPerIoContext,sizeof(PER_IO_DATA));	//�黹��Դ
						continue;
					}
					break;
				}

				if (dwRet == HAND_ERROR)
				{
					HandDataClose(lpPerIoContext);
					closesocket(lpPerIoContext->sClient);		//�رյ�ǰSOCKET
					m_share_mem.FreeBuffer(lpPerIoContext,sizeof(PER_IO_DATA));	//�黹��Դ
					continue;
				}

				memset(lpPerIoContext->m_buffer,0,sizeof(lpPerIoContext->m_buffer));
				lpPerIoContext->m_oper_try_num = MAX_LEN;
				lpPerIoContext->m_oper_success_num = 0;

				if(RecvData(lpPerIoContext) == FALSE)		//�ӷ���������
				{
					HandDataClose(lpPerIoContext);
					closesocket(lpPerIoContext->sClient);		//�رյ�ǰSOCKET
					m_share_mem.FreeBuffer(lpPerIoContext,sizeof(PER_IO_DATA));	//�黹��Դ
					continue;
				}
				
				break;
			default://�ͷŽ������ݵ���������Ϣ 
				break;
			}
		}
		catch(...)
		{
			printf("[CServerCommuTCPServer]CompletionWorkThread�쳣:%d\r\n",WSAGetLastError());
		}
	}

	return 0;
} 

DWORD	CTcpClient::HandRecvData(PPER_IO_DATA lpPerIoContext)
{

	return HAND_SUCCESS;
}


DWORD	CTcpClient::ConnectSuccess(PPER_IO_DATA lpPerIoContext)
{
	return	HAND_SUCCESS;
}