#include "StdAfx.h"
#include "TcpServer.h"
#include <MSTcpIP.h>


GUID			m_GUIDAcceptEx = WSAID_ACCEPTEX;

CTcpServer::CTcpServer(void)
{
 
	for (int i = 0; i < MAXTHREAD_COUNT; i++)
	{
		m_hThreadArray[i] = INVALID_HANDLE_VALUE;
	}
 
	m_uPort				= DEFAULT_PORT;

	//取得系统中CPU的数目,指定SysInfo.dwNumberOfProcessors*2为通信服务的工作线程数。
	SYSTEM_INFO SysInfo;
	GetSystemInfo(&SysInfo);
	m_ThreadNum = SysInfo.dwNumberOfProcessors*2;	
	if (MAXTHREAD_COUNT < m_ThreadNum)
	{
		m_ThreadNum = MAXTHREAD_COUNT;
	} 
}

CTcpServer::~CTcpServer(void)
{
	//释放监听套接字资源
	if(INVALID_SOCKET!=m_ListenSocket)
		closesocket(m_ListenSocket);
	//释放完成端口资源
	if(m_hCOP)
		CloseHandle(m_hCOP);

	m_share_mem.FreeAll();
}


BOOL CTcpServer::StartWork(DWORD dport)
{ 
	if(0 == dport)
		return FALSE;
 
	m_uPort =dport;

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
				TerminateThread(m_hThreadArray[i - 1],0);
				CloseHandle(m_hThreadArray[i-1]);
				m_hThreadArray[i-1] = INVALID_HANDLE_VALUE;
				i--;
			}
			CloseHandle(m_hCOP);
			m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"[CServerCommuTCPServer]创建工作线程失败\r\n");
			return FALSE;
		}
	}

	m_hThreadTimeOut = CreateThread(NULL, 0, TimeOutThread, (LPVOID)this,0,NULL);

	//调用InitWinsock函数初始化Winsock、建立一个监听套接字m_ListenSocket，
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

	if(!CreateClientInfo())
	{
		NoteWorkThreadExit();
		CloseThreadHandle();
		CloseHandle(m_hCOP);
		return FALSE;
	} 

	return TRUE;
}

BOOL CTcpServer::StopWork()
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
 

void CTcpServer::CloseThreadHandle()
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

void CTcpServer::NoteWorkThreadExit()
{
	for(DWORD i = 0; i < m_ThreadNum; i++)
	{
		PostQueuedCompletionStatus(m_hCOP, 0, NULL, NULL);
		Sleep(500);
	}	

	if (m_hThreadTimeOut)
	{
		TerminateThread(m_hThreadTimeOut,0);
		m_hThreadTimeOut = NULL;
	}

}

BOOL CTcpServer::InitWinsock()
{	
	//创建监听套接字
	m_ListenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, 
		NULL, 0, WSA_FLAG_OVERLAPPED); 
	if (INVALID_SOCKET == m_ListenSocket)
	{
		int ret = WSAGetLastError();
		m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"[CServerCommuTCPServer]创建监听套接字失败: %d", WSAGetLastError());
		return TRUE;
	}

	//获取微软SOCKET扩展GUIDAcceptEx函数指针
	DWORD dwResult;
	int nResult = WSAIoctl(m_ListenSocket,
		SIO_GET_EXTENSION_FUNCTION_POINTER,
		&m_GUIDAcceptEx,
		sizeof(m_GUIDAcceptEx),
		&m_lpAcceptEx,
		sizeof(m_lpAcceptEx),
		&dwResult,
		NULL,
		NULL
		);

	if (SOCKET_ERROR == nResult)
	{ 
		m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"[CServerCommuTCPServer]获取扩展GUIDAcceptEx函数指针失败: %d", WSAGetLastError());
		closesocket(m_ListenSocket);
		return FALSE;
	}
  
	//为监听套接字分配一个单句柄数据
	PPER_IO_DATA listen_context = (PPER_IO_DATA)m_share_mem.GetBuffer(sizeof(PER_IO_DATA));

	listen_context->sClient = m_ListenSocket;

	//将监听套接字m_ListenSocket和已经建立的完成端口关联起来
	HANDLE hrc = CreateIoCompletionPort((HANDLE)m_ListenSocket,
		m_hCOP,
		(ULONG_PTR)listen_context,
		0);
	if (NULL == hrc)
	{
		closesocket(m_ListenSocket);
		m_share_mem.FreeBuffer(listen_context,sizeof(PER_IO_DATA));
		m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"[CServerCommuTCPServer]将监听套接字和完成端口关联起来失败: %d", WSAGetLastError());
		return FALSE;
	}	

	//绑定监听套接字地址和端口
	SOCKADDR_IN InternetAddr = {0};
	InternetAddr.sin_family = AF_INET;
	InternetAddr.sin_port = htons((u_short)m_uPort);

	nResult = bind(m_ListenSocket, (PSOCKADDR)&InternetAddr, sizeof(InternetAddr));
	if (SOCKET_ERROR == nResult)
	{
		m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"[CServerCommuTCPServer]将监听套接字绑定地址失败: %d", WSAGetLastError());
		closesocket(m_ListenSocket);
		m_share_mem.FreeBuffer(listen_context,sizeof(PER_IO_DATA));       
		return FALSE;
	}	

	//打开探测时间
	BOOL bKeepAlive = TRUE;
	int nRet = ::setsockopt(m_ListenSocket, SOL_SOCKET, SO_KEEPALIVE, (char*)&bKeepAlive, sizeof(bKeepAlive));
	if (nRet == SOCKET_ERROR)
	{ 
		closesocket(m_ListenSocket);
		m_share_mem.FreeBuffer(listen_context,sizeof(PER_IO_DATA)); 
		return FALSE;
	}

	// 设置KeepAlive参数
	tcp_keepalive alive_in                = {0};
	tcp_keepalive alive_out                = {0};
	alive_in.keepalivetime                = 5000;            // 开始首次KeepAlive探测前的TCP空闭时间
	alive_in.keepaliveinterval        = 100;                // 两次KeepAlive探测间的时间间隔
	alive_in.onoff                = TRUE;
	unsigned long ulBytesReturn = 0;
	nRet = WSAIoctl(m_ListenSocket, SIO_KEEPALIVE_VALS, &alive_in, sizeof(alive_in),
		&alive_out, sizeof(alive_out), &ulBytesReturn, NULL, NULL);
	if (nRet == SOCKET_ERROR)
	{
		closesocket(m_ListenSocket);
		m_share_mem.FreeBuffer(listen_context,sizeof(PER_IO_DATA)); 
		return FALSE;
	}

	//设置可以强击关闭
	struct linger so_linger;
	so_linger.l_onoff = TRUE;
	so_linger.l_linger = 0;
	nRet = setsockopt(m_ListenSocket,SOL_SOCKET,SO_LINGER,(char*)&so_linger,sizeof(so_linger));
	if (nRet != 0)
	{
		closesocket(m_ListenSocket);
		m_share_mem.FreeBuffer(listen_context,sizeof(PER_IO_DATA)); 
		return FALSE;
	}

	//开始监听客户端
	nResult = listen(m_ListenSocket, LISTEN_NUM);
	if (SOCKET_ERROR == nResult)
	{
		m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"[CServerCommuTCPServer]将监听套接监听失败: %d", WSAGetLastError());
		closesocket(m_ListenSocket);
		m_share_mem.FreeBuffer(listen_context,sizeof(PER_IO_DATA));
		return FALSE;
	}			
	return TRUE;
}

DWORD WINAPI CTcpServer::CompletionWorkThread(LPVOID Param)
{
	CTcpServer	*lpthis = (CTcpServer*)Param;

	lpthis->CompletionWork();

	return 0;
}

DWORD	WINAPI	CTcpServer::TimeOutThread(PVOID pParam)
{
	CTcpServer	*lpthis = (CTcpServer*)pParam;

	lpthis->TimeOut();

	return 0;
}
 

BOOL CTcpServer::CreateClientInfo()
{
	ULONG	m_lAcceptExCounter = 0;
	while (m_lAcceptExCounter < ACCEPTEX_NUM)
	{
		SOCKET AcceptSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP,
			NULL, 0, WSA_FLAG_OVERLAPPED); 	
		if (INVALID_SOCKET == AcceptSocket)
		{
			m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"[CServerCommuTCPServer]CreateClientInfo创建客户端套接字失败:%d\r\n",WSAGetLastError());
			return false;
		}

		PPER_IO_DATA lpAcceptExIoContext = (PPER_IO_DATA)m_share_mem.GetBuffer(sizeof(PER_IO_DATA));

		lpAcceptExIoContext->clear();
		lpAcceptExIoContext->sClient = AcceptSocket;
		lpAcceptExIoContext->nOperationType = OP_ACCEPT;
 
		DWORD dwBytes = 0;
		BOOL bSuccess = m_lpAcceptEx(
			m_ListenSocket,
			lpAcceptExIoContext->sClient,
			lpAcceptExIoContext->m_buffer,
			0,	//接收多少数据，设置为0
			sizeof(SOCKADDR_IN) + 16,
			sizeof(SOCKADDR_IN) + 16,
			&dwBytes,
			&(lpAcceptExIoContext->ol));

		if (FALSE == bSuccess)
		{
			int nResult = WSAGetLastError();
			if (nResult != ERROR_IO_PENDING)
			{
				m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"连接失败，错误号：%d  %s %d",nResult,__FILE__,__LINE__);
				m_share_mem.FreeBuffer(lpAcceptExIoContext,sizeof(PER_IO_DATA));
				return false;
			}
		}
		m_lAcceptExCounter ++; 
	} 
	return TRUE;
}


void CTcpServer::PostOneClient()
{
	SOCKET AcceptSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP,
		NULL, 0, WSA_FLAG_OVERLAPPED); 	
	if (INVALID_SOCKET == AcceptSocket)
	{
		m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"[CServerCommuTCPServer]PostOneClient创建套接字失败:%d\r\n",WSAGetLastError());
		return;
	}

	PPER_IO_DATA lpAcceptExIoContext = (PPER_IO_DATA)m_share_mem.GetBuffer(sizeof(PER_IO_DATA));

	lpAcceptExIoContext->clear();
	lpAcceptExIoContext->sClient = AcceptSocket;
	lpAcceptExIoContext->nOperationType = OP_ACCEPT;

	DWORD dwBytes = 0;
	BOOL bSuccess = m_lpAcceptEx(
		m_ListenSocket,
		lpAcceptExIoContext->sClient,
		lpAcceptExIoContext->m_buffer,
		0,	//接收多少数据，设置为0
		sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16,
		&dwBytes,
		&(lpAcceptExIoContext->ol));

	if (FALSE == bSuccess)
	{
		int nResult = WSAGetLastError();
		if (nResult != ERROR_IO_PENDING)
		{
			m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"连接失败，错误号：%d  %s %d",nResult,__FILE__,__LINE__);
			closesocket(AcceptSocket);
			m_share_mem.FreeBuffer(lpAcceptExIoContext,sizeof(PER_IO_DATA));
			return ;
		}
	}
 
} 

BOOL	CTcpServer::RecvData(PPER_IO_DATA lpPerIoContext)
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
			m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"投递接收失败，错误号:%d ！%s %d",error,__FILE__,__LINE__);
			return FALSE;
		}
	}

	return TRUE;
}


BOOL	CTcpServer::SendData(PPER_IO_DATA lpPerIoContext)
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
			m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"投递发送失败，错误号:%d ！%s %d",error,__FILE__,__LINE__);
			return FALSE;
		}
	}
	return TRUE;
}

//给外部调用发送接口，为了实现，我分配内存，我释放内存
BOOL CTcpServer::SendData( const void * data,DWORD data_len,SOCKET s)
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
		lpper_io_data->m_buffer[data_len] = '\0';
		m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"发送：【%s】\r\n",lpper_io_data->m_buffer);
	}

	return	ret;
}


BOOL CTcpServer::SendDataForClose(SOCKET s)
{
 	shutdown(s,2);
 	MyCloseSocket(s);	 
	return	TRUE;
}

void CTcpServer::GetClientIPPort(SOCKET clientSock, char* pszIPAddr,int len,DWORD &dPeerport)
{ 
	SOCKADDR_IN addr = {0};
	int sock_size = sizeof(SOCKADDR);

	if (-1 == getpeername(clientSock, (SOCKADDR *)&addr, &sock_size))
	{ 
		return;
	}

	sprintf_s(pszIPAddr, len,
		"%u.%u.%u.%u", 
		addr.sin_addr.S_un.S_un_b.s_b1,
		addr.sin_addr.S_un.S_un_b.s_b2,
		addr.sin_addr.S_un.S_un_b.s_b3,
		addr.sin_addr.S_un.S_un_b.s_b4);

	dPeerport = ntohs(addr.sin_port);
}

DWORD	CTcpServer::CompletionWork()
{
	DWORD dwNumberBytes = 0;
	ULONG_PTR	pPerHandle = NULL;
	PPER_IO_DATA lpPerIoContext = NULL;
	DWORD		dwRet;

	int i = sizeof(PER_IO_DATA);
	while (true)
	{
		try
		{
			lpPerIoContext = NULL;

			BOOL bSuccess = GetQueuedCompletionStatus(m_hCOP,
				&dwNumberBytes,
				(PULONG_PTR )&pPerHandle,
				(LPOVERLAPPED *)&lpPerIoContext,
				WSA_INFINITE); 

			if (NULL == pPerHandle)
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
				case OP_ACCEPT: 		
				case OP_READ:
				default://释放接收数据的上下文信息
					HandDataClose(lpPerIoContext);
					MyCloseSocket(lpPerIoContext->sClient);
					m_share_mem.FreeBuffer(lpPerIoContext,sizeof(PER_IO_DATA));
					PostOneClient();	//再投递一个
					break;
				}
				continue;
			}

			int	   nResult = 0;
			HANDLE hResult = NULL;

			switch(lpPerIoContext->nOperationType)
			{
			case OP_WRIER://发送完成，归还发送数据的上下文信息
				SetSocket(lpPerIoContext->sClient);
				m_share_mem.FreeBuffer(lpPerIoContext,sizeof(PER_IO_DATA));
				break;
			case OP_ACCEPT:
				nResult = setsockopt(
					lpPerIoContext->sClient, 
					SOL_SOCKET,
					SO_UPDATE_ACCEPT_CONTEXT,
					(char *)&m_ListenSocket,
					sizeof(m_ListenSocket)
					);
				if(SOCKET_ERROR == nResult) 
				{
					MyCloseSocket(lpPerIoContext->sClient);		//关闭当前SOCKET
					m_share_mem.FreeBuffer(lpPerIoContext,sizeof(PER_IO_DATA));	//归还资源
					PostOneClient();		//重新投递一个
					continue;
				}
				//关联这个SOCKET，放在超时中
				SetSocket(lpPerIoContext->sClient);

				//将新建立的套接字关联到完成端口
				hResult = CreateIoCompletionPort(
					(HANDLE)lpPerIoContext->sClient,
					m_hCOP,
					(DWORD_PTR)lpPerIoContext,
					0);

				if(NULL == hResult)
				{
					MyCloseSocket(lpPerIoContext->sClient);		//关闭当前SOCKET
					m_share_mem.FreeBuffer(lpPerIoContext,sizeof(PER_IO_DATA));	//归还资源
					PostOneClient();		//重新投递一个
					continue;
				}

				//开始接收第一个包
				memset(lpPerIoContext->m_buffer,0,sizeof(lpPerIoContext->m_buffer));
				lpPerIoContext->m_oper_try_num = MAX_LEN;
				lpPerIoContext->m_oper_success_num = 0;

				if(RecvData(lpPerIoContext) == FALSE)
				{
					MyCloseSocket(lpPerIoContext->sClient);		//关闭当前SOCKET
					m_share_mem.FreeBuffer(lpPerIoContext,sizeof(PER_IO_DATA));	//归还资源
					PostOneClient();		//重新投递一个
					continue;
				}
				break;
			case OP_READ:
				if (dwNumberBytes == 0)
				{
					HandDataClose(lpPerIoContext);				//发送一个关闭信息到服务端
					MyCloseSocket(lpPerIoContext->sClient);		//关闭当前SOCKET
					m_share_mem.FreeBuffer(lpPerIoContext,sizeof(PER_IO_DATA));	//归还资源
					PostOneClient();		//重新投递一个
					continue;
				}

				lpPerIoContext->m_oper_success_num += dwNumberBytes;

				SetSocket(lpPerIoContext->sClient);

				dwRet = HandRecvData(lpPerIoContext);	//交给业务处理函数，此函数可以重写
				if(dwRet == RECV_MORE)
				{
					//数据不够，继续接收
					lpPerIoContext->m_oper_try_num = MAX_LEN;
					RecvData(lpPerIoContext);
					break;
				}

				if (dwRet == HAND_ERROR)
				{
					HandDataClose(lpPerIoContext);
					MyCloseSocket(lpPerIoContext->sClient);		//关闭当前SOCKET
					m_share_mem.FreeBuffer(lpPerIoContext,sizeof(PER_IO_DATA));	//归还资源
					PostOneClient();		//重新投递一个
					continue;
				}

				memset(lpPerIoContext->m_buffer,0,sizeof(lpPerIoContext->m_buffer));
				lpPerIoContext->m_oper_try_num = MAX_LEN;
				lpPerIoContext->m_oper_success_num = 0;

				if(RecvData(lpPerIoContext) == FALSE)
				{
					MyCloseSocket(lpPerIoContext->sClient);		//关闭当前SOCKET
					m_share_mem.FreeBuffer(lpPerIoContext,sizeof(PER_IO_DATA));	//归还资源
					PostOneClient();		//重新投递一个
					continue;
				}
				
				break;
			default://释放接收数据的上下文信息 
				break;
			}
		}
		catch(...)
		{
			m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"[CServerCommuTCPServer]CompletionWorkThread异常:%d\r\n",WSAGetLastError());
		}
	}

	return 0;
}

DWORD	CTcpServer::HandDataClose(PPER_IO_DATA lpPerIoContext)
{ 
	return HAND_SUCCESS;

}

DWORD	CTcpServer::HandRecvData(PPER_IO_DATA lpPerIoContext)
{ 
	return HAND_SUCCESS;
}

void CTcpServer::MyCloseSocket( SOCKET s )
{
	DelScoket(s);
	closesocket(s);
}
 

void CTcpServer::SetSocket( SOCKET s )
{
	yCMyseclock::Auto	lock(m_socket_time_lock);

	m_socket_time[s] = GetTickCount();
}

void CTcpServer::DelScoket( SOCKET s )
{
	yCMyseclock::Auto	lock(m_socket_time_lock);

	m_socket_time.erase(s);
}


void	CTcpServer::TimeOut()
{

	DWORD	time_out = GetTcpTimeOut();

	while(TRUE)
	{
		m_socket_time_lock.Lock();

		DWORD	curr_time = GetTickCount();

		for (SOCKET_TIME_MAP::iterator iter = m_socket_time.begin(); iter != m_socket_time.end(); )
		{
			if (curr_time - iter->second >= time_out)
			{
				m_socket_time.erase(iter++);
			}
			else
			{
				iter++;
			}
		}

		m_socket_time_lock.Unlock();

		Sleep(1000);
	}
}

DWORD CTcpServer::GetTcpTimeOut()
{
	return TCP_TIME_OUT;
}
