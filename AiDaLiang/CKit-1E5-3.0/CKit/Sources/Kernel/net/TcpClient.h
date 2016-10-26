#ifndef __TcpClient_H
#define __TcpClient_H
#define		MAXTHREAD_COUNT			256

class CTcpClient
{
public:
	enum
	{
		HAND_SUCCESS,		//一切正常进行
		RECV_MORE,			//需要更多的数据
		HAND_ERROR,			//连接错误，需要关闭，返回此值，上层会关闭SOCKET，并且调用HandDataClose

	};

public:
	CTcpClient(void);
	virtual ~CTcpClient(void);
public: 
	virtual BOOL StartWork();
	virtual BOOL StopWork();
private:

	static DWORD WINAPI CompletionWorkThread(LPVOID Param);
	DWORD CompletionWork();
private:
	void  NoteWorkThreadExit();
	void  CloseThreadHandle();
	BOOL  InitWinsock();
	BOOL  SendData(PPER_IO_DATA lpPerIoContext);
	BOOL  RecvData(PPER_IO_DATA lpPerIoContext);
	void  GetClientIPPort(SOCKET clientSock, char* pszIPAddr,int len,DWORD &dPeerport);

public:
	BOOL SendData( const void * data,DWORD data_len,SOCKET s);
	BOOL SendDataForClose(SOCKET s);
	BOOL MyConnect(const char * ip,int port,PVOID Ext_Data);

public:
	virtual DWORD HandDataClose(PPER_IO_DATA lpPerIoContext);		//失败了；关闭
	virtual DWORD HandRecvData(PPER_IO_DATA lpPerIoContext);		//接收到数据了
	virtual DWORD ConnectSuccess(PPER_IO_DATA lpPerIoContext);		//连接成功了，可以发送数据了
private:
	HANDLE m_hThreadArray[MAXTHREAD_COUNT];		//工作线程句柄
	DWORD			m_ThreadNum;				//线程数量
	HANDLE			m_hCOP;						//完成端口句柄

	CPoolMemData	m_share_mem;
public:
	CDebugLog		m_save_log;
};
#endif //__TcpClient_H


