#include "stdafx.h"
#include "CenterNetTool.h"

CCenterNetTool::CCenterNetTool(void)
{
	setCenterPort(CENTER_PORT);
}

CCenterNetTool::~CCenterNetTool(void)
{
}

void CCenterNetTool::AddCenterServer( LPCSTR lpszServer )
{
	mCenterServers.push_back(lpszServer);
}

DWORD CCenterNetTool::GetUpdateAddress(std::string agent_id,std::string &url,std::string &server,std::string &plug_web)
{
	if (mCenterServers.size()==0)
	{
		mCenterServers.push_back(CENTER_SERVER1);
		mCenterServers.push_back(CENTER_SERVER2);
		mCenterServers.push_back(CENTER_SERVER3);
	}

	KEY_VALUE_MAP	_key_value = CreateFlowQPack(agent_id.c_str());

	std::string		send_xml = CMapToXml::MapToXml(_key_value);

	std::string	recv_xml = "";

	for (std::vector<std::string>::iterator it = mCenterServers.begin();it != mCenterServers.end();it++)
	{
		if(SendRecv(it->c_str(),getCenterPort(),MSG_CENTER_GET_FILEURL_AND_SERVER_Q,send_xml,recv_xml) == NOERROR) break;
	}

	if (recv_xml.size())
	{
		_key_value.clear();
		if (CMapToXml::XmlToMap(recv_xml,_key_value))
		{
			url = _key_value["url"];
			server = _key_value["server"];
			plug_web = _key_value["plug_web"];
			return NOERROR;
		}
		else
		{
			return ERROR_INVALID_PARAMETER;
		}
	}
	else
	{
		return ERROR_NETWORK_BUSY;
	}

	return NOERROR;
}

//-------------------------------------------------------------------------------
//获取内容请求
typedef	struct	_TAG_GET_UPDATE_URL_Q
{
	char		mac[32];
	char		agent[32];
}TAG_GET_UPDATE_URL_Q,* PTAG_GET_UPDATE_URL_Q;

//回复的内容，直接连接在后面
typedef	struct	_TAG_GET_UPDATE_URL_A
{
	DWORD		dwStatus;
	char		url[1024];

	void Code()	//编码，把网络
	{ 
		dwStatus = htonl(dwStatus);		
	}

	void UnCode()
	{
		dwStatus = ntohl(dwStatus);
	}
}TAG_GET_UPDATE_URL_A,* PTAG_GET_UPDATE_URL_A;

#define		MSG_GET_UPDATE_URL_Q		1
#define		MSG_GET_UPDATE_URL_A		2

DWORD CCenterNetTool::GetUpdateAddress20( std::string agent_id,std::string &url,std::string &server,std::string &plug_web )
{
	url = "";

	if (mCenterServers.size()==0)
	{
		mCenterServers.push_back(CENTER_SERVER1);
		mCenterServers.push_back(CENTER_SERVER2);
		mCenterServers.push_back(CENTER_SERVER3);
	}

	TAG_GET_UPDATE_URL_Q	tag_url_q;
	TAG_GET_UPDATE_URL_A	tag_url_a;

	memset(&tag_url_q,0,sizeof(tag_url_q));
	memset(&tag_url_a,0,sizeof(tag_url_a));

	CToolOper tool ;
	strncpy(tag_url_q.agent,agent_id.c_str(),CountArr(tag_url_q.agent));
	tool.GetMac(tag_url_q.mac,NULL); 

	KEY_VALUE_MAP	_key_value = CreateFlowQPack(agent_id.c_str());

	std::string		send_xml = CMapToXml::MapToXml(_key_value);

	std::string	recv_xml = "";

	CNetOper netOper(false,0x19831984,"jdaf832rn,snde329r");
	std::vector<char> sRet;
	for (std::vector<std::string>::iterator it = mCenterServers.begin();it != mCenterServers.end();it++)
	{
		sRet = netOper.NetOper(MSG_GET_UPDATE_URL_Q,&tag_url_q,sizeof(tag_url_q),it->c_str(),getCenterPort());
		if (sRet.size()>0) break;
		//if(SendRecv(it->c_str(),getCenterPort(),MSG_GET_UPDATE_URL_Q,send_xml,recv_xml) == NOERROR) break;
	}

	if (sRet.size())
	{
		memcpy(&tag_url_a,&sRet[0],sizeof(tag_url_a));
		//std::copy_n(sRet.begin(),sizeof(tag_url_a),&tag_url_a);

		tag_url_a.UnCode();
		if (tag_url_a.dwStatus != 0)
		{
			return tag_url_a.dwStatus;
		}

		url = tag_url_a.url;
		server = "";
		plug_web = "";
	}
	else
	{
		return ERROR_NETWORK_BUSY;
	}

	return NOERROR;
}
