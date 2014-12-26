#ifndef MYSTRING_INC
#define MYSTRING_INC
#include <string.h>

class CMyString
{
public:
	char *m_pString;

	CMyString(char*p)
	{
		m_pString = p;
	}
	bool operator<(const CMyString& str)const
	{
		return strcmp(m_pString,str.m_pString)<0;
	}
};

#endif