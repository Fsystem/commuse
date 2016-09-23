// ToolOper.h: interface for the CToolOper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOOLOPER_H__6B192A69_20B7_492D_8BDC_3C3D94122A6F__INCLUDED_)
#define AFX_TOOLOPER_H__6B192A69_20B7_492D_8BDC_3C3D94122A6F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector> 
#include <map>

typedef	std::map<DWORD,std::string>		SYSTEM_PROCESS_MAP;

typedef	std::vector<std::string>		RET_STRING_VECTOR;

class CToolOper  
{
public:
	CToolOper();
	virtual ~CToolOper();

	//共享内存操作
public:
	DWORD CreateMapFile(const char * map_name,const char *file_buffer);
	std::string	ReadMapFile(const char* map_name);
	DWORD WriteMapFile(const char *map_name,const char *file_buffer);

	HANDLE CreateMemFile(const char * map_name,size_t len);
	BOOL ReadMemData(const char * map_name,PVOID buffer,size_t len);
	BOOL WriteMemData(const char * map_name,PVOID buffer,size_t len);

		//注册表操作
public:
	void SetRegData(const char * name_str,const char * value_str,const char * pszKey="SOFTWARE\\hidead");
	BOOL GetRegData(const char * name_str,char *value_str,int &len,const char * pszKey="SOFTWARE\\hidead");
	DWORD RegGetDWORD(HKEY hKeyRoot, LPCSTR lpSubKey,LPCSTR lpValueName);
	DWORD RegSetString(HKEY hKeyRoot, LPCSTR lpSubKey,LPCSTR lpValueName, std::string dwValue);

public:
	BOOL	ReadConfigData(char * agent_id,int len,const char * pszMapName="run_config_data");


	std::string	GetTempPathFile(std::string	filename);
	std::string GetExePathFile(HMODULE hmodule,std::string filename);
	std::string	GetTempName(std::string rand_str,std::string ext_name = ".dat");

	const char* GetAppPath();

	int  GetMac(char * m_mac,char * m_ip);
	BOOL UnZipFile(PCHAR dir_name,PCHAR m_zip_name);

	BOOL FindLocalIp(const char * ip_str);

	bool GetProcessList(SYSTEM_PROCESS_MAP &process_list,std::string	filter_name);

	std::string SubstStr(const char * buffer,const char * start_str,const char *end_str);
	int SubstStr(const char * buffer,const char * start_str,const char *end_str,RET_STRING_VECTOR &_str_list);
	std::string ReplaceStr(std::string s,std::string sOld,std::string sNew);

public:
	BOOL	AddDll(std::string path, DWORD pid);

	int		StringSubs(std::string	src_str,std::string begin_str,std::string end_str,RET_STRING_VECTOR &str_list);

	PVOID ExtractMem(LPCTSTR restype, int resid,HMODULE hModule);
	BOOL ExtractFile(LPCSTR restype, int resid, LPCSTR destpath,HMODULE hModule);
	char * ReMoveChar(char * src_data,char ch);
	std::string	GetSystemPathFile(std::string	filename);

	int isInstr(const char * buffer,int len, char ch);

	int	CmdLineToArgvA(const char * cmd_line,std::vector<std::string> &_list);
	int	CmdLineToArgvW(const wchar_t * cmd_line,std::vector<std::wstring> &_list);

	void DeleteSelfApp();

public:
	DWORD SendRecvData(const char * sever_ip,short port,PVOID send_data,int send_len,PVOID recv_data,int recv_len);

public:
	std::string GetSystemTime();
};

#endif // !defined(AFX_TOOLOPER_H__6B192A69_20B7_492D_8BDC_3C3D94122A6F__INCLUDED_)
