// gsoap_test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <conio.h>
#include "webservice/ProcessWebService.h"

#include <crtdbg.h>

//int http_get(struct soap* soap);

 class mewebtest:public IWebService_Process
 {
 public:
 	virtual bool ProcessWebPayRequest(unsigned long unIp, char* szAccount ,int nKindID,int nServerID)
 	{
 		printf("%s\n",szAccount);
 		return true;
 	}
 };


int _tmain(int argc, _TCHAR* argv[])
{
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG)|_CRTDBG_LEAK_CHECK_DF);
	//int m, s; /* master and slave sockets */
	//struct soap web_soap;
	//soap_init(&web_soap);
	////soap_set_namespaces(&add_soap, add_namespaces);
	////设置http模式
	//web_soap.fget=http_get;

	//m = soap_bind(&web_soap, ADDR_ANY, 10010, 100);
	//if (m < 0)
	//{
	//	soap_print_fault(&web_soap, stderr);
	//	exit(-1);
	//}

	//fprintf(stderr, "Socket connection successful: master socket = %d\n", m);
	//for ( ; ; )
	//{ 
	//	s = soap_accept(&web_soap); 
	//	if (!soap_valid_socket(s))
	//	{ 
	//		soap_print_fault(&web_soap, stderr);
	//		continue;
	//	}
	//	//客户端的IP地址
	//	fprintf(stderr,"Accepted connection from IP= %d.%d.%d.%d socket = %d \n",
	//		((web_soap.ip)>>24)&0xFF,((web_soap.ip)>>16)&0xFF,((web_soap.ip)>>8)&0xFF,(web_soap.ip)&0xFF,(web_soap.socket));

	//	soap_serve(&web_soap);//该句说明该server的服务
	//	soap_end(&web_soap);
	//}

	//soap_done(&web_soap);


	//////////////////////////////////////////////////////////////////////////
 	mewebtest test;
 	PROCESS_WEB_INSTANCE.SetSink(&test);
 	if(PROCESS_WEB_INSTANCE.StartService(10010,6))
	{
		getch();
 		PROCESS_WEB_INSTANCE.StopService();
	}
	return 0;
}


//
//int http_get(struct soap* soap) 
//{ 
//	FILE*fd = NULL;
//	fd = fopen("wsdl/WebService.wsdl", "rb"); //open WSDL file to copy
//	if (!fd)
//	{
//		return 404; //return HTTP not found error
//	}
//	soap->http_content = "text/xml";  //HTTP header with text /xml content
//	soap_response(soap,SOAP_FILE);
//	for(;;)
//	{
//		size_t r = fread(soap->tmpbuf,1, sizeof(soap->tmpbuf), fd);
//		if (!r)
//		{
//			break;
//		}
//		if (soap_send_raw(soap, soap->tmpbuf, r))
//		{
//			break; //cannot send, but little we can do about that
//		}
//	}
//	fclose(fd);
//	soap_end_send(soap);
//	return SOAP_OK; 
//} 