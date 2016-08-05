#ifndef __RawSocket_H
#define __RawSocket_H
//-------------------------------------------------------------------------------
//使用sendto可以伪装raw socket发包

typedef struct ip_hdr//定义IP首部
{
	unsigned char h_verlen;//4位首部长度，4位IP版本号
	unsigned char tos;//8位服务类型TOS
	unsigned short tatal_len;//16位总长度
	unsigned short ident;//16位标示
	unsigned short frag_and_flags;//偏移量和3位标志位
	unsigned char ttl;//8位生存时间TTL
	unsigned char proto;//8位协议（TCP,UDP或其他）
	unsigned short checksum;//16位IP首部检验和
	unsigned int sourceIP;//32位源IP地址
	unsigned int destIP;//32位目的IP地址
}IPHEADER;

typedef struct tsd_hdr//定义TCP伪首部
{
	unsigned long saddr;//源地址
	unsigned long daddr;//目的地址
	char mbz;
	char ptcl;//协议类型
	unsigned short tcpl;//TCP长度
}PSDHEADER;

typedef struct tcp_hdr//定义TCP首部
{
	unsigned short sport;//16位源端口
	unsigned short dport;//16位目的端口
	unsigned int seq;//32位序列号
	unsigned int ack;//32位确认号
	unsigned char lenres;//4位首部长度/6位保留字
	unsigned char flag;//6位标志位
	unsigned short win;//16位窗口大小
	unsigned short sum;//16位检验和
	unsigned short urp;//16位紧急数据偏移量
}TCPHEADER;

typedef struct udp_hdr//定义UDP首部
{
	unsigned short sport;//16位源端口
	unsigned short dport;//16位目的端口
	unsigned short len;//UDP 长度
	unsigned short cksum;//检查和
}UDPHEADER;

typedef struct icmp_hdr//定义ICMP首部
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