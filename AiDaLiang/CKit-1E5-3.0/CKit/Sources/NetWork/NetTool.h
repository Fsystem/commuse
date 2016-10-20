#pragma once

/**  
* @brief 网络通信业务基类
*
* @baseclass 
* @author Double Sword
* @data 2016-10-20
*   
* 详细描述：如果类需要发送网络数据继承这个类即可
*/
class CNetTool
{
public:
	CNetTool(DWORD net_tag = 0x20161011);
	virtual ~CNetTool(void);

private:
	BOOL OperDataCode(char * buffer,DWORD len);

private:

	DWORD UdpSendRecv(const char * server,DWORD dwport,DWORD msg_info,std::string send_xml,std::string &recv_xml);
	DWORD TcpSendRecv(const char * server,DWORD dwport,DWORD msg_info,std::string send_xml,std::string &recv_xml);
public:
	virtual KEY_VALUE_MAP CreateFlowQPack(const char *agent_id);
public:
	DWORD SendRecv(const char * server,DWORD dwport,DWORD msg_info,std::string send_xml,std::string &recv_xml);
private:
	std::string		work_key;
	DWORD			work_tag;
};
