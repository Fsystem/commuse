#ifndef __iocp_def_H
#define __iocp_def_H

#define		MAX_LEN				32*1024

#define		OP_READ				1
#define		OP_WRIER			2
#define		OP_ACCEPT			3
#define		OP_CONNECT			4

/*�˽ṹ������ɶ˿ڣ���������ʹ�ã��Ƚ��հ�ͷ���ٽ��հ��壬����ĳ��ȸ��ݰ�ͷ
�����еĳ��Ƚ��м���󣬶�̬���䳤�ȡ����������ɲ�Ͷ�ݵ���������������ͷ�*/
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
	{   //���б�Ҫ�ĳ�ʼ�� 
		clear();
	}

	~_PER_IO_DATA()
	{ 
	}
	OVERLAPPED			ol;							//�첽�ṹ
	SOCKET				sClient;					//�ͻ��˾��
	int					nOperationType;				//������ʽ
	byte				m_buffer[MAX_LEN];			//��Ű���
	DWORD				m_oper_try_num;				//��Ҫ���������ݳ���
	DWORD				m_oper_success_num;			//���պͷ��͵��ֽ���
	struct sockaddr_in	m_clientAddr;				//�ͻ���IP�Ͷ˿�
	int					client_len;					//�ͻ������ó���

	PVOID				ext_data;					//��չ����
}PER_IO_DATA,*PPER_IO_DATA;

#endif //__iocp_def_H


