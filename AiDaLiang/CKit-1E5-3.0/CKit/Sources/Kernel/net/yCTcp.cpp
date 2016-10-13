/*********************************************************
*   版权所有(C) 2000-2050, 深圳市艾默特科技有限公司.
*
*   文 件 名 : yCTcp.cpp
*             
*   作    者 : 张勇杰　马晓东
*
*   完成日期 : 2005-08-03
*
*   描    述 : 封装TCP操作
*
*   修改记录 :
*               
*   数据结构 :
*       
*   备注: 
*       
*********************************************************/
#include "StdAfx.h"
#include "yCTcp.h"
/************头文件************/
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <WinSock2.h>

#pragma comment(lib,"ws2_32.lib")

#ifdef WIN32
int yCTcp::SOCK_ERROR = -100;
int yCTcp::SOCK_TIMEOUT = -101;
#endif

//////////////////下面定义只是供GetMacByIp和retrieve_data使用
#define NETBIOS_REQUEST		("\xa2\x48\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x20\x43\x4b\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x00\x00\x21\x00\x01")
#define GROUP_NAME_FLAG		(128)
#define NAME_TYPE_DOMAIN	(0x00)
#define NAME_TYPE_MESSENGER	(0x03)
#pragma pack(push, 1)
typedef struct _node_status_resp
{
	unsigned short trn_id;
	unsigned short flags1;
	unsigned short flags2;
	unsigned short flags3;
	unsigned short flags4;
	unsigned short flags5;
	char rr_name[0x22];
	unsigned short nbstat;
	unsigned short in;
	unsigned long zilch;
	unsigned short rd_length;
	unsigned char num_names;
	struct {
		unsigned char nb_name[16];
		unsigned short name_flags;
	} name_array[1];
}NS_RESP;
#pragma pack(pop)
//////////////////上面定义只是供GetMacByIp和retrieve_data使用


//构造函数
yCTcp::yCTcp ()
{
	m_nSock = -1;
#ifdef WIN32
	WSADATA sockData;
	if (WSAStartup (MAKEWORD (2, 2), &sockData))
	{
		printf("TCP加载失败\n");
		exit (0);
	}
#endif
}

//析构函数
yCTcp::~yCTcp ()
{
	Close ();
#ifdef WIN32
	WSACleanup ();
#endif
}

/*
  入参：nSockfd - socket句柄
  出参：赋值后的socket句柄
*/

int yCTcp::operator = (int nSockfd)
{

#ifdef WIN32
	assert ((m_nSock == -1) && (nSockfd > -1));
#else
	assert ((m_nSock == -1) && (nSockfd > -1) && (isfdtype (nSockfd, S_IFSOCK) == 1));
#endif

	m_nSock = nSockfd;

	return m_nSock;
}

/*取出socket句柄
  入参：无
  出参：取出的socket句柄
*/
int yCTcp::GetHandle () const
{
	return m_nSock;
}

/*创建socket
  入参：无
  出参：1: 成功 ; 0: 失败
*/
int yCTcp::Open ()
{
	assert (m_nSock == -1);

	m_nSock = socket (AF_INET, SOCK_STREAM, 0);
/*
#ifdef WIN32
	if (m_nSock != -1)
	{
		SetSendTimeout (2000000);
		SetRecvTimeout (2000000);
	}
#endif
*/
	return (m_nSock != -1);
}

/*关闭socket
  入参：无
  出参：1: 成功 ; 0: 失败
*/
int yCTcp::Close ()
{
	if (m_nSock != -1)
	{
		closesocket (m_nSock);
		m_nSock = -1;
	}

	return 1;
}

/*连接，阻塞方式
  入参：pHost - IP地址或主机名
  	nPort - 端口
  出参：1: 成功 ; 0: 失败
*/
int yCTcp::Connect (const char *pHost, int nPort) const
{
	assert ((m_nSock != -1) && pHost && (nPort > 0));
	struct sockaddr_in addr;
	struct hostent *phe = NULL;

	memset (&addr, 0, sizeof (addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons (nPort);

	if ((addr.sin_addr.s_addr = inet_addr (pHost)) == (unsigned long int) -1)
	{
		if ((phe = gethostbyname (pHost)) == NULL)
		{
			return 0;
		}

		memcpy ((char *)&addr.sin_addr, phe->h_addr, phe->h_length);
	}

	int ret = connect (m_nSock, (struct sockaddr *)&addr, sizeof (addr));

	return (!ret);
}

/*连接，非阻塞模式
  入参：pHost - IP地址或主机名
  		nPort - 端口
		nTimeout - 超时值，单位：秒
  出参：1: 成功 ; 0: 失败
*/
int yCTcp::ConnectNoBlock (const char *pHost, int nPort, int nTimeout) const
{
	assert ((m_nSock != -1) && pHost && (nPort > 0));
	struct sockaddr_in addr;
	struct hostent *phe = NULL;
	int ret = 0;

	//设置非阻塞模式
	unsigned long ul = 1;
#ifdef WIN32
	ioctlsocket (m_nSock, FIONBIO, (unsigned long*)&ul);
#else
	setsockopt (m_nSock, SOL_SOCKET, FIONBIO, (const char *)&ul, sizeof(int));
#endif

	memset (&addr, 0, sizeof (addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons (nPort);

	if ((addr.sin_addr.s_addr = inet_addr (pHost)) == (unsigned long int) -1)
	{
		if ((phe = gethostbyname (pHost)) == NULL)
		{
			return 0;
		}

		memcpy ((char *)&addr.sin_addr, phe->h_addr, phe->h_length);
	}

	connect (m_nSock, (struct sockaddr *)&addr, sizeof (addr));

	struct timeval timeout ;
	fd_set r;
	FD_ZERO (&r);
	FD_SET ((unsigned int)m_nSock, &r);
	timeout.tv_sec = nTimeout; //连接超时nTimeout秒
	timeout.tv_usec =0;
	ret = select(0, 0, &r, 0, &timeout);
	if ( ret <= 0 )
	{
		return 0;
	}
	
	//把sock转回为阻塞模式
	unsigned long ul1= 0 ;
#ifdef WIN32
	ioctlsocket (m_nSock, FIONBIO, (unsigned long*)&ul1);
#else
	setsockopt (m_nSock, SOL_SOCKET, FIONBIO, (const char *)&ul1, sizeof(int));
#endif

	return 1;
}

/*绑定
  入参：pIP - IP地址
  	nPort - 端口
  出参：1: 成功 ; 0: 失败
*/
int yCTcp::Bind (const char *pIP, int nPort) const
{
	assert ((m_nSock != -1) && (nPort > 0));
	struct sockaddr_in addr;
	struct hostent *phe = NULL;
	int opt=1;

	if (setsockopt (m_nSock, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(int)) == -1)
	{	
		return 0;
	}

	memset (&addr, 0, sizeof (addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons (nPort);

	if (!pIP)
	{
		addr.sin_addr.s_addr = htonl (INADDR_ANY);
	}
	else
	{
		if ((addr.sin_addr.s_addr = inet_addr (pIP)) == (unsigned long int) -1)
		{
			if ((phe = gethostbyname (pIP)) == NULL)
				return 0;

			memcpy ((char *)&addr.sin_addr, phe->h_addr, phe->h_length);
		}
	}

	return (bind (m_nSock, (struct sockaddr *)&addr, sizeof (addr)) == 0);
}

/*监听
  入参：nNum - 监听数目
  出参：1: 成功 ; 0: 失败
*/
int yCTcp::Listen (int nNum) const
{
	assert ((m_nSock != -1) && (nNum > 0));

	return (listen (m_nSock, nNum) == 0);
}

/*接受连接
  入参：无
  出参：其他: 连接套接字句柄 ; -1: 失败
*/
int yCTcp::Accept () const
{
	assert (m_nSock != -1);

	return (accept (m_nSock, (struct sockaddr *)NULL, NULL));
}

/*设置地址可重用
  入参：无
  出参：1: 成功 ; 0: 失败
*/
int yCTcp::SetReuseAddr ()
{
	assert (m_nSock != -1);
	int opt = 1;
	
	if (setsockopt (m_nSock, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(int)) == -1)
	{
		return 0;
	}
	
	return 1;
}

/*发送指定数目数据
  入参：pBuf - 发送缓存
  	nCount - 需发送字节数
  	nMicsec - socket超时值，单位：微秒，缺省：TCP_TIMEOUT微秒
  出参：实际发送字节数 ，如果发送失败，返回负数
*/

int yCTcp::Sendn (const void *pBuf, int nCount, int nMicsec) const
{
	assert ((m_nSock != -1) && pBuf && (nCount > 0));
	int nLeft = nCount;
	int nWritten = 0;
	const char *ptr = (const char *)pBuf;
	
#ifdef WIN32

	//SetSendTimeout (nMicsec);

	while (nLeft > 0)
	{
		if ((nWritten = send (m_nSock, ptr, nLeft, 0)) <= 0)
		{
			return SOCK_ERROR;
		}
		
		nLeft -= nWritten;
		ptr += nWritten;
	}
	
#else
	
	int sn = 0;
	struct timeval tvlTime;
	fd_set wtfdset;
	
	signal (SIGPIPE, SIG_IGN);
	
	if (nMicsec >= 0)
	{
		tvlTime.tv_sec = nMicsec / 1000000;
		tvlTime.tv_usec = abs (nMicsec - tvlTime.tv_sec * 1000000);
	}
	
	FD_ZERO (&wtfdset);
	FD_SET (m_nSock, &wtfdset);
	
	if (nMicsec >= 0)
		sn = select (m_nSock + 1, NULL, &wtfdset, NULL, &tvlTime);
	else
		sn = select (m_nSock + 1, NULL, &wtfdset, NULL, NULL);
	
	switch (sn)
	{
	case -1:
		return SOCK_ERROR;
		
	case 0:
		return SOCK_TIMEOUT;
		
	default:
		while (nLeft > 0)
		{
			if ((nWritten = send (m_nSock, ptr, nLeft, 0)) <= 0)
			{
				if (errno == EINTR)
					nWritten = 0;
				else
					return SOCK_ERROR;
			}
			
			nLeft -= nWritten;
			ptr += nWritten;
		}
		
		break;
	}

#endif
	
	return nCount;
}

/*接收指定数目数据
  入参：pBuf - 接收缓存
  	nCount - 需接收字节数
  	nMicsec - socket超时值，单位：微妙，缺省：TCP_TIMEOUT微妙
  出参：实际接收字节数 ，如果接收失败，返回负数，如果对方关闭，返回0
*/
int yCTcp::Recvn (void *pBuf, int nCount, int nMicsec) const
{
	assert ((m_nSock != -1) && pBuf && (nCount > 0));
	int nLeft = nCount;
	int nRead = 0;
	char *ptr = (char *)pBuf;

#ifdef WIN32	
	
	SetRecvTimeout (nMicsec);

	while (nLeft > 0)
	{
		if ((nRead = recv (m_nSock, ptr, nLeft, 0)) < 0)
		{ 
			return SOCK_ERROR;
		}
		else if (nRead == 0)
		{
			return 0;
		}
		
		nLeft -= nRead;
		ptr += nRead;
	}
	
#else
	
	int sn = 0;	
	fd_set rdfdset;
	struct timeval tvlTime;
		
	if (nMicsec >= 0)
	{
		tvlTime.tv_sec = nMicsec / 1000000;
		tvlTime.tv_usec = abs (nMicsec - tvlTime.tv_sec * 1000000);
	}
	
	FD_ZERO (&rdfdset);
	FD_SET (m_nSock, &rdfdset);
	
	if (nMicsec >= 0)
		sn = select (m_nSock + 1, &rdfdset, NULL, NULL, &tvlTime);
	else
		sn = select (m_nSock + 1, &rdfdset, NULL, NULL, NULL);
	
	switch (sn)
	{
	case -1:
		return SOCK_ERROR;
		
	case 0:
		return SOCK_TIMEOUT;
		
	default:
		while (nLeft > 0)
		{
			if ((nRead = read (m_nSock, ptr, nLeft)) < 0)
			{
				if (errno == EINTR)
					nRead = 0;
				else
					return SOCK_ERROR;
			}
			else if (nRead == 0)
			{
				return 0;
			}
			
			nLeft -= nRead;
			ptr += nRead;
		}
		
		break;
	}

#endif

	return nCount;
}

/*发送数据
  入参：pBuf - 发送缓存
  	nCount - 需发送字节数
  	nMicsec - socket超时值，单位：微妙，缺省：TCP_TIMEOUT微妙
  出参：实际发送字节数 ，如果发送失败，返回负数
*/
int yCTcp::Send (const void *pBuf, int nCount, int nMicsec) const
{
	assert ((m_nSock != -1) && pBuf && (nCount > 0));
	int sn = 0, wn = 0;
	struct timeval tvlTime;
	char *ptr = (char *)pBuf;

#ifdef WIN32
	//SetSendTimeout (nMicsec);
#else
	fd_set wtfdset;
	signal (SIGPIPE, SIG_IGN);
#endif
	if (nMicsec >= 0)
	{
		tvlTime.tv_sec = nMicsec / 1000000;
		tvlTime.tv_usec = abs (nMicsec - tvlTime.tv_sec * 1000000);
	}

#ifdef WIN32
#else
	FD_ZERO (&wtfdset);
	FD_SET (m_nSock, &wtfdset);

	if (nMicsec >= 0)
		sn = select (m_nSock + 1, NULL, &wtfdset, NULL, &tvlTime);
	else
		sn = select (m_nSock + 1, NULL, &wtfdset, NULL, NULL);

	switch (sn)
	{
	case -1:
		return SOCK_ERROR;

	case 0:
		return SOCK_TIMEOUT;
	}
#endif
	if ((wn = send (m_nSock, ptr, nCount, 0)) <= 0)
		return SOCK_ERROR;

	return wn;
}
/*接收数据
  入参：pBuf - 接收缓存
  	nCount - 需接收字节数
  	nMicsec - socket超时值，单位：微妙，缺省：TCP_TIMEOUT微妙
  出参：实际接收字节数 ，如果接收失败，返回负数，如果对方关闭，返回0
*/
int yCTcp::Recv (void *pBuf, int nCount, int nMicsec) const
{
	assert ((m_nSock != -1) && pBuf && (nCount > 0));
	int sn = 0, rn = 0;
	struct timeval tvlTime;
	char *ptr = (char *)pBuf;

	if (nMicsec >= 0)
	{
		tvlTime.tv_sec = nMicsec / 1000000;
		tvlTime.tv_usec = abs (nMicsec - tvlTime.tv_sec * 1000000);
	}
#ifdef WIN32
	SetRecvTimeout (nMicsec);
	if ((rn = recv (m_nSock, ptr, nCount,0)) < 0)
#else
	fd_set rdfdset;
	FD_ZERO (&rdfdset);
	FD_SET (m_nSock, &rdfdset);

	if (nMicsec >= 0)
		sn = select (m_nSock + 1, &rdfdset, NULL, NULL, &tvlTime);
	else
		sn = select (m_nSock + 1, &rdfdset, NULL, NULL, NULL);

	switch (sn)
	{
	case -1:
		return SOCK_ERROR;

	case 0:
		return SOCK_TIMEOUT;
	}
	if ((rn = read (m_nSock, ptr, nCount)) < 0)
#endif
		return SOCK_ERROR;

	return rn;
}

/*得到本机机器名
  入参：pName - 存放本机机器名
  出参：1: 成功 ; 0: 失败
*/

int yCTcp::GetHostName (char *pName) const
{
	assert (pName);

	return (gethostname (pName, 100) == 0);
}

/*得到对方机器名
  入参：pName - 存放对方机器名
  出参：1: 成功 ; 0: 失败
*/
int yCTcp::GetPeerName (char *pName) const
{
	assert ((m_nSock != -1) );
	struct hostent *pht = NULL;
	struct sockaddr_in addr;

#ifdef WIN32
	int len = sizeof (sockaddr);
#else
	socklen_t len;
#endif
	
	if (getpeername (m_nSock, (struct sockaddr *)&addr, &len) == -1)
	{
		return 0;
	}

#ifdef WIN32
	if ((pht = gethostbyaddr ((char*)&addr.sin_addr.S_un.S_addr, sizeof (addr.sin_addr.S_un.S_addr), AF_INET)) != NULL)
	{
		strcpy(pName, pht->h_name);  
		return 1;
	}
#else
	if ((pht = gethostbyaddr (&addr.sin_addr, sizeof (addr.sin_addr), AF_INET)) != NULL)
	{
		strcpy (pName, pht->h_name);
		return 1;
	}
#endif

	return 0;
}

/*得到对方地址
  入参：pAddr - 存放对方地址
  出参：1: 成功 ; 0: 失败
*/
int yCTcp::GetPeerAddr (char *pAddr) const
{
	assert ((m_nSock != -1) );

	struct sockaddr_in addr;
	char *pCharAddr = NULL;

#ifdef WIN32
	int len = sizeof(sockaddr);
#else
	socklen_t len;
#endif

	if (getpeername (m_nSock, (struct sockaddr *)&addr, &len) == -1)
	{
		printf("获取对方主机名出错！\n");
		return 0;
	}
	pCharAddr = inet_ntoa(addr.sin_addr);

	if(strcpy(pAddr, pCharAddr) == NULL)
	{
		printf("拷贝字符串出错！\n");
		return 0;
	}

	return 1;
}

/*得到本机地址
  入参：地址列表存储空间
  出参：1，成功; 0，失败;
*/
int yCTcp::GetHostAddr (char *pLocalIPList) const
{
	char pName[100] = {0};
	struct hostent *pht = NULL;
	in_addr addr;

	if (!GetHostName (pName))
	{
		printf("获取主机名出错\n");
		return 0;
	}

	if ((pht = gethostbyname (pName)) == NULL)
	{
		printf("获取主机地址出错\n");
		return 0;
	}

	int i = 0;

	while (pht->h_addr_list[i] != 0)
    {
        char strTemp[100];
		memcpy(&addr,pht->h_addr_list[i++],sizeof(in_addr));
		sprintf(strTemp,"%s\t",inet_ntoa(addr));
		strcat(pLocalIPList, strTemp);
		strcat(pLocalIPList, "\n");
    }
	
	return 1;
}

/*设置发送超时值
  入参：发送超时值（单位：微秒）
  出参：1: 成功 ; 0: 失败
*/
int yCTcp::SetSendTimeout (int nTimeout) const
{
	assert ((m_nSock != -1) && (nTimeout > 0));

#ifdef WIN32
	int timeout = nTimeout / 1000;
	if (setsockopt (m_nSock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof (timeout)) == SOCKET_ERROR)
		return 0;
#else
	struct timeval	tv;
	tv.tv_sec = nTimeout / 1000000;
	tv.tv_usec = abs (nTimeout - tv.tv_sec * 1000000);
	if (setsockopt (m_nSock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof (tv)) == -1)
		return 0;
#endif

	return 1;
}

/*设置接收超时值
  入参：接收超时值（单位：微秒）
  出参：1: 成功 ; 0: 失败
*/
int yCTcp::SetRecvTimeout (int nTimeout) const
{
	assert ((m_nSock != -1) && (nTimeout > 0));

#ifdef WIN32
	int timeout = nTimeout / 1000;
	if (setsockopt (m_nSock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof (timeout)) == SOCKET_ERROR)
		return 0;
#else
	struct timeval	tv;
	tv.tv_sec = nTimeout / 1000000;
	tv.tv_usec = abs (nTimeout - tv.tv_sec * 1000000);
	if (setsockopt (m_nSock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof (tv)) == -1)
		return 0;
#endif

	return 1;
}

/*通过137端口获取数据
  入参：szIP - 主机IP地址(点分隔的IP)
  buf - 数据存放于此
  nBufSize - buf大小
  出参：成功返回获取数据的长度，失败返回0
*/
int yCTcp::retrieve_data (const char *szIP, char *buf, int nBufSize)
{
	struct sockaddr_in c_addr;
	int hSocket;
	struct	hostent *pHostent = NULL;
	fd_set	sFds;
	struct	timeval tv;
	int nDataLen = 0;

	//创建socket
	if ((hSocket = socket (AF_INET, SOCK_DGRAM, 0)) < 0)
        	return 0;

	//构造接收端IP、端口信息
	memset ((char*)&c_addr, 0, sizeof (c_addr));
	c_addr.sin_family = AF_INET;
	if ((pHostent = gethostbyname ((const char *)szIP)) == NULL)
	{
		closesocket (hSocket);
		return 0;
	}

	memcpy (&c_addr.sin_addr, pHostent->h_addr, pHostent->h_length);
	c_addr.sin_port = htons (137);

	//构造UDP数据包
	nDataLen = sizeof (NETBIOS_REQUEST) - 1;
	memcpy (buf, NETBIOS_REQUEST, nDataLen);

	//给目的主机发送数据
	if (sendto (hSocket, buf, nDataLen, 0, (struct sockaddr *) &c_addr, sizeof(c_addr)) < 0)
	{
		closesocket (hSocket);
		return 0;
	}

	//接收数据延时900000微秒，如果还没响应就认为此IP对应的主机不存在或不在线。
	tv.tv_sec = 0;
	tv.tv_usec = 900000;
	FD_ZERO (&sFds);
	FD_SET ((unsigned int)hSocket, &sFds);

	select (hSocket + 1, &sFds, NULL, NULL, &tv);

	if (FD_ISSET (hSocket, &sFds))
	{
		//接收目的主机发过来的数据。
		#ifdef WIN32
		nDataLen = recvfrom (hSocket, buf, nBufSize, 0, (struct sockaddr*) 0, (int *)0);
		#else
		nDataLen = recvfrom (hSocket, buf, nBufSize, 0, (struct sockaddr*) 0, (socklen_t *)0);
		#endif
	}
	else
	{
		closesocket (hSocket);
		return 0;
	}

	if (nDataLen < 0)
	{
		closesocket (hSocket);
		return 0;
	}

	closesocket (hSocket);
	return nDataLen;
}

/*通过137端口得到网络主机MAC地址
  入参：szIP - 主机IP地址（点分隔的IP）
		szMacAddress - MAC地址
		separator - MAC地址分隔符，缺省是空分隔符
  出参：1：成功；0：失败
*/
int yCTcp::GetMacByIp (const char *szIP, char *szMacAddress, char *separator)
{
	char buf[1000] = {0};
	NS_RESP *data = (NS_RESP *) buf;
	unsigned int nRetrievedLen = retrieve_data (szIP, buf, sizeof(buf));

	if (nRetrievedLen == 0)
		return 0;

	if (nRetrievedLen < sizeof (*data))
	{
		// response too small
		return 0;
	}

	if (nRetrievedLen < (sizeof (*data) + (data->num_names - 1) * sizeof (data->name_array[0])))
	{
		// response too small for num_names
		return 0;
	}
	
	// get mac address
	if (szMacAddress != NULL)
	{
		sprintf (szMacAddress, "%02X%s%02X%s%02X%s%02X%s%02X%s%02X",
			data->name_array[data->num_names].nb_name[0],
			separator,
			data->name_array[data->num_names].nb_name[1],
			separator,
			data->name_array[data->num_names].nb_name[2],
			separator,
			data->name_array[data->num_names].nb_name[3],
			separator,
			data->name_array[data->num_names].nb_name[4],
			separator,
			data->name_array[data->num_names].nb_name[5]);
	}

	return 1;
}
