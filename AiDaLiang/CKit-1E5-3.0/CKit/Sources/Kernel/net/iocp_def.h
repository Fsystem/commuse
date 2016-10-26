#ifndef __iocp_def_H
#define __iocp_def_H

#define		MAX_LEN				32*1024

#define		OP_READ				1
#define		OP_WRIER			2
#define		OP_ACCEPT			3
#define		OP_CONNECT			4

/*此结构用于完成端口，接收数据使用，先接收包头，再接收包体，包体的长度根据包头
数据中的长度进行计算后，动态分配长度。包体接收完成并投递到其它缓冲区后才释放*/
typedef struct _PER_IO_DATA
{
	void clear()
	{
		memset(&ol,0,sizeof(ol));
		memset(m_buffer, 0, sizeof(m_buffer));
		m_oper_success_num =	0;
		m_oper_try_num = 0;
		nOperationType = OP_READ;
		sClient = NULL;
		memset(&m_clientAddr,0,sizeof(m_clientAddr));
		client_len = sizeof(m_clientAddr);
		ext_data = NULL;
	}
	_PER_IO_DATA()
	{   //进行必要的初始化 
		clear();
	}

	~_PER_IO_DATA()
	{ 
	}
	OVERLAPPED			ol;							//异步结构
	SOCKET				sClient;					//客户端句柄
	int					nOperationType;				//操作方式
	byte				m_buffer[MAX_LEN];			//存放包体
	DWORD				m_oper_try_num;				//需要操作的数据长度
	DWORD				m_oper_success_num;			//接收和发送的字节数
	struct sockaddr_in	m_clientAddr;				//客户端IP和端口
	int					client_len;					//客户端配置长度

	PVOID				ext_data;					//扩展数据
}PER_IO_DATA,*PPER_IO_DATA;

#endif //__iocp_def_H


