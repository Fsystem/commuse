#include "stdafx.h"
#include "FileMonitor.h"

static CCritiSection mLock;

//-------------------------------------------------------------------------------
FileMonitor::FileMonitor()
{
	bRun = FALSE;
	hThreadMonitor = NULL;
	hDir = NULL;
	mSink = NULL;
}

FileMonitor::~FileMonitor()
{
	StopMonitor();
}

BOOL FileMonitor::StartMonitor(LPCSTR szDir,IFileMonitorDelegate* pSink)
{
	if (bRun == FALSE)
	{
		SAFE_CLOSE_HANDLE(hDir);

		char szDirTmp[MAX_PATH]={0};
		strncpy(chDir,szDir,MAX_PATH);
		strncpy(szDirTmp,szDir,MAX_PATH);
		strcat(szDirTmp,"\\");
		mSink = pSink;

		hDir = CreateFileA(szDirTmp,FILE_LIST_DIRECTORY
			,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE
			,NULL
			,OPEN_EXISTING
			,FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED
			,NULL);

		if (hDir == INVALID_HANDLE_VALUE)
		{
			LOGEVENA(">>>FileMonitor::StartMonitor[%u]\n",GetLastError());
			return FALSE;
		}

		bRun = TRUE;
		hThreadMonitor = (HANDLE)JKThread<FileMonitor>::Start(&FileMonitor::FileMonitorThread,this);
	}

	return TRUE;
}

void FileMonitor::StopMonitor()
{
	if (bRun)
	{
		bRun = FALSE;
		mSink = NULL;

		SAFE_STOP_THREAD(hThreadMonitor);

		SAFE_CLOSE_HANDLE(hDir);
	}
}

void FileMonitor::FileMonitorThread()
{
	char buffer[1024];
	DWORD dwTransfer = 0;

	while(bRun)
	{
		dwTransfer = 0;
		ZeroMemory(buffer,sizeof buffer);
		BOOL state = ReadDirectoryChangesW(hDir,buffer
			,sizeof buffer
			,TRUE
			,//FILE_NOTIFY_CHANGE_LAST_WRITE 
			 FILE_NOTIFY_CHANGE_SIZE 
			| FILE_NOTIFY_CHANGE_FILE_NAME 
			//| FILE_NOTIFY_CHANGE_CREATION 
			//| FILE_NOTIFY_CHANGE_LAST_ACCESS
			,&dwTransfer,NULL,NULL);
		if (GetLastError()==ERROR_INVALID_FUNCTION) 
		{ 
			LOGEVENA("系统不支持!"); 
		} 
		else if(state==FALSE) 
		{ 
			LOGEVENA("监控失败!"); 
		} 
		else if (GetLastError()==ERROR_NOTIFY_ENUM_DIR) 
		{ 
			LOGEVENA("内存溢出!"); 
		} 
		else 
		{
			PFILE_NOTIFY_INFORMATION pchInfo = (PFILE_NOTIFY_INFORMATION)buffer;
			DWORD cbOffset = 0;
			do{
				WCHAR szFile[MAX_PATH];
				if(pchInfo->FileNameLength >= MAX_PATH*sizeof WCHAR) pchInfo->FileNameLength = (MAX_PATH-1)*sizeof WCHAR;
				memcpy(szFile,pchInfo->FileName,pchInfo->FileNameLength);
				szFile[pchInfo->FileNameLength/sizeof WCHAR]=0;

				std::string fileName = W2AString(szFile);
				char szFullName[MAX_PATH];
				_snprintf(szFullName,MAX_PATH,"%s\\%s",chDir,fileName.c_str());

				if (mSink)
				{
					mSink->OnFileChange(szFullName,pchInfo->Action);
				}

				cbOffset = pchInfo->NextEntryOffset;
				pchInfo = (PFILE_NOTIFY_INFORMATION)((LPBYTE)pchInfo + cbOffset);
			}while(cbOffset);

		}

		Sleep(30);
	}
}
//-------------------------------------------------------------------------------
FileMonMgr::FileMonMgr()
{
	fileMonitors = NULL;
	mDriveCnt = 0;
	mSink = NULL;
}

FileMonMgr::~FileMonMgr()
{
	StopService();
}


BOOL FileMonMgr::StartService(BOOL bExeptRecycle,BOOL bExceptSysDrive,IFileMonitorDelegate* pSink)
{
	StopService();

	mSink = pSink;

	auto vecDrives = GetAllDisks();
	auto sSysDrive = GetSysDisk();
	
	if (bExceptSysDrive)
	{
		std::remove(vecDrives.begin(),vecDrives.end(),sSysDrive);
		vecDrives.pop_back();
	}
	
	mDriveCnt = vecDrives.size();
	fileMonitors = new FileMonitor*[vecDrives.size()];
	if (fileMonitors)
	{
		int i = 0;
		for(auto it = vecDrives.begin();it != vecDrives.end(); ++it,++i)
		{
			fileMonitors[i] = new FileMonitor;
			fileMonitors[i]->StartMonitor(it->c_str(),this);
		}
	}
	
	return TRUE;
}
void FileMonMgr::StopService()
{
	if (fileMonitors)
	{
		for (int i=0;i<mDriveCnt;i++)
		{
			if (fileMonitors[i])
			{
				fileMonitors[i]->StopMonitor();
				delete fileMonitors[i];
				fileMonitors[i] = NULL;
			}
			
		}

		delete [] fileMonitors;
		fileMonitors = NULL;
	}
	
	mSink = NULL;
}

std::string FileMonMgr::GetSysDisk()
{
	char szSysDir[MAX_PATH];
	GetSystemDirectoryA(szSysDir,MAX_PATH);

	_splitpath(szSysDir,szSysDir,NULL,NULL,NULL);

	return szSysDir;
}

std::vector<std::string> FileMonMgr::GetAllDisks()
{
	std::vector<std::string> vec;
	char szBuf [1024];
	GetLogicalDriveStringsA (1024, szBuf);

	char szDrive [64] = {0},szType[64]={0};
	for (char* pszDrive = (char*)szBuf; pszDrive != NULL && *pszDrive != 0; pszDrive += strlen (pszDrive) + 1) {
		strcpy(szDrive,pszDrive);
		_splitpath(szDrive,szDrive,NULL,NULL,NULL);
		switch(GetDriveTypeA(szDrive))
		{
		case DRIVE_FIXED:
		case DRIVE_REMOTE:
		case DRIVE_RAMDISK:
			vec.push_back(szDrive);
		}
	}

	return vec;
}

void FileMonMgr::OnFileChange(LPCSTR lpszFile,DWORD dwAction)
{
	bool bOpt = true;
	char szTmp[MAX_PATH];
	char szRecycleOpt[32]={0};
	bool bRecycle = false;
	if (stristr(lpszFile,":\\$recycle.bin"))
	{
		strcpy(szRecycleOpt,"[回收站]");
		bRecycle = true;
	}
	if(dwAction == FILE_ACTION_ADDED)
	{//创建
		sprintf(szTmp,"%s文件创建:%s\n",szRecycleOpt,lpszFile);
	}
	else if (dwAction == FILE_ACTION_MODIFIED)
	{//修改
		sprintf(szTmp,"%s文件修改:%s\n",szRecycleOpt,lpszFile);
	}
	else if (dwAction == FILE_ACTION_REMOVED)
	{//修改
		sprintf(szTmp,"%s文件删除:%s\n",szRecycleOpt,lpszFile);
		bOpt = false;
	}
	else if (dwAction == FILE_ACTION_RENAMED_OLD_NAME)
	{//修改
		sprintf(szTmp,"%s文件重命名:%s\n",szRecycleOpt,lpszFile);
		bOpt = false;
	}
	else if (dwAction == FILE_ACTION_RENAMED_NEW_NAME)
	{//修改
		sprintf(szTmp,"%s文件重命名(新):%s\n",szRecycleOpt,lpszFile);
		bOpt = false;
	}
	else
	{
		sprintf(szTmp,"%s未知文件操作(%d):%s\n",szRecycleOpt,dwAction,lpszFile);
		bOpt = false;
	}

	LOGEVENA(szTmp);

	if (bOpt && mMapFileOpt[lpszFile].size()<20 && bRecycle==false)
	{
		LOCK lk(mLock);
		mMapFileOpt[lpszFile].push_back(dwAction);
	}

	if (mSink && bRecycle == false)
	{
		mSink->OnFileChange(lpszFile,dwAction);
	}
}