#include "stdafx.h"
#include "NetPackManager.h"

#include <string>
#include <sstream>
#include <WinSock.h>
#include <Iptypes.h>
#include <Iphlpapi.h>
#include <tchar.h>


#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib, "IPHLPAPI.lib")

using std::string;
using std::ostringstream;

#include <locale>
using std::locale;

static locale gLocale = locale("");

class PKGNumpunct: public std::numpunct<char> {
	std::string do_grouping() const { return ""; }
}; 

std::locale gPkgNumpunct(gLocale,new PKGNumpunct);

//-------------------------------------------------------------------------------
static char szHostName[MAX_PATH]={0};

//-------------------------------------------------------------------------------
NetPackBase::NetPackBase(NetPackBase* pDependObj):m_pDependObj(pDependObj)
{
	WSADATA sockData;
	if (WSAStartup (MAKEWORD (2, 2), &sockData))
	{
		printf("WSAStartup failed");
		return;
	}

	InitData();
	WSACleanup();
}

NetPackBase::~NetPackBase()
{
}


void NetPackBase::InitData()
{
	m_sAgentId	= "";
	m_sBarId	= "";
	m_dwKeyMacLocalIP = 0;
	m_dwKeyMacHostName = 0;
	m_dwLocalIP = 0;
	memset(m_cMacAddr, 0, sizeof(m_cMacAddr));
	memset(m_cHostName, 0, sizeof(m_cHostName));

	//获取本地主机名称 
	if (szHostName[0]==0)
	{
		if (gethostname(m_cHostName, sizeof(m_cHostName)) == SOCKET_ERROR) 
		{ 
			//OutputDebugString(TEXT("error:%d when getting local host name. "), WSAGetLastError()); 
		} 
	}
	else
	{
		strcpy(m_cHostName,szHostName);
	}
	

	InitLocalData();

	string sIp = GetLocalStrIP();

	ostringstream ostr;
	string sTemp="";
	ostr<<m_cMacAddr<<" "<<sIp;
	sTemp = ostr.str();
	CCRC crc;
	m_dwKeyMacLocalIP = crc.GetCrcNumber((char*)sTemp.c_str(), sTemp.length() );

	ostr.clear();
	ostr.str("");
	ostr<<m_cMacAddr<<" "<<m_cHostName;
	sTemp = ostr.str();
	m_dwKeyMacHostName = crc.GetCrcNumber((char*)sTemp.c_str(), sTemp.length() );

	m_sBuildVer = __DATE__;
	m_sBuildVer += " ";
	m_sBuildVer += __TIME__;
}

void  NetPackBase::InitLocalData()
{
	// 全局数据
	UCHAR	g_ucLocalMac[6];	// 本地MAC地址
	DWORD	g_dwGatewayIP;		// 网关IP地址
	DWORD	g_dwLocalIP;		// 本地IP地址
	DWORD	g_dwMask;			// 子网掩码
	PIP_ADAPTER_INFO pAdapterInfo = NULL;
	ULONG ulLen = 0;

	// 为适配器结构申请内存
	::GetAdaptersInfo(pAdapterInfo,&ulLen);
	pAdapterInfo = (PIP_ADAPTER_INFO)::GlobalAlloc(GPTR, ulLen);
	PIP_ADAPTER_INFO pAdapterInfoOld = pAdapterInfo;
	// 取得本地适配器结构信息
	if(::GetAdaptersInfo(pAdapterInfo,&ulLen) ==  ERROR_SUCCESS)
	{
		do { 
			if (pAdapterInfo->Type == MIB_IF_TYPE_ETHERNET)
			{  
				memcpy(g_ucLocalMac, pAdapterInfo->Address, 6);
				if (g_ucLocalMac[0]==0 && g_ucLocalMac[1]==0 &&
					g_ucLocalMac[2]==0 && g_ucLocalMac[3]==0 &&
					g_ucLocalMac[4]==0 && g_ucLocalMac[5]==0)
				{
					pAdapterInfo = pAdapterInfo->Next; 
					continue;
				}
				g_dwGatewayIP = inet_addr(pAdapterInfo->GatewayList.IpAddress.String);
				g_dwLocalIP = inet_addr(pAdapterInfo->IpAddressList.IpAddress.String);
				if (g_dwLocalIP  <= 16843009)
				{ 
					pAdapterInfo = pAdapterInfo->Next; 
					continue;
				}
				g_dwMask = inet_addr(pAdapterInfo->IpAddressList.IpMask.String);

				break;
			} 
			pAdapterInfo = pAdapterInfo->Next; 
		}while(pAdapterInfo);

		if(pAdapterInfoOld) ::GlobalFree(pAdapterInfoOld);
	}
	else
	{	
		if(pAdapterInfoOld) ::GlobalFree(pAdapterInfoOld);
		return ;
	}

	//	printf(" \n -------------------- 本地主机信息 -----------------------\n\n");
	in_addr in;
	in.S_un.S_addr = g_dwLocalIP; 
	u_char *p = g_ucLocalMac;
	sprintf(m_cMacAddr, "%02X:%02X:%02X:%02X:%02X:%02X", p[0], p[1], p[2], p[3], p[4], p[5]);

	m_dwLocalIP = ntohl(g_dwLocalIP);
}

string NetPackBase::GetLocalStrIP()
{
	char* pIp = (char*)&m_dwLocalIP;
	char szIp[32]={0};
	sprintf(szIp,"%d.%d.%d.%d",(pIp[3]&0xff),(pIp[2]&0xff),(pIp[1]&0xff),(pIp[0]&0xff));
	
	//ostringstream ostr;
	//ostr<<(unsigned)(pIp[3]&0xff)<<"."<<(unsigned)(pIp[2]&0xff)<<"."<<(unsigned)(pIp[1]&0xff)<<"."<<(unsigned)(pIp[0]&0xff);

	return szIp;
}

string NetPackBase::MakeBaseXml()
{
	//基础信息
	USES_XML;

	ADD_XMLITEM(m_sAgentId,"agent");
	ADD_XMLITEM(m_sBarId,"bar_id");
	ADD_XMLITEM(m_cMacAddr,"mac");
	ADD_XMLITEM(m_dwLocalIP,"ip");
	ADD_XMLITEM(GetLocalStrIP(),"str_ip");
	ADD_XMLITEM(m_cHostName,"host_name");
	ADD_XMLITEM(m_dwKeyMacLocalIP,"mac_localipkey");
	ADD_XMLITEM(m_dwKeyMacHostName,"mac_hostnamekey");
	ADD_XMLITEM(m_sBuildVer,"build_version");

	return GET_XML;
}

void NetPackBase::ParseBaseData(string szXml)
{
	if (szXml.size()<=0) return;

	PARSE_XML_BEGIN;

	PARSE_ITEM_STDSTR(m_sAgentId,"agent");
	PARSE_ITEM_STDSTR(m_sBarId,"bar_id");
	PARSE_ITEM_CSTRING(m_cMacAddr,"mac",sizeof(m_cMacAddr));
	PARSE_ITEM_NUMBER(m_dwLocalIP,"ip");
	/*str_ip 不需要*/
	PARSE_ITEM_CSTRING(m_cHostName,"host_name",sizeof(m_cMacAddr) );
	PARSE_ITEM_NUMBER(m_dwKeyMacLocalIP,"mac_localipkey");
	PARSE_ITEM_NUMBER(m_dwKeyMacHostName,"mac_hostnamekey");

	PARSE_ITEM_STDSTR(m_sBuildVer,"build_version");

	PARSE_XML_END;
}

std::string NetPackBase::ToXmlStr()
{
	std::string xml = "<?xml version=\"1.0\" encoding=\"gb2312\" ?>\n";
	xml += "<r>\n";

	xml += MakeBaseXml();

	xml += MakeBodyData();

	xml += "</r>\n";

	return xml;
}

void NetPackBase::FromXmlStr(std::string sXml)
{
	ParseBaseData(sXml);
	ParseBodyData(sXml);
}



//-------------------------------------------------------------------------------
std::string PKGCommon_Q::MakeBodyData()
{
	return "";
}

void PKGCommon_Q::ParseBodyData(std::string szXml)
{

}

PKGCommon_A::PKGCommon_A(NetPackBase* pDependObj):NetPackBase(pDependObj)
{
	bStatus = FALSE;
}

std::string PKGCommon_A::MakeBodyData()
{
	USES_XML;

	ADD_XMLITEM(bStatus,"status");

	return GET_XML;
}

void PKGCommon_A::ParseBodyData(std::string szXml)
{
	PARSE_XML_BEGIN;

	PARSE_ITEM_NUMBER(bStatus,"status");

	PARSE_XML_END;
}
//-------------------------------------------------------------------------------