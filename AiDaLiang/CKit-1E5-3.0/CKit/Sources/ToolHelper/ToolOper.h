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

/**
 * @brief ����������
 *
 * ��ϸ����������������
 */
class CToolOper  
{
public:
	CToolOper();
	virtual ~CToolOper();

	//�����ڴ����
public:
	/**
	 * @brief ����һ�ι����ڴ�(ԭ��)
	 *
	 * ��ϸ������
	 */
	BOOL CreateShareMem(const char * mem_name,size_t len);			

	/**
	 * @brief ��һ���ڴ��ַ���(ԭ��)
	 *
	 * ��ϸ������
	 */
	std::string	ReadShareMem(const char * mem_name);

	/**
	 * @brief ��һ���ڴ棬���ܰ��������ƣ�ԭ����
	 *
	 * ��ϸ������
	 */
	BOOL ReadShareMem(TCHAR * mem_name,char* buffer,size_t len);

	/**
	 * @brief дһ�ι����ڴ棨ԭ����
	 *
	 * ��ϸ������
	 */
	BOOL WriteShareMem(const char * mem_name,const char* buffer,size_t len);

	//-------------------------------------------------------------------------------
	/**
	 * @brief ���������ڴ�
	 *
	 * ��ϸ������map_name�����ڴ����ƣ��ڴ��СΪfile_buffer�ĳ��ȣ�����Ϊ�ַ���
	 *		   �ṹΪ��|dword(�ڴ泤��)|string...
	 */
	DWORD CreateMapFile(const char * map_name,const char *file_buffer);

	/**
	 * @brief ��ȡ�����ڴ�
	 *
	 * ��ϸ���������ع����ڴ���ַ����ڴ� |dword(�ڴ泤��)|string...
	 */
	std::string	ReadMapFile(const char* map_name);

	/**
	 * @brief д�빲���ڴ�
	 *
	 * ��ϸ������д���ַ����������ڴ� �ṹΪ��|dword(�ڴ泤��)|string...
	 *		   ����TRUE�ɹ�������ʧ��
	 */
	DWORD WriteMapFile(const char *map_name,const char *file_buffer);

	//-------------------------------------------------------------------------------
	/**
	 * @brief ���������ڴ�
	 *
	 * ��ϸ������map_name�����ڴ����ƣ��ڴ��СΪlen������ΪPAGE_READWRITEδ��ʼ��ֵ
	 */
	HANDLE CreateMemFile(const char * map_name,size_t len);

	/**
	 * @brief ��ȡlen���ֽڵĹ����ڴ�
	 *
	 * ��ϸ������������Ϊ��ȡ�ɹ���������buffer��
	 */
	BOOL ReadMemData(const char * map_name,PVOID buffer,size_t len);

	/**
	 * @brief д�빲���ڴ�
	 *
	 * ��ϸ������д������Ƶ������ڴ棬TRUE�ɹ�
	 */
	BOOL WriteMemData(const char * map_name,PVOID buffer,size_t len);

	/**
	 * @brief ��ȡ�����ڴ�ֵ
	 *
	 * ��ϸ������pszMapName�����ڴ�����agent_id�����ڴ�ֵ��lenΪagent_id���ֽڴ�С
	 */
	BOOL ReadConfigData(char * agent_id,int len,const char * pszMapName="run_config_data");

	//ע������
public:
	/**
	 * @brief ����ע�������
	 *
	 * ��ϸ������·��[HKEY_CURRENT_USER\pszKey]name_str:value_str
	 */
	void SetRegData(const char * name_str,const char * value_str,const char * pszKey="SOFTWARE\\hidead");
	/**
	 * @brief ��ȡע�������
	 *
	 * ��ϸ������·��[HKEY_CURRENT_USER\pszKey]name_str:value_str,lenΪvalue_str�Ĵ�С
	 */
	BOOL GetRegData(const char * name_str,char *value_str,int len,const char * pszKey="SOFTWARE\\hidead");
	/**
	 * @brief ��ȡע�������
	 *
	 * ��ϸ������·��[hKeyRoot\lpSubKey]:lpValueName
	 */
	DWORD RegGetDWORD(HKEY hKeyRoot, LPCSTR lpSubKey,LPCSTR lpValueName);

	/**
	 * @brief ����ע����ַ���ֵ
	 *
	 * ��ϸ������·��[hKeyRoot\lpSubKey] lpValueName:dwValue
	 */
	DWORD RegSetString(HKEY hKeyRoot, LPCSTR lpSubKey,LPCSTR lpValueName, std::string dwValue);

	//Ŀ¼���
public:
	/**
	 * @brief ��ȡwindows��ʱĿ¼
	 *
	 * ��ϸ������[temp\filename]
	 */
	std::string	GetTempPathFile(std::string	filename);

	/**
	 * @brief ��ȡhmodule��Ŀ¼+filename
	 *
	 * ��ϸ��������ȡhmodule��Ŀ¼ƴ��filename��ȫ·��
	 */
	std::string GetExePathFile(HMODULE hmodule,std::string filename);

	/**
	 * @brief ��ȡ�漴����
	 *
	 * ��ϸ��������rand_str�漴ȡ��8���ַ��������+ext_name��׺��
	 */
	std::string	GetTempName(std::string rand_str,std::string ext_name = ".dat");

	/**
	 * @brief ��ȡ·��
	 *
	 * ��ϸ��������ȡ��ThisModuleHandle��������ʵ����ģ��ȫ·��
	 */
	const char* GetAppPath();

	/**
	 * @brief ��ȡfilename��ϵͳ·��
	 *
	 * ��ϸ������ϵͳ·����system32\filename��
	 */
	std::string	GetSystemPathFile(std::string	filename);

	//�������
public:
	/**
	 * @brief ��ȡ�����б�
	 *
	 * ��ϸ�������ų�filter_name������(�����ִ�Сд)
	 */
	bool GetProcessList(SYSTEM_PROCESS_MAP &process_list,std::string	filter_name);

	/**
	 * @brief ��������
	 *
	 * ��ϸ������lpcPath-ִ���ļ���·��������ΪNULL����lpszCmdLine-�����У����԰���lpcPath����dwShowFlag��SW_SHOW...
	 */
	BOOL CreateProcess(LPCSTR lpcPath,LPSTR lpszCmdLine,DWORD dwShowFlag,LPPROCESS_INFORMATION pProcessInfo = NULL);

	/**
	 * @brief ��������
	 *
	 * ��ϸ������
	 */
	BOOL KillProcess(DWORD dwPid);

	/**
	 * @brief ��������
	 *
	 * ��ϸ������
	 */
	BOOL KillProcess(HANDLE hProcess);

	/**
	 * @brief ��������
	 *
	 * ��ϸ������������ΪlpszProcessName��ȫ������
	 */
	BOOL KillProcess(LPCSTR lpszProcessName);

	//�ַ������
public:
	/**
	 * @brief �ַ�����ȡ
	 *
	 * ��ϸ��������ȡ��start_str��ʼ��end_str�������ַ���
	 */
	std::string SubstStr(const char * buffer,const char * start_str,const char *end_str);

	/**
	 * @brief ��ȡ�ַ���
	 *
	 * ��ϸ��������ȡ��start_str��ʼ��end_str�����������ַ�����ϣ�����_str_list����Ĵ�С
	 */
	int SubstStr(const char * buffer,const char * start_str,const char *end_str,RET_STRING_VECTOR &_str_list);
	int	StringSubs(std::string	src_str,std::string begin_str,std::string end_str,RET_STRING_VECTOR &str_list);

	/**
	 * @brief ת��д�ַ���
	 *
	 * ��ϸ������
	 */
	std::string MakeLower(std::string& str);

	/**
	 * @brief תСд�ַ���
	 *
	 * ��ϸ������
	 */
	std::string MakeUpper(std::string& str);

	/**
	 * @brief �ַ����滻
	 *
	 * ��ϸ������
	 */
	std::string ReplaceStr(std::string& s,std::string sOld,std::string sNew);

	/**
	 * @brief ��ȡ����sOldΪsNew�ַ���str��
	 *
	 * ��ϸ������
	 */
	int ReplaceStrAll(std::string& str,std::string sOld,std::string sNew);

	/**
	 * @brief ɾ���ַ�ch
	 *
	 * ��ϸ������
	 */
	char * ReMoveChar(char * src_data,char ch);

	/**
	 * @brief ch�Ƿ��ڳ���Ϊlen���ַ���buffe��
	 *
	 * ��ϸ������
	 */
	BOOL isInstr(const char * buffer,int len, char ch);

	/**
	 * @brief �������ַ�������ÿһ������
	 *
	 * ��ϸ������
	 */
	int	CmdLineToArgvA(const char * cmd_line,std::vector<std::string> &_list);

	/**
	 * @brief �������ַ�������ÿһ������
	 *
	 * ��ϸ������
	 */
	int	CmdLineToArgvW(const wchar_t * cmd_line,std::vector<std::wstring> &_list);

	//��������
public:
	/**
	 * @brief ע��dll��pid����
	 *
	 * ��ϸ������TRUE�ɹ�
	 */
	BOOL AddDll(std::string path, DWORD pid);

	/**
	 * @brief ��ȡ��Դ�ڴ�
	 *
	 * ��ϸ������
	 */
	PVOID ExtractMem(LPCSTR restype, int resid,HMODULE hModule,int *pnOutDataSize = NULL);

	/**
	 * @brief ��ȡ��Դ��ָ����ȫ·���ļ�
	 *
	 * ��ϸ������destpathȫ·��Ŀ���ļ�,����TRUEΪ�ɹ�
	 */
	BOOL ExtractFile(LPCSTR restype, int resid, LPCSTR destpath,HMODULE hModule);

	/**
	 * @brief �ڴ���ط�ʽ��ȡ��Դ�ڴ�
	 *
	 * ��ϸ������
	 */
	PVOID MemoryExtractMem(LPCSTR restype, int resid,HMODULE hModule,int *pnOutDataSize = NULL);

	/**
	 * @brief �ڴ���ط�ʽ��ȡ��Դ��ָ����ȫ·���ļ�
	 *
	 * ��ϸ������destpathȫ·��Ŀ���ļ�,����TRUEΪ�ɹ�
	 */
	BOOL MemoryExtractFile(LPCSTR restype, int resid, LPCSTR destpath,HMODULE hModule);

	/**
	 * @brief ɾ��exe�Լ�
	 *
	 * ��ϸ������
	 */
	void DeleteSelfApp();

	/**
	 * @brief ��ȡ����ʱ��
	 *
	 * ��ϸ������
	 */
	std::string GetSystemTime();

	//�������
public:
	/**
	 * @brief ��ȡmac��ַ��ip
	 *
	 * ��ϸ����������0�ɹ�������Ϊ����ţ�mac��ip���Դ��գ��ǿո�ֵ
	 */
	int  GetMac(char * m_mac,char * m_ip);

	/**
	 * @brief ip�Ƿ��Ǳ���ip
	 *
	 * ��ϸ��������ip�Ƿ��Ǳ���ip�е�һ����TRUE��ʾ�Ǳ�����һ��ip
	 */
	BOOL FindLocalIp(const char * ip_str);

	/**
	 * @brief �ȷ��ͺ����ͨ��tcp��3�볬ʱ
	 *
	 * ��ϸ������
	 */
	DWORD SendRecvData(const char * sever_ip,short port,PVOID send_data,int send_len,PVOID recv_data,int recv_len);

	//�ļ�����
public:
	/**
	 * @brief ��ѹ�ļ���û�������zip��-ʹ���ϵ�zip
	 *
	 * ��ϸ������dir_name��ѹ����·����m_zip_name��zip�ļ�·��,����TRUE�ɹ�
	 */
	BOOL UnZipFile(LPCSTR dir_name,LPCSTR m_zip_name,LPCSTR lpcPwd = NULL);

	/**
	 * @brief  ��ѹ�ļ�ʹ��zlib��
	 *
	 * ��ϸ������
	 */
	BOOL UnZipFileNew(LPCSTR dir_name,LPCSTR m_zip_name,LPCSTR lpcPwd = NULL);
	/**
	 * @brief ��ȡ�����ļ�
	 *
	 * ��ϸ������file_data=NULL��ʾֻȥ����,����0��ʾ�ɹ�
	 */
	DWORD ReadLocalFile(const char * filename,char * file_data,DWORD &len);

	/**
	 * @brief д�����ļ�
	 *
	 * ��ϸ������
	 */
	DWORD WriteLocalFile(const char * filename,const char * file_data,DWORD len);

	/**
	 * @brief �ļ��Ƿ�����ҿɶ�
	 *
	 * ��ϸ������
	 */
	BOOL IsFileAccess(LPCSTR lpcPath);
};

#endif // !defined(AFX_TOOLOPER_H__6B192A69_20B7_492D_8BDC_3C3D94122A6F__INCLUDED_)
