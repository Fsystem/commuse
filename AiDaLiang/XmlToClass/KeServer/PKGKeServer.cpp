#include "stdafx.h"
#include "PKGKeServer.h"

//-------------------------------------------------------------------------------
std::string PKGKeStartClient_Q::MakeBodyData()
{
	USES_XML;

	ADD_XMLITEM(sOSVersion,"sysVer");
	ADD_XMLITEM(sIEVersion,"ieVer");
	ADD_XMLITEM(sFlashVersion,"flashVer");
	ADD_XMLITEM(sDiskId,"diskId");
	ADD_XMLITEM(sCltVersion,"clientVer");

	ADD_XMLITEM(dwCrcMacHostIpstr,"mac_host_ipkey");

	return GET_XML;
}

void PKGKeStartClient_Q::ParseBodyData(std::string szXml)
{
	PARSE_XML_BEGIN;

	PARSE_ITEM_STDSTR(sOSVersion,"sysVer");
	PARSE_ITEM_STDSTR(sIEVersion,"ieVer");
	PARSE_ITEM_STDSTR(sFlashVersion,"flashVer");
	PARSE_ITEM_STDSTR(sDiskId,"diskId");
	PARSE_ITEM_STDSTR(sCltVersion,"clientVer");

	PARSE_ITEM_NUMBER(dwCrcMacHostIpstr,"mac_host_ipkey")

	PARSE_XML_END;
}
//-------------------------------------------------------------------------------
PKGKeClinetOnline_A::PKGKeClinetOnline_A()
{
	dwNextTime = 0;
}
std::string PKGKeClinetOnline_A::MakeBodyData()
{
	USES_XML;

	ADD_XMLITEM(bStatus,"status");
	ADD_XMLITEM(dwNextTime,"next_time");

	return GET_XML;
}

void PKGKeClinetOnline_A::ParseBodyData(std::string szXml)
{
	PARSE_XML_BEGIN;

	PARSE_ITEM_NUMBER(bStatus,"status");
	PARSE_ITEM_NUMBER(dwNextTime,"next_time");

	PARSE_XML_END;
}
//-------------------------------------------------------------------------------
PKGKeUpDateUrl_A::PKGKeUpDateUrl_A()
{
	bStatus = FALSE;
	sUrl = "";
}
std::string PKGKeUpDateUrl_A::MakeBodyData()
{
	USES_XML;

	ADD_XMLITEM(bStatus,"status");
	ADD_XMLITEM(sUrl,"url");

	return GET_XML;
}
void PKGKeUpDateUrl_A::ParseBodyData(std::string szXml)
{
	PARSE_XML_BEGIN;

	PARSE_ITEM_NUMBER(bStatus,"status");
	PARSE_ITEM_STDSTR(sUrl,"url");

	PARSE_XML_END;
}
//////////////////////////////////////////////////////////////////////////
//网吧服务与核心服务通信基类
std::string KeNetPackBase::MakeBodyData()
{
	USES_XML;

	ADD_XMLITEM(sOSVersion, "sysVer");
	ADD_XMLITEM(sIEVersion, "ieVer");
	ADD_XMLITEM(sFlashVersion, "flashVer");
	ADD_XMLITEM(sDiskId, "diskId");

	ADD_XMLITEM(sServerVersion,"serverver");
	ADD_XMLITEM(nClientCount, "clientnum");

	return GET_XML;
}

void KeNetPackBase::ParseBodyData(std::string szXml)
{
	PARSE_XML_BEGIN;

	PARSE_ITEM_STDSTR(sOSVersion, "sysVer");
	PARSE_ITEM_STDSTR(sIEVersion, "ieVer");
	PARSE_ITEM_STDSTR(sFlashVersion, "flashVer");
	PARSE_ITEM_STDSTR(sDiskId, "diskId");

	PARSE_ITEM_STDSTR(sServerVersion, "serverver");
	PARSE_ITEM_NUMBER(nClientCount, "clientnum");

	PARSE_XML_END;
}
//////////////////////////////////////////////////////////////////////////
//网吧服务向核心服务请求网吧ID
PKGKeBarIDServer_A::PKGKeBarIDServer_A()
{
	bStatus = FALSE;
	nBarId = 0;
}
std::string PKGKeBarIDServer_A::MakeBodyData()
{
	USES_XML;

	ADD_XMLITEM(bStatus,"status");
	ADD_XMLITEM(nBarId,"barid");

	return GET_XML;
}
void PKGKeBarIDServer_A::ParseBodyData(std::string szXml)
{
	PARSE_XML_BEGIN;

	PARSE_ITEM_NUMBER(bStatus,"status");
	PARSE_ITEM_NUMBER(nBarId,"barid");

	PARSE_XML_END;
}
//////////////////////////////////////////////////////////////////////////
//网吧服务向核心服务请求升级
PKGKeUpdateServer_A::PKGKeUpdateServer_A()
{
	bStatus = FALSE;
	strServUrl = "";
	strClientUrl = "";
	strServVer = "";
	strClientVer = "";
}  
std::string PKGKeUpdateServer_A::MakeBodyData()
{
	USES_XML;

	ADD_XMLITEM(bStatus,"status");
	ADD_XMLITEM(strServUrl,"server_url");
	ADD_XMLITEM(strClientUrl,"client_url");
	ADD_XMLITEM(strServVer,"server_ver");
	ADD_XMLITEM(strClientVer,"client_ver");

	return GET_XML;
}
void PKGKeUpdateServer_A::ParseBodyData(std::string szXml)
{
	PARSE_XML_BEGIN;

	PARSE_ITEM_NUMBER(bStatus,"status");
	PARSE_ITEM_STDSTR(strServUrl,"server_url");
	PARSE_ITEM_STDSTR(strClientUrl,"client_url");
	PARSE_ITEM_STDSTR(strServVer,"server_ver");
	PARSE_ITEM_STDSTR(strClientVer,"client_ver");

	PARSE_XML_END;
}
//////////////////////////////////////////////////////////////////////////
//网吧服务上报实时在线
PKGKeServerOnline_A::PKGKeServerOnline_A()
{
	dwNextTime = 0;
	//bWhiteList = FALSE;
	bUpdate = FALSE;
	bStopClient = FALSE;
	bMsgList = FALSE;
}
std::string PKGKeServerOnline_A::MakeBodyData()
{
	USES_XML;

	ADD_XMLITEM(bStatus,"status");
	ADD_XMLITEM(dwNextTime,"next_time");

	//ADD_XMLITEM(bWhiteList,"whitelist");
	ADD_XMLITEM(bUpdate,"update");
	ADD_XMLITEM(bStopClient,"stopclient");
	ADD_XMLITEM(bMsgList,"msglist");

	return GET_XML;
}

void PKGKeServerOnline_A::ParseBodyData(std::string szXml)
{
	PARSE_XML_BEGIN;

	PARSE_ITEM_NUMBER(bStatus,"status");
	PARSE_ITEM_NUMBER(dwNextTime,"next_time");

	//PARSE_ITEM_NUMBER(bWhiteList,"whitelist");
	PARSE_ITEM_NUMBER(bUpdate,"update");
	PARSE_ITEM_NUMBER(bStopClient,"stopclient");
	PARSE_ITEM_NUMBER(bMsgList,"msglist");

	PARSE_XML_END;
}
//////////////////////////////////////////////////////////////////////////
//网吧服务向核心服务请求策略
PKGKeGetRuleServer_A::PKGKeGetRuleServer_A()
{
	bStatus = FALSE;
	strConfigUrl = "";
}  
std::string PKGKeGetRuleServer_A::MakeBodyData()
{
	USES_XML;

	ADD_XMLITEM(bStatus,"status");
	ADD_XMLITEM(strConfigUrl,"config_url");

	return GET_XML;
}
void PKGKeGetRuleServer_A::ParseBodyData(std::string szXml)
{
	PARSE_XML_BEGIN;

	PARSE_ITEM_NUMBER(bStatus,"status");
	PARSE_ITEM_STDSTR(strConfigUrl,"config_url");

	PARSE_XML_END;
}
//////////////////////////////////////////////////////////////////////////
//网吧服务向核心请求循环消息
PKGKeGetMsgServer_A::PKGKeGetMsgServer_A()
{

}  
std::string PKGKeGetMsgServer_A::MakeBodyData()
{
	USES_XML;

	for (std::list<PKGMsgInfo>::iterator it = msglist.begin();
		it != msglist.end();it++)
	{
		ADDSTART_CHILDNODE("msg");

		ADD_XMLITEM((*it).strTitle,"title");
		ADD_XMLITEM((*it).strUrl,"url");

		ADDEND_CHILDNODE("msg");
	}

	return GET_XML;
}
void PKGKeGetMsgServer_A::ParseBodyData(std::string szXml)
{
	PARSE_XML_BEGIN;
	PKGMsgInfo procInfoEx;

	while(PARSE_XML_FINDELEM("msg"))
	{
		PARSE_XML_INTOELEM;

		memset(&procInfoEx,0,sizeof(procInfoEx));
		PARSE_ITEM_STDSTR(procInfoEx.strTitle,"title");
		PARSE_ITEM_STDSTR(procInfoEx.strUrl,"url");

		msglist.push_back(procInfoEx);

		PARSE_XML_OUTOFELEM;
	}

	PARSE_XML_END;
}
//////////////////////////////////////////////////////////////////////////
//网吧服务询问核心是否有要停止的客户端
PKGKeStopClientServer_A::PKGKeStopClientServer_A()
{
	iType = -1;
}  
std::string PKGKeStopClientServer_A::MakeBodyData()
{
	USES_XML;

	ADD_XMLITEM(iType,"type");

	return GET_XML;
}
void PKGKeStopClientServer_A::ParseBodyData(std::string szXml)
{
	PARSE_XML_BEGIN;

	PARSE_ITEM_NUMBER(iType,"type");

	PARSE_XML_END;
}
//////////////////////////////////////////////////////////////////////////
//网吧服务向核心请求循环白名单
/*PKGKeGetWhiteList_A::PKGKeGetWhiteList_A()
{

}  
std::string PKGKeGetWhiteList_A::MakeBodyData()
{
	USES_XML;

	for (std::list<PKGWhiteInfo>::iterator it = whitelist.begin();
		it != whitelist.end();it++)
	{
		ADDSTART_CHILDNODE("whitelist");

		ADD_XMLITEM((*it).strProcName,"procname");
		ADD_XMLITEM((*it).dwProcMD5,"procmd5");
		ADD_XMLITEM((*it).dwProcCRC,"proccrc");

		ADDEND_CHILDNODE("whitelist");
	}

	return GET_XML;
}
void PKGKeGetWhiteList_A::ParseBodyData(std::string szXml)
{
	PARSE_XML_BEGIN;
	PKGWhiteInfo procInfoEx;

	while(PARSE_XML_FINDELEM("whitelist"))
	{
		PARSE_XML_INTOELEM;

		memset(&procInfoEx,0,sizeof(procInfoEx));
		PARSE_ITEM_STDSTR(procInfoEx.strProcName,"procname");
		PARSE_ITEM_NUMBER(procInfoEx.dwProcMD5,"procmd5");
		PARSE_ITEM_NUMBER(procInfoEx.dwProcCRC,"proccrc");

		whitelist.push_back(procInfoEx);

		PARSE_XML_OUTOFELEM;
	}

	PARSE_XML_END;
}*/
//////////////////////////////////////////////////////////////////////////
PKGKeGetWhiteList_A::PKGKeGetWhiteList_A()
{
	bStatus = FALSE;
	strWhiteUrl = "";
}  
std::string PKGKeGetWhiteList_A::MakeBodyData()
{
	USES_XML;

	ADD_XMLITEM(bStatus,"status");
	ADD_XMLITEM(strWhiteUrl,"whitelist");

	return GET_XML;
}
void PKGKeGetWhiteList_A::ParseBodyData(std::string szXml)
{
	PARSE_XML_BEGIN;

	PARSE_ITEM_NUMBER(bStatus,"status");
	PARSE_ITEM_STDSTR(strWhiteUrl,"whitelist");

	PARSE_XML_END;
}
