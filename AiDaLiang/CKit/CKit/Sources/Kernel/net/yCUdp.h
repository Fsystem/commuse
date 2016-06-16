
#include <WINSOCK.H>
#ifndef __YCUDP_H__
#define __YCUDP_H__

/*****************************
**yCUdp��Ķ���
*****************************/
#ifdef WIN32
class  yCUdp
#else
class yCUdp
#endif /* WIN32 */
{
private:
	SOCKET		m_sock;		/*�׽���*/
	struct sockaddr_in m_SockAddr;	

public:
	yCUdp ();				/*���캯��*/
	virtual ~yCUdp ();		/*��������*/

public:
	int	GetNameToIP(const char * name,char * ip_str);
	int BindAddr (const char *, unsigned int,BOOL net_send = FALSE);/*�󶨵�ַ*/
	int RecvDate(void *buff, int len,void * m_ip,UINT & m_port,int time_out=0);/*�������ݲ��õ�����IP*/
	int SendDate (const char*, unsigned int, const void*, int);/*��������*/
	int RecvDate (void *buff, int len,int time_out=0);	/*��������*/
};

#endif 
