// P2pTransmission.h: interface for the CP2pTransmission class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_P2PTRANSMISSION_H__375BF49B_1D8A_416D_BF12_E99AA07D7EEA__INCLUDED_)
#define AFX_P2PTRANSMISSION_H__375BF49B_1D8A_416D_BF12_E99AA07D7EEA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CP2pTransmission  
{
public:

#define		MSG_UDP_GET_FILE		1
#define		MSG_UDP_PUT_FILE		2

#define		MSG_TCP_GET_FILE		1
#define		MSG_TCP_PUT_FILE		2
#define		MSG_TCP_END_FILE		3

	typedef	struct
	{
		DWORD		msg_info;	//命令
		DWORD		dwtag;		//标志
		DWORD		crc;		//请求的文件CRC
		DWORD		len;		//文件大小，发送时为空，回复时必须有值
	}UDP_TAG,*PUDP_TAG;

	typedef	struct
	{
		DWORD		msg_info;
		DWORD		dwtag;
		DWORD		crc;
		DWORD		len;
	}TCP_TAG,*PTCP_TAG;


	typedef	struct
	{
		DWORD		crc;
		DWORD		len;
		const char	*file_data;
	}P2P_FILE_INFO,*PP2P_FILE_INFO;

	typedef	std::map<DWORD,P2P_FILE_INFO>		P2P_FILE_INFO_MAP;

	typedef	std::vector<std::string>			P2P_SERVER_LIST;

public:
	CP2pTransmission(DWORD net_key = 0x20161011,WORD wUdpPort = 16101,WORD wTcpPort = 16101);
	virtual ~CP2pTransmission();

public:
	void	Start();		//启动P2P
	
	DWORD	DownLoadFile(DWORD crc,char * file_data,DWORD *len);
	DWORD	RegP2pFile(DWORD crc,const char * file_data,DWORD len);

private: //UDP协议操作
	
	static	DWORD	WINAPI	UdpServerThread(PVOID pParam);
	DWORD	UdpServerFun(); 
	DWORD	GetFileListForUdp(DWORD crc , P2P_SERVER_LIST &_list);
	BOOL	CheckInList(P2P_SERVER_LIST _list,std::string _ip_str);

private: //tcp协议
	void	InitNetWork();
	static	DWORD WINAPI TcpServerThread(PVOID pParam);
	DWORD	TcpServerFun();
	DWORD	DownLoadForTcp(DWORD crc,std::string ip_str);		//从TCP协议上，下载一个文件到本地的列表中

private: //操作P2P文件数据
	
	DWORD	FindP2pCrc(DWORD	crc);			//查找这个P2P文件，根据CRC值
	DWORD	ReadP2pFile( DWORD crc,char * file_data,DWORD *len);
	DWORD	AddP2pFile(DWORD crc,const char * file_data,DWORD len);
private:

	BOOL	m_bstart;

	yCMyseclock			m_p2p_file_lock;
	P2P_FILE_INFO_MAP	m_p2p_file_map;

private:
	yCTcp				m_tcp_file_listen;

private:
	DWORD				m_NetKey;
	WORD				m_UdpPort;
	WORD				m_TcpPort;
};

#endif // !defined(AFX_P2PTRANSMISSION_H__375BF49B_1D8A_416D_BF12_E99AA07D7EEA__INCLUDED_)
