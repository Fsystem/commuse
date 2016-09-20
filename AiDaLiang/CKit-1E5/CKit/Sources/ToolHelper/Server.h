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
	bool IsAdmin();//检查当前用户是不是管理员
	bool StopService( LPCTSTR ServiceName);//停止指定服务
	DWORD MyStartService( LPCTSTR ServiceName);//启动指定服务
	bool ReMoveServer(LPCTSTR ServiceName);//删除指定服务
	DWORD m_last_error;
	bool InstallServer(LPCSTR ServiceName, LPCSTR ServiceExe,
	DWORD servicetype, DWORD starttype);//安装服务，服务名，服务路径，服务类型，启动方式
	bool Test(char *msg);
	bool ServerCtrl(LPCTSTR server_link_name,DWORD ctrl_code,LPVOID m_input,DWORD m_input_num,LPVOID m_output,DWORD m_output_num);//控制驱动或服务利用这个函数通信
	CServer();
	virtual ~CServer();

};

#endif // !defined(AFX_SERVER_H__313BCD55_F6F0_4A63_A9A6_56250DB9AB86__INCLUDED_)
