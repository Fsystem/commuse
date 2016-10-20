// CRC.h: interface for the CCRC class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CRC_H__198948D2_E075_47FE_864C_E5A1051B864B__INCLUDED_)
#define AFX_CRC_H__198948D2_E075_47FE_864C_E5A1051B864B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

class  CCRC  
{
	DWORD crc;
public:
	CCRC();
	virtual ~CCRC();

public:
	DWORD GetCrcNumber(const char *buf,DWORD len);
	std::string GetCrcString(const char *buf,DWORD len);
	std::string GetCrcHexString(const char *buf,DWORD len);
	std::string GetFileCRC(std::string filename,LARGE_INTEGER * m_file = NULL,DWORD dwbeing = 0,DWORD dwend = 0);
	DWORD GetFileCRCNumber(std::string filename,LARGE_INTEGER * m_file = NULL,DWORD dwbeing = 0,DWORD dwend = 0);
	//std::string GetStdFileCRC(std::string filename,LARGE_INTEGER * m_file = NULL,DWORD dwbeing = 0,DWORD dwend = 0);

private:
	DWORD GetCrC();
	bool Get_CRC(unsigned char *csData, DWORD dwSize);
public:
	static void InitCrc();
protected:
	static void Init_CRC32_Table();
	static DWORD Reflect(DWORD ref, char ch);

};

#endif // !defined(AFX_CRC_H__198948D2_E075_47FE_864C_E5A1051B864B__INCLUDED_)
