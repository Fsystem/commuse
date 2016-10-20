// P2pTransmission.cpp: implementation of the CP2pTransmission class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "P2pTransmission.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CP2pTransmission::CP2pTransmission(DWORD net_key /*= 0x20161011*/
	,WORD wUdpPort /*= 16101*/
	,WORD wTcpPort /*= 16101*/ )
{
	m_bstart = FALSE;
	m_NetKey = net_key;
}

CP2pTransmission::~CP2pTransmission()
{

}

void CP2pTransmission::Start()
{
	if (m_bstart)
	{
		return ;
	}
	m_bstart = TRUE;

	//创建连接
	InitNetWork();
	
	CloseHandle(::CreateThread(NULL,0,CP2pTransmission::TcpServerThread,this,0,NULL));
	CloseHandle(::CreateThread(NULL,0,CP2pTransmission::UdpServerThread,this,0,NULL));
}

void	CP2pTransmission::InitNetWork()
{
	//初始化，并监听TCP端口
	if(!m_tcp_file_listen.Open())
	{
		return ;
	}
	
	if(!m_tcp_file_listen.Bind(NULL,m_TcpPort))
	{
		return ;
	}
	
	m_tcp_file_listen.Listen(100);

}

DWORD	WINAPI CP2pTransmission::UdpServerThread( PVOID pParam )
{
	CP2pTransmission	*lpthis = (CP2pTransmission*)pParam;
	if (lpthis == NULL)
	{
		return 0;
	}

	return lpthis->UdpServerFun();
	
}

DWORD CP2pTransmission::UdpServerFun()
{
	yCUdp		udp_server;
	char		my_ip_str[32];
	CToolOper	oper;
	
	memset(my_ip_str,0,sizeof(my_ip_str));
	
	UDP_TAG		udp_tag;
	
	if(!udp_server.BindAddr(NULL,m_UdpPort))
		return 1;
	
	while (TRUE)
	{
		memset(&udp_tag,0,sizeof(udp_tag));
		char		ip_str[32];
		UINT		dwport;
		
		memset(ip_str,0,sizeof(ip_str));
		if(udp_server.RecvDate(&udp_tag,sizeof(udp_tag),ip_str,dwport) != sizeof(udp_tag))
		{  
			//Sleep(100);
			continue;
		} 
		
		//判断是不是本机的包，如果是，直接扔掉
		if (strcmp(my_ip_str,ip_str) == 0)
		{
			continue;
		}
		
		if (oper.FindLocalIp(ip_str))
		{
			strncpy(my_ip_str,ip_str,31);
			continue;
		}
		
		if (udp_tag.dwtag != m_NetKey)
		{
			continue;
		}
		
		switch (udp_tag.msg_info)
		{
		case MSG_UDP_GET_FILE:
			udp_tag.len = FindP2pCrc(udp_tag.crc);
			if (udp_tag.len == 0)
			{
				continue;
			}

			udp_tag.msg_info = MSG_UDP_PUT_FILE;
			break;
		default:
			continue;
		} 
		
		//发送回复包
		udp_server.SendDate(ip_str,dwport,&udp_tag,sizeof(udp_tag));
	}
}


DWORD CP2pTransmission::GetFileListForUdp( DWORD crc , P2P_SERVER_LIST &_list)
{
	yCUdp		udp_server;
	char		my_ip_str[32];
	UINT		dwPort;
	DWORD		file_len = 0;

	_list.clear();
	//随便绑定一个端口
	if(!udp_server.BindAddr(NULL,0,TRUE))
		return 0;
	
	for (int i = 0; i != 3; i++)
	{
		UDP_TAG udp_tag;
		
		memset(&udp_tag,0,sizeof(udp_tag));
		udp_tag.dwtag = m_NetKey;
		udp_tag.msg_info = MSG_UDP_GET_FILE;
		udp_tag.crc = crc;
		
		for (int cs = 0; cs != 3; cs ++)
		{  
			udp_server.SendDate("255.255.255.255",m_UdpPort,&udp_tag,sizeof(udp_tag));
			Sleep(50);
		}
		
		
		DWORD begin_time = GetTickCount();
		while(GetTickCount() - begin_time < 2000)
		{ 
			memset(&udp_tag,0,sizeof(udp_tag));
			memset(my_ip_str,0,sizeof(my_ip_str));
			if(udp_server.RecvDate(&udp_tag,sizeof(udp_tag),my_ip_str,dwPort,1) == sizeof(udp_tag))
			{
				if (udp_tag.dwtag == m_NetKey && udp_tag.msg_info == MSG_UDP_PUT_FILE &&
					udp_tag.len != 0 && udp_tag.crc == crc && CheckInList(_list,my_ip_str) == FALSE)
				{
					_list.push_back(my_ip_str);
					file_len = udp_tag.len;
				}
			}
		}	
	}
	return file_len;
	
}

BOOL	CP2pTransmission::CheckInList(P2P_SERVER_LIST _list,std::string _ip_str)
{
	for (P2P_SERVER_LIST::iterator iter = _list.begin(); iter != _list.end(); iter++)
	{
		if (stricmp(iter->c_str(),_ip_str.c_str()) == 0)
		{
			return TRUE;
		}
	}

	return FALSE;
}


DWORD WINAPI CP2pTransmission::TcpServerThread(PVOID pParam)
{

	CP2pTransmission	*lpthis = (CP2pTransmission*)pParam;

	if (lpthis == NULL)
	{
		return 0;
	}

	return	lpthis->TcpServerFun();
	 
}


DWORD CP2pTransmission::TcpServerFun()
{
	yCTcp		file_tcp;
	
	if( ( file_tcp = m_tcp_file_listen.Accept() ) <= 0)
	{
		// 接收连接失败 
		return 0;
	}
	
	CloseHandle(CreateThread(NULL,0,CP2pTransmission::TcpServerThread,this,0,NULL)); 
	
	TCP_TAG		tcp_tag;
	memset(&tcp_tag,0,sizeof(tcp_tag));
	if( file_tcp.Recvn(&tcp_tag,sizeof(tcp_tag)) != sizeof(tcp_tag) )
	{ 
		file_tcp.Close();
		return 0;
	}
	
	if (tcp_tag.dwtag != m_NetKey || tcp_tag.msg_info != MSG_TCP_GET_FILE || tcp_tag.crc == 0)
	{
		file_tcp.Close();
		return 0;		//发送请求参数错误
	}

	//读取这个文件
	char	*file_buffer = NULL;
	DWORD	file_len = 0;

	ReadP2pFile(tcp_tag.crc,NULL,&file_len);

	if (file_len == 0)
	{
		//没有找到这个文件，发送错误了
		file_tcp.Close();
		return 0;
	}

	file_buffer = (char *)malloc(file_len);
	
	if (file_buffer == NULL)
	{
		file_tcp.Close();
		return 0;	//内存分配失败
	}

	if (ReadP2pFile(tcp_tag.crc,file_buffer,&file_len) != NO_ERROR)
	{	
		free(file_buffer);
		file_tcp.Close();
		return 0;	//读取文件失败
	}
	 
	//发送文件长度
	tcp_tag.msg_info = MSG_TCP_PUT_FILE;
	tcp_tag.len = file_len;
	if(file_tcp.Sendn(&tcp_tag,sizeof(tcp_tag)) != sizeof(tcp_tag))
	{
		free(file_buffer);
		file_tcp.Close();
		return 0;	//读取文件失败
	}
	
	if(file_tcp.Sendn(file_buffer,file_len) != file_len)
	{
		free(file_buffer);
		file_tcp.Close();
		return 0;	//读取文件失败
	}

	free(file_buffer);
	file_buffer = NULL;
	
	memset(&tcp_tag,0,sizeof(tcp_tag));	
	file_tcp.Recvn(&tcp_tag,sizeof(tcp_tag));
	file_tcp.Close();
	return 0; 
}


DWORD CP2pTransmission::DownLoadForTcp( DWORD crc ,std::string ip_str)
{ 
	yCTcp			file_tcp;
	
	file_tcp.Open();
	if(!file_tcp.ConnectNoBlock(ip_str.c_str(),m_TcpPort,3))
	{
		file_tcp.Close();
		return 1;
	}
	
	TCP_TAG		tcp_tag;
	memset(&tcp_tag,0,sizeof(tcp_tag));
	
	tcp_tag.msg_info = MSG_TCP_GET_FILE;
	tcp_tag.crc = crc;
	tcp_tag.dwtag = m_NetKey;
	
	if (file_tcp.Sendn(&tcp_tag,sizeof(tcp_tag)) != sizeof(tcp_tag))
	{ 
		file_tcp.Close();
		return 2;
	}
	
	memset(&tcp_tag,0,sizeof(tcp_tag));
	if (file_tcp.Recvn(&tcp_tag,sizeof(tcp_tag),TCP_TIMEOUT * 5) != sizeof(tcp_tag))
	{
		file_tcp.Close();
		return 3;
	}
	
	if (tcp_tag.msg_info != MSG_TCP_PUT_FILE || tcp_tag.dwtag != m_NetKey ||
		tcp_tag.len == 0 && tcp_tag.crc != crc)
	{
		file_tcp.Close();
		return 4;
	}

	//分配内存
	char	*lpbuffer = (char*)malloc(tcp_tag.len);

	if (lpbuffer == NULL)
	{
		file_tcp.Close();
		return 5;
	}
	
	if (file_tcp.Recvn(lpbuffer,tcp_tag.len) != tcp_tag.len)
	{
		free(lpbuffer);
		file_tcp.Close();
		return 6;
	}
   
	
	tcp_tag.msg_info = MSG_TCP_END_FILE;	
	file_tcp.Sendn(&tcp_tag,sizeof(tcp_tag));
	file_tcp.Close();
	
	CCRC		file_crc;
	DWORD		crc_msg = file_crc.GetCrcNumber(lpbuffer,tcp_tag.len);
	
	if (crc_msg == tcp_tag.crc)
	{
		//增加到列表中
		AddP2pFile(tcp_tag.crc,lpbuffer,tcp_tag.len);
		return 0;
	}
	
	return 7;	//文件效验失败
}

DWORD	CP2pTransmission::AddP2pFile(DWORD crc,const char * file_data,DWORD len)
{
	yCMyseclock::Auto			lock(m_p2p_file_lock);

	P2P_FILE_INFO				info;

	info.crc = crc;
	info.file_data = file_data;
	info.len = len;

	m_p2p_file_map[crc] = info;

	return 0;
}

DWORD CP2pTransmission::FindP2pCrc( DWORD crc )
{
	yCMyseclock::Auto			lock(m_p2p_file_lock);

	P2P_FILE_INFO_MAP::iterator iter = m_p2p_file_map.find(crc);

	if (iter == m_p2p_file_map.end())
	{
		return 0;
	}

	return iter->second.len;
}


DWORD CP2pTransmission::ReadP2pFile( DWORD crc,char * file_data,DWORD *len)
{
	yCMyseclock::Auto			lock(m_p2p_file_lock);
	
	P2P_FILE_INFO_MAP::iterator iter = m_p2p_file_map.find(crc);
	
	if (iter == m_p2p_file_map.end())
	{
		return ERROR_FILE_NOT_FOUND;
	}
	
	if (iter->second.len > *len || file_data == NULL)
	{
		*len = iter->second.len;
		return ERROR_INSUFFICIENT_BUFFER;		//内存太小
	}

	*len = iter->second.len;
	memcpy(file_data,iter->second.file_data,*len);

	return NO_ERROR;
}

DWORD CP2pTransmission::DownLoadFile( DWORD crc,char * file_data,DWORD *len )
{
	if(crc == 0)
	{
		return ERROR_INVALID_PARAMETER;
	}

	//查询本地是否有

	DWORD	file_len = FindP2pCrc(crc);

	if (file_len != 0)
	{
		//本地找到了
		return ReadP2pFile(crc,file_data,len);
	}
	

	//发送消息查询网络
	P2P_SERVER_LIST		_list;
	file_len = GetFileListForUdp(crc,_list);

	if (file_len == 0)
	{
		return ERROR_FILE_NOT_FOUND;
	}

	//下载回来
	for (P2P_SERVER_LIST::iterator iter = _list.begin(); iter != _list.end(); iter++)
	{
		if(DownLoadForTcp(crc,iter->c_str()) == 0)
			break;
	}

	return ReadP2pFile(crc,file_data,len);
}

DWORD CP2pTransmission::RegP2pFile( DWORD crc,const char * file_data,DWORD len )
{
	if (crc == 0 || file_data == NULL || len == 0)
	{
		return ERROR_INVALID_PARAMETER;
	}

	char		*buffer = (char*)malloc(len);

	if (buffer == NULL)
	{
		return ERROR_NO_SYSTEM_RESOURCES;		//分配内存失败
	}

	memcpy(buffer,file_data,len);

	return AddP2pFile(crc,buffer,len);
}

