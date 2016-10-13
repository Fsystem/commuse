

#include "StdAfx.h"
#include "yCUdp.h"
#include <stdlib.h>
#include <stdio.h>

#pragma comment(lib,"ws2_32.lib")
/***********************************
**   默认构造函数
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
**   析构函数
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
	发送数据
	入参：
	(IN) addr - 接收方的IP地址；
	(IN) port - 接收方的端口号；
	(IN) buff -  发送数据的缓冲区；
	(IN) len - 发送数据的缓冲区的长度；
	出参：1：成功；0：失败
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

	/*发送数据算法*/
	int	i = sendto (m_sock, buf,  len, 0, 
		(struct sockaddr*) &m_OtherAddr, sizeof(m_OtherAddr));
	if (i == -1)	//应该为缓冲区长度
	{
		DWORD dwerr = GetLastError();
		printf("发送数据失败!\n");
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
	接收数据
	入参：
	(IN) buff - 接收数据的缓冲区；
	(IN) len - 接收数据的缓冲区的长度；
	(IN) time_out 设置超时时间,0为永远不超时,单位为秒
	出参：1：成功；0：失败
******************************************/
int yCUdp::RecvDate(void *buff, int len,int time_out)
{
	/*接收数据算法*/

	char *buf = (char *)buff;


	//////加入接收超时////////////////////////////////////////////////////////////////////
	struct timeval  authtime;
	authtime.tv_usec = 0L;
	authtime.tv_sec = (long)time_out; // 1秒后重发
	fd_set  readfds;
	//////////////////////////////////////////////////////////////////////////
	int i=0;

	if (time_out!=0)
	{
		//设置超时
		FD_ZERO (&readfds);//
		FD_SET (m_sock, &readfds);//
		if(select(m_sock,&readfds,NULL,NULL,&authtime) < 0)
		{
			return 0;//超时
		}

		if (!FD_ISSET(m_sock, &readfds))
		{
			return 0;//超时时,还没有东西读
		}

	}
	i = recvfrom (m_sock, buf, len, 0,
		NULL,NULL);
	if (i == -1)
	{
		printf("接收数据失败!\n");
		return 0;
	}


	return i;
}

/*****************************************
	接收数据，可显示接收来自于某个IP
	入参：
	(IN) buff - 接收数据的缓冲区；
	(IN) len - 接收数据的缓冲区的长度；
	(OUT) m_ip -发送端的IP地址
	(OUT) m_port -发送端的端口
	(IN)  time_out 接收数据超时设置,单位为秒,默认永远不超时0

	出参：1：成功；0：失败
******************************************/
int yCUdp::RecvDate(void *buff, int len,void * m_ip,UINT & m_port,int time_out)
{
/*接收数据算法*/
	struct sockaddr_in m_clientAddr;
	
	int  addr_len=sizeof(struct sockaddr_in);

	char *buf = (char *)buff;

	//////加入接收超时//////段建均  2008-8-15//////////////////////////////////////////////////////////////
	struct timeval  authtime;
	authtime.tv_usec = 0L;
	authtime.tv_sec = (long)time_out; // 1秒后重发
	fd_set  readfds;
	//////////////////////////////////////////////////////////////////////////

	int i=0; 	 
	if (time_out!=0)
	{
		//设置超时
		FD_ZERO (&readfds);//
		FD_SET (m_sock, &readfds);//
		if(select(m_sock,&readfds,NULL,NULL,&authtime) < 0)
		{
			return 0;//超时
		}

		if (!FD_ISSET(m_sock, &readfds))
		{
			return 0;//超时时,还没有东西读
		}

	}
	i = recvfrom (m_sock, buf, len, 0,		    			
		(struct sockaddr *)&m_clientAddr, &addr_len);
	if (i == -1)
	{
		printf("接收数据失败!\n");
		return 0;
	} 

	char * ip=inet_ntoa(m_clientAddr.sin_addr);

	strcpy((char *)m_ip,ip);

	m_port=ntohs(m_clientAddr.sin_port);

	return i;
}
/****************************************
**  绑定本机地址与端口
**	const char	    *addr(I)IP地址；
**  unsigned int	port(I) 端口号；
**	int(O) 返回1操作成功，返回0操作失败。
**	net_send		是否绑定网络广播
****************************************/
int yCUdp::BindAddr (const char *addr, unsigned int port,BOOL net_send)
{
//	assert ((m_sock == -1) && (addr != NULL) && (port > 0));
	int opt = 1;

	m_sock = socket (AF_INET, SOCK_DGRAM, 0);

	if (m_sock == -1)
	{
		printf("UDP创建套接字失败! ");
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
		//设置该套接字为广播类型，
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
		printf("UDP绑定地址失败!");
		return 0;
	}

	return 1;
}

 