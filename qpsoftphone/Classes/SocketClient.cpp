
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 
#include "SocketClient.h"




//////////////////////////////////////////////////////////////////////////
//全局变量
static pthread_mutex_t s_lock;
//////////////////////////////////////////////////////////////////////////

bool Init_Network()
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	WSAStartup(MAKEWORD(2,2),&WSAData());
#endif

	return true;
}

void Unint_Network()
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	WSACleanup();
#endif
}

//////////////////////////////////////////////////////////////////////////

CSocketClient::CSocketClient()
{
	thread_.setSocketClientPtr(this);
	//memset(&tid_,0,sizeof(tid_));
	//bRun_ = false;
}

CSocketClient::~CSocketClient()
{
	//Disconnect();
}

void CSocketClient::SetSocketSink(ISocketSink* pSink)
{
	CCAssert(pSink,"call back sink not allow null");
	pSink_ = pSink;
}

bool CSocketClient::Connect(const char* sIp,unsigned short usPort)
{
	bool bRes = false;

	memset(outbuff_,0,sizeof(outbuff_));
	memset(inbuff_,0,sizeof(inbuff_));

	nSocket_ = socket(AF_INET,SOCK_STREAM,0);

	if(INVALID_SOCKET == nSocket_) return bRes;

	 //设置为非阻塞模式
	unsigned long ul = 1;
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	ioctlsocket(nSocket_,FIONBIO,&ul);
#else
	ioctl(nSocket_, FIONBIO, &ul);
#endif
	
	struct sockaddr_in svraddr;
	svraddr.sin_family		= AF_INET;
	svraddr.sin_addr.s_addr	= inet_addr(sIp);
	svraddr.sin_port		= htons(usPort);
	int nRes = connect(nSocket_,(struct sockaddr*)&svraddr,sizeof(svraddr));

	bRes = (nRes!=SOCKET_ERROR?true:false);
	if (bRes)
	{
		//启动线程
		bRes = thread_.CreateThred();
	}

	return bRes;
}

void CSocketClient::Disconnect()
{
	if (-1 != nSocket_)
	{
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
		closesocket(nSocket_);
#else
		close(nSocket_);
#endif

		thread_.DestroyThread();

	}
}


int CSocketClient::GetError()
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	return (WSAGetLastError());
#else
	return (errno);
#endif
}

bool CSocketClient::DnsParse(const char* domain, char* ip)
{
	struct hostent* p;
	if ( (p = gethostbyname(domain)) == NULL )
		return false;

	sprintf(ip, 
		"%u.%u.%u.%u",
		(unsigned char)p->h_addr_list[0][0], 
		(unsigned char)p->h_addr_list[0][1], 
		(unsigned char)p->h_addr_list[0][2], 
		(unsigned char)p->h_addr_list[0][3]);

	return true;
}


void CSocketClient::OnThreadWork(void* p)
{
	int err;	       // holds return values

	waitd_.tv_sec = 1;     // Make select wait up to 1 second for data
	waitd_.tv_usec = 0;    // and 0 milliseconds.
	FD_ZERO(&read_flags_); // Zero the flags ready for using
	FD_ZERO(&write_flags_);
	FD_SET(nSocket_, &read_flags_);
	if(strlen(outbuff_)!=0) FD_SET(nSocket_, &write_flags_);
	err=select(nSocket_+1, &read_flags_,&write_flags_,
		(fd_set*)0,&waitd_);
	if(err < 0) return;
	if(FD_ISSET(nSocket_, &read_flags_)) { //Socket ready for reading
		FD_CLR(nSocket_, &read_flags_);
		memset(&inbuff_,0,sizeof(inbuff_));
		if (read(nSocket_, inbuff_, sizeof(inbuff_)-1) <= 0) {
			Disconnect();
			return;
		}
		else printf("%s",inbuff_);
	}
	if(FD_ISSET(nSocket_, &write_flags_)) { //Socket ready for writing
		FD_CLR(nSocket_, &write_flags_);
		write(nSocket_,outbuff_,strlen(outbuff_));
		memset(&outbuff_,0,sizeof(outbuff_));
	}
	// now the loop repeats over again
	
}