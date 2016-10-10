
#if !defined(AFX_DEBUGLOG_H__C38FB7EB_0656_4F44_99B4_42C232FC267E__INCLUDED_)
#define AFX_DEBUGLOG_H__C38FB7EB_0656_4F44_99B4_42C232FC267E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <iostream>
#include <string>

using namespace std;

#define	PRINTF_FIRST_LOG	1	// 重要日志
#define PRINTF_SECOND_LOG	2	// 次要日志
#define PRINTF_THIRD_LOG	3	// 不重要日志

class CDebugLog  
{
public:
	const char * GetErrorString(DWORD nError);			                //获取系统错误号描述符
	int  GetPrintLevel() const;											//获取日志打印级别
	CDebugLog();
	virtual ~CDebugLog();
	bool SaveLog( const int logLevel, const char * filename, const char *  fmt, ...);//写日志
	void SetRatingLog(int logLevel);									//设置日志级别
	
private:
	bool Log (const char * filename, const char * fmt, va_list args);	// 供Log调用,日志级别够的保存
	
protected:
	int					m_logLevel;										// 写日志级别(默认是写所有等级日志)
	CRITICAL_SECTION	m_csLock;										// 线程同步	
	char				m_buffer[MAX_PATH];								//错误字符串
};

#endif // !defined(AFX_DEBUGLOG_H__C38FB7EB_0656_4F44_99B4_42C232FC267E__INCLUDED_)
