

#include "StdAfx.h"
#include "yCUdp.h"
#include <stdlib.h>
#include <stdio.h>

#pragma comment(lib,"ws2_32.lib")
/***********************************
**   Ĭ�Ϲ��캯��
***********************************/
yCUdp::yCUdp()
{
	m_sock = (UINT)-1;
	
#ifdef WIN32
	WSADATA sockData;
	if (WSAStartup (MAKEWORD (2, 2), &sockData))
	{ 
		;
	}
#endif
}

/***********************************
**   ��������
***********************************/
yCUdp::~yCUdp()
{
	if (m_sock != (UINT)-1)
	{
		closesocket (m_sock);
		m_sock = (UINT)-1;
	}

#ifdef WIN32
	WSACleanup();
#endif
}

/*****************************************
	��������
	��Σ�
	(IN) addr - ���շ���IP��ַ��
	(IN) port - ���շ��Ķ˿ںţ�
	(IN) buff -  �������ݵĻ�������
	(IN) len - �������ݵĻ������ĳ��ȣ�
	���Σ�1���ɹ���0��ʧ��
******************************************/
int yCUdp::SendDate (const char *addr, unsigned int port, const void *buff, int len)
{
	struct sockaddr_in m_OtherAddr;
	const char *buf = (const char *)buff;

	m_OtherAddr.sin_family = AF_INET;
	m_OtherAddr.sin_port = htons((unsigned short)port);
#ifdef WIN32
	char	ip_str[64];
	memset(ip_str,0,sizeof(ip_str));
	GetNameToIP(addr,ip_str);

	m_OtherAddr.sin_addr.S_un.S_addr = inet_addr(ip_str);
#else
	m_OtherAddr.sin_addr.s_addr = inet_addr(addr);
#endif

	/*���������㷨*/
	int	i = sendto (m_sock, buf,  len, 0, 
		(struct sockaddr*) &m_OtherAddr, sizeof(m_OtherAddr));
	if (i == -1)	//Ӧ��Ϊ����������
	{
		DWORD dwerr = GetLastError();
		printf("��������ʧ��!\n");
		return 0;
	} 

	return 1;
}


int	yCUdp::GetNameToIP(const char * name,char * ip_str)
{
	struct sockaddr_in addr;
	struct hostent *phe = NULL;
	if ((phe = gethostbyname (name)) == NULL)
	{
		return 1;
	}	
	memcpy ((char *)&addr.sin_addr, phe->h_addr, phe->h_length);
	
	char *ip = inet_ntoa(addr.sin_addr);
	
	if (ip_str)
	{
		strcpy(ip_str,ip);
	}
	
	return 0;
	
}
/*****************************************
	��������
	��Σ�
	(IN) buff - �������ݵĻ�������
	(IN) len - �������ݵĻ������ĳ��ȣ�
	(IN) time_out ���ó�ʱʱ��,0Ϊ��Զ����ʱ,��λΪ��
	���Σ�1���ɹ���0��ʧ��
******************************************/
int yCUdp::RecvDate(void *buff, int len,int time_out)
{
	/*���������㷨*/

	char *buf = (char *)buff;


	//////������ճ�ʱ////////////////////////////////////////////////////////////////////
	struct timeval  authtime;
	authtime.tv_usec = 0L;
	authtime.tv_sec = (long)time_out; // 1����ط�
	fd_set  readfds;
	//////////////////////////////////////////////////////////////////////////
	int i=0;

	if (time_out!=0)
	{
		//���ó�ʱ
		FD_ZERO (&readfds);//
		FD_SET (m_sock, &readfds);//
		if(select(m_sock,&readfds,NULL,NULL,&authtime) < 0)
		{
			return 0;//��ʱ
		}

		if (!FD_ISSET(m_sock, &readfds))
		{
			return 0;//��ʱʱ,��û�ж�����
		}

	}
	i = recvfrom (m_sock, buf, len, 0,
		NULL,NULL);
	if (i == -1)
	{
		printf("��������ʧ��!\n");
		return 0;
	}


	return i;
}

/*****************************************
	�������ݣ�����ʾ����������ĳ��IP
	��Σ�
	(IN) buff - �������ݵĻ�������
	(IN) len - �������ݵĻ������ĳ��ȣ�
	(OUT) m_ip -���Ͷ˵�IP��ַ
	(OUT) m_port -���Ͷ˵Ķ˿�
	(IN)  time_out �������ݳ�ʱ����,��λΪ��,Ĭ����Զ����ʱ0

	���Σ�1���ɹ���0��ʧ��
******************************************/
int yCUdp::RecvDate(void *buff, int len,void * m_ip,UINT & m_port,int time_out)
{
/*���������㷨*/
	struct sockaddr_in m_clientAddr;
	
	int  addr_len=sizeof(struct sockaddr_in);

	char *buf = (char *)buff;

	//////������ճ�ʱ//////�ν���  2008-8-15//////////////////////////////////////////////////////////////
	struct timeval  authtime;
	authtime.tv_usec = 0L;
	authtime.tv_sec = (long)time_out; // 1����ط�
	fd_set  readfds;
	//////////////////////////////////////////////////////////////////////////

	int i=0; 	 
	if (time_out!=0)
	{
		//���ó�ʱ
		FD_ZERO (&readfds);//
		FD_SET (m_sock, &readfds);//
		if(select(m_sock,&readfds,NULL,NULL,&authtime) < 0)
		{
			return 0;//��ʱ
		}

		if (!FD_ISSET(m_sock, &readfds))
		{
			return 0;//��ʱʱ,��û�ж�����
		}

	}
	i = recvfrom (m_sock, buf, len, 0,		    			
		(struct sockaddr *)&m_clientAddr, &addr_len);
	if (i == -1)
	{
		printf("��������ʧ��!\n");
		return 0;
	} 

	char * ip=inet_ntoa(m_clientAddr.sin_addr);

	strcpy((char *)m_ip,ip);

	m_port=ntohs(m_clientAddr.sin_port);

	return i;
}
/****************************************
**  �󶨱�����ַ��˿�
**	const char	    *addr(I)IP��ַ��
**  unsigned int	port(I) �˿ںţ�
**	int(O) ����1�����ɹ�������0����ʧ�ܡ�
**	net_send		�Ƿ������㲥
****************************************/
int yCUdp::BindAddr (const char *addr, unsigned int port,BOOL net_send)
{
//	assert ((m_sock == -1) && (addr != NULL) && (port > 0));
	int opt = 1;

	m_sock = socket (AF_INET, SOCK_DGRAM, 0);

	if (m_sock == -1)
	{
		printf("UDP�����׽���ʧ��! ");
		return 0;
	}

	m_SockAddr.sin_family = AF_INET;
	m_SockAddr.sin_port = htons((unsigned short)port);
#ifdef WIN32
	if (!addr)
	{
		m_SockAddr.sin_addr.s_addr = htonl (INADDR_ANY);
	}
	else
	{		
		m_SockAddr.sin_addr.S_un.S_addr = inet_addr(addr);
	}
#else
	m_SockAddr.sin_addr.s_addr = inet_addr(addr);
#endif
	
	if (net_send)
	{
		bool opt=true;
		//���ø��׽���Ϊ�㲥���ͣ�
		if(setsockopt(m_sock,SOL_SOCKET,SO_BROADCAST,(char FAR *)&opt,sizeof(opt)) == -1)
			return 0;
	}
	else
	{ 
		if (setsockopt (m_sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(int)) == -1)
		{	
			return 0;
		}
	}
	
	int Ni=bind(m_sock, (struct sockaddr*)&m_SockAddr,sizeof(m_SockAddr));
	if(Ni == -1)
	{
		printf("UDP�󶨵�ַʧ��!");
		return 0;
	}

	return 1;
}

 