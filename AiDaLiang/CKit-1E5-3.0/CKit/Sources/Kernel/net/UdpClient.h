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
		HAND_SUCCESS,		//һ����������
		RECV_MORE,			//��Ҫ���������
		HAND_ERROR,			//���Ӵ�����Ҫ�رգ����ش�ֵ���ϲ��ر�SOCKET�����ҵ���HandDataClose

	};
public:

	virtual DWORD HandRecvData(PPER_IO_DATA lpPerIoContext);		//��������ʱ������
	virtual DWORD HandDataClose(PPER_IO_DATA lpPerIoContext,BOOL isError);	//�ر�ʱ���ô���,UDPЭ���У��Ƿ���������ɺ���ã�IsError��ʾ�����Ƿ�ʧ��
public: 
	virtual BOOL StartWork(const char *ip_str,DWORD dport = 0);
	virtual BOOL StopWork();

	SOCKET  SendBuffer(const char * ip_str,DWORD dwport,const char * buffer,DWORD buffer_len); //���ͳɹ�������ʹ�õ�SOCKET

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
	HANDLE			m_hCompletion;	//ȫ��IOCP
	HANDLE			m_hThreadArray[MAXTHREAD_COUNT];		//�����߳̾��
	int				m_threads;

	std::string		m_local_ip;
	DWORD			m_local_port;

	CDebugLog		m_save_log;
	CPoolMemData	m_share_data;

	SOCKET_LIST		m_socket_list;
	yCMyseclock		m_socket_lock;
};

#endif //__UdpClient_H