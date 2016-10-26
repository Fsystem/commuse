#ifndef __UdpClient_H
#define __UdpClient_H

#define MAXTHREAD_COUNT 256

class CUdpClient
{
public:
	CUdpClient(void);
	virtual ~CUdpClient(void);

	typedef	std::list<SOCKET>		SOCKET_LIST;

	enum
	{
		HAND_SUCCESS,		//一切正常进行
		RECV_MORE,			//需要更多的数据
		HAND_ERROR,			//连接错误，需要关闭，返回此值，上层会关闭SOCKET，并且调用HandDataClose

	};
public:

	virtual DWORD HandRecvData(PPER_IO_DATA lpPerIoContext);		//接收数据时，处理
	virtual DWORD HandDataClose(PPER_IO_DATA lpPerIoContext,BOOL isError);	//关闭时调用处理,UDP协议中，是发送数据完成后调用，IsError表示发送是否失败
public: 
	virtual BOOL StartWork(const char *ip_str,DWORD dport = 0);
	virtual BOOL StopWork();

	SOCKET  SendBuffer(const char * ip_str,DWORD dwport,const char * buffer,DWORD buffer_len); //发送成功，返回使用的SOCKET

private:
	static DWORD WINAPI	CompletionWorkThread(PVOID pParam);
	void CompletionWork();
	void NoteWorkThreadExit();

	DWORD RecvData(PPER_IO_DATA pPerIO);
	DWORD SendBuffer(PPER_IO_DATA pPerIO);
	SOCKET GetOneSocket();
	SOCKET GetSocketForBuffer();
	void FreeSocket(SOCKET s);
private:  
	HANDLE			m_hCompletion;	//全局IOCP
	HANDLE			m_hThreadArray[MAXTHREAD_COUNT];		//工作线程句柄
	int				m_threads;

	std::string		m_local_ip;
	DWORD			m_local_port;

	CDebugLog		m_save_log;
	CPoolMemData	m_share_data;

	SOCKET_LIST		m_socket_list;
	yCMyseclock		m_socket_lock;
};

#endif //__UdpClient_H