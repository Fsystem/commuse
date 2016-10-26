#ifndef __TcpClient_H
#define __TcpClient_H
#define		MAXTHREAD_COUNT			256

class CTcpClient
{
public:
	enum
	{
		HAND_SUCCESS,		//һ����������
		RECV_MORE,			//��Ҫ���������
		HAND_ERROR,			//���Ӵ�����Ҫ�رգ����ش�ֵ���ϲ��ر�SOCKET�����ҵ���HandDataClose

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
	virtual DWORD HandDataClose(PPER_IO_DATA lpPerIoContext);		//ʧ���ˣ��ر�
	virtual DWORD HandRecvData(PPER_IO_DATA lpPerIoContext);		//���յ�������
	virtual DWORD ConnectSuccess(PPER_IO_DATA lpPerIoContext);		//���ӳɹ��ˣ����Է���������
private:
	HANDLE m_hThreadArray[MAXTHREAD_COUNT];		//�����߳̾��
	DWORD			m_ThreadNum;				//�߳�����
	HANDLE			m_hCOP;						//��ɶ˿ھ��

	CPoolMemData	m_share_mem;
public:
	CDebugLog		m_save_log;
};
#endif //__TcpClient_H


