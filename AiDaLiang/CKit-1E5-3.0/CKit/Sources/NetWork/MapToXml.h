#pragma once

typedef	std::map<std::string,std::string>		KEY_VALUE_MAP;

class CMapToXml
{
public:
	CMapToXml(void);
	~CMapToXml(void);
public:
	static	std::string	WINAPI	MapToXml(KEY_VALUE_MAP &_value_map);
	static	BOOL	WINAPI	XmlToMap(std::string xml,KEY_VALUE_MAP &_value_map);
};
