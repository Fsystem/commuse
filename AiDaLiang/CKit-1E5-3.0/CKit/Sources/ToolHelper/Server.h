// Server.h: interface for the CServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVER_H__313BCD55_F6F0_4A63_A9A6_56250DB9AB86__INCLUDED_)
#define AFX_SERVER_H__313BCD55_F6F0_4A63_A9A6_56250DB9AB86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Winsvc.h>
#include <winioctl.h>

#ifdef SERVER_LIB

#define SERVER_VC_BUILD	__declspec(dllimport)

#else

#define	SERVER_VC_BUILD	__declspec(dllexport)

#endif

class   CServer  
{
public:
	bool IsAdmin();//��鵱ǰ�û��ǲ��ǹ���Ա
	bool StopService( LPCTSTR ServiceName);//ָֹͣ������
	DWORD MyStartService( LPCTSTR ServiceName);//����ָ������
	bool ReMoveServer(LPCTSTR ServiceName);//ɾ��ָ������
	DWORD m_last_error;
	bool InstallServer(LPCSTR ServiceName, LPCSTR ServiceExe,
	DWORD servicetype, DWORD starttype);//��װ���񣬷�����������·�����������ͣ�������ʽ
	bool Test(char *msg);
	bool ServerCtrl(LPCTSTR server_link_name,DWORD ctrl_code,LPVOID m_input,DWORD m_input_num,LPVOID m_output,DWORD m_output_num);//������������������������ͨ��
	CServer();
	virtual ~CServer();

};

#endif // !defined(AFX_SERVER_H__313BCD55_F6F0_4A63_A9A6_56250DB9AB86__INCLUDED_)
