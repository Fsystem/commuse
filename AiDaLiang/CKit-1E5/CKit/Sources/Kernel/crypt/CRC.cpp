// CRC.cpp: implementation of the CCRC class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CRC.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static ULONG g_crc32_table[256];
static bool g_crc_init = false;
CCRC::CCRC()
{
	crc=0xffffffff;
	if(!g_crc_init)
	{
		InitCrc();
	}
}

CCRC::~CCRC()
{

}

void CCRC::InitCrc()
{
	if(!g_crc_init)
	{
		g_crc_init = true;
		Init_CRC32_Table(); 
	}

}


void CCRC::Init_CRC32_Table()
{
	ULONG ulPolynomial = 0x04c11db7;
	
	// 256 values representing ASCII character codes.
	for(int i = 0; i <= 0xFF; i++)
	{
		g_crc32_table[i]=Reflect(i, 8) << 24;
		for (int j = 0; j < 8; j++)
			g_crc32_table[i] = (g_crc32_table[i] << 1) ^ (g_crc32_table[i] & (1 << 31) ? ulPolynomial : 0);
		g_crc32_table[i] = Reflect(g_crc32_table[i], 32);
	}
}

DWORD CCRC::Reflect(ULONG ref, char ch)
{
	ULONG value(0);
	
	// Swap bit 0 for bit 7 , bit 1 for bit 6, etc.
	for(int i = 1; i < (ch + 1); i++)
	{
		if(ref & 1)
			value |= 1 << (ch - i);
		ref >>= 1;
	}
	return value;
}

bool CCRC::Get_CRC(unsigned char *csData, DWORD dwSize)
{

	int len;
	unsigned char* buffer;

	len = dwSize;
	// Save the text in the buffer.
	buffer = (unsigned char*)(LPCTSTR)csData;
	// Perform the algorithm on each character
	// in the string, using the lookup table values.
	while(len--)
		crc = (crc >> 8) ^ g_crc32_table[(crc & 0xFF) ^ *buffer++];
	// Exclusive OR the result with the beginning value.
	return TRUE;
}

DWORD CCRC::GetCrC()
{
	return crc^0xffffffff;
}



DWORD CCRC::GetCrcNumber(const char *buf, DWORD len)
{
	Get_CRC((unsigned char *)buf,len);
	ULONG dwCrc=GetCrC();
	crc=0xffffffff;
	return dwCrc;
}

std::string CCRC::GetCrcString(const char *buf,DWORD len)
{
	DWORD dwCrc = GetCrcNumber(buf,len);
	char szCrc[16];
	itoa(dwCrc,szCrc,10);
	strlwr(szCrc);
	return szCrc;
}

std::string CCRC::GetCrcHexString(const char *buf,DWORD len)
{
	DWORD dwCrc = GetCrcNumber(buf,len);
	char szCrc[16];
	itoa(dwCrc,szCrc,16);
	strlwr(szCrc);
	return szCrc;
}

std::string CCRC::GetFileCRC(std::string filename,LARGE_INTEGER * m_file)
{
	HANDLE hand;///句柄
	hand=CreateFileA(filename.c_str(),GENERIC_READ,///打开方式，可读
		FILE_SHARE_READ,//共享读
		NULL,OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	//没有打开
	if (hand==INVALID_HANDLE_VALUE)
	{

		return "";
	}
	LARGE_INTEGER m_file_len;
	BY_HANDLE_FILE_INFORMATION m_file_info;
	::GetFileInformationByHandle(hand,&m_file_info);
	m_file_len.LowPart=m_file_info.nFileSizeLow;
	m_file_len.HighPart=m_file_info.nFileSizeHigh;
	if (m_file!=NULL)
	{
		m_file->QuadPart=m_file_len.QuadPart;
	}	
	ULONG re;
	char buf[1024*8];
	while(m_file_len.QuadPart)
	{
		memset(buf,0,1024*8);
		if(!ReadFile(hand,buf,1024*8,&re,0))
		{
			///读取文件失败
			return "";
		}
		m_file_len.QuadPart=m_file_len.QuadPart-re;
		Get_CRC((unsigned char *)buf,re);
	}
	CloseHandle(hand);
	ULONG m_crc=GetCrC();
	char ch[16];
	itoa(m_crc, ch, 16);
	std::string m_csCRC32= ch;
	std::transform(m_csCRC32.begin(),m_csCRC32.end(),m_csCRC32.begin(),::toupper);
	//m_csCRC32.MakeUpper();
	crc=0xffffffff;
	return m_csCRC32;
}