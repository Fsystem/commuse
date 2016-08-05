// AssociateIWebBrowser2.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
//#include "AssociateIeHandle.h"
//#include "CatchDevDataIe.h"

#include <iostream>

#include <WinSock2.h>
#include <Mstcpip.h>


#pragma comment(lib,"ws2_32.lib")

using namespace std;

typedef struct _IP{
	union{ 
		BYTE Version; // 版本
		BYTE HdrLen; // IHL
	};
	BYTE ServiceType; // 服务类型
	WORD TotalLen; // 总长
	WORD ID; // 标识
	union{ 
		WORD Flags; // 标志
		WORD FragOff; // 分段偏移
	};
	BYTE TimeToLive; // 生命期
	BYTE Protocol; // 协议
	WORD HdrChksum; // 头校验和
	DWORD SrcAddr; // 源地址
	DWORD DstAddr; // 目的地址
	BYTE Options; // 选项
} IP; 
typedef IP * LPIP;
typedef IP UNALIGNED * ULPIP;

/*TCP头定义，共20个字节*/
typedef struct _TCP_HEADER 
{
	short m_sSourPort;					// 源端口号16bit
	short m_sDestPort;					// 目的端口号16bit
	unsigned int m_uiSequNum;			// 序列号32bit
	unsigned int m_uiAcknowledgeNum;	// 确认号32bit
	short m_sHeaderLenAndFlag;			// 前4位：TCP头长度；中6位：保留；后6位：标志位
	short m_sWindowSize;				// 窗口大小16bit
	short m_sCheckSum;					// 检验和16bit
	short m_surgentPointer;				// 紧急数据偏移量16bit
}TCP_HEADER, *PTCP_HEADER;

void CatchSocket()
{
	WORD wVersion;

	WSADATA wsadata;

	int err;

	wVersion = MAKEWORD(2,2);

	// WSAStartup() initiates the winsock,if successful,the function returns zero

	err = ::WSAStartup(wVersion,&wsadata);

	if(err!=0)

	{

		printf("Couldn't initiate the winsock!/n");

	}

	else

	{

		// create a socket

		SOCKET ServerSock=socket(AF_INET,SOCK_RAW,IPPROTO_IP);

		char mname[128];

		struct hostent* pHostent;

		sockaddr_in myaddr;

		//Get the hostname of the local machine

		if( -1 == gethostname(mname, sizeof(mname)))

		{

			closesocket(ServerSock);

			printf("%d",WSAGetLastError());

			exit(-1);

		}

		else

		{

			//Get the IP adress according the hostname and save it in pHostent

			pHostent=gethostbyname((char*)mname);

			//填充sockaddr_in结构

			myaddr.sin_addr.s_addr = inet_addr("192.168.1.88");//*(in_addr *)pHostent->h_addr_list[0];

			myaddr.sin_family = AF_INET;

			myaddr.sin_port = htons(8888);//对于IP层可随意填

			//bind函数创建的套接字句柄绑定到本地地址

			if(SOCKET_ERROR==bind(ServerSock,(struct sockaddr *)&myaddr,sizeof(myaddr)))

			{

				closesocket(ServerSock);

				cout<<WSAGetLastError()<<endl;

				printf("..............................Error……");

				getchar();

				exit(-1);

			}


			//设置该SOCKET为接收所有流经绑定的IP的网卡的所有数据，包括接收和发送的数据包

			u_long sioarg = 1;

			DWORD dwValue=0;

			if( SOCKET_ERROR == WSAIoctl( ServerSock, SIO_RCVALL , &sioarg,sizeof(sioarg),NULL,0,&dwValue,NULL,NULL ) )


			{

				closesocket(ServerSock);

				cout << WSAGetLastError();

				exit(-1);
			}

			//获取分析数据报文

			char buf[65535];

			int len = 0;

			listen(ServerSock,5);

			do

			{

				len = recv( ServerSock, buf, sizeof(buf),0);

				if( len > 0 )

				{
					LPIP pHead = (LPIP)buf;
					if (pHead->Protocol == IPPROTO_TCP)
					{
						PTCP_HEADER pTcpHead = PTCP_HEADER(pHead+1);
						char* pBody = (char*)(pHead+1);
						if ((pBody[0]=='P'&&pBody[1]=='O'&&pBody[2]=='S'&&pBody[3]=='T')
							||(pBody[0]=='G'&&pBody[1]=='E'&&pBody[2]=='T') )
						{
							//报文处理
							printf("%s\n",pBody);
						}
						std::string sBody = pBody;
						if(sBody.find("HTTP")!=-1)
						{
							printf("HTTP\n");
						}
						
					}
					

				}
			}while( len > 0 );

		} 
	}

	::WSACleanup();
}

int _tmain(int argc, _TCHAR* argv[])
{
	/*CoInitialize(NULL);

	CatchDevDataIe cie;
	cie.StartService();
	TestIe();*/

	CatchSocket();

	while(1) Sleep(1000);
	return 0;
}

