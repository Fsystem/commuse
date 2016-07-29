#include "stdafx.h"
#include "ProcessWebService.h"
#include<process.h>


//////////////////////////////////////////////////////////////////////////
//WebService Implement
//////////////////////////////////////////////////////////////////////////
//server端的实现函数与add.h中声明的函数相同，但是多了一个当前的soap连接的参数
int ns__currentTime(struct soap * soap,time_t& response)
{
	if(PROCESS_WEB_INSTANCE.GetSink())
	{
		PROCESS_WEB_INSTANCE.GetSink()->WebCurrentTime(response);
	}
	response = time(0);
	return SOAP_OK;
}

//////////////////////////////////////////////////////////////////////////

CProcessWebService::CProcessWebService()
{
	bRun_=false;
	pProcess_=NULL;
	nthread_num_=0;
	pthread_arr=NULL;
	pthread_acc_arr=NULL;
	nthread_acc_num_=0;
	process_web_soap_arr_=NULL;
}

CProcessWebService::~CProcessWebService()
{
	pProcess_=NULL;
	StopService();
}

CProcessWebService& CProcessWebService::CreateInstance()
{
	static CProcessWebService c;
	return c;
}


//设置回调函数
void CProcessWebService::SetSink(IWebService_Process* pSink)
{
	pProcess_=pSink;
}

IWebService_Process* CProcessWebService::GetSink()
{
	return pProcess_;
}

//启动服务
bool CProcessWebService::StartService(unsigned short usPort,int nThreadNum,std::string wsdlPath)
{
	if (bRun_)return true;

	int m; /* master and slave sockets */

	mWsdlPath = wsdlPath;

	soap_init(&web_soap_);

	//soap_set_namespaces(&add_soap, add_namespaces);
	//设置http模式
	web_soap_.fget=&CProcessWebService::http_get;

	m = soap_bind(&web_soap_, ADDR_ANY, usPort, 100);
	if (m < 0)
	{
		soap_print_fault(&web_soap_, stderr);
		return false;
	}

	bRun_=true;

	//启动接受线程
	nthread_acc_num_=1;
	pthread_acc_arr = new HANDLE[nthread_acc_num_];
	for (int i=0;i<nthread_acc_num_;i++)
	{
		pthread_acc_arr[i]=(HANDLE)_beginthread(process_web_accept_thread,0,&web_soap_);
	}

	nthread_num_=nThreadNum;
	pthread_arr = new HANDLE[nthread_num_];
	process_web_soap_arr_=new struct soap*[nthread_num_];
	for (int i=0;i<nthread_num_;i++)
	{
		process_web_soap_arr_[i]=soap_copy(&web_soap_);
		pthread_arr[i]=(HANDLE)_beginthread(process_web_thread,0,process_web_soap_arr_[i]);
	}

	return true;
}

//停止服务
void CProcessWebService::StopService()
{
	if (bRun_)
	{
		for (int i=0;i<nthread_num_;i++)
		{
			if(pthread_arr) 
			{
				::TerminateThread(pthread_arr[i],0);
			}

			if (process_web_soap_arr_)
			{
				free(process_web_soap_arr_[i]);
			}
		}
		nthread_num_=0;
		if(pthread_arr)
		{
			delete[] pthread_arr;
			pthread_arr=NULL;
		}

		if (process_web_soap_arr_)
		{
			delete[] process_web_soap_arr_;
			process_web_soap_arr_=NULL;
		}

		for (int i=0;i<nthread_acc_num_;i++)
		{
			if(pthread_acc_arr) 
			{
				::TerminateThread(pthread_acc_arr[i],0);
			}
		}
		nthread_acc_num_=0;
		if(pthread_acc_arr)
		{
			delete[] pthread_acc_arr;
			pthread_acc_arr=NULL;
		}

		soap_done(&web_soap_);

		bRun_=false;
	}
}

void CProcessWebService::EnQueue(SOCKET s)
{
	LOCK lk(lock_);
	soap_list_.push_back(s);
}

SOCKET CProcessWebService::DeQueue()
{
	SOCKET s=INVALID_SOCKET;
	LOCK lk(lock_);
	if (soap_list_.size()>0)
	{
		s=soap_list_.front();
		soap_list_.pop_front();
	}
	
	return s;
}

int CProcessWebService::http_get(struct soap* soap) 
{ 
	try
	{
		FILE*fd = NULL;
		//char szPath[MAX_PATH];
		//::GetModuleFileNameA(NULL,szPath,MAX_PATH);
		//strrchr(szPath,'\\')[1]=0;
		//strcat(szPath,"wsdl\\WebService.wsdl");
		fd = fopen(PROCESS_WEB_INSTANCE.GetWsdl().c_str(), "rb"); //open WSDL file to copy
		if (!fd) throw 0;
	
		soap->http_content = "text/xml";  //HTTP header with text /xml content
		soap_response(soap,SOAP_FILE);
		for(;;)
		{
			size_t r = fread(soap->tmpbuf,1, sizeof(soap->tmpbuf), fd);
			if (!r)
			{
				break;
			}
			if (soap_send_raw(soap, soap->tmpbuf, r))
			{
				break; //cannot send, but little we can do about that
			}
		}
		fclose(fd);
		soap_end_send(soap);
	}
	catch(...)
	{
		return 404; //return HTTP not found error
	}
	
	return SOAP_OK; 
} 

void CProcessWebService::process_web_accept_thread(void* p)
{
	struct soap* web_soap=(struct soap*)p;
	int s;
	while(1)
	{
		try
		{
			s = soap_accept(web_soap); 
			if (!soap_valid_socket(s)) throw 0;

			PROCESS_WEB_INSTANCE.EnQueue(s);
		}
		catch (...)
		{
			soap_print_fault(web_soap, stderr);
			continue;
		}
		
	}
	
}

void CProcessWebService::process_web_thread(void* p)
{
	struct soap* web_soap=(struct soap*)p;
	struct sockaddr_in addrin;
	int len =sizeof(struct sockaddr_in);
	
	while (true)
	{ 
		try
		{
			SOCKET s=PROCESS_WEB_INSTANCE.DeQueue();
			if(INVALID_SOCKET == s) throw 0;

			getpeername(s,(struct sockaddr*)&addrin,&len);
			web_soap->ip=htonl(addrin.sin_addr.S_un.S_addr);
			//客户端的IP地址
			//fprintf(stderr,"Accepted connection from IP= %d.%d.%d.%d socket = %d \n",
			//	((web_soap.ip)>>24)&0xFF,((web_soap.ip)>>16)&0xFF,((web_soap.ip)>>8)&0xFF,(web_soap.ip)&0xFF,(web_soap.socket));
			web_soap->socket=s;
			soap_serve(web_soap);//该句说明该server的服务
			soap_end(web_soap);
		}
		catch(...)
		{
			Sleep(10);
			continue;
		}
	}

	_endthread();
}