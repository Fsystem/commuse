#include <stdafx.h>



int _tmain(int argc, char **argv)
{
	WSADATA wsd;
	WSAStartup(MAKEWORD(2, 2), &wsd);

	DWORD dwPid=0;
	std::string sIp;
	
	RawSocket rawSock;
	std::cout<<"������Ҫץȡ�Ľ���id:";
	std::cin>>dwPid;
	std::cout<<"������Ҫץȡ��������ַ:";
	std::cin>>sIp;

	HandleRawData handleData(dwPid);
	rawSock.StartCatch(sIp.c_str(),&handleData);

	while(1) Sleep(1000);
	return 0;
}