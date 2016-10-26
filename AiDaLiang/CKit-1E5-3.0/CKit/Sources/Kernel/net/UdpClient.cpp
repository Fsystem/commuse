#include "StdAfx.h"
#include "UdpClient.h"

CUdpClient::CUdpClient(void)
{
}

CUdpClient::~CUdpClient(void)
{
}

DWORD CUdpClient::HandRecvData( PPER_IO_DATA lpPerIoContext )
{

	return HAND_SUCCESS;
}

DWORD CUdpClient::HandDataClose( PPER_IO_DATA lpPerIoContext,BOOL isError )
{
	return HAND_SUCCESS;
}

BOOL CUdpClient::StartWork( const char *ip_str,DWORD dport /*= 0*/ )
{
	WORD version;
	WSAData data;
	version = MAKEWORD(2,2);
	int err = WSAStartup(version,&data);
	if(err != 0)
	{
		m_save_log.SaveLog(PRINTF_FIRST_LOG, NULL, "%s,%d行，加载套接字失败!\r\n", __FILE__, __LINE__);
		return FALSE;
	}

	//创建完成端口CreateIoCompletionPort
	m_hCompletion = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);

	//启动cpu数目两倍的线程
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	m_threads = sysInfo.dwNumberOfProcessors * 2;

	if (m_threads > MAXTHREAD_COUNT)
	{
		m_threads = MAXTHREAD_COUNT;
	}

	int i = 0;
	for (i = 0;i < m_threads; i++)
	{ 
		m_hThreadArray[i] = CreateThread(NULL, 0, CompletionWorkThread,this, NULL, NULL);

		if (NULL == m_hThreadArray[i])
		{
			while (i > 0)
			{
				TerminateThread(m_hThreadArray[i - 1],0);
				CloseHandle(m_hThreadArray[i-1]);
				m_hThreadArray[i-1] = INVALID_HANDLE_VALUE;
				i--;
			}
			CloseHandle(m_hCompletion);
			m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"[StartWork]创建工作线程失败\r\n");
			return FALSE;
		}
	}

	if (ip_str == NULL)
	{
		m_local_ip = "";
	}
	else
	{
		m_local_ip = ip_str;
	}

	m_local_port = dport;

	//先准备一千个SOCKET
	m_socket_lock.Lock();

	for (int i = 0; i != 500; i++)
	{
		SOCKET	s = GetOneSocket();
		if (s == 0)
		{
			m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"[StartWork]分配SOCKET句柄失败\r\n");
			continue;
		}
		m_socket_list.push_back(s);
	}

	m_socket_lock.Unlock();

	return TRUE; 
}

SOCKET	CUdpClient::GetSocketForBuffer()
{
	yCMyseclock::Auto	lock(m_socket_lock);

	if (m_socket_list.size() == 0)
	{
		for (int i = 0; i != 10; i++)
		{
			SOCKET	s = GetOneSocket();
			if (s == 0)
			{
				m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"[StartWork]分配SOCKET句柄失败\r\n");
				continue;
			}
			m_socket_list.push_back(s);
		}
	}

	if (m_socket_list.size() == 0)
	{
		return 0;
	}

	SOCKET	s = m_socket_list.front();
	m_socket_list.pop_front();

	return s;
}

void	CUdpClient::FreeSocket(SOCKET s)
{
	yCMyseclock::Auto	lock(m_socket_lock);

	if (s)
	{
		m_socket_list.push_back(s);
	}

}

SOCKET CUdpClient::GetOneSocket()
{
	SOCKET sclient = WSASocket(AF_INET,SOCK_DGRAM,0,NULL,0,WSA_FLAG_OVERLAPPED);
	if(INVALID_SOCKET == sclient)
	{  
		m_save_log.SaveLog(PRINTF_FIRST_LOG, NULL, "%s,%d行，创建套接字失败!\r\n", __FILE__, __LINE__);
		return 0;
	}

	SOCKADDR_IN si;
	si.sin_family = AF_INET;
	si.sin_port = htons((short)m_local_port);

	if (m_local_ip.size() == 0)
	{
		si.sin_addr.S_un.S_addr = INADDR_ANY;
	}
	else
	{
		si.sin_addr.S_un.S_addr = inet_addr(m_local_ip.c_str());
	}

	int opt = 1;
	if (setsockopt (sclient, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(int)) == -1)
	{	
		closesocket(sclient);
		return 0;
	}


	//绑定套接字
	if(SOCKET_ERROR == bind(sclient, (sockaddr *)&si, sizeof(si)))
	{
		closesocket(sclient);
		m_save_log.SaveLog(PRINTF_FIRST_LOG, NULL, "%s,%d行，绑定套接字失败!\r\n", __FILE__, __LINE__);
		return 0;
	}


	//把套接字关联到完成端口
	CreateIoCompletionPort((HANDLE)sclient, m_hCompletion, (DWORD)1, 0);
	//加入统一管理队列中


	PPER_IO_DATA	lpinfo = (PPER_IO_DATA)m_share_data.GetBuffer(sizeof(PER_IO_DATA));
	if (lpinfo == NULL)
	{ 
		closesocket(sclient);
		m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"资源不足，获取SOCKET信息失败；%s %d",__FILE__,__LINE__);
		return 0;
	}

	lpinfo->clear();
	lpinfo->sClient = sclient;

	if (RecvData(lpinfo) != 0)
	{
		closesocket(sclient);
		m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"监听线程，投递SOCKET:%d 投递失败！%s %d",
			sclient,__FILE__,__LINE__);
		m_share_data.FreeBuffer(lpinfo,sizeof(PER_IO_DATA));
		return 0;
	}

	return sclient;
}

BOOL CUdpClient::StopWork()
{
	NoteWorkThreadExit();
	for (int i = 0; i < m_threads; i++)
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

DWORD WINAPI CUdpClient::CompletionWorkThread( PVOID pParam )
{
	CUdpClient	*lpthis = (CUdpClient*)pParam;

	lpthis->CompletionWork();

	return 0;
}

void CUdpClient::CompletionWork()
{
	DWORD dwTrans;
	PPER_IO_DATA pPerIO;
	PPER_IO_DATA	pPerRecvIO = NULL;
	ULONG_PTR	pPerHandle = NULL;
	DWORD	dwret = 0; 

	while(true)
	{   
		BOOL bOK = false;
		pPerIO = NULL;
		pPerHandle = NULL;
		bOK = GetQueuedCompletionStatus(m_hCompletion, &dwTrans, (PULONG_PTR)&pPerHandle, (LPOVERLAPPED *)&pPerIO, WSA_INFINITE);

		if(pPerIO == NULL || pPerHandle == NULL)
		{
			//主程序要求应用程序退出
			m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"[CompletionWork]线程接收退出通知\r\n");
			return ;
		}

		if(!bOK)
		{ 
			//客户端已关闭				
			switch(pPerIO->nOperationType)
			{
			case OP_WRIER://发送出错，归还发送数据的上下文信息
				HandDataClose(pPerIO,FALSE);
				m_share_data.FreeBuffer(pPerIO,sizeof(PER_IO_DATA));
				FreeSocket(pPerIO->sClient);
				break; 	
			case OP_READ:
				while(RecvData(pPerIO))		//重新投递	
				{
					Sleep(5000);
				}
				break;
			} 
			continue;
		} 

		switch(pPerIO->nOperationType)
		{
		case OP_READ:
			{ 
				pPerIO->m_oper_success_num = dwTrans;

				dwret = HandRecvData(pPerIO);
				//UDP协议不处理错误，因为接收时，肯定是一个包一个包接收的

				//重新投递头
				pPerIO->m_oper_success_num = 0;
				pPerIO->m_oper_try_num = MAX_LEN / 2;
				pPerIO->nOperationType = OP_READ;
				memset(pPerIO->m_buffer,0,sizeof(pPerIO->m_buffer));

				while(RecvData(pPerIO))
				{
					Sleep(5000);
				}
			}
			break;
		case OP_WRIER:
			{
				HandDataClose(pPerIO,TRUE);
				m_share_data.FreeBuffer(pPerIO,sizeof(PER_IO_DATA));
				break;
			}			
		}     /*end of switch(pPerIO->nOperationType)*/

	}     /*end of while(1)*/

	return ;
}

void CUdpClient::NoteWorkThreadExit()
{
	for(int i = 0; i < m_threads; i++)
	{
		PostQueuedCompletionStatus(m_hCompletion, 0, NULL, NULL);
		Sleep(500);
	}
}

DWORD CUdpClient::RecvData( PPER_IO_DATA pPerIO)
{
	WSABUF buf;
	DWORD dwTrans,nFlags = 0;

	pPerIO->m_oper_try_num = MAX_LEN;
	pPerIO->m_oper_success_num = 0;
	buf.buf = (char *)pPerIO->m_buffer;
	buf.len = MAX_LEN; //接收长度等于没有接收完成的长度
	pPerIO->nOperationType = OP_READ;

	int ret = WSARecvFrom(pPerIO->sClient,&buf,1,&dwTrans,&nFlags,(struct sockaddr *)&pPerIO->m_clientAddr,
		&pPerIO->client_len,&pPerIO->ol,NULL);
	if(SOCKET_ERROR == ret)
	{
		int error = WSAGetLastError();
		if( error != WSA_IO_PENDING)
		{  
			m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"投递失败，错误号:%d ！%s %d",error,__FILE__,__LINE__);
			return 1;
		}
	}
	return 0;
}

DWORD CUdpClient::SendBuffer( PPER_IO_DATA pPerIO)
{
	WSABUF buf;
	int ret; 
	DWORD dwRecv;
	DWORD dwFlags = 0;

	//投递包头包尾，一起进行 
	pPerIO->nOperationType = OP_WRIER;
	buf.buf = (char *)pPerIO->m_buffer;       //投递包尾缓冲区 
	buf.len = pPerIO->m_oper_try_num;
	
	ret = WSASendTo(pPerIO->sClient, &buf, 1, &dwRecv, dwFlags,(struct sockaddr *)&pPerIO->m_clientAddr,pPerIO->client_len, &pPerIO->ol, NULL);
	if( SOCKET_ERROR == ret)
	{
		int error = WSAGetLastError();
		if( error != WSA_IO_PENDING)
		{    
			m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"SOCKET:%d 投递失败，错误号:%d ！%s %d",
				pPerIO->sClient,error,__FILE__,__LINE__);
			return 1;
		}
	}
	return 0;
}

SOCKET CUdpClient::SendBuffer(const char * ip_str,DWORD dwport,const char * buffer,DWORD buffer_len)
{
	WSABUF buf;
	int ret; 
	DWORD dwRecv;
	DWORD dwFlags = 0;
	struct hostent *phe = NULL;

	if (ip_str == NULL || strlen(ip_str) == 0 || dwport == 0 || buffer == NULL || buffer_len == 0)
	{
		return 0;
	}

	PPER_IO_DATA	lpper_io_data = (PPER_IO_DATA)m_share_data.GetBuffer(sizeof(PER_IO_DATA));

	if (lpper_io_data == NULL)
	{
		return 0;
	}

	lpper_io_data->clear();
 
	lpper_io_data->nOperationType = OP_WRIER;
	memcpy(lpper_io_data->m_buffer,buffer,buffer_len);
	lpper_io_data->m_oper_try_num = buffer_len;
	lpper_io_data->m_oper_success_num = 0;

	buf.buf = (char *)lpper_io_data->m_buffer;       //投递包尾缓冲区 
	buf.len = lpper_io_data->m_oper_try_num;

	//处理转化IP和端口 
	lpper_io_data->m_clientAddr.sin_family = AF_INET;
	lpper_io_data->m_clientAddr.sin_port = htons((short)dwport);
 
	if ((lpper_io_data->m_clientAddr.sin_addr.S_un.S_addr = inet_addr (ip_str)) == (unsigned long int) -1)
	{
		if ((phe = gethostbyname (ip_str)) == NULL)
		{
			return 0;
		}

		memcpy ((char *)&lpper_io_data->m_clientAddr.sin_addr, phe->h_addr, phe->h_length);
	}

	//取一个SOCKET出来，此处使用一个中间变量，为了防止瞬间使用完成，然后被释放的情况，虽然不可能，但是不排除
	SOCKET s = GetSocketForBuffer();
	lpper_io_data->sClient = s;

	ret = WSASendTo(lpper_io_data->sClient, &buf, 1, &dwRecv, dwFlags,(struct sockaddr *)&lpper_io_data->m_clientAddr,lpper_io_data->client_len, &lpper_io_data->ol, NULL);
	if( SOCKET_ERROR == ret)
	{
		int error = WSAGetLastError();
		if( error != WSA_IO_PENDING)
		{    
			m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"SOCKET:%d 投递失败，错误号:%d ！%s %d",
				lpper_io_data->sClient,error,__FILE__,__LINE__);
			FreeSocket(lpper_io_data->sClient);
			return 0;
		}
	}
	return s;
}