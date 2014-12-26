#ifndef __BaseDefine_H
#define __BaseDefine_H

#include <Log/CommLog.h>

#pragma region 常用的API

//指针有效
#define ISVALID_PTR_NORETURN(ptr) {							\
	assert(ptr);											\
	if(!ptr)return ;										\
	else (void)0; }				

#define ISVALID_PTR_RETURN(ptr)	{							\
	assert(ptr);											\
	if(!ptr)return false;									\
	else (void)0; }

//安全删除
#define	SAFE_DELETE(p)	{	if( p!= NULL )	{	delete	p; p = NULL; } }

#define	SAFE_DELETE_ARRAY(p)	{	if( p!= NULL )	{	delete[]	p; p = NULL; } }

//内敛属性函数
#define ATTRIBUTE_FUNC(func_name,return_type,return_obj) inline return_type func_name(){return (return_type)return_obj;}
#define ATTRIBUTE_SETFUNC(func_name,set_obj_type,set_obj,set_val) inline void func_name(set_obj_type set_val){set_obj=set_val;}
//循环赋值
#define LOOPVAL(obj,num,val) \
	for(int i=0;i<(int)num;i++)	\
{							\
	obj[i]=val;				\
}
//循环删除
#define LOOPDEL(obj,num)	\
	for(int i=0;i<(int)num;i++)	\
{							\
	SAFE_DELETE(obj[i]);	\
}

//异常捕获
// #ifdef _DEBUG
// #	define BEGIN_FUNC	(void)0;
// #	define END_FUNC	(void)0;
// #else
#	define BEGIN_FUNC	try{ 

#	define END_FUNC	}catch(...){	\
	LOGEVEN(TEXT("EXCEPTION:[%s][%s][%d]\n"),__FILE__,__FUNCTION__,__LINE__);	\
	assert(false);														\
		}
/*#endif*/

#pragma region

#endif //__BaseDefine_H