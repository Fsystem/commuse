// NetOper.h: interface for the CNetOper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETOPER_H__824E22E9_B888_40EB_838C_FC53CC5A7E24__INCLUDED_)
#define AFX_NETOPER_H__824E22E9_B888_40EB_838C_FC53CC5A7E24__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CNetOper  
{
public:
	CNetOper();
	virtual ~CNetOper();

private: 

public:
	std::string NetOperForTcp(DWORD msg_type,std::string xml,const char *addr,int iport);
	std::string NetOperForUdp(DWORD msg_type,std::string xml,const char *addr,int iport);
	std::string	NetOper(DWORD msg_type,std::string xml,const char *addr,int iport);
	void        NetOperForUdpWithout(DWORD msg_type,std::string xml,const char *addr,int iport);
	
	std::vector<char> NetOperForTcp(DWORD msg_type,const void* pdata,int ndatalen,const char *addr,int iport);
	std::vector<char> NetOperForUdp(DWORD msg_type,const void* pdata,int ndatalen,const char *addr,int iport);
	std::vector<char> NetOper(DWORD msg_type,const void* pdata,int ndatalen,const char *addr,int iport);
	void			  NetOperForUdpWithout(DWORD msg_type,const void* pdata,int ndatalen,const char *addr,int iport);

	BOOL		OperDataCode(char	* buffer,DWORD len); 
private:
//	std::string	CreateFlowQPack(char *agent_id);

};

#endif // !defined(AFX_NETOPER_H__824E22E9_B888_40EB_838C_FC53CC5A7E24__INCLUDED_)
