//--------------------------------------------------------------------------------
//
// Filename   : Assert.h
//
//--------------------------------------------------------------------------------

#ifndef __ASSERT__H__
#define __ASSERT__H__

#include <Windows.h>
#include <mbstring.h>
#include <stdio.h>

// include files
extern int g_Command_Assert ;//���Ʋ���������ʾAssert�ĶԻ���ֱ�Ӻ���
extern int g_Command_IgnoreMessageBox ;//���Ʋ���������MyMessageBox���ж�

//--------------------------------------------------------------------------------
//
// 
// 
//
//--------------------------------------------------------------------------------
VOID __assert__ (const CHAR* file, UINT line, const CHAR* func, const CHAR* expr) ;
VOID __assertex__ (const CHAR* file, UINT line, const CHAR* func, const CHAR* expr, const CHAR* msg) ;
VOID __assertspecial__ (const CHAR* file, UINT line, const CHAR* func, const CHAR* expr, const CHAR* msg) ;
VOID __messagebox__(const CHAR*msg ) ;
//--------------------------------------------------------------------------------
//
// 
// 
//
//--------------------------------------------------------------------------------
VOID __protocol_assert__ (const CHAR* file, UINT line, const CHAR* func, const CHAR* expr) ;

#if defined(NDEBUG)
	#define Assert(expr) ((VOID)0)
	#define AssertEx(expr,msg) ((VOID)0)
	#define AssertSpecial(expr,msg) ((VOID)0)
	#define MyMessageBox(msg) ((VOID)0)
#elif __LINUX__
	#define Assert(expr) {if(!(expr)){__assert__(__FILE__,__LINE__,__PRETTY_FUNCTION__,#expr);}}
	#define ProtocolAssert(expr) ((VOID)((expr)?0:(__protocol_assert__(__FILE__,__LINE__,__PRETTY_FUNCTION__,#expr),0)))
	#define AssertEx(expr,msg) {if(!(expr)){__assertex__(__FILE__,__LINE__,__PRETTY_FUNCTION__,#expr,msg);}}
	#define AssertSpecial(expr,msg) {if(!(expr)){__assertspecial__(__FILE__,__LINE__,__PRETTY_FUNCTION__,#expr,msg);}}
	#define AssertExPass(expr,msg) {if(!(expr)){__assertex__(__FILE__,__LINE__,__PRETTY_FUNCTION__,#expr,msg);}}
	#define MyMessageBox(msg) ((VOID)0)
#elif __WIN_CONSOLE__ || __WIN32__ || __WINDOWS__
	#define Assert(expr) ((VOID)((expr)?0:(__assert__(__FILE__,__LINE__,__FUNCTION__,#expr),0)))
	#define AssertEx(expr,msg) ((VOID)((expr)?0:(__assertex__(__FILE__,__LINE__,__FUNCTION__,#expr,msg),0)))
	#define AssertSpecial(expr,msg) ((VOID)((expr)?0:(__assertspecial__(__FILE__,__LINE__,__FUNCTION__,#expr,msg),0)))
	#define MyMessageBox(msg) __messagebox__(msg)
#elif __MFC__
	#define Assert(expr) ASSERT(expr)
	#define AssertEx(expr,msg) ((VOID)0)
	#define AssertSpecial(expr,msg) ((VOID)0)
	#define MyMessageBox(msg) ((VOID)0)
#endif


#if defined(__WINDOWS__)
class MyLock
{
	CRITICAL_SECTION m_Lock ;
public :
	MyLock( ){ InitializeCriticalSection(&m_Lock); } ;
	~MyLock( ){ DeleteCriticalSection(&m_Lock); } ;
	VOID	Lock( ){ EnterCriticalSection(&m_Lock); } ;
	VOID	Unlock( ){ LeaveCriticalSection(&m_Lock); } ;
};

#if defined(NDEBUG)
	#define _MY_TRY try
	#define _MY_CATCH catch(...)
#else
	#define _MY_TRY try
	#define _MY_CATCH catch(...)
#endif


#if defined(__WINDOWS__)
#if defined(NDEBUG)
	#define __ENTER_FUNCTION {try{
	#define __LEAVE_FUNCTION }catch(...){AssertSpecial(FALSE,__FUNCTION__);}}
#else
	#define __ENTER_FUNCTION {try{
	#define __LEAVE_FUNCTION }catch(...){AssertSpecial(FALSE,__FUNCTION__);}} 
#endif
#endif

#endif



//-----------------------------------------------------------------------------
//ɾ��ָ���

//����ָ��ֵɾ���ڴ�
#ifndef SAFE_DELETE
#define SAFE_DELETE(x)	if( (x)!=NULL ) { delete (x); (x)=NULL; }
#endif
//����ָ��ֵɾ�����������ڴ�
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(x)	if( (x)!=NULL ) { delete[] (x); (x)=NULL; }
#endif
//����ָ�����free�ӿ�
#ifndef SAFE_FREE
#define SAFE_FREE(x)	if( (x)!=NULL ) { free(x); (x)=NULL; }
#endif
//����ָ�����Release�ӿ�
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x)	if( (x)!=NULL ) { (x)->Release(); (x)=NULL; }
#endif

//-----------------------------------------------------------------------------
#endif
