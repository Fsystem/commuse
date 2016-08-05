#ifndef __RawSocket_H
#define __RawSocket_H
//-------------------------------------------------------------------------------
//ʹ��sendto����αװraw socket����

typedef struct ip_hdr//����IP�ײ�
{
	unsigned char h_verlen;//4λ�ײ����ȣ�4λIP�汾��
	unsigned char tos;//8λ��������TOS
	unsigned short tatal_len;//16λ�ܳ���
	unsigned short ident;//16λ��ʾ
	unsigned short frag_and_flags;//ƫ������3λ��־λ
	unsigned char ttl;//8λ����ʱ��TTL
	unsigned char proto;//8λЭ�飨TCP,UDP��������
	unsigned short checksum;//16λIP�ײ������
	unsigned int sourceIP;//32λԴIP��ַ
	unsigned int destIP;//32λĿ��IP��ַ
}IPHEADER;

typedef struct tsd_hdr//����TCPα�ײ�
{
	unsigned long saddr;//Դ��ַ
	unsigned long daddr;//Ŀ�ĵ�ַ
	char mbz;
	char ptcl;//Э������
	unsigned short tcpl;//TCP����
}PSDHEADER;

typedef struct tcp_hdr//����TCP�ײ�
{
	unsigned short sport;//16λԴ�˿�
	unsigned short dport;//16λĿ�Ķ˿�
	unsigned int seq;//32λ���к�
	unsigned int ack;//32λȷ�Ϻ�
	unsigned char lenres;//4λ�ײ�����/6λ������
	unsigned char flag;//6λ��־λ
	unsigned short win;//16λ���ڴ�С
	unsigned short sum;//16λ�����
	unsigned short urp;//16λ��������ƫ����
}TCPHEADER;

typedef struct udp_hdr//����UDP�ײ�
{
	unsigned short sport;//16λԴ�˿�
	unsigned short dport;//16λĿ�Ķ˿�
	unsigned short len;//UDP ����
	unsigned short cksum;//����
}UDPHEADER;

typedef struct icmp_hdr//����ICMP�ײ�
{
	unsigned short sport;
	unsigned short dport;
	unsigned char type;
	unsigned char code;
	unsigned short cksum;
	unsigned short id;
	unsigned short seq;
	unsigned long timestamp;
}ICMPHEADER;
//-------------------------------------------------------------------------------

struct IHandleRawSocketData
{
	virtual void HandleData(IPHEADER* pIpHead) = 0;
};

class RawSocket
{
public:
	RawSocket();
	BOOL StartCatch(const char* szLocalNetIp,IHandleRawSocketData* pDelegate);
protected:
	void CatchThread();
protected:
	SOCKET mRawSock;
	IHandleRawSocketData* mDelegate;
private:
};

#endif //__RawSocket_H