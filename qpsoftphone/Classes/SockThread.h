#ifndef __SockThread_H
#define __SockThread_H
/** socket thread
*   @FuncName : 
*   @Author   : Double sword
*   @Params   : 	
		
*   @Return   :	
						
*   @Date     :	2012-12-14
*/
#include "AppInc.h"
#include "COThread.h"

class CSocketThread : public CCOThread
{
public:
	virtual void* ThreadWork(void* p);
protected:
private:
	CC_SYNTHESIZE(void* , psockclien,SocketClientPtr);
};
#endif //__SockThread_H