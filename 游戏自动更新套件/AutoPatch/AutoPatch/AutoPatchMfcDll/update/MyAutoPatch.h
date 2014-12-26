#ifndef __MyAutoPatch_H
#define __MyAutoPatch_H


#pragma once
#include <vector>


//////////////////////////////////////////////////////////////////////////
//ö�ٶ���

//����״̬
enum enDownLoadStatus
{
	enDownLoadStatus_Unknow,			//δ֪״̬
	enDownLoadStatus_Ready,				//׼��״̬
	enDownLoadStatus_DownLoadIng,		//����״̬
	enDownLoadStatus_Compressing,		//����ѹ��	
	enDownLoadStatus_Finish,			//���״̬
	enDownLoadStatus_Fails,				//����ʧ��
};

//����ö��
enum enDownLoadResult
{
	enDownLoadResult_Noknow,			//û�д���
	enDownLoadResult_Exception,			//�쳣����
	enDownLoadResult_CreateFileFails,	//����ʧ��
	enDownLoadResult_InternetReadError,	//��ȡ����
};
//////////////////////////////////////////////////////////////////////////
#define NAME_LEN						32
//////////////////////////////////////////////////////////////////////////
//�ṹ��
//����״̬
struct tagDownLoadStatus
{
	DWORD								wProgress;						//���ؽ���
	char								szStatus[128];					//״̬����
	enDownLoadStatus					DownLoadStatus;					//����״̬
};

struct tagDownStatusInfo
{
	char  szDownFile[MAX_PATH];			//���ص��ļ���
	DWORD dwTotal;						//�ļ��ܴ�С
	DWORD dwTransfered;					//������
};

//��������
struct tagAutoPatchReq
{
	bool							bDisplay;							//�Ƿ���ʾ
	bool							bIsFtpMode;							//FTPģʽ 
	char							sTitleName[NAME_LEN];				//���ڱ���
	char							lpcPatchFile[MAX_PATH];				//autopatch�ļ�
	char							sUrl[MAX_PATH];						//����URL
	char							sLocalListFile[MAX_PATH];			//�����б�
	char							sRemoteListFile[MAX_PATH];			//Զ���б�
	char							sDownloadListFile[MAX_PATH];		//�����ص��б�
	char							sLocalTempFolder[MAX_PATH];			//���ش��Ŀ¼
	char							sFtpUsr[NAME_LEN+1];				//FTP�û���
	char							sFtpPwd[NAME_LEN+1];				//FTP����
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
	//���ֱ��뱣֤��autopatch.ini�ļ�
 	static CMyAutoPatch* CreateInstance(LPCSTR lpcPatchFile,LPCSTR sLocalTempFolder);
	/** 
	*   @FuncName : 
	*   @Author   : Double sword
	*   @Params   : bIsFtpMode�Ƿ���FTPģʽ
	*				sUrl��ԴԶ��URL
	*				sLocalListFile�����ļ��б����ں�Զ���ļ��б�Ա�
	*				sRemoteListFileԶ���ļ��б����غͱ����ļ��б�Ա�
	*				sLocalTempFolder�����б��ļ�����Ŀ¼(ע�⣺�����exe·��fish---"fish\\")
	*				���bIsFtpMode����ôbIsFtpMode��sFtpPwd��Ч
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
	//��ʼ����
	BOOL				BeginDown();
	//���������߳�
	void				EndThread();
	//�õ������ܴ�С
	DWORD				GetSizeTotal()	 const;
	//�õ���ǰ�����ܴ�С
	DWORD				GetDownSize()	 const;
	//�õ���ǰ�����ļ���Ϣ
	tagDownStatusInfo	GetCurDownFileInfo() const;
	//��Ҫ���ص��ļ�����
	int					GetNeedDownFileNum() const;
	//��ǰ�Ѿ������ļ���
	int					GetDownedFileNum() const;
	//�õ�����״̬
	void				GetDownLoadStatus(tagDownLoadStatus & tagStatus);
	//�����ļ��б�(true-��Ҫ����fasle-����Ҫ����)
	bool				DownLoadFileList();		
	//��ȡ���ظ�Ŀ¼
	CString		GetLocalRootDir();
	//��ȡ�������ػ���Ŀ¼
	CString		GetLocalTempFolderDir();


#ifdef DLLEXPORT
private:
	CMyAutoPatch(LPCSTR lpcPatchFile,LPCSTR sLocalTempFolder);//���ֱ��뱣֤��autopatch.ini�ļ�

	/** 
	*   @FuncName : 
	*   @Author   : Double sword
	*   @Params   : bIsFtpMode�Ƿ���FTPģʽ
	*				sUrl��ԴԶ��URL
	*				sLocalListFile�����ļ��б����ں�Զ���ļ��б�Ա�
	*				sRemoteListFileԶ���ļ��б����غͱ����ļ��б�Ա�
	*				sLocalTempFolder�����б��ļ�����Ŀ¼(ע�⣺�����exe·��fish---"fish\\")
	*				���bIsFtpMode����ôbIsFtpMode��sFtpPwd��Ч
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

	//������ص���ʱĿ¼
	void		DelDir(CString strPath);
	//�������ɱ����ļ�
	BOOL		ReBuildLocal();
	//���س�ʼ��
	BOOL		Init();
	//���������ļ�
	BOOL		LoadCfg();	
	//�õ�����Ŀ¼	
	BOOL		GetDirectory();	
	//��ӡ������Ϣ
	void		Printf( const char* pszString, ... );
	//���غ���
	void		Download();	
	//�����ļ�
	BOOL		GetRemoteFile( CString& strURL, CString& strLocalFile, void* pParam );
	//�����ļ��б�
	BOOL		GetRemoteListFile( );
	//�����ļ��б�
	BOOL		AnalyseListFile( const char* pszFilename, std::vector<CFileDesc>& vectorFiles );
	//���������߳�
	static void StartDownLoadThread(LPVOID p);

private:
	string		m_strExeFilename;				//exe�ļ���
	string		m_sPatchFile;					//�Զ����µ������ļ�
	BOOL		m_bIsClone;						//
	BOOL		m_bFtpDownload;					//����ftp��http���أ�
	CString		m_strURLRoot;					//"http://192.168.1.30/update/"
	CString		m_strFtpSite;
	CString		m_strFtpUsername;
	CString		m_strFtpPassword;
	CString		m_strLocalRoot;					// = _T("E://ftpdownload/");
	CString		m_strRemoteListFile;
	CString		m_strDownloadListFile;
	CString		m_strLocalListFile;
	CString     m_sLocalTempFolder;				//�����ļ�����Ŀ¼
	

	CString		m_strGenExt;
	CString		m_strLaunchExe;					//���Ŀ¼
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
	DWORD				   m_dwDownLoadTotalSize;		//���ص��ļ��ܴ�С
	CWebGrab			   m_grab;						//web�����
	enDownLoadStatus	   m_DownLoadStatus;			//����״̬
	enDownLoadResult	   m_DownLoadResult;			//���ش����
	HANDLE				   m_hUpdateThread;				//�߳�ָ��
	bool				   m_bIsEndThread;				//�Ƿ�����߳�
	std::vector<_ZipFile>  m_nZipIndex;					//��Zip��Index
	//-------------------------------------------------------------------
	int					   m_nDownedFileNum;			//�����ص��ļ���
	char				   m_sCurDownFile[MAX_PATH];	//��ǰ�������ص��ļ�
	typedef map<string,tagDownStatusInfo> MAP_DOWNSTATUSINFO;
	MAP_DOWNSTATUSINFO	   m_DownStatusInfoMap;
#endif
};


#endif //__MyAutoPatch_H