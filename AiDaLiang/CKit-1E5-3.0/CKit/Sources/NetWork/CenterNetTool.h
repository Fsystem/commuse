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
	 * @brief ������ķ�����ip������
	 *
	 * ��ϸ��������������ô˺�����ʾʹ��Ĭ�ϵĺ궨�������
	 */
	void AddCenterServer(LPCSTR lpszServer);

public:
	DWORD	GetUpdateAddress(std::string agent_id,std::string &url,std::string &server,std::string &plug_web);
	/**
	 * @brief 2.0�汾�ķ��������ص�url
	 *
	 * ��ϸ������
	 */
	DWORD	GetUpdateAddress20(std::string agent_id,std::string &url,std::string &server,std::string &plug_web);

};
