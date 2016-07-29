/** 
*   @FileName  : ProcessWebService.h
*   @Author    : Double Sword
*   @date      : 2013-2-26
*   @Copyright : Copyright belong to Author and ZhiZunNet.CO.
*   @RefDoc    : --
*/

#ifndef __ProcessWebService_H
#define __ProcessWebService_H

#include "SafeCriticalSection/SafeCriticalSection.h"
#include <list>
using std::list;

using namespace COMMUSE;

#define PROCESS_WEB_INSTANCE (CProcessWebService::CreateInstance())

struct IWebService_Process
{
	virtual bool WebCurrentTime(time_t& response)=0;
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
	bool StartService(unsigned short usPort,int nThreadNum,std::string wsdlPath);
	//停止服务
	void StopService();

	//获取wsdl
	std::string GetWsdl(){return mWsdlPath;};

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
	std::string mWsdlPath;
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