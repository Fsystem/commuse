#ifndef __TcpServer_H
#define __TcpServer_H
#define		MAXTHREAD_COUNT			256
#define		DEFAULT_PORT			9000
#define		ACCEPTEX_NUM			(4096)
#define		LISTEN_NUM				5000
#define		TCP_TIME_OUT			(1000*3)





class CTcpServer
{
public:
	enum
	{
		HAND_SUCCESS,		//一切正常进行
		RECV_MORE,			//需要更多的数据
		HAND_ERROR,			//连接错误，需要关闭，返回此值，上层会关闭SOCKET，并且调用HandDataClose

	};

	typedef	std::map<SOCKET,DWORD>		SOCKET_TIME_MAP;

public:
	CTcpServer(void);
	virtual ~CTcpServer(void);
public:

	virtual BOOL StartWork(DWORD dport = DEFAULT_PORT);
	virtual BOOL StopWork();

private:

	static DWORD WINAPI CompletionWorkThread(LPVOID Param);
	DWORD CompletionWork();

	void	SetSocket(SOCKET s);
	void	DelScoket(SOCKET s);
	void	MyCloseSocket(SOCKET s);
	void	TimeOut();
	static DWORD WINAPI TimeOutThread(PVOID pParam);

private:
	void  NoteWorkThreadExit();
	void  CloseThreadHandle();
	BOOL  InitWinsock(); 
	BOOL  CreateClientInfo(); 
	void  PostOneClient();
	BOOL  SendData(PPER_IO_DATA lpPerIoContext);
	BOOL  RecvData(PPER_IO_DATA lpPerIoContext);
public:
	BOOL	SendData(const void * data,DWORD data_len,SOCKET s);
	void	GetClientIPPort(SOCKET clientSock, char* pszIPAddr,int len,DWORD &dPeerport);
	BOOL	SendDataForClose(SOCKET s);
public:

	virtual DWORD HandRecvData(PPER_IO_DATA lpPerIoContext);		//接收数据时，处理
	virtual DWORD HandDataClose(PPER_IO_DATA lpPerIoContext);	//关闭时调用处理

private:
	HANDLE			m_hThreadArray[MAXTHREAD_COUNT];		//工作线程句柄
	DWORD			m_ThreadNum;				//线程数量
	HANDLE			m_hThreadTimeOut;			//检查SOCKET超时线程

	LPFN_ACCEPTEX	m_lpAcceptEx;				//扩展接收函数

	HANDLE			m_hCOP;						//完成端口句柄
	SOCKET			m_ListenSocket;				//监听的SOCKET句柄
	UINT			m_uPort;					//绑定的端口


	SOCKET_TIME_MAP	m_socket_time;				//记录端口连接时间
	yCMyseclock		m_socket_time_lock;			//锁

	CPoolMemData	m_share_mem;					//内存池对像

public:
	CDebugLog		m_save_log;						//日志

};

#endif //__TcpServer_H

