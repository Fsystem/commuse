

#ifndef __YCTCP_H__
#define __YCTCP_H__

#define TCP_TIMEOUT 2000000	//socket��ʱֵ����λ��΢�

class yCTcp
{

//���캯��
public:
	yCTcp ();
	virtual ~yCTcp ();

public:
	int operator = (int);//��ֵ

//���г�Ա����
public:
virtual	int GetHandle () const;//ȡ��m_nSock
virtual	int Open ();//����socket
virtual	int Close ();//�ر�socket
virtual	int Connect (const char *, int) const;//���ӣ�����ģʽ
virtual int ConnectNoBlock (const char *pHost, int nPort, int nTimeout) const;//���ӣ�������ģʽ
virtual	int Bind (const char *, int) const;//��
virtual	int Listen (int nNum) const;//����
virtual	int SetReuseAddr ();//���õ�ַ������
virtual	int Accept () const;//��������

virtual	int Sendn (const void *, int, int = TCP_TIMEOUT) const;//����n���ֽ�����	
virtual	int Recvn (void *, int, int = TCP_TIMEOUT) const;//����n���ֽ�����
virtual	int Send (const void *, int , int = TCP_TIMEOUT) const;//��������
virtual	int Recv (void *, int , int = TCP_TIMEOUT) const;//��������

virtual	int GetHostName (char *) const;//�õ�����������
virtual	int GetPeerName (char *) const;//�õ��Է�������
virtual	int GetHostAddr (char *) const;//�õ�����IP��ַ
virtual	int GetPeerAddr (char *) const;//�õ��Է�IP��ַ

virtual int SetSendTimeout (int) const;//���÷��ͳ�ʱֵ
virtual int SetRecvTimeout (int) const;//���ý��ճ�ʱֵ

virtual int GetMacByIp (const char *szIP, char *szMacAddress, char *separator = "");//ͨ��137�˿ڵõ�MAC��ַ

//˽�г�Ա����
protected:
	int m_nSock;
	static int retrieve_data (const char *szIP, char *buf, int nBufSize);//ͨ��137�˿ڻ�ȡ����

//���г�Ա����
public:
#ifdef WIN32
	static int SOCK_ERROR;//socket����
	static int SOCK_TIMEOUT;//socket��ʱ
#else
	static const int SOCK_ERROR = -100;//socket����
	static const int SOCK_TIMEOUT = -101;//socket��ʱ
#endif
};

#endif /* __YCTCP_H__ */
