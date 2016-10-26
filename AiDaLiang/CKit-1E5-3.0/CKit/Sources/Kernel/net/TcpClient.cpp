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
 

	//取得系统中CPU的数目,指定SysInfo.dwNumberOfProcessors*2为通信服务的工作线程数。
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
	//释放完成端口资源
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
	//创建通信服务完成端口
	m_hCOP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	if (NULL == m_hCOP)
	{
		m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"[CServerCommuTCPServer]创建完成端口失败\r\n");
		return FALSE;
	}

	//根据线程数创建工作线程
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
			m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"[CServerCommuTCPServer]创建工作线程失败\r\n");
			return FALSE;
		}
	}

	//并将此套接字同完成端口关联起来，获取AcceptEx指针。
	BOOL bSuccess = InitWinsock();
	if (!bSuccess)
	{
		//给完成端口线程发送消息，指示线程退出。
		NoteWorkThreadExit();
		CloseThreadHandle();
		CloseHandle(m_hCOP);		
		return FALSE;
	}
	return TRUE;
}


BOOL CTcpClient::StopWork()
{
	//通知所有工作线程退出,未退出的强制结束.
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
		m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"[CServerCommuTCPServer]PostOneClient创建套接字失败:%d\r\n",WSAGetLastError());
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
		m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"绑定套接字失败!\r\n");
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

	//获取微软SOCKET扩展GUIDAcceptEx函数指针
	DWORD dwResult;

	LPFN_CONNECTEX  lpConnectEx;				//连接扩展

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
		m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"[CServerCommuTCPServer]获取扩展GUIDConnectEx函数指针失败: %d", WSAGetLastError());
		closesocket(ConnectSocket);
		return FALSE;
	}

	//将新建立的套接字关联到完成端口
	HANDLE hResult = CreateIoCompletionPort(
		(HANDLE)lpConnectExIoContext->sClient,
		m_hCOP,
		(DWORD_PTR)lpConnectExIoContext,
		0);

	if(NULL == hResult)
	{ 
		closesocket(lpConnectExIoContext->sClient);		//关闭当前SOCKET
		m_share_mem.FreeBuffer(lpConnectExIoContext,sizeof(PER_IO_DATA));	//归还资源
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
	buf.len = lpPerIoContext->m_oper_try_num - lpPerIoContext->m_oper_success_num; //接收长度等于没有接收完成的长度
	lpPerIoContext->nOperationType = OP_READ;  

	int ret = WSARecv(lpPerIoContext->sClient, &buf, 1, &dwTrans, &nFlags, &lpPerIoContext->ol, NULL);
	if(SOCKET_ERROR == ret)
	{
		int error = WSAGetLastError();
		if( error != WSA_IO_PENDING)
		{  
			m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"投递失败，错误号:%d ！%s %d",error,__FILE__,__LINE__);
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

	//投递包头包尾，一起进行 
	lpPerIoContext->nOperationType = OP_WRIER;
	buf.buf = (char *)lpPerIoContext->m_buffer + lpPerIoContext->m_oper_success_num;       //投递包尾缓冲区 
	buf.len = lpPerIoContext->m_oper_try_num - lpPerIoContext->m_oper_success_num;

	ret = WSASend(lpPerIoContext->sClient, &buf, 1, &dwRecv, dwFlags, &(lpPerIoContext->ol), NULL);
	if( SOCKET_ERROR == ret)
	{
		int error = WSAGetLastError();
		if( error != WSA_IO_PENDING)
		{    
			m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"投递发送失败，错误号:%d ！%s %d",
						error,__FILE__,__LINE__);
			return FALSE;
		}
	}
	return TRUE;
}

//给外部调用发送接口，为了实现，我分配内存，我释放内存
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
		m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"客户端，发送数据：【%s】\r\n\r\n",lpper_io_data->m_buffer);
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

//清理关联资源，主要是把缓存中，对应的这个连接所有资源清空，并在向客户端发送的包中，加一个断开连接包
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
				m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"[CServerCommuTCPServer]线程接收退出通知\r\n");
				//PostQueuedCompletionStatus发过来一个空的单句柄数据，表示线程要退出了。
				return 0;
			}

			if (FALSE == bSuccess)//此处是因为客户端异常关闭，造成套接字I/O停止；或是由于操作I/O的线程退出，也会造成false
			{
				//客户端已关闭				
				switch(lpPerIoContext->nOperationType)
				{
				case OP_WRIER://发送出错，归还发送数据的上下文信息
					m_share_mem.FreeBuffer(lpPerIoContext,sizeof(PER_IO_DATA));
					break;
				case OP_CONNECT:
					{
						DWORD err = WSAGetLastError();
						printf("连接失败，错误号：%d \r\n",err);
					}
				case OP_READ:
				default://释放接收数据的上下文信息
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
			case OP_WRIER://发送完成，归还发送数据的上下文信息
				m_share_mem.FreeBuffer(lpPerIoContext,sizeof(PER_IO_DATA));
				break;

			case OP_CONNECT:
				 
				//开始接收第一个包
				memset(lpPerIoContext->m_buffer,0,sizeof(lpPerIoContext->m_buffer));
				lpPerIoContext->m_oper_try_num = MAX_LEN;
				lpPerIoContext->m_oper_success_num = 0;

				if(RecvData(lpPerIoContext) == FALSE)		//从服务器接收
				{
					HandDataClose(lpPerIoContext);
					closesocket(lpPerIoContext->sClient);		//关闭当前SOCKET
					m_share_mem.FreeBuffer(lpPerIoContext,sizeof(PER_IO_DATA));	//归还资源
					continue;
				}

				ConnectSuccess(lpPerIoContext);		//通知应用程序，连接成功了，可以发送数据了

				break;
			case OP_READ:
				if (dwNumberBytes == 0)
				{
					HandDataClose(lpPerIoContext);
					closesocket(lpPerIoContext->sClient);		//关闭当前SOCKET
					m_share_mem.FreeBuffer(lpPerIoContext,sizeof(PER_IO_DATA));	//归还资源
					continue;
				}

				lpPerIoContext->m_oper_success_num += dwNumberBytes;

				dwRet = HandRecvData(lpPerIoContext);

				if(dwRet == RECV_MORE)
				{
					//数据不够，继续接收
					lpPerIoContext->m_oper_try_num = MAX_LEN;
					if(RecvData(lpPerIoContext) == FALSE)		//从服务器接收
					{
						HandDataClose(lpPerIoContext);
						closesocket(lpPerIoContext->sClient);		//关闭当前SOCKET
						m_share_mem.FreeBuffer(lpPerIoContext,sizeof(PER_IO_DATA));	//归还资源
						continue;
					}
					break;
				}

				if (dwRet == HAND_ERROR)
				{
					HandDataClose(lpPerIoContext);
					closesocket(lpPerIoContext->sClient);		//关闭当前SOCKET
					m_share_mem.FreeBuffer(lpPerIoContext,sizeof(PER_IO_DATA));	//归还资源
					continue;
				}

				memset(lpPerIoContext->m_buffer,0,sizeof(lpPerIoContext->m_buffer));
				lpPerIoContext->m_oper_try_num = MAX_LEN;
				lpPerIoContext->m_oper_success_num = 0;

				if(RecvData(lpPerIoContext) == FALSE)		//从服务器接收
				{
					HandDataClose(lpPerIoContext);
					closesocket(lpPerIoContext->sClient);		//关闭当前SOCKET
					m_share_mem.FreeBuffer(lpPerIoContext,sizeof(PER_IO_DATA));	//归还资源
					continue;
				}
				
				break;
			default://释放接收数据的上下文信息 
				break;
			}
		}
		catch(...)
		{
			printf("[CServerCommuTCPServer]CompletionWorkThread异常:%d\r\n",WSAGetLastError());
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