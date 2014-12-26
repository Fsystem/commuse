#pragma once
#pragma warning(disable:4996)

#include <stdio.h>
#include <Windows.h>
#include <stdio.h>

#include <process.h>
#include <conio.h>
#include <atltrace.h>
#include <atlbase.h>
#include <atlstr.h>


#include "MemPool/MEMPool.h"
#include "SafeCriticalSection/SafeCriticalSection.h"

#include "ThreadPool/ThreadPool.h"

#include "SafeList/SafeList.h"
#include "Log/CommLog.h"

#include "MySqlDBBase/TMy_Sql_Pool.h"





using namespace COMMUSE;

class TestThread
{
public:

	void SetVal(int num );

	void* pThis;
	int a;
	int b;
	int c;
	char msg[255];
protected:
private:

};

void TestThread::SetVal(int num )
{

	a=num;
	b= a*num;
	c=b*num;
	sprintf_s(msg,255,"当前是第%d个对象",num);
}


void Work(void* p , unsigned n)
{
	TestThread* p1 = static_cast<TestThread*>(p);
	if (p)
	{
		//TestThread* pTest = (TestThread*)p;
		//unsigned nSize = n;
		char t[1024]={0};
		sprintf_s(t,1024,"CID:%u || a=%d,b=%d,c=%d,[%s]\n",GetCurrentThreadId(),p1->a,p1->b,p1->c,p1->msg);
		OutputDebugStringA(t);
		//printf(atlTraceGeneral,0, TEXT("CID:%u || a=%d,b=%d,c=%d,[%s]\n"),GetCurrentThreadId(),p1->a,p1->b,p1->c,p1->msg/*pTest->a,pTest->b,pTest->c,pTest->msg */);
// 
// 		HeapFree(GetProcessHeap(),0,p1->pThis);
	}
}

COMMUSE::SafeList sl;
static int i = 0;

unsigned __stdcall WrtieThrd(void* p)
{
	
	while(1)
	{
		InterlockedIncrement((long*)&i);
		sl.Insert((void*)(i));
		Sleep(5);

		LOGEVEN(TEXT("write==>"));
	}
	return 0;
}

unsigned __stdcall ReadThrd(void* p)
{
	int p1=0;
	while(1)
	{
		
		p1 = (int)sl.RemoveHead();
		if (p1 )
		{
			ATLTRACE2(atlTraceGeneral,0, "out==>ID:%d\n",p1 );
			LOGEVEN(TEXT("out==>ID:%d\n"),p1);
		}
		Sleep(5);
	}
	return 0;
}

int main()
{
//	PrintInit();

// 	const char* s = "iios/12DDWDFF@122";
// 	char buf[20];
// 
// 	sscanf( s, "%*[^/]/%[^@]", buf );
// 	printf( "%s\n", buf );

	//测试list
//==============================================================
	/*HANDLE hArr[4]={0};
	for (int j = 0 ; j< sizeof(hArr)/sizeof(HANDLE);j++)
	{	
		if ( 0 == j )
		{
			hArr[j]=(HANDLE)_beginthreadex(0,0,WrtieThrd,0,0,0);
		}
		else
		{
			hArr[j]=(HANDLE)_beginthreadex(0,0,ReadThrd,0,0,0);
		}
	}


	char cmd[255]={0};
	while(gets(cmd))
	{
		if (!stricmp(cmd,"max"))
		{
			printf("max=%u\n",sl.GetCount() );
		}
		else if (!stricmp(cmd,"ival"))
		{
			printf("i=%u\n",i );
		}
		else
		{
			for (int j = 0 ; j< sizeof(hArr)/sizeof(HANDLE);j++)
			{	
				if ( hArr[j] )
				{
					TerminateThread(hArr[j],0);
				}
				
			}
			break;
		}
	}
	
	PrintUnInit();*/
//==============================================================

	//测试内存池AWE
//==============================================================
	COMMUSE::AWEMEMPool<TestThread> mempool(1000);
	TestThread** pt1 = mempool.Allocate();
	if (pt1)
	{
		(*pt1)->a = 1;
		(*pt1)->b = 1;
		(*pt1)->c = 1;
		strcpy((*pt1)->msg,"test");
	}
	
	mempool.Free(pt1);

	COMMUSE::MEMPool<TestThread> mempool1(1000);
	TestThread** pt2= mempool1.Allocate();
	if (pt2)
	{
		(*pt2)->a = 1;
		(*pt2)->b = 1;
		(*pt2)->c = 1;
		strcpy((*pt2)->msg,"test");
	}

	mempool1.Free(pt2);

//==============================================================

	//system("pause");
//==============================================================
	//测试锁
	COMMUSE::CCritiSection cs;
	if (1)
	{//进锁
		COMMUSE::LOCK lock(cs);
	}//出锁
//==============================================================


//==============================================================
	//线程池
 	int iCount = 0;
 	
 	ThreadPool tp(6);
 
 	tp.Start(Work);
 
 	int c = 1000;
	TestThread **ts  = NULL;
	COMMUSE::MEMPool<TestThread> mempoolThread(c);
 	while(c-- > 0)
 	{
 		ts = mempoolThread.Allocate();

 		(*ts)->pThis = ts;
 		(*ts)->SetVal(++iCount);
 		tp.AddTask( (*ts),sizeof(TestThread) );
 		//Sleep(1);
 	}
	Sleep(1000);
	tp.Stop();



//==============================================================
//测试MYsql
	COMMUSE::TMySqlPool* my  = new COMMUSE::TMySqlPool("192.168.1.56","metin","123321",3306,"lottery");
	TMySql* pMy = my->GetConnect("lottery");
	if (pMy)
	{
		pMy->ExecuteSql("call lottery_run('测试','20141030','20141111','pgn',2,@res,@status);",false,0,0);
		pMy->ExecuteSql("select @res,@status;",false,0,0);
		printf(pMy->GetResult(0,(unsigned int)0));
		printf("\n");
		printf(pMy->GetResult(0,(unsigned int)1));

		my->FreeConnect(pMy);
	}

//==============================================================

	
	return 0;
}