#pragma once

#include <winsock2.h>
//所有网络发送，遵守此协议头
//发送时，需要注意，TCP协议，头尾分开加密
//UDP协议，整个一起加密
//回复的信息中，由是全部加密的，TCP协议服务设置超时，5秒，客户机尽可能收完所有数据，主动断开，服务器不会断开连接

//网络协议定义
typedef	struct	_TAG_NET_HEAD
{
	DWORD		tag;  //NETWORK_TAG
	DWORD		ver;	//版本号
	DWORD		msg_len;	//长度
	DWORD		msg_info;	//消息头
	DWORD		reserve;	//保留字段

	void	Clear()
	{
		tag = 0;
		ver = 0;
		msg_len = 0;
		msg_info = 0;
		reserve = 0;
	}

	_TAG_NET_HEAD()
	{
		Clear();
	}

	void Code()	//编码，把网络
	{ 
		tag = htonl(tag);
		msg_len = htonl(msg_len);
		ver = htonl(ver);
		msg_info = htonl(msg_info);
		reserve = htonl(reserve);
	}

	void UnCode()
	{
		tag = ntohl(tag);
		msg_len = ntohl(msg_len);
		ver = ntohl(ver);
		msg_info = ntohl(msg_info);
		reserve = ntohl(reserve);
	}

}TAG_NET_HEAD,*PTAG_NET_HEAD;


//统一管理请求类型，规则如下：
//1）100以上由核心组件使用，如：请求下载服务之类的
//2）100以上每100个段，由一个插件使用；
//3) 其他人使用，从400这个段开始，前二个段，已分配


#define		MSG_CENTER_GET_FILEURL_AND_SERVER_Q		11				//请求下载业务包及服务器地址
#define		MSG_CENTER_GET_FILEURL_AND_SERVER_A		12

#define		MSG_REPORT_KERNEL_RUN_SUCCESS_Q			21				//核心插件加载完成
#define		MSG_REPORT_KERNEL_RUN_SUCCESS_A			22
