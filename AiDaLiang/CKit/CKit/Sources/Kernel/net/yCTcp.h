

#ifndef __YCTCP_H__
#define __YCTCP_H__

#define TCP_TIMEOUT 2000000	//socket超时值（单位：微妙）

class yCTcp
{

//构造函数
public:
	yCTcp ();
	virtual ~yCTcp ();

public:
	int operator = (int);//赋值

//公有成员函数
public:
virtual	int GetHandle () const;//取出m_nSock
virtual	int Open ();//创建socket
virtual	int Close ();//关闭socket
virtual	int Connect (const char *, int) const;//连接，阻塞模式
virtual int ConnectNoBlock (const char *pHost, int nPort, int nTimeout) const;//连接，非阻塞模式
virtual	int Bind (const char *, int) const;//绑定
virtual	int Listen (int nNum) const;//监听
virtual	int SetReuseAddr ();//设置地址可重用
virtual	int Accept () const;//接受连接

virtual	int Sendn (const void *, int, int = TCP_TIMEOUT) const;//发送n个字节数据	
virtual	int Recvn (void *, int, int = TCP_TIMEOUT) const;//接收n个字节数据
virtual	int Send (const void *, int , int = TCP_TIMEOUT) const;//发送数据
virtual	int Recv (void *, int , int = TCP_TIMEOUT) const;//接受数据

virtual	int GetHostName (char *) const;//得到本机机器名
virtual	int GetPeerName (char *) const;//得到对方机器名
virtual	int GetHostAddr (char *) const;//得到本地IP地址
virtual	int GetPeerAddr (char *) const;//得到对方IP地址

virtual int SetSendTimeout (int) const;//设置发送超时值
virtual int SetRecvTimeout (int) const;//设置接收超时值

virtual int GetMacByIp (const char *szIP, char *szMacAddress, char *separator = "");//通过137端口得到MAC地址

//私有成员变量
protected:
	int m_nSock;
	static int retrieve_data (const char *szIP, char *buf, int nBufSize);//通过137端口获取数据

//公有成员变量
public:
#ifdef WIN32
	static int SOCK_ERROR;//socket错误
	static int SOCK_TIMEOUT;//socket超时
#else
	static const int SOCK_ERROR = -100;//socket错误
	static const int SOCK_TIMEOUT = -101;//socket超时
#endif
};

#endif /* __YCTCP_H__ */
