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
		HAND_SUCCESS,		//һ����������
		RECV_MORE,			//��Ҫ���������
		HAND_ERROR,			//���Ӵ�����Ҫ�رգ����ش�ֵ���ϲ��ر�SOCKET�����ҵ���HandDataClose

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

	virtual DWORD HandRecvData(PPER_IO_DATA lpPerIoContext);		//��������ʱ������
	virtual DWORD HandDataClose(PPER_IO_DATA lpPerIoContext);	//�ر�ʱ���ô���

private:
	HANDLE			m_hThreadArray[MAXTHREAD_COUNT];		//�����߳̾��
	DWORD			m_ThreadNum;				//�߳�����
	HANDLE			m_hThreadTimeOut;			//���SOCKET��ʱ�߳�

	LPFN_ACCEPTEX	m_lpAcceptEx;				//��չ���պ���

	HANDLE			m_hCOP;						//��ɶ˿ھ��
	SOCKET			m_ListenSocket;				//������SOCKET���
	UINT			m_uPort;					//�󶨵Ķ˿�


	SOCKET_TIME_MAP	m_socket_time;				//��¼�˿�����ʱ��
	yCMyseclock		m_socket_time_lock;			//��

	CPoolMemData	m_share_mem;					//�ڴ�ض���

public:
	CDebugLog		m_save_log;						//��־

};

#endif //__TcpServer_H

