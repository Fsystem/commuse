#ifndef __SocketClient_H
#define __SocketClient_H
/** ��ƽ̨socket�ͻ��(win32,android,ios)
*   @FileName  : SocketClient.h
*   @Author    : Double Sword
*   @date      : 2012-12-13
*   @Copyright : Copyright belong to Author and ZhiZunNet.CO.
*   @RefDoc    : --
*/

//////////////////////////////////////////////////////////////////////////
#include "AppInc.h"
#include "SockThread.h"

using namespace cocos2d;

//////////////////////////////////////////////////////////////////////////



class CSocketClient;
class CSocketThread;

/** socket˻ص�ӿ
*   @ClassName	: 
*   @Purpose	:
*
*
*
*
*   @Author	: Double Sword
*   @Data	: 2012-12-13
*/

typedef struct _ISocketSink{
	virtual void OnSocketConnect(int nErrCode,CSocketClient* pSocket)=0;
	virtual void OnSocketRead(CSocketClient* pSocket,void* pDate,unsigned int unDateLen)=0;
	virtual void OnSocketClose(CSocketClient* pSocket)=0;
}ISocketSink;




//socket
class CSocketClient
{
public:
	CSocketClient();
	~CSocketClient();
	void SetSocketSink(ISocketSink* pSink);
	bool Connect(const char* sIp,unsigned short usPort);
	void Disconnect();
	int	 GetError();
	bool DnsParse(const char* domain, char* ip);
	void OnThreadWork(void* p);


private:
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 
	SOCKET			nSocket_;
#else 
	int				nSocket_;
#endif
	ISocketSink*	pSink_;
	//pthread_t		tid_;
	//bool			bRun_;
	CSocketThread	thread_;


	fd_set read_flags_,write_flags_; // you know what these are
	struct timeval waitd_;          
	int thefd_;             // The socket
	char outbuff_[8192];     // Buffer to hold outgoing data
	char inbuff_[8192];      // Buffer to read incoming data into
	
};

extern "C" bool Init_Network();
extern "C" void Unint_Network();
#endif //__SocketClient