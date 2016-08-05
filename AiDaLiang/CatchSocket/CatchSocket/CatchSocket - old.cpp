// AssociateIWebBrowser2.cpp : �������̨Ӧ�ó������ڵ㡣
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
		BYTE Version; // �汾
		BYTE HdrLen; // IHL
	};
	BYTE ServiceType; // ��������
	WORD TotalLen; // �ܳ�
	WORD ID; // ��ʶ
	union{ 
		WORD Flags; // ��־
		WORD FragOff; // �ֶ�ƫ��
	};
	BYTE TimeToLive; // ������
	BYTE Protocol; // Э��
	WORD HdrChksum; // ͷУ���
	DWORD SrcAddr; // Դ��ַ
	DWORD DstAddr; // Ŀ�ĵ�ַ
	BYTE Options; // ѡ��
} IP; 
typedef IP * LPIP;
typedef IP UNALIGNED * ULPIP;

/*TCPͷ���壬��20���ֽ�*/
typedef struct _TCP_HEADER 
{
	short m_sSourPort;					// Դ�˿ں�16bit
	short m_sDestPort;					// Ŀ�Ķ˿ں�16bit
	unsigned int m_uiSequNum;			// ���к�32bit
	unsigned int m_uiAcknowledgeNum;	// ȷ�Ϻ�32bit
	short m_sHeaderLenAndFlag;			// ǰ4λ��TCPͷ���ȣ���6λ����������6λ����־λ
	short m_sWindowSize;				// ���ڴ�С16bit
	short m_sCheckSum;					// �����16bit
	short m_surgentPointer;				// ��������ƫ����16bit
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

			//���sockaddr_in�ṹ

			myaddr.sin_addr.s_addr = inet_addr("192.168.1.88");//*(in_addr *)pHostent->h_addr_list[0];

			myaddr.sin_family = AF_INET;

			myaddr.sin_port = htons(8888);//����IP���������

			//bind�����������׽��־���󶨵����ص�ַ

			if(SOCKET_ERROR==bind(ServerSock,(struct sockaddr *)&myaddr,sizeof(myaddr)))

			{

				closesocket(ServerSock);

				cout<<WSAGetLastError()<<endl;

				printf("..............................Error����");

				getchar();

				exit(-1);

			}


			//���ø�SOCKETΪ�������������󶨵�IP���������������ݣ��������պͷ��͵����ݰ�

			u_long sioarg = 1;

			DWORD dwValue=0;

			if( SOCKET_ERROR == WSAIoctl( ServerSock, SIO_RCVALL , &sioarg,sizeof(sioarg),NULL,0,&dwValue,NULL,NULL ) )


			{

				closesocket(ServerSock);

				cout << WSAGetLastError();

				exit(-1);
			}

			//��ȡ�������ݱ���

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
							//���Ĵ���
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

