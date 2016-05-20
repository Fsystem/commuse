// MsgShareMemTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <conio.h>

#include <process.h>

#include "ShareMem/ShareMemry.h"
#include <map>

using std::map;
using COMMUSE::ShareMemry;

#define		MEM_SHARE_FILE_NAME		"test_rule_pid_collect_map"
#define		MAX_SHARE_MEM_SIZE	10000

//////////////////////////////////////////////////////////////////////////
/*!
* 共享内存信息节点
* 1字节（bool是否可以读）|ShareMemNode-1|...|ShareMemNode-n| (最大10000个)
*/
#pragma pack(push,1)
typedef struct ShareMemNode
{
	DWORD		dwPid;							//进程ID
	bool		bTrust;							//是否信任
}ShareMemNode;
#pragma pack(pop)

//////////////////////////////////////////////////////////////////////////
ShareMemry gShareMem;
map<DWORD,ShareMemNode> mapShareInfo;

void LoadMapData(void*);

int _tmain(int argc, _TCHAR* argv[])
{
	for (int i=0;i<100;i++)
	{
		_beginthread(LoadMapData,0,0);
	}
	

	getch();
	return 0;
}


void LoadMapData(void*)
{
	while(1)
	{
		void * pBase = gShareMem.OpenShareMemry(MEM_SHARE_FILE_NAME);
		if (pBase)
		{
			int nCanRead = *((char*)pBase);
			if (nCanRead == 0)
			{
				mapShareInfo.clear();

				ShareMemNode* pBaseNode = (ShareMemNode*)((char*)pBase+1);

				for (int i=0;i<MAX_SHARE_MEM_SIZE;i++)
				{
					if (pBaseNode->dwPid ==0 ) break;

					printf("[%d]pid:%d trust:\n",GetCurrentThreadId(),pBaseNode->dwPid,pBaseNode->bTrust);
					mapShareInfo[pBaseNode->dwPid] = *pBaseNode;

					pBaseNode++;
				}
			}

			gShareMem.CloseShareMemry();
		}
		Sleep(10);
	}
	
}