#ifndef __MyAutoPatch_H
#define __MyAutoPatch_H


#pragma once
#include <vector>


//////////////////////////////////////////////////////////////////////////
//枚举定义

//下载状态
enum enDownLoadStatus
{
	enDownLoadStatus_Unknow,			//未知状态
	enDownLoadStatus_Ready,				//准备状态
	enDownLoadStatus_DownLoadIng,		//下载状态
	enDownLoadStatus_Compressing,		//数据压缩	
	enDownLoadStatus_Finish,			//完成状态
	enDownLoadStatus_Fails,				//下载失败
};

//错误枚举
enum enDownLoadResult
{
	enDownLoadResult_Noknow,			//没有错误
	enDownLoadResult_Exception,			//异常错误
	enDownLoadResult_CreateFileFails,	//创建失败
	enDownLoadResult_InternetReadError,	//读取错误
};
//////////////////////////////////////////////////////////////////////////
#define NAME_LEN						32
//////////////////////////////////////////////////////////////////////////
//结构体
//下载状态
struct tagDownLoadStatus
{
	DWORD								wProgress;						//下载进度
	char								szStatus[128];					//状态描述
	enDownLoadStatus					DownLoadStatus;					//下载状态
};

struct tagDownStatusInfo
{
	char  szDownFile[MAX_PATH];			//下载的文件名
	DWORD dwTotal;						//文件总大小
	DWORD dwTransfered;					//已下载
};

//下载请求
struct tagAutoPatchReq
{
	bool							bDisplay;							//是否显示
	bool							bIsFtpMode;							//FTP模式 
	char							sTitleName[NAME_LEN];				//窗口标题
	char							lpcPatchFile[MAX_PATH];				//autopatch文件
	char							sUrl[MAX_PATH];						//下载URL
	char							sLocalListFile[MAX_PATH];			//本地列表
	char							sRemoteListFile[MAX_PATH];			//远程列表
	char							sDownloadListFile[MAX_PATH];		//需下载的列表
	char							sLocalTempFolder[MAX_PATH];			//本地存放目录
	char							sFtpUsr[NAME_LEN+1];				//FTP用户名
	char							sFtpPwd[NAME_LEN+1];				//FTP密码
};
typedef  struct ZipFile
{
	int       Index;
	CString   zipPath;
}_ZipFile;

//class CFileDesc;
//class CHttpFile;
//class CMapStrToIDMgr;
//class CFtpConnection;

#ifdef DLLEXPORT
class _declspec(dllexport) CMyAutoPatch
#else
class CMyAutoPatch
#endif
{
public:
	//这种必须保证有autopatch.ini文件
 	static CMyAutoPatch* CreateInstance(LPCSTR lpcPatchFile,LPCSTR sLocalTempFolder);
	/** 
	*   @FuncName : 
	*   @Author   : Double sword
	*   @Params   : bIsFtpMode是否是FTP模式
	*				sUrl资源远程URL
	*				sLocalListFile本地文件列表用于和远程文件列表对比
	*				sRemoteListFile远程文件列表下载和本地文件列表对比
	*				sLocalTempFolder本地列表文件缓存目录(注意：相对于exe路径fish---"fish\\")
	*				如果bIsFtpMode真那么bIsFtpMode和sFtpPwd生效
	*   @Return   :	
	*						
	*   @Date     :	2012-11-30
	*/
	static CMyAutoPatch* CreateInstance( 
		LPCSTR lpcPatchFile,
		bool    bIsFtpMode, 
		LPCSTR sUrl,
		LPCSTR sLocalListFile, 
		LPCSTR sRemoteListFile,
		LPCSTR sDownloadListFile,
		LPCSTR sLocalTempFolder,
		LPCSTR sFtpUsr="",
		LPCSTR sFtpPwd="");

	void Release();
public:
	//开始下载
	BOOL				BeginDown();
	//结束下载线程
	void				EndThread();
	//得到下载总大小
	DWORD				GetSizeTotal()	 const;
	//得到当前下载总大小
	DWORD				GetDownSize()	 const;
	//得到当前下载文件信息
	tagDownStatusInfo	GetCurDownFileInfo() const;
	//需要下载的文件总数
	int					GetNeedDownFileNum() const;
	//当前已经下载文件数
	int					GetDownedFileNum() const;
	//得到下载状态
	void				GetDownLoadStatus(tagDownLoadStatus & tagStatus);
	//下载文件列表(true-需要下载fasle-不需要下载)
	bool				DownLoadFileList();		
	//获取本地根目录
	CString		GetLocalRootDir();
	//获取本地下载缓冲目录
	CString		GetLocalTempFolderDir();


#ifdef DLLEXPORT
private:
	CMyAutoPatch(LPCSTR lpcPatchFile,LPCSTR sLocalTempFolder);//这种必须保证有autopatch.ini文件

	/** 
	*   @FuncName : 
	*   @Author   : Double sword
	*   @Params   : bIsFtpMode是否是FTP模式
	*				sUrl资源远程URL
	*				sLocalListFile本地文件列表用于和远程文件列表对比
	*				sRemoteListFile远程文件列表下载和本地文件列表对比
	*				sLocalTempFolder本地列表文件缓存目录(注意：相对于exe路径fish---"fish\\")
	*				如果bIsFtpMode真那么bIsFtpMode和sFtpPwd生效
	*   @Return   :	
	*						
	*   @Date     :	2012-11-30
	*/
	CMyAutoPatch( 
		LPCSTR lpcPatchFile,
		bool    bIsFtpMode, 
		LPCSTR sUrl,
		LPCSTR sLocalListFile, 
		LPCSTR sRemoteListFile,
		LPCSTR sDownloadListFile,
		LPCSTR sLocalTempFolder,
		LPCSTR sFtpUsr,
		LPCSTR sFtpPwd);
	~CMyAutoPatch();

	//输出下载的临时目录
	void		DelDir(CString strPath);
	//重新生成本地文件
	BOOL		ReBuildLocal();
	//下载初始化
	BOOL		Init();
	//加载配置文件
	BOOL		LoadCfg();	
	//得到程序目录	
	BOOL		GetDirectory();	
	//打印下载信息
	void		Printf( const char* pszString, ... );
	//下载函数
	void		Download();	
	//下载文件
	BOOL		GetRemoteFile( CString& strURL, CString& strLocalFile, void* pParam );
	//下载文件列表
	BOOL		GetRemoteListFile( );
	//分析文件列表
	BOOL		AnalyseListFile( const char* pszFilename, std::vector<CFileDesc>& vectorFiles );
	//启动下载线程
	static void StartDownLoadThread(LPVOID p);

private:
	string		m_strExeFilename;				//exe文件名
	string		m_sPatchFile;					//自动更新的配置文件
	BOOL		m_bIsClone;						//
	BOOL		m_bFtpDownload;					//区分ftp，http下载，
	CString		m_strURLRoot;					//"http://192.168.1.30/update/"
	CString		m_strFtpSite;
	CString		m_strFtpUsername;
	CString		m_strFtpPassword;
	CString		m_strLocalRoot;					// = _T("E://ftpdownload/");
	CString		m_strRemoteListFile;
	CString		m_strDownloadListFile;
	CString		m_strLocalListFile;
	CString     m_sLocalTempFolder;				//本地文件缓存目录
	

	CString		m_strGenExt;
	CString		m_strLaunchExe;					//存放目录
	BOOL		m_bClone;
	BOOL		m_bNeedClone;
	//-------------------------------------------------------------------
	BOOL		m_bDownComplete;
	CString		m_strBuffer;
	CHttpFile*	m_pFile;
	LPSTR		m_pBuf;
	DWORD		m_dwCount;
	std::vector<CFileDesc> m_RemoteFilesDesc;
	CMapStrToIDMgr		   m_LocalFilesMgr;
	std::vector<CFileDesc> m_BadFilesDesc;
	CFtpConnection*		   m_pFtpConnect;
	std::vector<CFileDesc> m_LocalFilesDesc;
	DWORD				   m_dwDownLoadTotalSize;		//下载的文件总大小
	CWebGrab			   m_grab;						//web类对象
	enDownLoadStatus	   m_DownLoadStatus;			//下载状态
	enDownLoadResult	   m_DownLoadResult;			//下载错误号
	HANDLE				   m_hUpdateThread;				//线程指针
	bool				   m_bIsEndThread;				//是否结束线程
	std::vector<_ZipFile>  m_nZipIndex;					//打开Zip的Index
	//-------------------------------------------------------------------
	int					   m_nDownedFileNum;			//已下载的文件数
	char				   m_sCurDownFile[MAX_PATH];	//当前正在下载的文件
	typedef map<string,tagDownStatusInfo> MAP_DOWNSTATUSINFO;
	MAP_DOWNSTATUSINFO	   m_DownStatusInfoMap;
#endif
};


#endif //__MyAutoPatch_H