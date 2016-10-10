
#include <WINSOCK.H>
#ifndef __YCUDP_H__
#define __YCUDP_H__

/*****************************
**yCUdp类的定义
*****************************/
#ifdef WIN32
class  yCUdp
#else
class yCUdp
#endif /* WIN32 */
{
private:
	SOCKET		m_sock;		/*套接字*/
	struct sockaddr_in m_SockAddr;	

public:
	yCUdp ();				/*构造函数*/
	virtual ~yCUdp ();		/*析构函数*/

public:
	int	GetNameToIP(const char * name,char * ip_str);
	int BindAddr (const char *, unsigned int,BOOL net_send = FALSE);/*绑定地址*/
	int RecvDate(void *buff, int len,void * m_ip,UINT & m_port,int time_out=0);/*接收数据并得到发送IP*/
	int SendDate (const char*, unsigned int, const void*, int);/*发送数据*/
	int RecvDate (void *buff, int len,int time_out=0);	/*接收数据*/
};

#endif 
