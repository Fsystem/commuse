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



DWORD CCRC::GetCrcNumber(char *buf, DWORD len)
{
	Get_CRC((unsigned char *)buf,len);
	ULONG dwCrc=GetCrC();
	crc=0xffffffff;
	return dwCrc;
}

std::string CCRC::GetCrcString(char *buf,DWORD len)
{
	DWORD dwCrc = GetCrcNumber(buf,len);
	char szCrc[16];
	itoa(dwCrc,szCrc,10);
	strlwr(szCrc);
	return szCrc;
}

std::string CCRC::GetCrcHexString(char *buf,DWORD len)
{
	DWORD dwCrc = GetCrcNumber(buf,len);
	char szCrc[16];
	itoa(dwCrc,szCrc,16);
	strlwr(szCrc);
	return szCrc;
}