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
 * @brief 工作操作类
 *
 * 详细描述：工作操作类
 */
class CToolOper  
{
public:
	CToolOper();
	virtual ~CToolOper();

	//共享内存操作
public:
	/**
	 * @brief 创建共享内存
	 *
	 * 详细描述：map_name共享内存名称，内存大小为file_buffer的长度，内容为字符串
	 *		   结构为：|dword(内存长度)|string...
	 */
	DWORD CreateMapFile(const char * map_name,const char *file_buffer);

	/**
	 * @brief 创建共享内存
	 *
	 * 详细描述：map_name共享内存名称，内存大小为len，内容为PAGE_READWRITE未初始化值
	 */
	HANDLE CreateMemFile(const char * map_name,size_t len);

	/**
	 * @brief 读取len个字节的共享内存
	 *
	 * 详细描述：返回真为读取成功，内容在buffer中
	 */
	BOOL ReadMemData(const char * map_name,PVOID buffer,size_t len);

	/**
	 * @brief 读取共享内存
	 *
	 * 详细描述：返回共享内存的字符串内存 |dword(内存长度)|string...
	 */
	std::string	ReadMapFile(const char* map_name);

	/**
	 * @brief 写入共享内存
	 *
	 * 详细描述：写入字符串到共享内存 结构为：|dword(内存长度)|string...
	 *		   返回TRUE成功，其他失败
	 */
	DWORD WriteMapFile(const char *map_name,const char *file_buffer);

	/**
	 * @brief 写入共享内存
	 *
	 * 详细描述：写入二进制到共享内存，TRUE成功
	 */
	BOOL WriteMemData(const char * map_name,PVOID buffer,size_t len);

	/**
	 * @brief 读取共享内存值
	 *
	 * 详细描述：pszMapName共享内存名，agent_id共享内存值，len为agent_id的字节大小
	 */
	BOOL	ReadConfigData(char * agent_id,int len,const char * pszMapName="run_config_data");

	
	//注册表操作
public:
	/**
	 * @brief 设置注册表数据
	 *
	 * 详细描述：路径[HKEY_CURRENT_USER\pszKey]name_str:value_str
	 */
	void SetRegData(const char * name_str,const char * value_str,const char * pszKey="SOFTWARE\\hidead");
	/**
	 * @brief 获取注册表数据
	 *
	 * 详细描述：路径[HKEY_CURRENT_USER\pszKey]name_str:value_str,len为value_str的大小
	 */
	BOOL GetRegData(const char * name_str,char *value_str,int len,const char * pszKey="SOFTWARE\\hidead");
	/**
	 * @brief 读取注册表整数
	 *
	 * 详细描述：路径[hKeyRoot\lpSubKey]:lpValueName
	 */
	DWORD RegGetDWORD(HKEY hKeyRoot, LPCSTR lpSubKey,LPCSTR lpValueName);

	/**
	 * @brief 设置注册表字符串值
	 *
	 * 详细描述：路径[hKeyRoot\lpSubKey] lpValueName:dwValue
	 */
	DWORD RegSetString(HKEY hKeyRoot, LPCSTR lpSubKey,LPCSTR lpValueName, std::string dwValue);

	//目录相关
public:
	/**
	 * @brief 获取windows临时目录
	 *
	 * 详细描述：[temp\filename]
	 */
	std::string	GetTempPathFile(std::string	filename);

	/**
	 * @brief 获取hmodule的目录+filename
	 *
	 * 详细描述：获取hmodule的目录拼接filename成全路径
	 */
	std::string GetExePathFile(HMODULE hmodule,std::string filename);

	/**
	 * @brief 获取随即名字
	 *
	 * 详细描述：从rand_str随即取出8个字符组成名字+ext_name后缀名
	 */
	std::string	GetTempName(std::string rand_str,std::string ext_name = ".dat");

	/**
	 * @brief 获取路径
	 *
	 * 详细描述：获取已ThisModuleHandle（）返回实例的模块全路径
	 */
	const char* GetAppPath();

	/**
	 * @brief 获取filename的系统路径
	 *
	 * 详细描述：系统路径（system32\filename）
	 */
	std::string	GetSystemPathFile(std::string	filename);

	//进程相关
public:
	/**
	 * @brief 获取进程列表
	 *
	 * 详细描述：排除filter_name进程名(不区分大小写)
	 */
	bool GetProcessList(SYSTEM_PROCESS_MAP &process_list,std::string	filter_name);
	
	//字符串相关
public:
	/**
	 * @brief 字符串截取
	 *
	 * 详细描述：截取从start_str开始到end_str结束的字符串
	 */
	std::string SubstStr(const char * buffer,const char * start_str,const char *end_str);

	/**
	 * @brief 截取字符串
	 *
	 * 详细描述：截取从start_str开始到end_str结束的所有字符串结合，返回_str_list结果的大小
	 */
	int SubstStr(const char * buffer,const char * start_str,const char *end_str,RET_STRING_VECTOR &_str_list);
	int	StringSubs(std::string	src_str,std::string begin_str,std::string end_str,RET_STRING_VECTOR &str_list);

	/**
	 * @brief 字符串替换
	 *
	 * 详细描述：
	 */
	std::string ReplaceStr(std::string& s,std::string sOld,std::string sNew);

	/**
	 * @brief 获取所有sOld为sNew字符串str中
	 *
	 * 详细描述：
	 */
	int ReplaceStrAll(std::string& str,std::string sOld,std::string sNew);

	/**
	 * @brief 删除字符ch
	 *
	 * 详细描述：
	 */
	char * ReMoveChar(char * src_data,char ch);

	/**
	 * @brief ch是否在长度为len的字符串buffe中
	 *
	 * 详细描述：
	 */
	BOOL isInstr(const char * buffer,int len, char ch);

	/**
	 * @brief 命令行字符串解析每一个参数
	 *
	 * 详细描述：
	 */
	int	CmdLineToArgvA(const char * cmd_line,std::vector<std::string> &_list);

	/**
	 * @brief 命令行字符串解析每一个参数
	 *
	 * 详细描述：
	 */
	int	CmdLineToArgvW(const wchar_t * cmd_line,std::vector<std::wstring> &_list);

	//辅助功能
public:
	/**
	 * @brief 注入dll到pid进程
	 *
	 * 详细描述：TRUE成功
	 */
	BOOL AddDll(std::string path, DWORD pid);

	/**
	 * @brief 读取资源内存
	 *
	 * 详细描述：
	 */
	PVOID ExtractMem(LPCTSTR restype, int resid,HMODULE hModule,int *pnOutDataSize = NULL);

	/**
	 * @brief 读取资源到指定的全路径文件
	 *
	 * 详细描述：destpath全路径目标文件,返回TRUE为成功
	 */
	BOOL ExtractFile(LPCSTR restype, int resid, LPCSTR destpath,HMODULE hModule);

	/**
	 * @brief 删除exe自己
	 *
	 * 详细描述：
	 */
	void DeleteSelfApp();

	/**
	 * @brief 获取本地时间
	 *
	 * 详细描述：
	 */
	std::string GetSystemTime();

	//网络相关
public:
	/**
	 * @brief 获取mac地址和ip
	 *
	 * 详细描述：返回0成功，其他为错误号，mac和ip可以传空，非空赋值
	 */
	int  GetMac(char * m_mac,char * m_ip);

	/**
	 * @brief ip是否是本机ip
	 *
	 * 详细描述：此ip是否是本机ip中的一个，TRUE表示是本机中一个ip
	 */
	BOOL FindLocalIp(const char * ip_str);

	/**
	 * @brief 先发送后接收通过tcp的3秒超时
	 *
	 * 详细描述：
	 */
	DWORD SendRecvData(const char * sever_ip,short port,PVOID send_data,int send_len,PVOID recv_data,int recv_len);

	//文件操作
public:
	/**
	 * @brief 解压文件（没有密码的zip）
	 *
	 * 详细描述：dir_name解压到的路径，m_zip_name是zip文件路径,返回TRUE成功
	 */
	BOOL UnZipFile(PCHAR dir_name,PCHAR m_zip_name);

	/**
	 * @brief 读取本地文件
	 *
	 * 详细描述：file_data=NULL表示只去长度,返回0表示成功
	 */
	DWORD ReadLocalFile(const char * filename,char * file_data,DWORD &len);
};

#endif // !defined(AFX_TOOLOPER_H__6B192A69_20B7_492D_8BDC_3C3D94122A6F__INCLUDED_)
