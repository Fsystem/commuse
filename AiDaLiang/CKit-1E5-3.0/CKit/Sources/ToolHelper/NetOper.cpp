// NetOper.cpp: implementation of the CNetOper class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "NetOper.h"

#pragma warning(disable:4996)

//#define MAX_40K_BUFFER 40*1024
//#define MSG_TAG_FLAG 0x19831984

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//char	*work_key = "jdaf832rn,snde329r";
					 
/*
UDP协议的一些约定：
1、单包发送：
		1）客户端发送请求数据；
		2）服务端收到数据后，发送回复数据
		3）异常处理：客户端在指定时间未收到回复，再次发送，重试

2、下载文件，多包发送
		1）客户端发送请求数据；
		2）服务端回送处理数据，并告之后继数据包个数（N个）；
		3）客户端发送请求N次单包请求，获取后继所有数据

*/
typedef	struct	_TAG_HEAD
{
	DWORD		tag;  //0x2010103
	DWORD		ver;	//版本号
	DWORD		msg_len;	//长度
	DWORD		msg_info;	//消息头

	void Code()	//编码，把网络
	{ 
		tag = htonl(tag);
		msg_len = htonl(msg_len);
		ver = htonl(ver);		
	}

	void UnCode()
	{
		tag = ntohl(tag);
		msg_len = ntohl(msg_len);
		ver = ntohl(ver);
	}
}TAG_HEAD,*PTAG_HEAD;

typedef	struct	_TAG_HEAD_UCHAR
{
	DWORD		tag;  //0x2010103
	DWORD		ver;	//版本号
	DWORD		msg_len;	//长度
	UCHAR		msg_info;	//消息头

	void Code()	//编码，把网络
	{ 
		tag = htonl(tag);
		msg_len = htonl(msg_len);
		ver = htonl(ver);		
	}

	void UnCode()
	{
		tag = ntohl(tag);
		msg_len = ntohl(msg_len);
		ver = ntohl(ver);
	}
}TAG_HEAD_UCHAR,*PTAG_HEAD_UCHAR;


//#define		WORK_PORT		6000

//#define		HEAD_VER		1
//#define		APP_VER			7


CNetOper::CNetOper(bool bUseDwordHead,DWORD dwTagHead ,LPCSTR lpKey)
{
	mUseDwordHead = bUseDwordHead;
	mTagHead = dwTagHead;
	strncpy(mNetKey,lpKey,sizeof(mNetKey)-1);
	mNetKey[sizeof(mNetKey)-1] = 0;
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

	memset(lpdata,0,sizeof(lpdata));

	yrc4((const unsigned char *)STR_NET_KEY,LEN_NET_KEY,(const unsigned char *)buffer,len,(unsigned char *)lpdata);

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
	NetOperForUdpWithout(msg_type,xml.c_str(),xml.size(),addr,iport);
}

void CNetOper::NetOperForUdpWithout(DWORD msg_type,const void* pdata,int ndatalen,const char *addr,int iport)
{
	yCUdp		udp_server; 

	udp_server.BindAddr(NULL,0);

	char					send_buffer[MAX_40K_BUFFER];
	int						data_len = 0;
	int						head_len = 0;

	memset(send_buffer,0,sizeof(send_buffer));
	data_len = ndatalen;  

	if (mUseDwordHead)
	{
		TAG_HEAD				tag_head; 

		head_len = sizeof(tag_head);

		tag_head.tag = mTagHead;
		tag_head.msg_info = msg_type;
		tag_head.msg_len = head_len + data_len;
		tag_head.ver = HEAD_VER;

		tag_head.Code();

		memcpy(send_buffer,&tag_head,sizeof(tag_head));

		
	}
	else
	{
		TAG_HEAD_UCHAR			tag_head; 

		head_len = sizeof(tag_head);

		tag_head.tag = mTagHead;
		tag_head.msg_info = msg_type;
		tag_head.msg_len = head_len + data_len;
		tag_head.ver = HEAD_VER;

		tag_head.Code();

		memcpy(send_buffer,&tag_head,sizeof(tag_head));
	}

	memcpy(send_buffer + head_len,pdata,data_len);

	int		recv_len = 0;

	OperDataCode(send_buffer,data_len + head_len);

	BOOL			recv_ret = FALSE;

	udp_server.SendDate(addr,iport,send_buffer,data_len + head_len);
}

//参数：		agent_id		代理商ID
//				buffer			数据缓冲区
//				len				数据缓冲区长度
//功能：得到劫持的配置数据
std::string CNetOper::NetOperForUdp(DWORD msg_type,std::string xml,const char *addr,int iport)
{ 
	std::string sres;
	std::vector<char> vec = NetOperForUdp(msg_type,xml.c_str(),xml.size(),addr,iport);
	if (vec.size() <= 0) sres="error";
	else sres.assign(vec.begin(),vec.end());

	return sres;
}


std::vector<char> CNetOper::NetOperForUdp(DWORD msg_type,const void* pdata,int ndatalen,const char *addr,int iport)
{
	std::vector<char>		vres;
	yCUdp		udp_server; 

	udp_server.BindAddr(NULL,0);

	char					recv_buffer[MAX_40K_BUFFER];
	char					send_buffer[MAX_40K_BUFFER];
	int						head_len = 0;
	int						data_len = 0;

	memset(send_buffer,0,sizeof(send_buffer));

	data_len = ndatalen;  

	if (mUseDwordHead)
	{
		TAG_HEAD				tag_head;

		head_len = sizeof(tag_head);

		tag_head.tag = mTagHead;
		tag_head.msg_info = msg_type;
		tag_head.msg_len = head_len + data_len;
		tag_head.ver = HEAD_VER;
		tag_head.Code();

		memcpy(send_buffer,&tag_head,sizeof(tag_head));
	}
	else
	{
		TAG_HEAD_UCHAR			tag_head;

		head_len = sizeof(tag_head);

		tag_head.tag = mTagHead;
		tag_head.msg_info = msg_type;
		tag_head.msg_len = head_len + data_len;
		tag_head.ver = HEAD_VER;
		tag_head.Code();

		memcpy(send_buffer,&tag_head,sizeof(tag_head));
	}
	
	memcpy(send_buffer + head_len,pdata,data_len);

	OperDataCode(send_buffer,data_len + head_len);

	BOOL			recv_ret = FALSE;
	int				recv_len = 0;
	int				recv_content_len = 0;

	for (int i = 0; i != 3; i++)
	{ 
		if(udp_server.SendDate(addr,iport,send_buffer,data_len + head_len) == 0)
			break;

		memset(recv_buffer,0,sizeof(recv_buffer));
		recv_len = udp_server.RecvDate(recv_buffer,sizeof(recv_buffer),5);
		if (recv_len >= head_len )
		{

			OperDataCode(recv_buffer,recv_len);

			if (mUseDwordHead)
			{
				TAG_HEAD				tag_head;
				memcpy(&tag_head,recv_buffer,sizeof(tag_head));
				tag_head.UnCode();
				if (tag_head.msg_info != msg_type + 1)
				{
					continue;
				}

				recv_content_len = tag_head.msg_len;
			}
			else
			{
				TAG_HEAD_UCHAR			tag_head;
				memcpy(&tag_head,recv_buffer,sizeof(tag_head));
				tag_head.UnCode();
				if (tag_head.msg_info != msg_type + 1)
				{
					continue;
				}
				
				recv_content_len = tag_head.msg_len;
			}
			
			recv_ret = TRUE;
			//接收成功，处理并退出
			break;
		}
	} 

	if (recv_ret)
	{    
		std::copy(recv_buffer+head_len,recv_buffer+recv_content_len,std::back_inserter(vres));
		return vres;
	}	

	return vres; 
}


//参数：		agent_id		代理商ID
//				buffer			数据缓冲区
//				len				数据缓冲区长度
//功能：得到劫持的配置数据
std::string CNetOper::NetOperForTcp(DWORD msg_type,std::string xml,const char *addr,int iport)
{  
	std::string sres;
	std::vector<char> vec = NetOperForTcp(msg_type,xml.c_str(),xml.size(),addr,iport);
	if (vec.size() <= 0) sres="";
	else if (vec.size()>=5 && strncmp(&vec[0],"error",5)==0) sres = "error";
	else sres.assign(vec.begin(),vec.end());

	return sres;
}

std::vector<char> CNetOper::NetOperForTcp(DWORD msg_type,const void* pdata,int ndatalen,const char *addr,int iport)
{
	std::vector<char>		vres;
	char					recv_buffer[MAX_40K_BUFFER];
	char					send_buffer[MAX_40K_BUFFER];
	int						send_len = 0;
	int						data_len = 0; 
	int						head_len = 0; 

	memset(send_buffer,0,sizeof(send_buffer));

	data_len = ndatalen;

	if (mUseDwordHead)
	{
		TAG_HEAD				tag_head; 

		head_len = sizeof(tag_head);

		memcpy(send_buffer+sizeof(tag_head),pdata,data_len);

		OperDataCode(send_buffer+sizeof(tag_head),data_len);

		tag_head.tag = MSG_TAG_FLAG;
		tag_head.msg_info = msg_type;
		tag_head.msg_len = sizeof(tag_head) + data_len;
		tag_head.ver = HEAD_VER;

		send_len = sizeof(tag_head) + data_len;

		tag_head.Code();

		memcpy(send_buffer,&tag_head,sizeof(tag_head));
	}
	else
	{
		TAG_HEAD_UCHAR			tag_head; 

		head_len = sizeof(tag_head);

		memcpy(send_buffer+sizeof(tag_head),pdata,data_len);

		OperDataCode(send_buffer+sizeof(tag_head),data_len);

		tag_head.tag = MSG_TAG_FLAG;
		tag_head.msg_info = msg_type;
		tag_head.msg_len = sizeof(tag_head) + data_len;
		tag_head.ver = HEAD_VER;

		send_len = sizeof(tag_head) + data_len;

		tag_head.Code();

		memcpy(send_buffer,&tag_head,sizeof(tag_head));
	}

	std::string		work_server = addr;

	yCTcp		tcp;
	char		ip_str[MAX_PATH];
	memset(ip_str,0,sizeof(ip_str));

	int		recv_len = 0;

	for (int i = 0; i != 3; i++)
	{ 
		yCTcp		oper_tcp;
		int			recv_len = 0;
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

		if (mUseDwordHead)
		{
			TAG_HEAD				tag_head; 
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
				return vres;
			}

			memset(recv_buffer,0,sizeof(recv_buffer));
			if (oper_tcp.Recvn(recv_buffer,tag_head.msg_len) != (int)tag_head.msg_len)
			{
				oper_tcp.Close();
				continue;
			}

			recv_len = tag_head.msg_len;

		}
		else
		{
			TAG_HEAD_UCHAR			tag_head;
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
				return vres;
			}

			memset(recv_buffer,0,sizeof(recv_buffer));
			if (oper_tcp.Recvn(recv_buffer,tag_head.msg_len) != (int)tag_head.msg_len)
			{
				oper_tcp.Close();
				continue;
			}

			recv_len = tag_head.msg_len;
		}
		
		oper_tcp.Close();  

		OperDataCode(recv_buffer,recv_len);

		std::copy(recv_buffer,recv_buffer+recv_len,std::back_inserter(vres));
		return vres;
	} 

	//char* szErr="error";
	//vres.assign(szErr,szErr+5);
	return vres; 
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

std::vector<char> CNetOper::NetOper(DWORD msg_type,const void* pdata,int ndatalen,const char *addr,int iport)
{
	std::vector<char>	recieve = NetOperForUdp(msg_type,pdata,ndatalen,addr,iport);
	if (recieve.size()>0)
	{
		return recieve;
	}

	recieve = NetOperForTcp(msg_type,pdata,ndatalen,addr,iport);

	return recieve;
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