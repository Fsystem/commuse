#ifndef __WinServer_H
#define __WinServer_H

/**
 * @brief 自己的程序变成windows服务
 *
 * 详细描述：自己的程序变成windows服务,需要继承这个接口
 */
class CWinServerInterface
{
public:
	CWinServerInterface(){};
	~CWinServerInterface(){};


public:
	virtual void	ServiceStart(DWORD dwArgc, LPSTR *lpszArgv) = 0;
	virtual void	ServiceStop() = 0;
	virtual	BOOL	GetServerInfo(std::string &_sz_appname,std::string	&_sz_service_name,std::string	&_sz_service_displayname) = 0;
};

/**
 * @brief 服务实现类
 *
 * 详细描述：
 */
class CWinServer : public SingletonBase<CWinServer>
{

public:
	CWinServer(void);
	~CWinServer(void);

public: 
	BOOL					StartRun();
	std::string				GetAppName();
	void					SetInterface(CWinServerInterface * _interface);

public:
	CWinServerInterface		*m_server_interface;

private:
	static void WINAPI service_ctrl(DWORD dwCtrlCode);
	static void WINAPI service_main(DWORD dwArgc, LPSTR *lpszArgv);
	static	BOOL WINAPI ControlHandler ( DWORD dwCtrlType );

private:
	BOOL	Init();
	BOOL	ReportStatusToSCMgr(DWORD dwCurrentState);
	void	SetServiceStatusHandle(SERVICE_STATUS_HANDLE Handle);
	void	AddToMessageLog(LPSTR lpszMsg,DWORD dwErr);
	void	CmdInstallService();
	void	CmdRemoveService();
	void	CmdDebugService(int argc, char ** argv);
private:
	std::string		m_sz_appname;		//应用程序的名称
	std::string		m_sz_service_name;	//服务名
	std::string		m_sz_serviced_displayname;	//服务显示的名称

	BOOL			m_bDebug;
private:
	SERVICE_STATUS_HANDLE m_StatusHandle;
};

////-------------------------------------------------------------------------------
////调用例子
//class CMyWinServer : public CWinServerInterface
//{
//public:
//	//这个函数需要死循环否则服务很快就会退出
//	virtual void	ServiceStart(DWORD dwArgc, LPTSTR *lpszArgv) {};
//	virtual void	ServiceStop() {};
//};
//
//int main()
//{
//	CWinServer::Instance().SetInterface(new CMyWinServer);
//	CWinServer::Instance().StartRun();
//	return 0;
//}


#endif //__WinServer_H