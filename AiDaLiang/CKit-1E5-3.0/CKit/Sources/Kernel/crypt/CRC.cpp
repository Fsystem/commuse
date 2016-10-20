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

//-------------------------------------------------------------------------------
// std crc
//-------------------------------------------------------------------------------
//const unsigned int CrcSeed = 0xFFFFFFFF;
static ULONG g_crc32_table[256];
static bool g_crc_init = false;
//static unsigned int CrcTable[256]={-1};
////static const unsigned int CrcTable[] = {
////	0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419,
////	0x706AF48F, 0xE963A535, 0x9E6495A3, 0x0EDB8832, 0x79DCB8A4,
////	0xE0D5E91E, 0x97D2D988, 0x09B64C2B, 0x7EB17CBD, 0xE7B82D07,
////	0x90BF1D91, 0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE,
////	0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7, 0x136C9856,
////	0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9,
////	0xFA0F3D63, 0x8D080DF5, 0x3B6E20C8, 0x4C69105E, 0xD56041E4,
////	0xA2677172, 0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
////	0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940, 0x32D86CE3,
////	0x45DF5C75, 0xDCD60DCF, 0xABD13D59, 0x26D930AC, 0x51DE003A,
////	0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423, 0xCFBA9599,
////	0xB8BDA50F, 0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
////	0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D, 0x76DC4190,
////	0x01DB7106, 0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F,
////	0x9FBFE4A5, 0xE8B8D433, 0x7807C9A2, 0x0F00F934, 0x9609A88E,
////	0xE10E9818, 0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
////	0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E, 0x6C0695ED,
////	0x1B01A57B, 0x8208F4C1, 0xF50FC457, 0x65B0D9C6, 0x12B7E950,
////	0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3,
////	0xFBD44C65, 0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2,
////	0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB, 0x4369E96A,
////	0x346ED9FC, 0xAD678846, 0xDA60B8D0, 0x44042D73, 0x33031DE5,
////	0xAA0A4C5F, 0xDD0D7CC9, 0x5005713C, 0x270241AA, 0xBE0B1010,
////	0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
////	0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17,
////	0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD, 0xEDB88320, 0x9ABFB3B6,
////	0x03B6E20C, 0x74B1D29A, 0xEAD54739, 0x9DD277AF, 0x04DB2615,
////	0x73DC1683, 0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8,
////	0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1, 0xF00F9344, 
////	0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB,
////	0x196C3671, 0x6E6B06E7, 0xFED41B76, 0x89D32BE0, 0x10DA7A5A,
////	0x67DD4ACC, 0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
////	0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1,
////	0xA6BC5767, 0x3FB506DD, 0x48B2364B, 0xD80D2BDA, 0xAF0A1B4C,
////	0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55, 0x316E8EEF,
////	0x4669BE79, 0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
////	0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F, 0xC5BA3BBE,
////	0xB2BD0B28, 0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31,
////	0x2CD99E8B, 0x5BDEAE1D, 0x9B64C2B0, 0xEC63F226, 0x756AA39C,
////	0x026D930A, 0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
////	0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38, 0x92D28E9B,
////	0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21, 0x86D3D2D4, 0xF1D4E242,
////	0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1,
////	0x18B74777, 0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C,
////	0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45, 0xA00AE278,
////	0xD70DD2EE, 0x4E048354, 0x3903B3C2, 0xA7672661, 0xD06016F7,
////	0x4969474D, 0x3E6E77DB, 0xAED16A4A, 0xD9D65ADC, 0x40DF0B66,
////	0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
////	0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605,
////	0xCDD70693, 0x54DE5729, 0x23D967BF, 0xB3667A2E, 0xC4614AB8,
////	0x5D681B02, 0x2A6F2B94, 0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 
////	0x2D02EF8D};
//
//class Crc32
//{
//private:
//	const unsigned int CrcSeed ;
//
//	unsigned int crc ; // crc data checksum so far.
//
//
//public:
//	Crc32():CrcSeed(0xFFFFFFFF)
//	{
//		crc = 0xFFFFFFFF;
//		if (!g_crc_init)
//		{
//			Init_CRC32_Table();
//			g_crc_init = true;
//		}
//
//		if(CrcTable[0] == (unsigned int)(-1)) memcpy(CrcTable,g_crc32_table,sizeof CrcTable);
//	}
//
//	unsigned int Reflect(unsigned int ref, char ch)
//	{
//		unsigned int value(0);
//
//		// Swap bit 0 for bit 7 , bit 1 for bit 6, etc.
//		for(int i = 1; i < (ch + 1); i++)
//		{
//			if(ref & 1)
//				value |= 1 << (ch - i);
//			ref >>= 1;
//		}
//		return value;
//	}
//
//	void Init_CRC32_Table()
//	{
//		unsigned int ulPolynomial = 0x04c11db7;
//
//		// 256 values representing ASCII character codes.
//		for(int i = 0; i <= 0xFF; i++)
//		{
//			g_crc32_table[i]=Reflect(i, 8) << 24;
//			for (int j = 0; j < 8; j++)
//				g_crc32_table[i] = (g_crc32_table[i] << 1) ^ (g_crc32_table[i] & (1 << 31) ? ulPolynomial : 0);
//			g_crc32_table[i] = Reflect(g_crc32_table[i], 32);
//		}
//	}
//
//
//	unsigned int GetCrc32Value()
//	{
//		return crc ^ 0xFFFFFFFF;;
//	}
//
//
//	static unsigned int GetCRC32(const char* parr, int nBytes)
//	{
//		unsigned int unRes = 0;
//		if ( nBytes > 0 )
//		{
//			Crc32 crc32 ;
//			crc32.Update(parr,nBytes, 0, nBytes);
//			unRes = crc32.GetCrc32Value();
//		}
//		return unRes;
//	}
//
//	/// <summary>
//	/// Resets the CRC32 data checksum as if no update was ever called.
//	/// </summary>
//	void Reset()
//	{
//		crc = 0;
//	}
//
//	/// <summary>
//	/// Updates the checksum with the int bval.
//	/// </summary>
//	/// <param name = "bval">
//	/// the byte is taken as the lower 8 bits of bval
//	/// </param>
//	void Update(int bval)
//	{
//		crc ^= CrcSeed;
//		crc = CrcTable[(crc ^ bval) & 0xFF] ^ (crc >> 8);
//		crc ^= CrcSeed;
//	}
//
//	/// <summary>
//	/// Updates the checksum with the bytes taken from the array.
//	/// </summary>
//	/// <param name="buffer">
//	/// buffer an array of bytes
//	/// </param>
//	void Update(const char* pbuffer,int nBytes)
//	{
//		Update(pbuffer,nBytes, 0, nBytes);
//	}
//
//	/// <summary>
//	/// Adds the byte array to the data checksum.
//	/// </summary>
//	/// <param name = "buf">
//	/// the buffer which contains the data
//	/// </param>
//	/// <param name = "off">
//	/// the offset in the buffer where the data starts
//	/// </param>
//	/// <param name = "len">
//	/// the length of the data
//	/// </param>
//	void Update(const char* pbuffer,int nBytes, int off, int len)
//	{
//		if (pbuffer == 0)
//		{
//			return;
//		}
//
//		if (off < 0 || len < 0 || off + len > nBytes)
//		{
//			//throw ("len err");
//			return;
//		}
//
//		while (--len >= 0)
//		{
//			crc = CrcTable[(crc ^ pbuffer[off++]) & 0xFF] ^ (crc >> 8);
//		}
//	}
//
//};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


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

//std::string CCRC::GetFileCRC(std::string filename,LARGE_INTEGER * m_file)
//{
//	HANDLE hand;///���
//	hand=CreateFileA(filename.c_str(),GENERIC_READ,///�򿪷�ʽ���ɶ�
//		FILE_SHARE_READ,//�����
//		NULL,OPEN_EXISTING,
//		FILE_ATTRIBUTE_NORMAL,
//		NULL);
//	//û�д�
//	if (hand==INVALID_HANDLE_VALUE)
//	{
//		return "";
//	}
//	LARGE_INTEGER m_file_len;
//	BY_HANDLE_FILE_INFORMATION m_file_info;
//	::GetFileInformationByHandle(hand,&m_file_info);
//	m_file_len.LowPart=m_file_info.nFileSizeLow;
//	m_file_len.HighPart=m_file_info.nFileSizeHigh;
//	if (m_file!=NULL)
//	{
//		m_file->QuadPart=m_file_len.QuadPart;
//	}	
//	ULONG re;
//	char buf[1024*8];
//	while(m_file_len.QuadPart)
//	{
//		memset(buf,0,1024*8);
//		if(!ReadFile(hand,buf,1024*8,&re,0))
//		{
//			CloseHandle(hand);
//			///��ȡ�ļ�ʧ��
//			return "";
//		}
//		m_file_len.QuadPart=m_file_len.QuadPart-re;
//		Get_CRC((unsigned char *)buf,re);
//	}
//	CloseHandle(hand);
//	ULONG m_crc=GetCrC();
//	char ch[16];
//	itoa(m_crc, ch, 16);
//	_strupr_s(ch,15);
//	ch[15] = 0;
//	std::string m_csCRC32= ch;
//	//m_csCRC32.MakeUpper();
//	crc=0xffffffff;
//	return m_csCRC32;
//}

std::string CCRC::GetFileCRC(std::string filename,LARGE_INTEGER * m_file,DWORD	dwbeing,DWORD dwend)
{
	ULONG m_crc = GetFileCRCNumber(filename,m_file,dwbeing,dwend);
	CHAR ch[16];
	itoa(m_crc, ch, 16);

	_strupr_s(ch,15);
	ch[15] = 0;

	return ch;
}

DWORD CCRC::GetFileCRCNumber( std::string filename,LARGE_INTEGER * m_file /*= NULL*/,DWORD dwbeing /*= 0*/,DWORD dwend /*= 0*/ )
{
	HANDLE hand;///���
	hand=CreateFileA(filename.c_str(),GENERIC_READ,///�򿪷�ʽ���ɶ�
		FILE_SHARE_READ,//�����
		NULL,OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	//û�д�
	if (hand==INVALID_HANDLE_VALUE)
	{
		//CDebugLog g_log;
		//g_log.SaveLog(1,"c:\\start.log","���ļ�ʧ�ܣ��ļ�����%s",filename.c_str());
		return 0;
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

	if (dwbeing !=0 || dwend != 0)
	{
		if (dwend == 0)
		{
			dwend = (DWORD)m_file_len.QuadPart;
		}

		if (m_file_len.QuadPart <= dwend)
		{
			CloseHandle(hand);
			return 0;
		}

		m_file_len.QuadPart = dwend;	//ֻȡ����һ��

		SetFilePointer(hand,dwbeing,NULL,FILE_BEGIN);

		m_file_len.QuadPart = m_file_len.QuadPart - dwbeing;
	}

	ULONG re;
	char buf[1024*8];
	DWORD	try_read = 1024 * 8;
	while(m_file_len.QuadPart)
	{
		memset(buf,0,1024*8);
		try_read = min(m_file_len.QuadPart,1024*8);
		if(!ReadFile(hand,buf,try_read,&re,0))
		{
			CloseHandle(hand);
			///��ȡ�ļ�ʧ��
			return 0;
		}
		m_file_len.QuadPart = m_file_len.QuadPart - re;
		Get_CRC((unsigned char *)buf,re);
	}
	CloseHandle(hand);
	DWORD crcRes = GetCrC();
	crc=0xffffffff;
	
	return crcRes;
}

//std::string CCRC::GetStdFileCRC(std::string filename,LARGE_INTEGER * m_file,DWORD	dwbeing,DWORD dwend)
//{
//	HANDLE hand;///���
//	hand=CreateFileA(filename.c_str(),GENERIC_READ,///�򿪷�ʽ���ɶ�
//		FILE_SHARE_READ,//�����
//		NULL,OPEN_EXISTING,
//		FILE_ATTRIBUTE_NORMAL,
//		NULL);
//	//û�д�
//	if (hand==INVALID_HANDLE_VALUE)
//	{
//		//CDebugLog g_log;
//		//g_log.SaveLog(1,"c:\\start.log","���ļ�ʧ�ܣ��ļ�����%s",filename.c_str());
//		return "";
//	}
//
//	LARGE_INTEGER m_file_len;
//	BY_HANDLE_FILE_INFORMATION m_file_info;
//	::GetFileInformationByHandle(hand,&m_file_info);
//	m_file_len.LowPart=m_file_info.nFileSizeLow;
//	m_file_len.HighPart=m_file_info.nFileSizeHigh;
//
//	if (m_file!=NULL)
//	{
//		m_file->QuadPart=m_file_len.QuadPart;
//	}	
//
//	if (dwbeing !=0 || dwend != 0)
//	{
//		if (dwend == 0)
//		{
//			dwend = (DWORD)m_file_len.QuadPart;
//		}
//
//		if (m_file_len.QuadPart <= dwend)
//		{
//			CloseHandle(hand);
//			return "";
//		}
//
//		m_file_len.QuadPart = dwend;	//ֻȡ����һ��
//
//		SetFilePointer(hand,dwbeing,NULL,FILE_BEGIN);
//
//		m_file_len.QuadPart = m_file_len.QuadPart - dwbeing;
//	}
//
//	ULONG re;
//	char buf[1024*8];
//	DWORD	try_read = 1024 * 8;
//	Crc32 crcStd;
//	while(m_file_len.QuadPart)
//	{
//		memset(buf,0,1024*8);
//		try_read = min(m_file_len.QuadPart,1024*8);
//		if(!ReadFile(hand,buf,try_read,&re,0))
//		{
//			CloseHandle(hand);
//			///��ȡ�ļ�ʧ��
//			return "";
//		}
//		m_file_len.QuadPart = m_file_len.QuadPart - re;
//		crcStd.Update(buf,re,0,re);
//	}
//	CloseHandle(hand);
//	ULONG m_crc = crcStd.GetCrc32Value();
//	CHAR ch[16];
//	itoa(m_crc, ch, 16);
//
//	_strupr_s(ch,15);
//	ch[15] = 0;
//	std::string m_csCRC32 = ch;
//	crc=0xffffffff;
//	return m_csCRC32;
//}