// GSoapTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <windows.h>
#include <fstream>
#include <sstream>
#include <process.h>
#include "../jkSoapProcess/ProcessWebService.h"

//先去生成wsdl

using namespace std;

std::string xml;

int _tmain(int argc, _TCHAR* argv[])
{
	char szAppDir[MAX_PATH];
	GetModuleFileNameA(NULL,szAppDir,MAX_PATH);
	strrchr(szAppDir,'\\')[1]=0;
	strcat(szAppDir,"..\\WebInterfaceDef\\WebServerProcess\\WebCall.wsdl");

	
	std::ostringstream os;

	ifstream fi(szAppDir);

	if (fi.is_open())
	{
		os<<fi.rdbuf();
		xml = os.str();
	}

	PROCESS_WEB_INSTANCE.StartService(9696,6,szAppDir);

	while(1)Sleep(1000);
	return 0;
}