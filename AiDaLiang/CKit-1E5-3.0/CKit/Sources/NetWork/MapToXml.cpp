#include "StdAfx.h"
#include "MapToXml.h"

CMapToXml::CMapToXml(void)
{
}

CMapToXml::~CMapToXml(void)
{
}

std::string	WINAPI CMapToXml::MapToXml( KEY_VALUE_MAP &_value_map )
{
	CMarkup	xml;
 
	xml.SetDoc(TEXT("<?xml version=\"1.0\" encoding=\"gb2312\" ?>"));

	xml.AddElem(TEXT("r") );

	xml.IntoElem();

	for (KEY_VALUE_MAP::iterator iter = _value_map.begin(); iter != _value_map.end(); iter++)
	{
		xml.AddElem(TEXT("i") );
		xml.SetAttrib(TEXT("n"),A2TString(iter->first.c_str()) );
		xml.SetAttrib(TEXT("v"),A2TString(iter->second.c_str()));
	}
	std::string sRes = T2AString(xml.GetDoc().c_str());
	return sRes;

}

BOOL	WINAPI CMapToXml::XmlToMap( std::string xml,KEY_VALUE_MAP &_value_map )
{
	CMarkup	oper_xml;

	_value_map.clear();

	if (oper_xml.SetDoc(A2TString(xml.c_str()) ) == FALSE)
	{
		return FALSE;
	}

	if(oper_xml.FindElem(TEXT("r")) == FALSE)
	{
		return FALSE;
	}
	
	oper_xml.IntoElem();

	std::string		name,value_str;
	while(oper_xml.FindElem(TEXT("i")) )
	{
		name = T2AString(oper_xml.GetAttrib(TEXT("n") ).c_str());
		value_str = T2AString(oper_xml.GetAttrib(TEXT("v") ).c_str());

		_value_map[name] = value_str;
	}

	return TRUE;
}
