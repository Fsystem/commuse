#include "stdafx.h"
#include <Windows.h>
#include "RawSocket.h"


//-------------------------------------------------------------------------------
#define _CRT_SECURE_NO_WARNINGS 
#include <stdio.h>


//-------------------------------------------------------------------------------



#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"Iphlpapi.lib")

#define SOURCE_PORT 7234
#define MAX_RECEIVEBYTE 255
#define MAX_ADDR_LEN 32
#define SIO_RCVALL  (IOC_IN|IOC_VENDOR|1)//定义网卡为混杂模式

char TcpFlag[6] = { 'F', 'S', 'R', 'A', 'U' };//定义TCP标志位
std::string sLog;

//-------------------------------------------------------------------------------

void WriteDataToFile(LPCSTR szFileName,LPCSTR szData)
{
	std::ofstream of(szFileName,std::ios::app);
	if (of.is_open())
	{
		of<<szData<<std::endl;
	}
}

////CheckSum:计算校验和的子函数(参数为：tcp伪首部+tcp首部作为缓冲区)
//USHORT ChechSum(USHORT *buffer, int size)
//{
//	unsigned long cksum=0;
//	while(size >1)
//	{
//		cksum+=*buffer++;
//		size -=sizeof(USHORT);
//	}
//	if(size )
//	{
//		cksum += *(UCHAR*)buffer;
//	}
//
//	cksum = (cksum >> 16) + (cksum & 0xffff);
//	cksum += (cksum >>16);
//	return (USHORT)(~cksum);
//}

//DWORD GetPidBySocketLink(DWORD dwSrcIp,DWORD dwSrcPort,DWORD dwDesIp,DWORD dwDesPort)
//{
//	DWORD dwSize = 0;
//	if( GetExtendedTcpTable(NULL, &dwSize, TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0)==ERROR_INSUFFICIENT_BUFFER )
//	{
//		PMIB_TCPTABLE_OWNER_PID pTcpTab = (PMIB_TCPTABLE_OWNER_PID)malloc(dwSize);
//		if ( GetExtendedTcpTable(pTcpTab, &dwSize, TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0)==NO_ERROR )
//		{
//			for (DWORD i=0;i<pTcpTab->dwNumEntries;i++)
//			{
//				if (dwSrcIp == pTcpTab->table[i].dwLocalAddr && dwSrcPort == pTcpTab->table[i].dwLocalPort
//					&& dwDesIp == pTcpTab->table[i].dwRemoteAddr && dwDesPort == pTcpTab->table[i].dwRemotePort)
//				{
//					return pTcpTab->table[i].dwOwningPid;
//				}
//			}
//		}
//	}
//
//	return 0;
//}

void HandlePacket(IPHEADER* pIpHeader)
{
	//判断是否是TCP包
	if (pIpHeader->proto == IPPROTO_TCP)
	{
		TCPHEADER* pTcpheader = (TCPHEADER*)(pIpHeader+1);

		std::ostringstream os;
		os<<"\n#################数据包长度%d"<<pIpHeader->tatal_len-sizeof(IPHEADER)-sizeof(TCPHEADER);
		os<<"\n**********TCP协议头部***********";
		os<<"\n源端口："<<ntohs(pTcpheader->sport);
		os<<"\n目的端口："<< ntohs(pTcpheader->dport);
		os<<"\n序列号："<< ntohs(pTcpheader->seq);
		os<<"\n应答号："<< ntohs(pTcpheader->ack);
		os<<"\n检验和："<< ntohs(pTcpheader->sum);
		os<<"\n标志位：";
		//打印标志位
		unsigned char FlagMask = 1;
		for (int k = 0; k < 6; k++)
		{
			if ((pTcpheader->flag)&FlagMask)
				os<<(char)TcpFlag[k];
			else
				os<<" ";
			FlagMask = FlagMask << 1;
		}
		os<<"\nPID："<< GetPidBySocketLink(pIpHeader->sourceIP,pTcpheader->sport,pIpHeader->destIP,pTcpheader->dport)<<"\n";

		sLog+= os.str();
		std::cout<<os.str();

		WriteDataToFile("IPHead.txt",sLog.c_str());


		//判断是不是HTTP协议请求
		char* pData = (char*)(pTcpheader+1);
		if (strstr(pData,"HTTP"))
		{
			printf("\nHTTP数据:%s\n",pData);
			WriteDataToFile("./HttpData.txt",pData);
		}
	}
}

//-------------------------------------------------------------------------------
RawSocket::RawSocket()
{
	mDelegate = NULL;
}

void RawSocket::CatchThread()
{
	SOCKET sock = mRawSock;
	
	char recvBuf[65535] = { 0 };
	char temp[65535] = { 0 };
	
	int pCount = 0;
	
	unsigned char* dataip = nullptr;
	unsigned char* datatcp = nullptr;
	unsigned char* dataudp = nullptr;
	unsigned char* dataicmp = nullptr;

	int lentcp, lenudp, lenicmp, lenip;

	UDPHEADER * pUdpheader;//UDP头结构体指针
	IPHEADER * pIpheader;//IP头结构体指针
	TCPHEADER * pTcpheader;//TCP头结构体指针
	ICMPHEADER * pIcmpheader;//ICMP头结构体指针
	char szSourceIP[MAX_ADDR_LEN], szDestIP[MAX_ADDR_LEN];//源IP和目的IP
	SOCKADDR_IN saSource, saDest;//源地址结构体，目的地址结构体

	//设置各种头指针
	pIpheader = (IPHEADER*)recvBuf;
	pTcpheader = (TCPHEADER*)(recvBuf + sizeof(IPHEADER));
	pUdpheader = (UDPHEADER*)(recvBuf + sizeof(IPHEADER));
	pIcmpheader = (ICMPHEADER*)(recvBuf + sizeof(IPHEADER));
	int iIphLen = sizeof(unsigned long)*(pIpheader->h_verlen & 0x0f);
	while (1)
	{
		memset(recvBuf, 0, sizeof(recvBuf));//清空缓冲区
		recv(sock, recvBuf, sizeof(recvBuf), 0);//接收包

		//获得源地址和目的地址
		saSource.sin_addr.s_addr = pIpheader->sourceIP;
		strncpy(szSourceIP, inet_ntoa(saSource.sin_addr), MAX_ADDR_LEN);
		saDest.sin_addr.s_addr = pIpheader->destIP;
		strncpy(szDestIP, inet_ntoa(saDest.sin_addr), MAX_ADDR_LEN);

		//计算各种包的长度（只有判断是否是该包后才有意义，先计算出来）
		lenip = ntohs(pIpheader->tatal_len);
		lentcp = ntohs(pIpheader->tatal_len) - (sizeof(IPHEADER) + sizeof(TCPHEADER));
		lenudp = ntohs(pIpheader->tatal_len) - (sizeof(IPHEADER) + sizeof(UDPHEADER));
		lenicmp = ntohs(pIpheader->tatal_len) - (sizeof(IPHEADER) + sizeof(ICMPHEADER));

		/*std::ostringstream os;
		os<<"\n**********IP协议头部***********";
		os<<"\n标示："<<ntohs(pIpheader->ident);
		os<<"\n总长度："<<ntohs(pIpheader->tatal_len);
		os<<"\n偏移量："<< ntohs(pIpheader->frag_and_flags);
		os<<"\n生存时间："<<(int)pIpheader->ttl;
		os<<"\n服务类型："<<(int)pIpheader->tos;
		os<<"\n协议类型："<<(int)pIpheader->proto;
		os<<"\n检验和："<< ntohs(pIpheader->checksum);
		os<<"\n源IP："<< szSourceIP;
		os<<"\n目的IP："<< szDestIP;

		std::cout<<os.str();*/
		//+++++++++++++++++++++++++++++
		//在这里可以加入其它封包的判断和处理
		//+++++++++++++++++++++++++++++

		if (mDelegate)
		{
			mDelegate->HandleData(pIpheader);
		}
	}
}

BOOL RawSocket::StartCatch(LPCSTR szLocalNetIp,IHandleRawSocketData* pDelegate)
{

	mDelegate = pDelegate;

	WSADATA wsd;
	WSAStartup(MAKEWORD(2, 2), &wsd);

	if ((mRawSock = socket(AF_INET, SOCK_RAW, IPPROTO_IP)) == SOCKET_ERROR)//创建一个原始套接字
	{
		return FALSE;
	}

	char FAR name[MAXBYTE];
	gethostname(name, MAXBYTE);
	struct hostent FAR* pHostent;
	pHostent = gethostbyname(name);

	SOCKADDR_IN sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(1);//原始套接字没有端口的概念，所以这个值随便设置
	sa.sin_addr.s_addr = inet_addr(szLocalNetIp);

	bind(mRawSock, (SOCKADDR*)&sa, sizeof(sa));//绑定
	if (WSAGetLastError() == 10013)
	{
		return FALSE;
	}

	//设置网卡为混杂模式，也叫泛听模式。可以侦听经过的所有的包。
	unsigned int optval = 1;
	DWORD dwBytesRet;
	WSAIoctl(mRawSock, SIO_RCVALL, &optval, sizeof(optval), nullptr, 0, &dwBytesRet,nullptr,nullptr);

	JKThread<RawSocket>::Start(&RawSocket::CatchThread,this);
	return TRUE;
}