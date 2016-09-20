// MonitorDirCon.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <Windows.h>
#include <cstdio>
#include <string>
//#include <cstdafx>

using namespace std;

void MonitorDir(char *file);

int _tmain(int argc, _TCHAR* argv[])
{
	MonitorDir("D:\\");
	return 0;
}

std::string GetDesByAction(DWORD dwAction)
{
	std::string s = "";
	if(dwAction == FILE_ACTION_ADDED)
	{//创建
		s = "文件创建";
	}
	else if (dwAction == FILE_ACTION_MODIFIED)
	{//修改
		s = "文件修改";
	}
	else if (dwAction == FILE_ACTION_REMOVED)
	{//修改
		s = "文件删除";
	}
	else if (dwAction == FILE_ACTION_RENAMED_OLD_NAME)
	{//修改
		s = "文件重命名";
	}
	else if (dwAction == FILE_ACTION_RENAMED_NEW_NAME)
	{//修改
		s = "文件重命名(新)";
	}
	else
	{
		s = "未知文件操作";
	}

	return s;
}


void MonitorDir(char *file){
	int nBufferSize = 1<<10;
	char buffer[1<<10];
	bool bIncludeSubdirectories = true;
	HANDLE hDirectoryHandle = NULL;

	hDirectoryHandle = ::CreateFileA(
		file,
		FILE_LIST_DIRECTORY,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
		NULL);

	if(hDirectoryHandle == INVALID_HANDLE_VALUE)	return ;

	while(1){
		DWORD dwBytes = 0;
		memset(buffer, 0, nBufferSize);
		if(!::ReadDirectoryChangesW(
			hDirectoryHandle,
			buffer,
			nBufferSize,
			bIncludeSubdirectories,
			FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_FILE_NAME |
			FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_LAST_ACCESS,
			&dwBytes,
			NULL,
			NULL) || GetLastError() == ERROR_INVALID_HANDLE){

			break;
		}
		if(!dwBytes){
			printf("Buffer Overflow\n");
		}

		PFILE_NOTIFY_INFORMATION record = (PFILE_NOTIFY_INFORMATION)buffer;
		DWORD cbOffset = 0;
		do{
			char fileName[100];
			cout<<"Action: "<<GetDesByAction(record->Action)<<endl;
			cout<<"NextEntryOffset: "<<record->NextEntryOffset<<endl;
			cout<<"FileNameLength: "<<record->FileNameLength<<endl;
			//printf("%s\n", record->FileName);
			//wcstombs(fileName, (wchar_t)record->FileName[1], sizeof(fileName));
			WCHAR sz[MAX_PATH];
			memcpy(sz,record->FileName,record->FileNameLength);
			sz[record->FileNameLength/2] = 0;
			std::string szFile = W2AString(sz);
			cout<<"FileName: "<<szFile<<endl<<endl;
			cbOffset = record->NextEntryOffset;
			record = (PFILE_NOTIFY_INFORMATION)((LPBYTE)record + cbOffset);
		}while(cbOffset);
	}//while(1)
	delete buffer;

	if(hDirectoryHandle)
		CloseHandle(hDirectoryHandle);
}

