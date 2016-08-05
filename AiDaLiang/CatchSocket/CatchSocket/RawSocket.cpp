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
#define SIO_RCVALL  (IOC_IN|IOC_VENDOR|1)//��������Ϊ����ģʽ

char TcpFlag[6] = { 'F', 'S', 'R', 'A', 'U' };//����TCP��־λ
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

////CheckSum:����У��͵��Ӻ���(����Ϊ��tcpα�ײ�+tcp�ײ���Ϊ������)
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
	//�ж��Ƿ���TCP��
	if (pIpHeader->proto == IPPROTO_TCP)
	{
		TCPHEADER* pTcpheader = (TCPHEADER*)(pIpHeader+1);

		std::ostringstream os;
		os<<"\n#################���ݰ�����%d"<<pIpHeader->tatal_len-sizeof(IPHEADER)-sizeof(TCPHEADER);
		os<<"\n**********TCPЭ��ͷ��***********";
		os<<"\nԴ�˿ڣ�"<<ntohs(pTcpheader->sport);
		os<<"\nĿ�Ķ˿ڣ�"<< ntohs(pTcpheader->dport);
		os<<"\n���кţ�"<< ntohs(pTcpheader->seq);
		os<<"\nӦ��ţ�"<< ntohs(pTcpheader->ack);
		os<<"\n����ͣ�"<< ntohs(pTcpheader->sum);
		os<<"\n��־λ��";
		//��ӡ��־λ
		unsigned char FlagMask = 1;
		for (int k = 0; k < 6; k++)
		{
			if ((pTcpheader->flag)&FlagMask)
				os<<(char)TcpFlag[k];
			else
				os<<" ";
			FlagMask = FlagMask << 1;
		}
		os<<"\nPID��"<< GetPidBySocketLink(pIpHeader->sourceIP,pTcpheader->sport,pIpHeader->destIP,pTcpheader->dport)<<"\n";

		sLog+= os.str();
		std::cout<<os.str();

		WriteDataToFile("IPHead.txt",sLog.c_str());


		//�ж��ǲ���HTTPЭ������
		char* pData = (char*)(pTcpheader+1);
		if (strstr(pData,"HTTP"))
		{
			printf("\nHTTP����:%s\n",pData);
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

	UDPHEADER * pUdpheader;//UDPͷ�ṹ��ָ��
	IPHEADER * pIpheader;//IPͷ�ṹ��ָ��
	TCPHEADER * pTcpheader;//TCPͷ�ṹ��ָ��
	ICMPHEADER * pIcmpheader;//ICMPͷ�ṹ��ָ��
	char szSourceIP[MAX_ADDR_LEN], szDestIP[MAX_ADDR_LEN];//ԴIP��Ŀ��IP
	SOCKADDR_IN saSource, saDest;//Դ��ַ�ṹ�壬Ŀ�ĵ�ַ�ṹ��

	//���ø���ͷָ��
	pIpheader = (IPHEADER*)recvBuf;
	pTcpheader = (TCPHEADER*)(recvBuf + sizeof(IPHEADER));
	pUdpheader = (UDPHEADER*)(recvBuf + sizeof(IPHEADER));
	pIcmpheader = (ICMPHEADER*)(recvBuf + sizeof(IPHEADER));
	int iIphLen = sizeof(unsigned long)*(pIpheader->h_verlen & 0x0f);
	while (1)
	{
		memset(recvBuf, 0, sizeof(recvBuf));//��ջ�����
		recv(sock, recvBuf, sizeof(recvBuf), 0);//���հ�

		//���Դ��ַ��Ŀ�ĵ�ַ
		saSource.sin_addr.s_addr = pIpheader->sourceIP;
		strncpy(szSourceIP, inet_ntoa(saSource.sin_addr), MAX_ADDR_LEN);
		saDest.sin_addr.s_addr = pIpheader->destIP;
		strncpy(szDestIP, inet_ntoa(saDest.sin_addr), MAX_ADDR_LEN);

		//������ְ��ĳ��ȣ�ֻ���ж��Ƿ��Ǹð���������壬�ȼ��������
		lenip = ntohs(pIpheader->tatal_len);
		lentcp = ntohs(pIpheader->tatal_len) - (sizeof(IPHEADER) + sizeof(TCPHEADER));
		lenudp = ntohs(pIpheader->tatal_len) - (sizeof(IPHEADER) + sizeof(UDPHEADER));
		lenicmp = ntohs(pIpheader->tatal_len) - (sizeof(IPHEADER) + sizeof(ICMPHEADER));

		/*std::ostringstream os;
		os<<"\n**********IPЭ��ͷ��***********";
		os<<"\n��ʾ��"<<ntohs(pIpheader->ident);
		os<<"\n�ܳ��ȣ�"<<ntohs(pIpheader->tatal_len);
		os<<"\nƫ������"<< ntohs(pIpheader->frag_and_flags);
		os<<"\n����ʱ�䣺"<<(int)pIpheader->ttl;
		os<<"\n�������ͣ�"<<(int)pIpheader->tos;
		os<<"\nЭ�����ͣ�"<<(int)pIpheader->proto;
		os<<"\n����ͣ�"<< ntohs(pIpheader->checksum);
		os<<"\nԴIP��"<< szSourceIP;
		os<<"\nĿ��IP��"<< szDestIP;

		std::cout<<os.str();*/
		//+++++++++++++++++++++++++++++
		//��������Լ�������������жϺʹ���
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

	if ((mRawSock = socket(AF_INET, SOCK_RAW, IPPROTO_IP)) == SOCKET_ERROR)//����һ��ԭʼ�׽���
	{
		return FALSE;
	}

	char FAR name[MAXBYTE];
	gethostname(name, MAXBYTE);
	struct hostent FAR* pHostent;
	pHostent = gethostbyname(name);

	SOCKADDR_IN sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(1);//ԭʼ�׽���û�ж˿ڵĸ���������ֵ�������
	sa.sin_addr.s_addr = inet_addr(szLocalNetIp);

	bind(mRawSock, (SOCKADDR*)&sa, sizeof(sa));//��
	if (WSAGetLastError() == 10013)
	{
		return FALSE;
	}

	//��������Ϊ����ģʽ��Ҳ�з���ģʽ�������������������еİ���
	unsigned int optval = 1;
	DWORD dwBytesRet;
	WSAIoctl(mRawSock, SIO_RCVALL, &optval, sizeof(optval), nullptr, 0, &dwBytesRet,nullptr,nullptr);

	JKThread<RawSocket>::Start(&RawSocket::CatchThread,this);
	return TRUE;
}