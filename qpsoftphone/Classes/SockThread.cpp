#include "SockThread.h"
#include "SocketClient.h"

void* CSocketThread::ThreadWork(void* p)
{
	((CSocketClient*)getSocketClientPtr())->OnThreadWork(p);
	return NULL;
}