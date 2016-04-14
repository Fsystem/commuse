// CRC.h: interface for the CCRC class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CRC_H__198948D2_E075_47FE_864C_E5A1051B864B__INCLUDED_)
#define AFX_CRC_H__198948D2_E075_47FE_864C_E5A1051B864B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifdef CRC_LIB

#define	CRC_VC_BUILD	__declspec(dllexport)

#else

#define CRC_VC_BUILD	__declspec(dllimport)

#endif


class  CCRC  
{
public:
	std::string GetStrCrc(char *buf,DWORD len);
	std::string GetResCRC(LPCTSTR restype, int resid);//计算出当前应用程序资源的CRC值
	std::string GetFileCRC(std::string filename,LARGE_INTEGER  * m_file);//计算出一个文件的CRC值
	ULONG GetStrCrc_Key(char *buf, DWORD len);
	CCRC();
	virtual ~CCRC();
private:
	ULONG GetCrC();
	bool Get_CRC(unsigned char *csData, DWORD dwSize);
	ULONG Reflect(ULONG ref, char ch);
	
	void Init_CRC32_Table();
	ULONG crc;
	

};

#endif // !defined(AFX_CRC_H__198948D2_E075_47FE_864C_E5A1051B864B__INCLUDED_)
