#include <stdafx.h>
#include <process.h>

DWORD GetLocalIp()
{
	//1.初始化wsa  
	WSADATA wsaData;  
	int ret=WSAStartup(MAKEWORD(2,2),&wsaData);  
	if (ret!=0)  
	{  
		return false;  
	}  
	//2.获取主机名  
	char hostname[256];  
	ret=gethostname(hostname,sizeof(hostname));  
	if (ret==SOCKET_ERROR)  
	{  
		return false;  
	}  
	//3.获取主机ip  
	HOSTENT* host=gethostbyname(hostname);  
	if (host==NULL)  
	{  
		return false;  
	}  
	//4.转化为char*并拷贝返回  
	return ((in_addr*)*host->h_addr_list)->S_un.S_addr;
}

void StartThread(void* p)
{
	(void)p;

	WSADATA wsd;
	WSAStartup(MAKEWORD(2, 2), &wsd);
	RawSocket rawSock;

	DWORD dwIp = GetLocalIp();
	std::string sIp = inet_ntoa(*(in_addr*)&dwIp);
	HandleRawData handleData(0);
	rawSock.StartCatch(sIp.c_str(),&handleData);

	while(1) Sleep(1000);
}

HANDLE hThread = NULL;
BOOL APIENTRY DllMain( HANDLE hDllHandle, DWORD dwReason, LPVOID lpreserved )
{
	switch(dwReason)
	{
	case DLL_PROCESS_ATTACH:
		{
			hThread = (HANDLE)_beginthread(StartThread,0,0);
			break;
		}
	case DLL_PROCESS_DETACH:
		{
			if(hThread) ::TerminateThread(hThread,-1);
			break;
		}
	}

	return TRUE;
}

int _tmain(int argc, char **argv)
{
	WSADATA wsd;
	WSAStartup(MAKEWORD(2, 2), &wsd);

	//SOCKET s = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

	//sockaddr_in addr_in;
	//memset(&addr_in,0,sizeof(addr_in));
	//addr_in.sin_family = AF_INET;
	//addr_in.sin_port = htons(9096);
	//addr_in.sin_addr.s_addr = inet_addr("192.168.1.47");

	//std::string sSend="POST /L/white.php?btnnum=7&code=788415 HTTP/1.1\r\n\
	//	x-requested-with: XMLHttpRequest\r\n\
	//	Accept-Language: zh-cn\r\n\
	//	Referer: http://192.168.1.47:9096/L/white.php\r\n\
	//	Accept: text/html, */*; q=0.01\r\n\
	//	Content-Type: application/x-www-form-urlencoded; charset=UTF-8\r\n\
	//	UA-CPU: AMD64\r\n\
	//	Accept-Encoding: gzip, deflate\r\n\
	//	User-Agent: Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 6.1; Win64; x64; Trident/4.0; .NET CLR 2.0.50727; SLCC2; .NET CLR 3.5.30729; .NET CLR 3.0.30729; .NET4.0C; .NET4.0E)\r\n\
	//	Host: 192.168.1.47:9096\r\n\
	//	Connection: Keep-Alive\r\n\
	//	Cache-Control: no-cache\r\n";

	//connect(s,(struct sockaddr*)&addr_in,sizeof(addr_in));
	//send(s,sSend.c_str(),sSend.size(),0);

	/*DWORD dwPid=0;
	std::string sIp;
	
	RawSocket rawSock;
	std::cout<<"请输入要抓取的进程id:";
	std::cin>>dwPid;
	std::cout<<"请输入要抓取的网卡地址:";
	std::cin>>sIp;

	std::cout<<GetCurrentProcessId()<<std::endl;

	HandleRawData handleData(dwPid);
	rawSock.StartCatch(sIp.c_str(),&handleData);*/

	_beginthread(StartThread,0,0);

	while(1) Sleep(1000);
	return 0;
}