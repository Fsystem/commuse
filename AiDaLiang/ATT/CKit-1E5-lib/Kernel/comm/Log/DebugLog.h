
#if !defined(AFX_DEBUGLOG_H__C38FB7EB_0656_4F44_99B4_42C232FC267E__INCLUDED_)
#define AFX_DEBUGLOG_H__C38FB7EB_0656_4F44_99B4_42C232FC267E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <iostream>
#include <string>

using namespace std;

#define	PRINTF_FIRST_LOG	1	// ��Ҫ��־
#define PRINTF_SECOND_LOG	2	// ��Ҫ��־
#define PRINTF_THIRD_LOG	3	// ����Ҫ��־

class CDebugLog  
{
public:
	const char * GetErrorString(DWORD nError);			                //��ȡϵͳ�����������
	int  GetPrintLevel() const;											//��ȡ��־��ӡ����
	CDebugLog();
	virtual ~CDebugLog();
	bool SaveLog( const int logLevel, const char * filename, const char *  fmt, ...);//д��־
	void SetRatingLog(int logLevel);									//������־����
	
private:
	bool Log (const char * filename, const char * fmt, va_list args);	// ��Log����,��־���𹻵ı���
	
protected:
	int					m_logLevel;										// д��־����(Ĭ����д���еȼ���־)
	CRITICAL_SECTION	m_csLock;										// �߳�ͬ��	
	char				m_buffer[MAX_PATH];								//�����ַ���
};

#endif // !defined(AFX_DEBUGLOG_H__C38FB7EB_0656_4F44_99B4_42C232FC267E__INCLUDED_)
