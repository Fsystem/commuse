#include "stdafx.h"
#include "NetManager.h"

#include "../Kernel/net/yCTcp.h"
#include "../Kernel/net/yCUdp.h"

#include "../ToolHelper/NetOper.h"

//网络协议定义
typedef	struct	_TAG_HEAD
{
	DWORD		tag;  //标志位，定义为：0x20150801
	DWORD		ver;	//协议的版本号
	DWORD		msg_len;	//数据长度，包括TAG_HEAD
	DWORD		msg_info;	//消息类型头

	void Code()	//编码，把网络
	{ 
		tag = htonl(tag);
		msg_len = htonl(msg_len);
		ver = htonl(ver);		
		msg_info = htonl(msg_info);	
	}

	void UnCode()
	{
		tag = ntohl(tag);
		msg_len = ntohl(msg_len);
		ver = ntohl(ver);
		msg_info = ntohl(msg_info);
	}
}TAG_HEAD,*PTAG_HEAD;

//服务信息
typedef struct SERVER_INFO{
	DWORD			dwIp;
	WORD			wPort;
}SERVER_INFO;

//-------------------------------------------------------------------------------
static std::map<DWORD,SERVER_INFO> MAP_TCPSVR_INFO,MAP_UDPSVR_INFO;
static CNetOper NetOper;

//-------------------------------------------------------------------------------
static BOOL OperDataCode(char * buffer,DWORD len)
{
	char	*lpdata = (char*)malloc(len);
	if (lpdata == NULL)
	{
		return FALSE;
	}

	memset(lpdata,0,len);

	yrc4((const unsigned char *)STR_NET_KEY,LEN_NET_KEY,(const unsigned char *)buffer,len,(unsigned char *)lpdata);

	memcpy(buffer,lpdata,len);

	free(lpdata);
	lpdata = NULL;

	return TRUE;
}

//-------------------------------------------------------------------------------
NetManager::NetManager()
{

}

NetManager::~NetManager()
{
	MAP_TCPSVR_INFO.clear();
	MAP_UDPSVR_INFO.clear();
}

//添加/更新一个服务器
void NetManager::AddServer(DWORD dwSvrId,enNetMethod method,LPCSTR szIP,WORD wPort)
{
	SERVER_INFO info;
	info.dwIp = Str2NIp(szIP);
	info.wPort = wPort;
	if (method==enTCP)
	{
		MAP_TCPSVR_INFO[dwSvrId] = info;
	}
	else 
	{
		MAP_UDPSVR_INFO[dwSvrId] = info;
	}
	
}

//发送数据（短链接,直接返回数据）-字符串数据
std::string NetManager::SendDataByShortLink(DWORD dwSvrId,DWORD dwCMD,std::string strData,INetDelegate* pINetDelegate )
{
	std::string sRet = "error";

	sRet = UDPSendDataByShortLink(dwSvrId,dwCMD,strData,pINetDelegate);

	if (sRet == "error")
	{
		sRet = TCPSendDataByShortLink(dwSvrId,dwCMD,strData,pINetDelegate);
	}

	return sRet;
}

std::string NetManager::TCPSendDataByShortLink(DWORD dwSvrId,DWORD dwCMD,std::string strData,INetDelegate* pINetDelegate )
{
	std::string sRet = "error";
	auto itSend = MAP_TCPSVR_INFO.find(dwSvrId);
	if (itSend!=MAP_TCPSVR_INFO.end())
	{	
		TAG_HEAD			tag_head; 

		char				recv_buffer[MAX_40K_BUFFER];
		char				send_buffer[MAX_40K_BUFFER];
		char				xml_buffer[MAX_40K_BUFFER];
		int					send_len = 0;
		int					xml_len = 0; 

		memset(xml_buffer,0,sizeof(xml_buffer));
		memset(send_buffer,0,sizeof(send_buffer));

		xml_len = strData.size();
		strncpy(xml_buffer,strData.c_str(),xml_len);

		OperDataCode(xml_buffer,xml_len);

		tag_head.tag = MSG_TAG_FLAG;
		tag_head.msg_info = dwCMD;
		tag_head.msg_len = sizeof(TAG_HEAD) + xml_len;
		tag_head.ver = HEAD_VER;

		send_len = sizeof(TAG_HEAD) + xml_len;

		tag_head.Code();

		memcpy(send_buffer,&tag_head,sizeof(tag_head));
		memcpy(send_buffer + sizeof(TAG_HEAD),xml_buffer,xml_len);

		std::string	work_server = NIp2str(itSend->second.dwIp);
		WORD iport = itSend->second.wPort;

		yCTcp tcp;
		int		recv_len = 0;

		for (int i = 0; i != 3; i++)
		{ 

			yCTcp		oper_tcp;
			oper_tcp.Open();

			if(!oper_tcp.ConnectNoBlock(work_server.c_str(),iport,3))
			{
				continue;
			}

			if (oper_tcp.Sendn(send_buffer,send_len) != send_len)
			{
				oper_tcp.Close();
				continue;
			}

			memset(&tag_head,0,sizeof(tag_head));
			if (oper_tcp.Recvn(&tag_head,sizeof(tag_head)) != sizeof(tag_head))
			{
				oper_tcp.Close();
				continue;
			}

			tag_head.UnCode();

			tag_head.msg_len -= sizeof(tag_head);

			if (tag_head.msg_len == 0)
			{
				oper_tcp.Close(); 
				sRet = "";
				goto TCPFINISH;
			}

			memset(recv_buffer,0,sizeof(recv_buffer));
			if (oper_tcp.Recvn(recv_buffer,tag_head.msg_len) != (int)tag_head.msg_len)
			{
				oper_tcp.Close();
				continue;
			} 		
			oper_tcp.Close();  

			OperDataCode(recv_buffer,tag_head.msg_len);

			sRet = recv_buffer;
			goto TCPFINISH;
		} 

		sRet = "error"; 

TCPFINISH:
		if (sRet != "error")
		{
			if(pINetDelegate) 
				pINetDelegate->OnSocketRecv(enTCP,tag_head.msg_info,recv_buffer,tag_head.msg_len);
		}
		else
		{
			if(pINetDelegate)
				pINetDelegate->OnSocketErr(enTCP,dwCMD,strData.c_str(),strData.size());
		}
	}


	return sRet;
}

std::string NetManager::UDPSendDataByShortLink(DWORD dwSvrId,DWORD dwCMD,std::string strData,INetDelegate* pINetDelegate )
{
	std::string sRet = "error";
	auto itSend = MAP_UDPSVR_INFO.find(dwSvrId);
	if (itSend != MAP_UDPSVR_INFO.end())
	{
		yCUdp		udp_server; 

		udp_server.BindAddr(NULL,0);

		TAG_HEAD				tag_head; 

		char					recv_buffer[MAX_40K_BUFFER];
		char					send_buffer[MAX_40K_BUFFER];
		int						xml_len = 0;

		memset(send_buffer,0,sizeof(send_buffer));

		xml_len = strData.size();  

		tag_head.tag = MSG_TAG_FLAG;
		tag_head.msg_info = dwCMD;
		tag_head.msg_len = sizeof(TAG_HEAD) + xml_len;
		tag_head.ver = HEAD_VER;

		tag_head.Code();

		memcpy(send_buffer,&tag_head,sizeof(tag_head));
		memcpy(send_buffer + sizeof(TAG_HEAD),strData.c_str(),xml_len);

		int		recv_len = 0;

		OperDataCode(send_buffer,xml_len + sizeof(TAG_HEAD));

		BOOL			recv_ret = FALSE;

		std::string	work_server = NIp2str(itSend->second.dwIp);
		WORD iport = itSend->second.wPort;

		for (int i = 0; i != 3; i++)
		{ 
			if(udp_server.SendDate(work_server.c_str(),iport,send_buffer,xml_len + sizeof(TAG_HEAD)) == 0)
				break;

			memset(recv_buffer,0,sizeof(recv_buffer));
			recv_len = udp_server.RecvDate(recv_buffer,sizeof(recv_buffer),5);
			if (recv_len >= sizeof(TAG_HEAD) )
			{

				OperDataCode(recv_buffer,recv_len);

				memcpy(&tag_head,recv_buffer,sizeof(tag_head));
				tag_head.UnCode();
				if (tag_head.msg_info != dwCMD + 1)
				{
					continue;
				}

				recv_ret = TRUE;
				//接收成功，处理并退出
				break;
			}
		} 

		if (recv_ret)
		{    
			if (pINetDelegate)
			{
				pINetDelegate->OnSocketRecv(enUDP,tag_head.msg_info,recv_buffer + sizeof(TAG_HEAD),recv_len-sizeof(TAG_HEAD) );
			}

			return recv_buffer + sizeof(TAG_HEAD);
		}	

		if (pINetDelegate)
		{
			pINetDelegate->OnSocketErr(enUDP,dwCMD,strData.c_str(),strData.size() );
		}
	}
	
	return sRet; 
}

void NetManager::UDPSendDataByShortLinkNoRecv(DWORD dwSvrId,DWORD dwCMD,std::string strData)
{
	auto itSend = MAP_UDPSVR_INFO.find(dwSvrId);
	if (itSend != MAP_UDPSVR_INFO.end())
	{
		std::string	work_server = NIp2str(itSend->second.dwIp);
		WORD iport = itSend->second.wPort;

		yCUdp		udp_server; 

		udp_server.BindAddr(NULL,0);

		TAG_HEAD				tag_head; 

		char					send_buffer[MAX_40K_BUFFER];
		int						xml_len = 0;

		memset(send_buffer,0,sizeof(send_buffer));

		xml_len = strData.size();  

		tag_head.tag = MSG_TAG_FLAG;
		tag_head.msg_info = dwCMD;
		tag_head.msg_len = sizeof(TAG_HEAD) + xml_len;
		tag_head.ver = HEAD_VER;

		tag_head.Code();

		memcpy(send_buffer,&tag_head,sizeof(tag_head));
		memcpy(send_buffer + sizeof(TAG_HEAD),strData.c_str(),xml_len);

		int		recv_len = 0;

		OperDataCode(send_buffer,xml_len + sizeof(TAG_HEAD));

		BOOL			recv_ret = FALSE;

		udp_server.SendDate(work_server.c_str(),iport,send_buffer,xml_len + sizeof(TAG_HEAD));
	}
	
}