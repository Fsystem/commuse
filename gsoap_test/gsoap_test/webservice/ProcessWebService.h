/** 
*   @FileName  : ProcessWebService.h
*   @Author    : Double Sword
*   @date      : 2013-2-26
*   @Copyright : Copyright belong to Author and ZhiZunNet.CO.
*   @RefDoc    : --
*/

#ifndef __ProcessWebService_H
#define __ProcessWebService_H

#include "soapH.h"
#include "SafeCriticalSection.h"
#include <list>
using std::list;
using namespace COMMUSE;


#define PROCESS_WEB_INSTANCE (CProcessWebService::CreateInstance())

struct IWebService_Process
{
	virtual bool ProcessWebPayRequest(unsigned long unIp, char* szAccount ,int nKindID,int nServerID)=0;
};

class CProcessWebService
{
public:
	virtual ~CProcessWebService();
	static CProcessWebService& CreateInstance();

public:
	//回调函数
	void SetSink(IWebService_Process* pSink);
	IWebService_Process* GetSink();

	//启动服务
	bool StartService(unsigned short usPort,int nThreadNum);
	//停止服务
	void StopService();

protected:

private:
	struct IWebService_Process* pProcess_;
	bool bRun_ ;
	struct soap web_soap_;
	int	nthread_num_;
	HANDLE* pthread_arr;
	int	nthread_acc_num_;
	HANDLE* pthread_acc_arr;
	list<SOCKET> soap_list_;
	CCritiSection lock_;
	struct soap** process_web_soap_arr_;

	//////////////////////////////////////////////////////////////////////////
	CProcessWebService();
	
private:
	void EnQueue(SOCKET s);
	SOCKET DeQueue();
	//////////////////////////////////////////////////////////////////////////
	static int http_get(struct soap* soap);
	static void process_web_accept_thread(void* p);
	static void process_web_thread(void* p);
};

#endif //__ProcessWebService_H