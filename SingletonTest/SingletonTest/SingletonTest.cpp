// SingletonTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <conio.h>

class S1:public SingletonBase<S1>
{
public:
	void test()
	{
		printf("%s\n",GetClassName());
	}

	~S1()
	{
		printf("~s1()\n");
	}
};

class S2:public SingletonBase<S2>
{
public:
	void test()
	{
		printf("%s\n",GetClassName());
	}
	~S2()
	{
		printf("~s2()\n");
	}
};

struct S3 : public SingletonBase<S3>
{
	void test()
	{
		printf("%s\n",GetClassName());
	}
	~S3()
	{
		printf("~s3()\n");
	}
};

enum Temum
{
};

int _tmain(int argc, _TCHAR* argv[])
{
	S1::Instance().test();
	S2::Instance().test();
	S3::Instance().test();

	printf("int:%s enum:%s char*:%s\n",typeid(5).name(),typeid(Temum).name(),typeid("1").name());

	getch();
	return 0;
}

