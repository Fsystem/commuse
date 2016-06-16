// NetOper.cpp: implementation of the CNetOper class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "NetOper.h"
#include "../Kernel/net/yCUdp.h"
#include "../Kernel/net/yCTcp.h"
#include "../Kernel/crypt/yrc4.h"

#pragma warning(disable:4996)

#define MAX_40K_BUFFER 40*1024
#define MSG_TAG_FLAG 0x20150801

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
char	*work_key = "jdaf832rn,snde329r";

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


#define		WORK_PORT		6000

#define		HEAD_VER		1
#define		APP_VER			7


CNetOper::CNetOper()
{

}

CNetOper::~CNetOper()
{

}

 

BOOL CNetOper::OperDataCode(char	* buffer,DWORD len)
{
	char	*lpdata = (char*)malloc(len);
	if (lpdata == NULL)
	{
		return FALSE;
	}
	
	memset(lpdata,0,len);

	//g_log.SaveLog(PRINTF_THIRD_LOG, NULL, "%s,%d行，调用接口OperDataCode向客户端发送的数据长度为%d.\r\n", __FILE__, __LINE__,len);
	yrc4((const unsigned char *)work_key,strlen(work_key),(const unsigned char *)buffer,len,(unsigned char *)lpdata);
	
	memcpy(buffer,lpdata,len);
	
	free(lpdata);
	lpdata = NULL;
	
	return TRUE;
}
 

//参数：		agent_id		代理商ID
//				buffer			数据缓冲区
//				len				数据缓冲区长度
//功能：不等待返回的udp发送
void CNetOper::NetOperForUdpWithout(DWORD msg_type,std::string xml,const char *addr,int iport)
{ 
	yCUdp		udp_server; 

	udp_server.BindAddr(NULL,0);

	TAG_HEAD				tag_head; 

	char					send_buffer[MAX_40K_BUFFER];
	int						xml_len = 0;


	memset(send_buffer,0,sizeof(send_buffer));

	xml_len = xml.size();  

	tag_head.tag = MSG_TAG_FLAG;
	tag_head.msg_info = msg_type;
	tag_head.msg_len = sizeof(TAG_HEAD) + xml_len;
	tag_head.ver = HEAD_VER;

	tag_head.Code();

	memcpy(send_buffer,&tag_head,sizeof(tag_head));
	memcpy(send_buffer + sizeof(TAG_HEAD),xml.c_str(),xml_len);

	int		recv_len = 0;

	OperDataCode(send_buffer,xml_len + sizeof(TAG_HEAD));

	BOOL			recv_ret = FALSE;

	udp_server.SendDate(addr,iport,send_buffer,xml_len + sizeof(TAG_HEAD));
}

//参数：		agent_id		代理商ID
//				buffer			数据缓冲区
//				len				数据缓冲区长度
//功能：得到劫持的配置数据
std::string CNetOper::NetOperForUdp(DWORD msg_type,std::string xml,const char *addr,int iport)
{ 
	yCUdp		udp_server; 

	udp_server.BindAddr(NULL,0);

	TAG_HEAD				tag_head; 

	char					recv_buffer[MAX_40K_BUFFER];
	char					send_buffer[MAX_40K_BUFFER];
	int						xml_len = 0;
 

	memset(send_buffer,0,sizeof(send_buffer));

	xml_len = xml.size();  

	tag_head.tag = MSG_TAG_FLAG;
	tag_head.msg_info = msg_type;
	tag_head.msg_len = sizeof(TAG_HEAD) + xml_len;
	tag_head.ver = HEAD_VER;

	tag_head.Code();

	memcpy(send_buffer,&tag_head,sizeof(tag_head));
	memcpy(send_buffer + sizeof(TAG_HEAD),xml.c_str(),xml_len);

	int		recv_len = 0;

	OperDataCode(send_buffer,xml_len + sizeof(TAG_HEAD));
		
	BOOL			recv_ret = FALSE;

	for (int i = 0; i != 3; i++)
	{ 
		if(udp_server.SendDate(addr,iport,send_buffer,xml_len + sizeof(TAG_HEAD)) == 0)
			break;

		memset(recv_buffer,0,sizeof(recv_buffer));
		recv_len = udp_server.RecvDate(recv_buffer,sizeof(recv_buffer),5);
		if (recv_len >= sizeof(TAG_HEAD) )
		{

			OperDataCode(recv_buffer,recv_len);

			memcpy(&tag_head,recv_buffer,sizeof(tag_head));
			tag_head.UnCode();
			if (tag_head.msg_info != msg_type + 1)
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
		return recv_buffer + sizeof(TAG_HEAD);
	}	
	return "error"; 
}



//参数：		agent_id		代理商ID
//				buffer			数据缓冲区
//				len				数据缓冲区长度
//功能：得到劫持的配置数据
std::string CNetOper::NetOperForTcp(DWORD msg_type,std::string xml,const char *addr,int iport)
{  

	TAG_HEAD			tag_head; 

	char					recv_buffer[MAX_40K_BUFFER];
	char					send_buffer[MAX_40K_BUFFER];
	char					xml_buffer[MAX_40K_BUFFER];
	int						send_len = 0;
	int						xml_len = 0; 

	memset(xml_buffer,0,sizeof(xml_buffer));
	memset(send_buffer,0,sizeof(send_buffer));

	xml_len = xml.size();
	strncpy(xml_buffer,xml.c_str(),xml_len);

	OperDataCode(xml_buffer,xml_len);

	tag_head.tag = MSG_TAG_FLAG;
	tag_head.msg_info = msg_type;
	tag_head.msg_len = sizeof(TAG_HEAD) + xml_len;
	tag_head.ver = HEAD_VER;

	send_len = sizeof(TAG_HEAD) + xml_len;

	tag_head.Code();

	memcpy(send_buffer,&tag_head,sizeof(tag_head));
	memcpy(send_buffer + sizeof(TAG_HEAD),xml_buffer,xml_len);

	std::string		work_server = addr;

	yCTcp		tcp;
	char		ip_str[MAX_PATH];
	memset(ip_str,0,sizeof(ip_str));

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
			return "";
		}

		memset(recv_buffer,0,sizeof(recv_buffer));
		if (oper_tcp.Recvn(recv_buffer,tag_head.msg_len) != (int)tag_head.msg_len)
		{
			oper_tcp.Close();
			continue;
		} 		
		oper_tcp.Close();  

		OperDataCode(recv_buffer,tag_head.msg_len);

		return recv_buffer;
	} 

	return "error"; 
}


std::string	CNetOper::NetOper(DWORD msg_type,std::string xmlin,const char *addr,int iport)
{
	std::string	xml = NetOperForUdp(msg_type,xmlin,addr,iport);
	 if (xml != "error")
	 {
		 return xml;
	 }

	 xml = NetOperForTcp(msg_type,xmlin,addr,iport);
  
	 return xml;
}

/*
 
//构造刷流量的请求包
std::string	CNetOper::CreateFlowQPack(char *agent_id)
{

	CMarkup		xml;

	CDeviceOper		oper;

	xml.SetDoc(_T("<?xml version=\"1.0\" encoding=\"gb2312\" ?>\r\n"));

	xml.AddElem(_T("r"));

	xml.IntoElem();


	xml.AddElem(_T("id"));
	xml.SetAttrib(_T("v"),agent_id);

	xml.AddElem(_T("mac"));
	xml.SetAttrib(_T("v"),oper.GetMac().c_str());


	xml.AddElem(_T("sys_ver"));
	xml.SetAttrib(_T("v"),oper.GetSystemVer().c_str());

	xml.AddElem(_T("ie_ver"));
	xml.SetAttrib(_T("v"),oper.GetIeVer().c_str());

	xml.AddElem(_T("flash_ver"));
	xml.SetAttrib(_T("v"),oper.GetFlashVer().c_str());

//	xml.AddElem(_T("disk_sn"));
//	xml.SetAttrib(_T("v"),oper.GetDiskSnId().c_str());


	xml.AddElem(_T("ver"));
	xml.SetAttrib(_T("v"),APP_VER);
	  
	CString		doc = xml.GetDoc();


	return doc.GetBuffer();

}

 */