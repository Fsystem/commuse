/** 上报管理
*   @FileName  : ReportManager.h
*   @Author    : Double Sword(doublesword.jack@gmail.com)
*   @date      : 2016-5-3
*   @Copyright : Copyright belong to Author and 9u2,CO.LTD
*   @RefDoc    : 一定要注意，xml的操作都是多字节的，如果需要传递宽字符，请先转为多字节临时变量进行操作
*/

#ifndef __NetPackManager_H
#define __NetPackManager_H

#include <locale>

extern std::locale gPkgNumpunct;
class NetPackBase
{			
#if _MSC_VER < 1600
#	define DECLTYPE(item) double 
#else 
#	define DECLTYPE(item) decltype(item)
#endif
protected: 
	NetPackBase* m_pDependObj;
public:
	std::string		m_sBarId;						//网吧ID
	std::string		m_sAgentId;						//渠道ID
	DWORD			m_dwKeyMacLocalIP;				//mac和本地IP的主键
	DWORD			m_dwKeyMacHostName;				//mac和主机名称的主键
	DWORD			m_dwLocalIP;
	char			m_cMacAddr[20];					//mac地址
	char			m_cHostName[MAX_PATH];			//主机名称
	std::string     m_sBuildVer;					//编译版本 
public:
	NetPackBase(NetPackBase* pDependObj = NULL);
	virtual ~NetPackBase();
	std::string GetLocalStrIP();
public:
	std::string ToXmlStr();
	void FromXmlStr(std::string sXml);
	virtual bool PrepareData(){ return true;};
protected:
	virtual std::string MakeBodyData()=0;
	virtual void ParseBodyData(std::string szXml)=0;
protected:
	void InitData();
	void InitLocalData();
	std::string MakeBaseXml();
	void ParseBaseData(std::string szXml);

	//void AddStringItem(std::ostringstream os,std::string sItem,std::string sKey);
	//void AddNumberItem(std::ostringstream os, double numItem,std::string sKey);
	//std::string ParseItem(std::string sKey);


	//生成xml宏
#define USES_XML ostringstream ostringstreamIn; string stringRet = ""

#define ADD_XMLITEM(item,name) \
	ostringstreamIn.imbue(gPkgNumpunct);\
	ostringstreamIn<<"<"<<name<<" v=\""<<item<<"\"/>\n";\
	stringRet += ostringstreamIn.str();\
	ostringstreamIn.str("");

#define GET_XML stringRet

#define ADDSTART_CHILDNODE(xmlNODE) \
	ostringstreamIn<<"<"<<xmlNODE<<">\n";\
	stringRet += ostringstreamIn.str();\
	ostringstreamIn.str("");

#define ADDEND_CHILDNODE(xmlNODE) \
	ostringstreamIn<<"</"<<xmlNODE<<">\n";\
	stringRet += ostringstreamIn.str();\
	ostringstreamIn.str("");

	//解析xml宏
	//-------------------------------------------------------------------------------
#if defined(UNICODE) || defined(_UNICODE)
#	define PARSE_XML_BEGINW \
	std::string sRet = "";\
	CMarkup xml;\
	std::wstring sParseXml = A2WString(szXml.c_str());\
	bool bHasKey = false;\
	if (xml.SetDoc(sParseXml))\
	{\
		if(xml.FindElem(TEXT("r")) )\
		{\
			xml.IntoElem();\
			std::wstring sParseKey;

#	define PARSE_XML_INTOELEMW \
	xml.IntoElem();

#	define PARSE_XML_FINDELEMW(xmlNODE) \
	xml.FindElem(A2WString(xmlNODE))

#	define PARSE_XML_OUTOFELEMW \
	xml.OutOfElem();

	//-parse
#	define PARSE_ITEM_NUMBERW(item,xmlkey) \
	sParseKey = A2WString(xmlkey);\
	bHasKey = xml.FindElem(sParseKey);\
	if(bHasKey)\
	{\
		sRet = T2AString(xml.GetAttrib(TEXT("v")).c_str());\
		item = (DECLTYPE(item))atof(sRet.c_str());\
	}

#	define PARSE_ITEM_CSTRINGW(item,xmlkey,itemsize) \
	sParseKey = A2WString(xmlkey);\
	bHasKey = xml.FindElem(sParseKey);\
	if(bHasKey)\
	{\
		sRet = T2AString(xml.GetAttrib(TEXT("v")).c_str());\
		strncpy(item,sRet.c_str(),itemsize);\
	}

#	define PARSE_ITEM_STDSTRW(item,xmlkey) \
	sParseKey = A2WString(xmlkey);\
	bHasKey = xml.FindElem(sParseKey);\
	if(bHasKey)\
	{\
		sRet = T2AString(xml.GetAttrib(TEXT("v")).c_str());\
		item = sRet;\
	} 

#	define XML_LOOP_VERIFY bHasKey
	//-parse end

	//-
#	define PARSE_XML_BEGIN		PARSE_XML_BEGINW
#	define PARSE_ITEM_NUMBER	PARSE_ITEM_NUMBERW
#	define PARSE_ITEM_CSTRING	PARSE_ITEM_CSTRINGW
#	define PARSE_ITEM_STDSTR	PARSE_ITEM_STDSTRW
#	define PARSE_XML_INTOELEM	PARSE_XML_INTOELEMW
#	define PARSE_XML_OUTOFELEM	PARSE_XML_OUTOFELEMW
#	define PARSE_XML_FINDELEM	PARSE_XML_FINDELEMW
	//-
#else
#	define PARSE_XML_BEGINA \
	std::string sRet = "";\
	CMarkup xml;\
	std::string sParseXml = szXml;\
	bool bHasKey = false;\
	if (xml.SetDoc(sParseXml))\
	{\
		if(xml.FindElem(TEXT("r")) )\
		{\
			xml.IntoElem();\
			std::string sParseKey;

#	define PARSE_XML_INTOELEMA \
	xml.IntoElem();

#	define PARSE_XML_FINDELEMA(xmlNODE) \
	xml.FindElem(xmlNODE)

#	define PARSE_XML_OUTOFELEMA \
	xml.OutOfElem();

	//-parse
#	define PARSE_ITEM_NUMBERA(item,xmlkey) \
	sParseKey = xmlkey;\
	bHasKey = xml.FindElem(sParseKey);\
	if(bHasKey)\
	{\
		sRet = T2AString(xml.GetAttrib(TEXT("v")).c_str());\
		item = (DECLTYPE(item))atof(sRet.c_str());\
	}

#	define PARSE_ITEM_CSTRINGA(item,xmlkey,itemsize) \
	sParseKey = xmlkey;\
	bHasKey = xml.FindElem(sParseKey);\
	if(bHasKey)\
	{\
		sRet = T2AString(xml.GetAttrib(TEXT("v")).c_str());\
		strncpy(item,sRet.c_str(),itemsize);\
	}

#	define PARSE_ITEM_STDSTRA(item,xmlkey) \
	sParseKey = xmlkey;\
	bHasKey = xml.FindElem(sParseKey);\
	if(bHasKey)\
	{\
		sRet = T2AString(xml.GetAttrib(TEXT("v")).c_str());\
		item = sRet;\
	} 

#	define XML_LOOP_VERIFY bHasKey
	//-parse end

#	define PARSE_XML_BEGIN			PARSE_XML_BEGINA
#	define PARSE_ITEM_NUMBER		PARSE_ITEM_NUMBERA
#	define PARSE_ITEM_CSTRING		PARSE_ITEM_CSTRINGA
#	define PARSE_ITEM_STDSTR		PARSE_ITEM_STDSTRA
#	define PARSE_XML_INTOELEM		PARSE_XML_INTOELEMA
#   define PARSE_XML_OUTOFELEM		PARSE_XML_OUTOFELEMA
#	define PARSE_XML_FINDELEM		PARSE_XML_FINDELEMA
#endif

#define PARSE_XML_END } }

	private:

	};
	
//-------------------------------------------------------------------------------
class PKGCommon_Q : public NetPackBase
{
public:

protected:
	virtual std::string MakeBodyData();
	virtual void ParseBodyData(std::string szXml);
private:
};

class PKGCommon_A : public NetPackBase
{
public:
	PKGCommon_A(NetPackBase* pDependObj = NULL);
	BOOL bStatus;
	virtual std::string MakeBodyData();
	virtual void ParseBodyData(std::string szXml);
};

#endif //__NetPackManager_H

//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//class NetPackManager : public SingletonBase<NetPackManager>
//{
//public:
//	NetPackManager();
//	~NetPackManager();
//
//	void SetServerInfo(const char * szSrvIp, unsigned short usTCPPort,unsigned short usUDPPort=0);
//public:
//	void ReportToKernelServer(DWORD dwCmd,NetPackBase * const cmdXmlData);
//	void ReportToKernelServer(DWORD dwCmd,string xml);
//protected:
//	char szServerIp[64];
//	unsigned short usTCPPort;
//	unsigned short usUDPPort;
//private:
//};



//-------------------------------------------------------------------------------
//从以下开始是需要自定义自己的上报包体类，必须实现MakeKillSelfData方法，
//调用USES_XML，ADD_XMLITEM，GET_XML,来实现构造包体
//宏使用案例:
//string XXX::MakeKillSelfData()
//{
//	USES_XML(ostringstreamIn,stringRet);
//
//	ADD_XMLITEM(ostringstreamIn,stringRet,m_cMacAddr);
//	ADD_XMLITEM(ostringstreamIn,stringRet,m_dwLocalIP);
//	ADD_XMLITEM(ostringstreamIn,stringRet,GetLocalStrIP());
//	ADD_XMLITEM(ostringstreamIn,stringRet,m_cHostName);
//	ADD_XMLITEM(ostringstreamIn,stringRet,m_dwKeyMacLocalIP);
//	ADD_XMLITEM(ostringstreamIn,stringRet,m_dwKeyMacHostName);
//	
//	return GET_XML;
//}
//-------------------------------------------------------------------------------