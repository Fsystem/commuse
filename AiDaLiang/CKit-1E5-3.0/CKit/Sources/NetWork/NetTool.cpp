#include "stdafx.h"
#include "NetTool.h"

CNetTool::CNetTool(DWORD net_tag)
{
	work_key = NETWORK_PASSWROD;
	work_tag = net_tag;
}

CNetTool::~CNetTool(void)
{
}


BOOL CNetTool::OperDataCode(char	* buffer,DWORD len)
{
	char	*lpdata = (char*)malloc(len);
	if (lpdata == NULL)
	{
		return FALSE;
	}

	memset(lpdata,0,sizeof(lpdata));

	yrc4((const unsigned char *)work_key.c_str(),work_key.size(),(const unsigned char *)buffer,len,(unsigned char *)lpdata);

	memcpy(buffer,lpdata,len);

	free(lpdata);
	lpdata = NULL;

	return TRUE;
}

DWORD CNetTool::SendRecv( const char * server,DWORD dwport,DWORD msg_info,std::string send_xml,std::string &recv_xml )
{
	DWORD	ret = UdpSendRecv(server,dwport,msg_info,send_xml,recv_xml);
	if (ret == NOERROR)
	{
		return ret;
	}

	for (int i = 0; i != 3; i++)
	{
		ret = TcpSendRecv(server,dwport,msg_info,send_xml,recv_xml);

		if (ret == NOERROR)
		{
			break;
		}
	}

	return ret;
}

DWORD CNetTool::UdpSendRecv( const char * server,DWORD dwport,DWORD msg_info,std::string send_xml,std::string &recv_xml )
{
	yCUdp					udp_oper;
	TAG_NET_HEAD			tag_head;

	tag_head.Clear();

	tag_head.msg_info = msg_info;
	tag_head.msg_len = send_xml.size() + sizeof(TAG_NET_HEAD);
	tag_head.tag = work_tag;
	tag_head.ver = NETWORK_VER;

	tag_head.Code();

	//加密
	DWORD		send_len = send_xml.size() + sizeof(TAG_NET_HEAD);
	char		*send_buffer = (char*)malloc(send_len);
	if (send_buffer == NULL)
	{
		return ERROR_NO_SYSTEM_RESOURCES;
	}
	memcpy(send_buffer,&tag_head,sizeof(TAG_NET_HEAD));
	memcpy(send_buffer + sizeof(TAG_NET_HEAD),send_xml.c_str(),send_xml.size());
	OperDataCode(send_buffer,send_len);

	udp_oper.BindAddr(NULL,0);

	char		*recv_buffer = (char*)malloc(UDP_PACK_LEN);
	if (recv_buffer == NULL)
	{
		free(send_buffer);
		send_buffer = NULL;
		return ERROR_NO_SYSTEM_RESOURCES;
	}

	DWORD		recv_len = 0;
	BOOL		recv_ret = FALSE;

	//发送
	for (int i = 0; i != 3; i++)
	{ 
		if(udp_oper.SendDate(server,dwport,send_buffer,send_len) == 0)
			break;

		memset(recv_buffer,0,UDP_PACK_LEN);		
		recv_len = udp_oper.RecvDate(recv_buffer,UDP_PACK_LEN,UDP_RECV_TIME_OUT);

		if (recv_len >= sizeof(TAG_NET_HEAD) )
		{ 
			OperDataCode(recv_buffer,recv_len);

			memcpy(&tag_head,recv_buffer,sizeof(tag_head));
			tag_head.UnCode();

			if (tag_head.msg_info != msg_info + 1 || tag_head.msg_len != recv_len)
			{
				continue;
			}

			recv_ret = TRUE;
			//接收成功，处理并退出
			break;
		}
	}

	if (recv_ret == TRUE)
	{
		recv_xml = recv_buffer + sizeof(TAG_NET_HEAD);
	}

	free(send_buffer);
	send_buffer = NULL;

	free(recv_buffer);
	recv_buffer = NULL;

	return recv_ret == TRUE ? NOERROR : ERROR_NETWORK_ACCESS_DENIED;
}

DWORD CNetTool::TcpSendRecv( const char * server,DWORD dwport,DWORD msg_info,std::string send_xml,std::string &recv_xml )
{
	yCTcp					tcp_oper;
	TAG_NET_HEAD			tag_head;

	tag_head.Clear();

	tag_head.msg_info = msg_info;
	tag_head.msg_len = send_xml.size() + sizeof(TAG_NET_HEAD);
	tag_head.tag = work_tag;
	tag_head.ver = NETWORK_VER;

	tag_head.Code();


	tcp_oper.Open();
	if(tcp_oper.ConnectNoBlock(server,dwport,5) == FALSE)
	{
		return	ERROR_NETWORK_ACCESS_DENIED;
	}

	//加密
	DWORD		send_len = send_xml.size() + sizeof(TAG_NET_HEAD);
	char		*send_buffer = (char*)malloc(send_len);
	if (send_buffer == NULL)
	{
		return ERROR_NO_SYSTEM_RESOURCES;
	}
	memcpy(send_buffer,&tag_head,sizeof(TAG_NET_HEAD));
	OperDataCode(send_buffer,sizeof(TAG_NET_HEAD));			//加密第一段头

	memcpy(send_buffer + sizeof(TAG_NET_HEAD),send_xml.c_str(),send_xml.size());
	OperDataCode(send_buffer + sizeof(TAG_NET_HEAD),send_xml.size());	//加密第二段


	char		*recv_buffer = (char*)malloc(UDP_PACK_LEN);
	if (recv_buffer == NULL)
	{
		free(send_buffer);
		send_buffer = NULL;
		return ERROR_NO_SYSTEM_RESOURCES;
	}

	DWORD		recv_len = 0;
	BOOL		recv_ret = FALSE;

	do 
	{
		if(tcp_oper.Sendn(send_buffer,send_len) != send_len)
			break;

				
		recv_len = tcp_oper.Recvn(&tag_head,sizeof(tag_head));

		if (recv_len == sizeof(TAG_NET_HEAD) )
		{ 
			OperDataCode((char*)&tag_head,sizeof(tag_head));	//解密第一段
			tag_head.UnCode();

			if (tag_head.msg_info != msg_info + 1)
			{
				break;
			}

			tag_head.msg_len -= sizeof(TAG_NET_HEAD);

			if (tag_head.msg_len == 0)
			{
				recv_ret = TRUE;
				break;
			}
			memset(recv_buffer,0,UDP_PACK_LEN);
			if(tcp_oper.Recvn(recv_buffer,tag_head.msg_len) == tag_head.msg_len)
			{
				recv_ret = TRUE;
				OperDataCode((char*)recv_buffer,tag_head.msg_len);
				//接收成功，处理并退出
				break;
			}
		}
	} while (FALSE);

	if (recv_ret == TRUE)
	{
		recv_xml = recv_buffer;
	}

	free(send_buffer);
	send_buffer = NULL;

	free(recv_buffer);
	recv_buffer = NULL;

	return recv_ret == TRUE ? NOERROR : ERROR_NETWORK_ACCESS_DENIED;;
}



//构造刷流量的请求包
KEY_VALUE_MAP	CNetTool::CreateFlowQPack(const char *agent_id)
{ 
 	
	KEY_VALUE_MAP	_key_value;
	CDeviceOper		oper;

	_key_value["id"] = agent_id;
	_key_value["mac"] = oper.GetMac();
	_key_value["sys_ver"] = oper.GetSystemVer();
	_key_value["ie_ver"] = oper.GetIeVer();
	_key_value["flash_ver"] = oper.GetFlashVer();
	char	ver_str[MAX_PATH];
	memset(ver_str,0,sizeof(ver_str));
	sprintf_s(ver_str,MAX_PATH,"%d",APP_VER);
	_key_value["ver"] = ver_str;
	 

	return _key_value; 

}