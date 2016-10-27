#ifndef __UdpServer_H
#define __UdpServer_H
#define		DEFAULT_PORT		9000
#define		MAXTHREAD_COUNT			256

class CUdpServer
{
public:
	CUdpServer(void);
	~CUdpServer(void);

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
	virtual BOOL StartWork(const char *ip_str,DWORD port = DEFAULT_PORT);
	virtual BOOL StopWork();
	DWORD SendBuffer(SOCKADDR_IN clientAddr,const char *buffer,DWORD buffer_len);

private:
	static DWORD WINAPI	CompletionWorkThread(PVOID pParam);
	void CompletionWork();
	void NoteWorkThreadExit();

	DWORD RecvData(PPER_IO_DATA pPerIO);
	DWORD SendBuffer(PPER_IO_DATA pPerIO);
private: 
	SOCKET			m_sListen;
	HANDLE			m_hCompletion;	//ȫ��IOCP
	HANDLE			m_hThreadArray[MAXTHREAD_COUNT];		//�����߳̾��
	int				m_threads;

	CDebugLog		m_save_log;

	CPoolMemData	m_share_data;
};
#endif //__UdpServer_H


