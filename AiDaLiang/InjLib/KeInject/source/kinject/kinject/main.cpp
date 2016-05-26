#include <stdio.h>
#include <Windows.h>

typedef struct _INJECT_INFO
{
	HANDLE ProcessId;
	wchar_t DllName[1024];
}INJECT_INFO,*PINJECT_INFO;

int wmain(int argc,wchar_t* argv[])
{
	HANDLE hFile;
	DWORD write;

	INJECT_INFO InjectInfo;

	if(argc<3)
	{
		printf("\nUsage: kinject [PID] [DLL name]\n");
		return -1;
	}

	hFile=CreateFile(L"\\\\.\\KeInject",GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);

	if(hFile==INVALID_HANDLE_VALUE)
	{
		printf("\nError: Unable to connect to the driver (%d)\n",GetLastError());
		return -1;
	}

	memset(&InjectInfo,0,sizeof(INJECT_INFO));

	InjectInfo.ProcessId=(HANDLE)wcstoul(argv[1],NULL,0);
	wcscpy(InjectInfo.DllName,argv[2]);

	if(!WriteFile(hFile,&InjectInfo,sizeof(INJECT_INFO),&write,NULL))
	{
		printf("\nError: Unable to write data to the driver (%d)\n",GetLastError());
		
		CloseHandle(hFile);
		return -1;
	}

	CloseHandle(hFile);
	return 0;
}