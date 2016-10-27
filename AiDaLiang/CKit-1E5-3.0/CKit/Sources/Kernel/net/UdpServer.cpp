#include "StdAfx.h"
#include "UdpServer.h"

CUdpServer::CUdpServer(void)
{ 
	m_hCompletion = NULL;
	m_threads = 0;
}

CUdpServer::~CUdpServer(void)
{
}


BOOL CUdpServer::StartWork(const char *ip_str,DWORD port)
{    
	WORD version;
	WSAData data;
	version = MAKEWORD(2,2);
	int err = WSAStartup(version,&data);
	if(err != 0)
	{
		m_save_log.SaveLog(PRINTF_FIRST_LOG, NULL, "%s,%d�У������׽���ʧ��!\r\n", __FILE__, __LINE__);
		return FALSE;
	}

	//������ɶ˿�CreateIoCompletionPort
	m_hCompletion = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);

	//����cpu��Ŀ�������߳�
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
			m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"[CServerCommuTCPServer]���������߳�ʧ��\r\n");
			return FALSE;
		}
	}
	 
	//����
	//�����׽���
	m_sListen = WSASocket(AF_INET,SOCK_DGRAM,0,NULL,0,WSA_FLAG_OVERLAPPED);
	if(INVALID_SOCKET == m_sListen)
	{  
		m_save_log.SaveLog(PRINTF_FIRST_LOG, NULL, "%s,%d�У������׽���ʧ��!\r\n", __FILE__, __LINE__);
		return FALSE;
	}

	SOCKADDR_IN si;
	si.sin_family = AF_INET;
	si.sin_port = htons((short)port);

	if (ip_str == NULL)
	{
		si.sin_addr.S_un.S_addr = INADDR_ANY;
	}
	else
	{
		si.sin_addr.S_un.S_addr = inet_addr(ip_str);
	}

	int opt = 1;
	if (setsockopt (m_sListen, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(int)) == -1)
	{	
		return FALSE;
	}


	//���׽���
	if(SOCKET_ERROR == bind(m_sListen, (sockaddr *)&si, sizeof(si)))
	{
		m_save_log.SaveLog(PRINTF_FIRST_LOG, NULL, "%s,%d�У����׽���ʧ��!\r\n", __FILE__, __LINE__);
		return FALSE;
	}


	//���׽��ֹ�������ɶ˿�
	CreateIoCompletionPort((HANDLE)m_sListen, m_hCompletion, (DWORD)1, 0);
	//����ͳһ���������


	for (i = 0; i != m_threads * 2; i++)
	{
		PPER_IO_DATA	lpinfo = (PPER_IO_DATA)m_share_data.GetBuffer(sizeof(PER_IO_DATA));
		if (lpinfo == NULL)
		{ 
			m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"��Դ���㣬��ȡSOCKET��Ϣʧ�ܣ�%s %d",__FILE__,__LINE__);
			continue;
		}

		lpinfo->clear();

		if (RecvData(lpinfo) != 0)
		{
			m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"�����̣߳�Ͷ��SOCKET:%d Ͷ��ʧ�ܣ�%s %d",
				m_sListen,__FILE__,__LINE__);
			m_share_data.FreeBuffer(lpinfo,sizeof(PER_IO_DATA));
		}
	}

	return true; 
}


void CUdpServer::NoteWorkThreadExit()
{
	for(int i = 0; i < m_threads; i++)
	{
		PostQueuedCompletionStatus(m_hCompletion, 0, NULL, NULL);
		Sleep(500);
	}	

}


BOOL CUdpServer::StopWork()
{
	//֪ͨ���й����߳��˳�,δ�˳���ǿ�ƽ���.
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

//Ͷ���������
DWORD CUdpServer::RecvData(PPER_IO_DATA pPerIO)
{
	WSABUF buf;
	DWORD dwTrans,nFlags = 0;

	pPerIO->m_oper_try_num = MAX_LEN;
	pPerIO->m_oper_success_num = 0;
	buf.buf = (char *)pPerIO->m_buffer;
	buf.len = MAX_LEN; //���ճ��ȵ���û�н�����ɵĳ���
	pPerIO->nOperationType = OP_READ;

	int ret = WSARecvFrom(m_sListen,&buf,1,&dwTrans,&nFlags,(struct sockaddr *)&pPerIO->m_clientAddr,
		&pPerIO->client_len,&pPerIO->ol,NULL);
	if(SOCKET_ERROR == ret)
	{
		int error = WSAGetLastError();
		if( error != WSA_IO_PENDING)
		{  
			m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"Ͷ��ʧ�ܣ������:%d ��%s %d",error,__FILE__,__LINE__);
			return 1;
		}
	}
	return 0;
}


//����
DWORD CUdpServer::SendBuffer(PPER_IO_DATA pPerIO)
{
	WSABUF buf;
	int ret; 
	DWORD dwRecv;
	DWORD dwFlags = 0;

	//Ͷ�ݰ�ͷ��β��һ����� 
	pPerIO->nOperationType = OP_WRIER;
	buf.buf = (char *)pPerIO->m_buffer;       //Ͷ�ݰ�β������ 
	buf.len = pPerIO->m_oper_try_num;

	ret = WSASendTo(m_sListen, &buf, 1, &dwRecv, dwFlags,(struct sockaddr *)&pPerIO->m_clientAddr,pPerIO->client_len, &pPerIO->ol, NULL);
	if( SOCKET_ERROR == ret)
	{
		int error = WSAGetLastError();
		if( error != WSA_IO_PENDING)
		{    
			m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"SOCKET:%d Ͷ��ʧ�ܣ������:%d ��%s %d",
				m_sListen,error,__FILE__,__LINE__);
			return 1;
		}
	}
	return 0;
}
 

//����
DWORD CUdpServer::SendBuffer(SOCKADDR_IN clientAddr,const char *buffer,DWORD buffer_len)
{
	WSABUF buf;
	int ret; 
	DWORD dwRecv;
	DWORD dwFlags = 0;

	PPER_IO_DATA pPerIO = (PPER_IO_DATA)m_share_data.GetBuffer(sizeof(PER_IO_DATA));

	if (pPerIO == NULL)
	{
		m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"�����ڴ�ʧ�ܣ�%s %d",
			__FILE__,__LINE__);
		return 2;
	}

	//Ͷ�ݰ�ͷ��β��һ����� 
	pPerIO->nOperationType = OP_WRIER;
	memcpy(pPerIO->m_buffer,buffer,buffer_len);
	pPerIO->m_oper_try_num = buffer_len;
	pPerIO->m_oper_success_num = 0;

	buf.buf = (char *)pPerIO->m_buffer;       //Ͷ�ݰ�β������ 
	buf.len = pPerIO->m_oper_try_num;

	memcpy(&pPerIO->m_clientAddr,&clientAddr,sizeof(SOCKADDR_IN));
	pPerIO->client_len = sizeof(SOCKADDR_IN);

	ret = WSASendTo(m_sListen, &buf, 1, &dwRecv, dwFlags,(struct sockaddr *)&pPerIO->m_clientAddr,pPerIO->client_len, &pPerIO->ol, NULL);
	if( SOCKET_ERROR == ret)
	{
		int error = WSAGetLastError();
		if( error != WSA_IO_PENDING)
		{    
			m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"SOCKET:%d Ͷ��ʧ�ܣ������:%d ��%s %d",
				m_sListen,error,__FILE__,__LINE__);
			m_share_data.FreeBuffer(pPerIO,sizeof(PER_IO_DATA));
			return 1;
		}
	}
	return 0;
}


DWORD WINAPI CUdpServer::CompletionWorkThread( PVOID pParam )
{
	CUdpServer	*lpthis = (CUdpServer*)pParam;

	lpthis->CompletionWork();

	return 0;
}

void	CUdpServer::CompletionWork()
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
			//������Ҫ��Ӧ�ó����˳�
			m_save_log.SaveLog(PRINTF_FIRST_LOG,NULL,"[CompletionWork]�߳̽����˳�֪ͨ\r\n");
			return ;
		}

		if(!bOK)
		{ 
			//�ͻ����ѹر�				
			switch(pPerIO->nOperationType)
			{
			case OP_WRIER://���ͳ����黹�������ݵ���������Ϣ
				HandDataClose(pPerIO,FALSE);
				m_share_data.FreeBuffer(pPerIO,sizeof(PER_IO_DATA));
				break; 	
			case OP_READ:
				while(RecvData(pPerIO))		//����Ͷ��	
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
				//UDPЭ�鲻���������Ϊ����ʱ���϶���һ����һ�������յ�
 
				//����Ͷ��ͷ
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


DWORD	CUdpServer::HandDataClose(PPER_IO_DATA lpPerIoContext,BOOL isError)
{ 
	return HAND_SUCCESS;

}

DWORD	CUdpServer::HandRecvData(PPER_IO_DATA lpPerIoContext)
{ 
	return HAND_SUCCESS;
}

