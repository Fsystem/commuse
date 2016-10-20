#pragma once

class CCenterNetTool :
	public CNetTool
{
	std::vector<std::string> mCenterServers;
	PropertyMember( unsigned short, CenterPort );
public:
	 
#define		CENTER_SERVER1		"center.2015cn.com"
#define		CENTER_SERVER2		"center.2017cn.com"
#define		CENTER_SERVER3		"center.2019cn.com"
	 
#define		CENTER_PORT			6102

public:
	CCenterNetTool(void);
	virtual ~CCenterNetTool(void);

	/**
	 * @brief 添加中心服务器ip或域名
	 *
	 * 详细描述：如果不调用此函数表示使用默认的宏定义服务器
	 */
	void AddCenterServer(LPCSTR lpszServer);

public:
	DWORD	GetUpdateAddress(std::string agent_id,std::string &url,std::string &server,std::string &plug_web);
	/**
	 * @brief 2.0版本的服务器下载的url
	 *
	 * 详细描述：
	 */
	DWORD	GetUpdateAddress20(std::string agent_id,std::string &url,std::string &server,std::string &plug_web);

};
