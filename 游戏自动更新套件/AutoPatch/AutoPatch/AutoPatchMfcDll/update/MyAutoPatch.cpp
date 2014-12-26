// MyAutoPatch.cpp : ÊµÏÖÎÄ¼ş
//

//#include "stdafx.h"
//----------------------------------------
#include <afxinet.h>
#include <stdio.h>
#include <Shlwapi.h>
#include <string>
#include <assert.h>
#include "scriptparser.h"
#include "FilePath.h"
#include "MapStrToID.h"
#include "webgrab.h"
#include "Zlib/ILibInterface.h"
#include "CfgFileLoader.h"
#include <assert.h>
//#include <atlconv.h>
#include <process.h>
//----------------------------------------
#include "MyAutoPatch.h"

#define PATCH_SESSION	TEXT("MotingPlatformAutoPatch")

using namespace std;

HINSTANCE gMeModule=NULL;
//CMyAutoPatch*	CMyAutoPatch::m_pThis = NULL;
typedef struct  
{
	char zipName[MAX_PATH];
	char zipPassWord[MAX_PATH];
}ZipNode;

const ZipNode  g_ZIPinfo[]={{( "sound.paq"	   ),( "‹Eø‹MÔ‰HT‹Eø‹H@ƒÁ‹Uø‰" )},
							{( "texture.paq"   ),( "kh?ƒ}?t‹ôj‹EÔ‹‹MÔ‹Bÿ@" )},
							{( "ui.paq"        ),( "‹MàQ‹UÜRjÌÌÌÌÌÌÌÌÌÌÌÌ"  )},
							{( "Particles.paq" ),( "xLt@‹Eì‹HL‰ÛEì‹Mø‹I‰•" )},
							{( "path.paq"      ),( "‹Eì‹M‰H8‹Eì‹M$‰H<ÛEì‹M" )}};
enum
{
	eCloneFile = 1<<0,
};

 CMyAutoPatch* CMyAutoPatch::CreateInstance( LPCSTR lpcPatchFile ,LPCSTR sLocalTempFolder)
 {
 	return new CMyAutoPatch(lpcPatchFile,sLocalTempFolder);
 }
 
 CMyAutoPatch* CMyAutoPatch::CreateInstance( LPCSTR lpcPatchFile,
	 bool bIsFtpMode, 
	 LPCSTR sUrl,
	 LPCSTR sLocalListFile, 
	 LPCSTR sRemoteListFile,
	 LPCSTR sDownloadListFile,
	 LPCSTR sLocalTempFolder,
	 LPCSTR sFtpUsr,
	 LPCSTR sFtpPwd)
 {
 	return new CMyAutoPatch(lpcPatchFile,
		bIsFtpMode,
		sUrl,
		sLocalListFile,
		sRemoteListFile,
		sDownloadListFile,
		sLocalTempFolder,
		sFtpUsr,
		sFtpPwd);
 }

void CMyAutoPatch::Release()
{
	delete this;
}

CMyAutoPatch::CMyAutoPatch( LPCSTR lpcPatchFile ,LPCSTR sLocalTempFolder )
{
	m_bIsClone		= FALSE;
	m_bFtpDownload	= FALSE;
	m_bNeedClone	= FALSE;
	m_bDownComplete = FALSE;
	m_pFtpConnect	= NULL;
	m_hUpdateThread	= NULL;
	m_bIsEndThread	= false;
	m_dwDownLoadTotalSize= 0;
	m_nDownedFileNum=0;
//	m_pThis			= this;
	m_DownLoadStatus = enDownLoadStatus_Unknow;
	m_DownLoadResult = enDownLoadResult_Noknow;

	m_LocalFilesDesc.clear();
	//m_pLocalFilesDesc=NULL;
	//m_nLocalFileNum=0;
	memset(m_sCurDownFile,0,sizeof(m_sCurDownFile));

	char strPatchFile[MAX_PATH]={0};
	char strUrlFolder[MAX_PATH]={0};
	strncpy(strUrlFolder,sLocalTempFolder,MAX_PATH-1);
	if (0<strlen(strUrlFolder))
	{
		if ('\\' != strUrlFolder[strlen(strUrlFolder)-1] || 
			'/' != strUrlFolder[strlen(strUrlFolder)-1])
		{
			strUrlFolder[strlen(strUrlFolder)]='\\';
		}
	}
	sprintf(strPatchFile,"%s%s",strUrlFolder,lpcPatchFile) ;

	char szIniFilePath[MAX_PATH]={0};
	char szIniFile[MAX_PATH]={0};
	GetModuleFileNameA(gMeModule,szIniFile,MAX_PATH);
	char* p=strrchr(szIniFile,'\\');
	if(p) 
	{
		strcpy(p+1,strUrlFolder);
		strcpy(szIniFilePath,szIniFile);
		strcpy(p+1,strPatchFile);
	}
	//´´½¨Ä¿Â¼
	::CreateDirectoryA(szIniFilePath,NULL);

	m_sPatchFile = string(szIniFile);
	
	Init();
	ReBuildLocal();

}

CMyAutoPatch::CMyAutoPatch( LPCSTR lpcPatchFile,
						   bool bIsFtpMode, 
						   LPCSTR sUrl,
						   LPCSTR sLocalListFile, 
						   LPCSTR sRemoteListFile,
						   LPCSTR sDownloadListFile,
						   LPCSTR sLocalTempFolder,
						   LPCSTR sFtpUsr,
						   LPCSTR sFtpPwd)
{
	m_bIsClone		= FALSE;
	m_bFtpDownload	= FALSE;
	m_bNeedClone	= FALSE;
	m_bDownComplete = FALSE;
	m_pFtpConnect	= NULL;
	m_hUpdateThread	= NULL;
	m_bIsEndThread	= false;
	m_dwDownLoadTotalSize= 0;
	m_nDownedFileNum=0;
	m_DownLoadStatus = enDownLoadStatus_Unknow;
	m_DownLoadResult = enDownLoadResult_Noknow;

	m_LocalFilesDesc.clear();
	//m_pLocalFilesDesc=NULL;
	//m_nLocalFileNum=0;
	memset(m_sCurDownFile,0,sizeof(m_sCurDownFile));

	char strPatchFile[MAX_PATH]={0};
	char strUrlFolder[MAX_PATH]={0};
	strncpy(strUrlFolder,sLocalTempFolder,MAX_PATH-1);
	if (0<strlen(strUrlFolder))
	{
		if ('\\' != strUrlFolder[strlen(strUrlFolder)-1] || 
			'/' != strUrlFolder[strlen(strUrlFolder)-1])
		{
			strUrlFolder[strlen(strUrlFolder)]='\\';
		}
	}
	sprintf(strPatchFile,"%s%s",strUrlFolder,lpcPatchFile) ;

	char szIniFilePath[MAX_PATH]={0};
	char szIniFile[MAX_PATH]={0};
	GetModuleFileNameA(gMeModule,szIniFile,MAX_PATH);
	char* p=strrchr(szIniFile,'\\');
	if(p) 
	{
		strcpy(p+1,strUrlFolder);
		strcpy(szIniFilePath,szIniFile);
		strcpy(p+1,strPatchFile);
	}
	//´´½¨Ä¿Â¼
	::CreateDirectoryA(szIniFilePath,NULL);
	m_sPatchFile = string(szIniFile);

	//Éú³ÉINIÎÄ¼ş
 	FILE* pf = fopen(szIniFile,"wb");

 	if (pf)
 	{
 		//fflush(pf);
 		fclose(pf);
 	}
	//::remove(pszIniFile);

	char szSection[256];
	char szContent[1024];
	
	//[main]
	memset(szSection,0,sizeof(szSection));
	memset(szContent,0,sizeof(szContent));
	_snprintf(szSection,sizeof(szSection),"main");
	_snprintf(szContent,sizeof(szContent),"FtpDownload = %d\r\n",(int)bIsFtpMode);
	WritePrivateProfileSectionA(szSection,szContent,szIniFile);

	//[Http]
	memset(szSection,0,sizeof(szSection));
	memset(szContent,0,sizeof(szContent));
	_snprintf(szSection,sizeof(szSection),"Http");
	_snprintf(szContent,sizeof(szContent),"URLRoot			= %s\r\n"
										  "LocalListFile	= %s\r\n"
										  "RemoteListFile	= %s\r\n"
										  "DownloadListFile	= %s\r\n"
										  "LocalTempFolder	= %s\r\n",
			  sUrl,sLocalListFile,sRemoteListFile,sDownloadListFile,sLocalTempFolder);
	WritePrivateProfileSectionA(szSection,szContent,szIniFile);

	//[Ftp]
	memset(szSection,0,sizeof(szSection));
	memset(szContent,0,sizeof(szContent));
	_snprintf(szSection,sizeof(szSection),"Ftp");
	_snprintf(  szContent,sizeof(szContent),
				"FtpSite			= %s\r\n"
				"FtpUsername		= %s\r\n"
				"FtpPassword		= %s\r\n"
				"LocalListFile		= %s\r\n"
			    "RemoteListFile		= %s\r\n"
				"DownloadListFile	= %s\r\n"
				"LocalTempFolder	= %s\r\n",
				 sUrl,sFtpUsr,sFtpPwd,
				 sLocalListFile,sRemoteListFile,sDownloadListFile,sLocalTempFolder);
	WritePrivateProfileSectionA(szSection,szContent,szIniFile);

	//[Loader]
	memset(szSection,0,sizeof(szSection));
	memset(szContent,0,sizeof(szContent));
	_snprintf(szSection,sizeof(szSection),"Loader");
	_snprintf(szContent,sizeof(szContent),"LoadExe = CUpDate.exe\r\n",(int)bIsFtpMode);
	WritePrivateProfileSectionA(szSection,szContent,szIniFile);

	Init();
	ReBuildLocal();
}

CMyAutoPatch::~CMyAutoPatch()
{
	m_bClone = FALSE;
	if (m_hUpdateThread)
	{
		DWORD dwRes=WaitForSingleObject(m_hUpdateThread,100);
		if (WAIT_TIMEOUT == dwRes ) ::TerminateThread(m_hUpdateThread,0);
		m_hUpdateThread = NULL;
	}

	m_LocalFilesDesc.clear();
// 	if(m_pLocalFilesDesc){
// 		delete[] m_pLocalFilesDesc;
// 		m_pLocalFilesDesc=NULL;
// 	}
//	m_nLocalFileNum=0;
}
// CMyAutoPatch ÏûÏ¢´¦Àí³ÌĞò

BOOL CMyAutoPatch::LoadCfg()
{
	CCfgFileLoader loader;
	if( !loader.LoadFromFile( m_sPatchFile.c_str() ) )
		return FALSE;

	if( !loader.OpenSession( "main" ))
		return FALSE;

	m_bFtpDownload		= loader.GetIntValue( "FtpDownload" );

	if( m_bFtpDownload )
	{
		if( !loader.OpenSession( "Ftp" ))
			return FALSE;
		m_strURLRoot = loader.GetStringValue( "URLRoot" );
		m_sLocalTempFolder	= CString(loader.GetStringValue( "LocalTempFolder" ));
		if(!m_sLocalTempFolder.IsEmpty() && 
			'\\' != m_sLocalTempFolder.GetAt(m_sLocalTempFolder.GetLength()-1) &&
			'/' != m_sLocalTempFolder.GetAt(m_sLocalTempFolder.GetLength()-1)  )
		{
			m_sLocalTempFolder += "\\";
		}
		m_strFtpSite = loader.GetStringValue( "FtpSite" );
		m_strFtpUsername = loader.GetStringValue( "FtpUsername" );
		m_strFtpPassword = loader.GetStringValue( "FtpPassword" );
		m_strLocalListFile = m_strLocalRoot+"\\"+m_sLocalTempFolder+loader.GetStringValue( "LocalListFile" );
		m_strRemoteListFile = m_strURLRoot+"/"+loader.GetStringValue( "RemoteListFile" );
		m_strDownloadListFile = m_strLocalRoot+"\\"+m_sLocalTempFolder+loader.GetStringValue( "DownloadListFile" );
	}
	else
	{
		if( !loader.OpenSession( "Http" ) )
			return FALSE;
		m_strURLRoot = loader.GetStringValue( "URLRoot" );
		m_sLocalTempFolder	= CString(loader.GetStringValue( "LocalTempFolder" ));
		if(!m_sLocalTempFolder.IsEmpty() && 
			'\\' != m_sLocalTempFolder.GetAt(m_sLocalTempFolder.GetLength()-1) &&
			'/' != m_sLocalTempFolder.GetAt(m_sLocalTempFolder.GetLength()-1)  )
		{
			m_sLocalTempFolder += "\\";
		}
		m_strLocalListFile =  m_strLocalRoot+"\\"+m_sLocalTempFolder+loader.GetStringValue( "LocalListFile" );
		m_strRemoteListFile = m_strURLRoot+"/"+loader.GetStringValue( "RemoteListFile" );
		m_strDownloadListFile = m_strLocalRoot+ "\\"+m_sLocalTempFolder+loader.GetStringValue( "DownloadListFile" );
	}


	loader.OpenSession( "Loader" );
	m_strLaunchExe = loader.GetStringValue( "LoadExe" );


	return TRUE;
}


BOOL CMyAutoPatch::GetDirectory()
{
	PROCESS_INFORMATION pi;
	STARTUPINFO si = {sizeof(si)};

	// µÃµ½WindowsÄ¿Â¼
	BOOL ret = CreateProcess(
		NULL,
		m_strLaunchExe.GetBuffer(),  
		NULL, 
		NULL, 
		FALSE, 
		0, 
		NULL, 
		NULL, 
		&si,
		&pi );
	if(!ret) 
	{
		assert( false && "cannot lunch app" );
		return FALSE;
	}
	else
	{
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}
	return TRUE;
}

BOOL	CMyAutoPatch::Init()
{
	char szPath[MAX_PATH];
	if( GetModuleFileNameA( NULL, szPath, MAX_PATH ) == 0 )
		return FALSE;
	// exeÍêÕûÂ·¾¶
	m_strExeFilename = szPath;

	char szCurrentDir[MAX_PATH];
	strcpy(szCurrentDir,szPath);
	char* p=strrchr(szCurrentDir,'\\');
	if(p) *p=0;

	//GetCurrentDirectoryA( MAX_PATH, szCurrentDir );

	// Ä¬ÈÏÄ¿Â¼ÊÇµ±Ç°Ä¿Â¼
	m_strLocalRoot = CString(szCurrentDir);

	LoadCfg();


	if( m_bNeedClone )
	{
		STARTUPINFO AppSI;
		GetStartupInfo( &AppSI );
		// µ±Ç°exeÒÑ¾­ÊÇclone°æ±¾£¬
		if( AppSI.dwX & eCloneFile )
		{

		}
		else
		{
			char szCloneFilename[MAX_PATH];
			strcpy( szCloneFilename,m_strExeFilename.c_str() );
			szCloneFilename[strlen(szCloneFilename)-4] = 0;
			strncat( szCloneFilename, ("_Clone.exe") ,sizeof(szCloneFilename));
			CopyFileA( m_strExeFilename.c_str(), szCloneFilename, FALSE );

			// ÁÙÊ±±äÁ¿
			PROCESS_INFORMATION pi;
			STARTUPINFO CloneSI = {sizeof(CloneSI)};

			CloneSI.dwX |= eCloneFile;

			CString strCommand( szCloneFilename );
			//strCommand += " "+strPassword;

			TCHAR* ppp = strCommand.GetBuffer();

			// µÃµ½WindowsÄ¿Â¼
			BOOL ret = CreateProcess(
				NULL,
				strCommand.GetBuffer(),
				//NULL,
				//szCloneFilename,  
				//szCommandLine,

				//NULL,
				NULL, 
				NULL, 
				FALSE, 
				0, 
				NULL, 
				NULL, 
				&CloneSI,
				&pi );
			if(!ret) 
			{
				assert( false && "cannot run clone" );
				return FALSE;
			}
			else
			{
				CloseHandle(pi.hThread);
				CloseHandle(pi.hProcess);
			}
			return TRUE;
		}

	}
	return FALSE;
}
void CMyAutoPatch::Printf( const char* pszString, ... )
{
	va_list	va;
	static char data[1024];
	va_start( va, pszString );
	vsprintf( (char*)data, pszString, va );
	va_end( va );

}

void CMyAutoPatch::StartDownLoadThread(LPVOID p)
{
	CMyAutoPatch *pthis = (CMyAutoPatch *)p;
	if( pthis->m_bFtpDownload )
	{//ÅĞ¶ÏÊÇ²»ÊÇftpÏÂÔØ
		CInternetSession sess(_T("AutoPatchMain"));
		try
		{
			pthis->m_pFtpConnect = sess.GetFtpConnection(
				pthis->m_strFtpSite,
				pthis->m_strFtpUsername,
				pthis->m_strFtpPassword );
			if( pthis->m_pFtpConnect )
				pthis->Download();
		}
		catch (CInternetException* pEx)
		{
			TCHAR sz[1024];
			pEx->GetErrorMessage(sz, 1024);
			printf("ERROR!  %s\n", sz);
			pEx->Delete();
		}

		if( pthis->m_pFtpConnect )
		{
			pthis->m_pFtpConnect->Close();	
			delete pthis->m_pFtpConnect;
			pthis->m_pFtpConnect = NULL;
		}
	}
	else
	{//httpÏÂÔØ
		CString strPath=pthis->m_strLocalRoot+"\\"+pthis->m_sLocalTempFolder+"downLoadFile";
		pthis->DelDir( strPath);
		pthis->Download();
	}

	if( pthis->m_bDownComplete )
	{//ÏÂÔØÍê³É
		vector <_ZipFile>::iterator it= pthis->m_nZipIndex.begin();
		for (;it != pthis->m_nZipIndex.end();it++)
		{
			if((*it).Index != 0)
			{
				//TRACE("+++++¹Ø±ÕZIP %d\n",(*it).Index);
				pthis->m_DownLoadStatus = enDownLoadStatus_Compressing;
				UpdateZipFile((*it).Index);
				CloseZip((*it).Index);
			}
		}
		
		pthis->m_DownLoadStatus = enDownLoadStatus_Finish;
	}
	else
	{//ÏÂÔØÊ§°Ü
		vector <_ZipFile>::iterator it= pthis->m_nZipIndex.begin();
		for (;it != pthis->m_nZipIndex.end();it++)
		{
			if((*it).Index != 0)
				CloseZip((*it).Index);
		}
		pthis->m_DownLoadStatus = enDownLoadStatus_Fails;
	}

	pthis->m_nZipIndex.clear();
	CString strPath=pthis->m_strLocalRoot+"\\"+pthis->m_sLocalTempFolder+"downLoadFile";
	pthis->DelDir(strPath);

	_endthread();
	return ;
}

void	CMyAutoPatch::DelDir(CString strPath)
{
	//------------------------------------------------------
	//É¾³ıÄ¿Â¼
	CFileFind ff;
	CString path = strPath;

	if(path.Right(1) != "\\")
		path += "\\";

	path += "*.*";
	BOOL res = ff.FindFile(path);

	while(res)
	{
		res = ff.FindNextFile();
		//ÊÇÎÄ¼şÊ±Ö±½ÓÉ¾³ı
		//AfxMessageBox(ff.GetFilePath());
		if (!ff.IsDots() && !ff.IsDirectory())
			DeleteFile(ff.GetFilePath());
		else if (ff.IsDots())
			continue;
		else if (ff.IsDirectory())
		{
			path = ff.GetFilePath();
			//ÊÇÄ¿Â¼Ê±¼ÌĞøµİ¹é£¬É¾³ı¸ÃÄ¿Â¼ÏÂµÄÎÄ¼ş
			DelDir(path);
			//Ä¿Â¼Îª¿ÕºóÉ¾³ıÄ¿Â¼
			RemoveDirectory((path));
		}	
	}
	ff.Close();
	//×îÖÕÄ¿Â¼±»Çå¿ÕÁË£¬ÓÚÊÇÉ¾³ı¸ÃÄ¿Â¼
	RemoveDirectory((strPath));
}


void		CMyAutoPatch::EndThread()
{
	m_bIsEndThread = true;
}


DWORD	CMyAutoPatch::GetSizeTotal() const
{
	return m_dwDownLoadTotalSize;
}

DWORD	CMyAutoPatch::GetDownSize() const
{
	return m_grab.GetDownSize();
}

tagDownStatusInfo	CMyAutoPatch::GetCurDownFileInfo() const
{
	USES_CONVERSION;
	tagDownStatusInfo tag={0};
	char szCurFile[MAX_PATH]={0};
	strcpy_s(szCurFile,_countof(szCurFile),m_sCurDownFile);
	if ( strcmp(szCurFile,"") )
	{
		MAP_DOWNSTATUSINFO::const_iterator it = m_DownStatusInfoMap.find(szCurFile);
		if (it != m_DownStatusInfoMap.end())
		{
			strcpy(tag.szDownFile,it->second.szDownFile);
			tag.dwTotal = it->second.dwTotal;
			tag.dwTransfered = it->second.dwTransfered;
		}
	}
	
	return tag;
}

//ĞèÒªÏÂÔØµÄÎÄ¼ş×ÜÊı
int		CMyAutoPatch::GetNeedDownFileNum() const
{
	return m_DownStatusInfoMap.size();
}

//µ±Ç°ÒÑ¾­ÏÂÔØÎÄ¼şÊı
int		CMyAutoPatch::GetDownedFileNum() const
{
	return m_nDownedFileNum;
}

void	CMyAutoPatch::GetDownLoadStatus(tagDownLoadStatus & tagStatus)
{
	
	//ÉèÖÃ±äÁ¿
	tagStatus.DownLoadStatus=m_DownLoadStatus;
	switch (m_DownLoadStatus)
	{
	case enDownLoadStatus_Unknow:
	case enDownLoadStatus_Ready:
		{
			tagStatus.wProgress=0;
			lstrcpynA(tagStatus.szStatus,"ÕıÔÚ»ñÈ¡ÎÄ¼ş...",_countof(tagStatus.szStatus));
			break;
		}
	case enDownLoadStatus_DownLoadIng:
		{
			DWORD dwDownSize=m_grab.GetDownSize();
			tagStatus.wProgress=(dwDownSize*1.0/m_dwDownLoadTotalSize)*100L;
			_snprintf(tagStatus.szStatus,_countof(tagStatus.szStatus),
				"ÒÑ¸´ÖÆ£º%ld KB £¨¹² %ld KB£©",dwDownSize/1000L,m_dwDownLoadTotalSize/1000L);
			break;
		}
	case enDownLoadStatus_Finish:
		{
			tagStatus.wProgress=100;
			lstrcpynA(tagStatus.szStatus,"ÏÂÔØ³É¹¦",_countof(tagStatus.szStatus));
			break;
		}
	case enDownLoadStatus_Fails:
		{
			tagStatus.wProgress=0;
			_snprintf(tagStatus.szStatus,_countof(tagStatus.szStatus),"ÏÂÔØÊ§°Ü£¬´íÎóºÅ£º%ld",m_DownLoadResult);
			break;
		}
	case enDownLoadStatus_Compressing:
		{
			tagStatus.wProgress=100;
			lstrcpynA(tagStatus.szStatus,"Êı¾İÑ¹ËõÖĞ...",_countof(tagStatus.szStatus));
			break;
		}
	default: 
		{
			ASSERT(FALSE);
			memset(&tagStatus,0,sizeof(tagStatus));
			break;
		}
	} 
}

BOOL	CMyAutoPatch::BeginDown()
{
	m_grab.Initialise(PATCH_SESSION,NULL);
	m_hUpdateThread = (HANDLE)_beginthread(StartDownLoadThread,0,this);
	return TRUE;
}


bool	CMyAutoPatch::DownLoadFileList()
{
	USES_CONVERSION;
	m_DownLoadStatus= enDownLoadStatus_Ready;
	m_bDownComplete = FALSE;
	// ±ØĞëÇå¿Õ
	m_nDownedFileNum=0;
	m_RemoteFilesDesc.clear();
	m_DownStatusInfoMap.clear();
	m_DownStatusInfoMap.swap(MAP_DOWNSTATUSINFO());

	strcpy_s( m_sCurDownFile,_countof(m_sCurDownFile),T2A(m_strRemoteListFile.GetBuffer() ) );

	string sName = m_sCurDownFile;
	tagDownStatusInfo tag;
	memset(&tag,0,sizeof(tag));
	strcpy(tag.szDownFile,sName.c_str());

	m_DownStatusInfoMap[sName]=tag;

	if( GetRemoteListFile()/*GetRemoteFile( m_strRemoteListFile,  m_strDownloadListFile,NULL)*/ )
	{
		if( AnalyseListFile( T2A(m_strDownloadListFile.GetBuffer()), m_RemoteFilesDesc ) )
		{
			//--------------------------------------------------------------------------
			//Óë±¾µØÎÄ¼şCRCÑéÖ¤
			std::vector<CFileDesc>::iterator it=m_RemoteFilesDesc.begin();
			bool   bFlag =  false;
			for (; it != m_RemoteFilesDesc.end();)
			{
				CFileDesc* remote = &(*it);
				assert( remote->m_dwSize > 0 );
				CFileDesc* local = 
					(CFileDesc*)m_LocalFilesMgr.GetDstByName( remote->m_szFilename );

				bFlag = false;
				// Èç¹û¸ÃÎÄ¼şÔÚ±¾µØ´æÔÚ
				if( local )
				{
					// Èç¹ûÁ½Õß±»ÈÏÎªÊÇÒ»ÑùµÄ£¬¾Í²»ĞèÒªÏÂÔØÁË
					if(stricmp( local->m_szFilename, remote->m_szFilename ) == 0 &&
						stricmp( local->m_szCRC, remote->m_szCRC ) == 0	&&
						local->m_dwSize	== remote->m_dwSize)
					{
						// ÎªÁË±£ÏÕÆğ¼û,ÔÙ¿´¿´localµÄÎÄ¼şÊÇ·ñ´æÔÚ£¬ÉõÖÁ¼ì²âÒ»ÏÂÎÄ¼ş´óĞ¡		
						it=m_RemoteFilesDesc.erase(it);
						bFlag	 = true;
					}	
				}
				if (!bFlag)
				{//ĞèÒªÏÂÔØ
					tagDownStatusInfo tag;
					memset(&tag,0,sizeof(tag));
					strcpy(tag.szDownFile,it->m_szFilename );
					tag.dwTotal=it->m_dwSize;

					m_DownStatusInfoMap[it->m_szFilename]=tag;

					it++;
				}
			}

			//--------------------------------------------------------------------------

			for (int i = 0; i < m_RemoteFilesDesc.size(); i++)
			{
				m_dwDownLoadTotalSize+=m_RemoteFilesDesc[i].m_dwSize;
			}

			if (m_dwDownLoadTotalSize > 0)
			{
				m_grab.SetDownSize();
				return true;
			}
		}
	}
	CString strPath=m_strLocalRoot+"\\"+m_sLocalTempFolder+"downLoadFile";
	DelDir(strPath);
	return false;
}

//»ñÈ¡±¾µØ¸ùÄ¿Â¼
CString	CMyAutoPatch::GetLocalRootDir()
{
	return m_strLocalRoot;
}

//»ñÈ¡±¾µØÏÂÔØ»º³åÄ¿Â¼
CString	CMyAutoPatch::GetLocalTempFolderDir()
{
	return m_sLocalTempFolder;
}


void CMyAutoPatch::Download()
{
	USES_CONVERSION;
	m_DownLoadStatus=enDownLoadStatus_DownLoadIng;
	std::vector<CFileDesc> vecLocalNoExist;
	vecLocalNoExist.clear();
	for( int i = 0; i < m_RemoteFilesDesc.size(); i++ )
	{
		if (m_bIsEndThread)
		{		
			return;
		}
		CFileDesc* remote = &m_RemoteFilesDesc[i];
		assert( remote->m_dwSize > 0 );

		CFileDesc* local = (CFileDesc*)m_LocalFilesMgr.GetDstByName( remote->m_szFilename );

		//µ±Ç°½«ÒªÏÂÔØµÄÎÄ¼şÃû
		strcpy_s(m_sCurDownFile,_countof(m_sCurDownFile),remote->m_szFilename);

		CString src = m_strURLRoot+"/"+m_sCurDownFile;
		CString dst = m_strLocalRoot+"\\"+m_sLocalTempFolder+"downLoadFile"+"\\"+m_sCurDownFile;
		BOOL bDownloadSucc = FALSE;
		int const MAX_TRY = 10;
		// ½øĞĞÈô¸É´ÎµÄÖØÊÔ
		for( int nTry = 0; nTry < MAX_TRY; nTry++ )
		{
			if( GetRemoteFile( src, dst, NULL) )
			{
				bDownloadSucc = TRUE;
				CFilePath path;
				if( path.GetFileSize( T2A(dst.GetBuffer()) ) == 0 )
					assert( false && "[ÏÂÔØÁË0´óĞ¡ÎÄ¼ş]" );
					break;
			}
		}
		// Èç¹ûÏÂÔØ³É¹¦
		if( bDownloadSucc )
		{
			//Printf( "%s ok.", remote->m_szFilename );
			// ÏÂÔØ³É¹¦ÁË£¬ĞŞ¸Ä±¾µØµÄÊı¾İ
			//TRACE("++++++++++%sÏÂÔØ³É¹¦",remote->m_szFilename);
			//-----------------------------------------------------
			//¹¹ÔìÎÄ¼şµÄÒÆ¶¯Â·¾¶
			if (m_RemoteFilesDesc[i].m_nIsZip == 1)
			{//zipÎÄ¼şÌí¼Ó¸üĞÂ²Ù×÷
				for (int j=0;j<sizeof(g_ZIPinfo)/sizeof(ZipNode);j++)
				{
					if (0 == strcmp(m_RemoteFilesDesc[i].m_szZipFileName,g_ZIPinfo[j].zipName))
					{
						CString zipDstPath = m_strLocalRoot+"\\"+ m_RemoteFilesDesc[i].m_szZipFilePath;
						CString zipSrcPath = m_strLocalRoot+"\\"+m_sLocalTempFolder+"downLoadFile"+"\\"+m_sCurDownFile;
						int nIndex = 0;
						vector <_ZipFile>::iterator it = m_nZipIndex.begin();
						for (;it!=m_nZipIndex.end();it++)
						{
							if ((*it).zipPath==zipDstPath)
							{
								nIndex = (*it).Index;
								break;
							}
						}
						if (it == m_nZipIndex.end())
						{	
							nIndex= OpenZip(T2A(zipDstPath.GetBuffer()),g_ZIPinfo[j].zipPassWord);
							//TRACE("+++++++[%d] %s\n",nIndex,zipDstPath);
							_ZipFile  zipFile;
							zipFile.Index = nIndex;
							zipFile.zipPath =  zipDstPath;
							m_nZipIndex.push_back(zipFile);
						}
						
						if (0 == nIndex)
								return;
						FILE * fp =NULL;
						if((fp = fopen(T2A(zipSrcPath.GetBuffer()),"rb"))==NULL)
						{	
								printf("open file is error\n!");
								ASSERT(FALSE);
								return;
						}
						fseek(fp,0L,SEEK_END);  /* ¶¨Î»µ½ÎÄ¼şÄ©Î² */ 
						int flen=ftell(fp);		/* µÃµ½ÎÄ¼ş´óĞ¡ */ 
						char * pszBUF= new char[flen+1];
						memset(pszBUF,0,flen+1);
						fseek(fp,0L,SEEK_SET);
						fread(pszBUF,flen,1,fp);
						//TRACE("+++++++[%d] %s\n",nIndex,m_RemoteFilesDesc[i].m_szFilePath);
// 						if(strcmp(m_RemoteFilesDesc[i].m_szFilePath,"")==0
// 						  ||!AfxIsValidAddress(&m_RemoteFilesDesc[i],sizeof(CFileDesc)))
// 						{
// 							ASSERT(FALSE);
// 	
// 						}

						AddUpdateFile(nIndex,m_RemoteFilesDesc[i].m_szFilePath,pszBUF,flen);
						
						if (pszBUF)
						{
							delete[] pszBUF;
							pszBUF = NULL;
						}
						fclose(fp);
						DeleteFile(zipSrcPath);
						break;
					}
				}
			}
			else	//²»ÊÇzipÎÄ¼ş
			{
				//¸²¸Ç±¾µØÎÄ¼ş
				CString dstFile = m_strLocalRoot+"\\"+m_RemoteFilesDesc[i].m_szFilePath;
				//´´½¨Ä¿Â¼
				TCHAR sDriver[MAX_PATH],sDirectory[MAX_PATH],sFile[MAX_PATH],sFileExt[MAX_PATH];
				_tsplitpath(dstFile.GetBuffer(),sDriver,sDirectory,sFile,sFileExt);
				_tcscat(sDriver,sDirectory);
				::CreateDirectory(sDriver,NULL);
				CString srcFile = m_strLocalRoot+"\\"+m_sLocalTempFolder+"downLoadFile"+"\\"+m_sCurDownFile;
				if(CopyFile(srcFile,dstFile,FALSE)) 
				{
					DeleteFile(srcFile);
				}
			}
			if( local )
			{
				local->m_dwSize		= remote->m_dwSize;
				local->m_dwCheckSum = remote->m_dwCheckSum;
				local->m_dwBitSum	= remote->m_dwBitSum;
				local->m_nIsZip		= remote->m_nIsZip;
				strcpy_s(local->m_szFilename,MAX_PATH,remote->m_szFilename);local->m_szFilename[MAX_PATH-1]=0;
				strcpy_s(local->m_szFilePath,MAX_PATH,remote->m_szFilePath);local->m_szFilePath[MAX_PATH-1]=0;
				strcpy_s(local->m_szZipFileName,MAX_PATH,remote->m_szZipFileName);local->m_szZipFileName[MAX_PATH-1]=0;
				strcpy_s(local->m_szZipFilePath,MAX_PATH,remote->m_szZipFilePath);local->m_szZipFilePath[MAX_PATH-1]=0;
				strcpy_s(local->m_szCRC,MAX_PATH,remote->m_szCRC);local->m_szCRC[MAX_PATH-1]=0;
			}
			else
			{
				vecLocalNoExist.push_back(*remote);
			}
		}
		else 
		{//ÏÂÔØÊ§°Ü
			//TRACE("++++++++++%sÏÂÔØÊ§°Ü",remote->m_szFilename);
			m_BadFilesDesc.push_back( *remote );
		}

	}

	//°Ñ±¾µØÃ»ÓĞ²¢ÇÒÒÑ¾­ÏÂÔØÍê±ÏµÄµÄ¼ÇÂ¼µ½±¾µØµÄÎÄ¼şÁĞ±í
	for (std::vector<CFileDesc>::iterator it = vecLocalNoExist.begin();
		it != vecLocalNoExist.end();it++)
	{
		m_LocalFilesDesc.push_back(*it);
	}

	//ÖØĞÂ°ó¶¨
	m_LocalFilesMgr.Destroy();
	for( int i = 0; i < m_LocalFilesDesc.size(); i++ )
	{
		CFileDesc* pDesc = &m_LocalFilesDesc[i];
		m_LocalFilesMgr.Register( pDesc->m_szFilename, pDesc );
	}

	if(!m_bIsEndThread)
	{
		// Ö»ÓĞÈ«²¿ÎÄ¼şÏÂÔØ³É¹¦£¬²Å»á¸²¸Ç±¾µØÁĞ±í
		if( m_BadFilesDesc.size() == 0 )
		{
			// ×îºó£¬ÓÃÏÂÔØµÄĞÂ°ælistfile¸²¸Ç±¾µØµÄ
			if( CopyFile( m_strDownloadListFile, m_strLocalListFile, FALSE ) )
			{
					//m_pDownInfoFun("ĞÂ°æÁĞ±íÎÄ¼ş¸²¸Ç³É¹¦.",strlen("ĞÂ°æÁĞ±íÎÄ¼ş¸²¸Ç³É¹¦."));
					Printf( "ĞÂ°æÁĞ±íÎÄ¼ş¸²¸Ç³É¹¦." );
			}
			else
			{
					m_DownLoadResult =  enDownLoadResult_Exception;
					Printf( "ĞÂ°æÁĞ±íÎÄ¼ş¸²¸Ç¸ÃÊ§°Ü." );
			}
			m_bDownComplete = TRUE;
		}
		else
		{
			m_DownLoadResult =  enDownLoadResult_Exception;
			//TRACE("++++++++++ ´æÔÚÏÂÔØÊ§°ÜµÄÎÄ¼ş");
		}
		DeleteFile( m_strDownloadListFile );

	}

	FILE* fp = fopen( T2A(m_strLocalListFile.GetBuffer()), "wb" );
	if( fp )
	{
		int nFileCount = m_LocalFilesDesc.size();
		fwrite( &nFileCount, sizeof(int), 1, fp );
		for( int i = 0; i < m_LocalFilesDesc.size(); i++ )
		{
			CFileDesc* pDesc = &m_LocalFilesDesc[i];
			fwrite( pDesc, sizeof( CFileDesc ), 1, fp );
		}
		/*int nFileCount = m_nLocalFileNum;
		fwrite( &nFileCount, sizeof(int), 1, fp );
		for( int i = 0; i < m_nLocalFileNum; i++ )
		{
			CFileDesc* pDesc = &m_pLocalFilesDesc[i];
			fwrite( pDesc, sizeof( CFileDesc ), 1, fp );
		}*/
		fclose( fp );
	}

}

BOOL CMyAutoPatch::GetRemoteFile( CString& strURL, CString& strLocalFile, void* pParam )
{
	USES_CONVERSION;
	CFilePath path;
	path.Split( T2A(strLocalFile.GetBuffer()));
	// ÔÚµ±Ç°ÌåÏµÖĞ,Ã»ÓĞÀ©Õ¹ÃûµÄÎÄ¼şÈÏÎªÊÇÄ¿Â¼,
	// Ç¿ÖÆĞÔµÄ
	if( !path.GetExt() )
		return FALSE;
	if( !path.MakeDirectory( T2A(strLocalFile.GetBuffer()) ) )
		return FALSE;


	//CString strBuffer;
	std::vector<char> vtBuffer;
	CString strRealURL = strURL+m_strGenExt;

	string sName=m_sCurDownFile;

	if( m_bFtpDownload )
	{
		CInternetFile* pInterFile = NULL;

		if( !m_pFtpConnect )
		{
			assert( false && "ftp has not connectted" );
			return FALSE;
		}
		try
		{
			CFtpFileFind finder(m_pFtpConnect);
			if(!finder.FindFile(strRealURL)) {
				assert( false && "file isn't exist!"  );
				return FALSE;
			}
			finder.FindNextFile();
			int nSize=finder.GetLength();
			//m_DownStatusInfoMap[sName].dwTotal=nSize;

			pInterFile = m_pFtpConnect->OpenFile(strRealURL);
			
			int nReadBytes=0;
			const int nMaxBytes=4096;
			char* Buf = new char[nMaxBytes+1];Buf[nMaxBytes]=0;
			CFile file(strLocalFile,CFile::modeCreate|CFile::modeWrite|CFile::typeBinary);

			do 
			{
				if (pInterFile)
				{
					nReadBytes = pInterFile->Read(Buf,nMaxBytes);
				}

				if (0 < nReadBytes)
				{
					Buf[nReadBytes]=0;
					file.Write(Buf,nReadBytes);
					m_DownStatusInfoMap[sName].dwTransfered += nReadBytes;
				}

			} while (0 < nReadBytes);
			
			file.Close();
			delete[] Buf;
			if(pInterFile)pInterFile->Close();
			//old
			// 		if( !m_pFtpConnect->GetFile( strRealURL, strLocalFile, FALSE ) )
			// 			return FALSE;
		}
		catch (CInternetException* pEx)
		{
			TCHAR sz[1024];
			pEx->GetErrorMessage(sz, 1024);
			printf("ERROR!  %s\n", sz);
			pEx->Delete();
			if(pInterFile)pInterFile->Close();
		}

		
	}
	else
	{
		//if( URLDownloadToFile( NULL, strRealURL, strLocalFile, 0, NULL ) != S_OK )
		DWORD dwTotal=0;
		if( !m_grab.GetFile(strRealURL, vtBuffer,m_DownStatusInfoMap[sName].dwTransfered,
			dwTotal,PATCH_SESSION,(CWnd*)pParam ) )
			return FALSE;

		//int nSize = strBuffer.GetLength();
		int nSize = vtBuffer.size();
		// ÔÚÓĞĞ©Çé¿öÏÂ,»á³öÏÖ0³¤¶ÈµÄÎÄ¼ş
		// ´ËÊ±ÎÄ¼şÏÂÔØ³ö´í
		if( nSize == 0 )
			//return FALSE;
			return TRUE;

		FILE* fp = fopen( T2A(strLocalFile.GetBuffer()), "wb" );
		if( !fp )
			return FALSE;
		fwrite(&vtBuffer[0],nSize, 1, fp );
		fclose( fp );
	}

	m_nDownedFileNum++;

	return TRUE;
}

//ÏÂÔØÎÄ¼şÁĞ±í
BOOL CMyAutoPatch::GetRemoteListFile( )
{
	BOOL bRes=FALSE;
	if (m_bFtpDownload)
	{
		CInternetSession sess(_T("AutoPatchÅäÖÃÎÄ¼ş"));
		try
		{
			m_pFtpConnect = sess.GetFtpConnection(
				m_strFtpSite,
				m_strFtpUsername,
				m_strFtpPassword );
			if( m_pFtpConnect ) bRes=GetRemoteFile( m_strRemoteListFile,  m_strDownloadListFile,NULL) ;
		}
		catch (CInternetException* pEx)
		{
			TCHAR sz[1024];
			pEx->GetErrorMessage(sz, 1024);
			printf("ERROR!  %s\n", sz);
			pEx->Delete();
		}

		if( m_pFtpConnect )
		{
			m_pFtpConnect->Close();	
			delete (m_pFtpConnect);
			m_pFtpConnect = NULL;
		}
	}
	else
	{
		bRes=GetRemoteFile( m_strRemoteListFile,  m_strDownloadListFile,NULL) ;
	}

	
	
	return bRes;
}

//·ÖÎöÁĞ±íÎÄ¼ş
BOOL CMyAutoPatch::AnalyseListFile( const char* pszFilename, std::vector<CFileDesc>& vectorFiles )
{
	m_dwDownLoadTotalSize= 0;
	assert( vectorFiles.size() == 0 );
	if( !pszFilename || 
		pszFilename[0] == 0 )
		return FALSE;
	FILE* fp = fopen( pszFilename, "rb" );
	if( !fp )
		return FALSE;
	int nFileCount = 0;//
	fread(&nFileCount, sizeof(int), 1, fp  );
	if (nFileCount <=0 || nFileCount >1024)
	{
		return FALSE;
	}
	vectorFiles.reserve( nFileCount );
	vectorFiles.resize( nFileCount );
	if (nFileCount > 0 && nFileCount< 1024)
	{
		fread( &vectorFiles[0], sizeof( CFileDesc ), nFileCount, fp);
	}
	fclose( fp );
	return TRUE;
}

BOOL	 CMyAutoPatch::ReBuildLocal()
{
	USES_CONVERSION;
	m_LocalFilesDesc.clear();
	m_LocalFilesMgr.Destroy();
	//std::vector<CFileDesc> local;
	if( !AnalyseListFile( T2A(m_strLocalListFile.GetBuffer()), m_LocalFilesDesc ) )
		return FALSE;
	
// 	if (!m_pLocalFilesDesc)
// 	{
// 		m_nLocalFileNum=local.size();
// 		m_pLocalFilesDesc = new CFileDesc[m_nLocalFileNum+1000];
// 		memset(m_pLocalFilesDesc,0,(m_nLocalFileNum+1000)*sizeof(CFileDesc) );
// 	}
	for( int i = 0; i < m_LocalFilesDesc.size(); i++ )
	//for( int i = 0; i < m_nLocalFileNum; i++ )
	{
		CFileDesc* pDesc = &m_LocalFilesDesc[i];
		//m_pLocalFilesDesc[i]=local[i];
		//CFileDesc* pDesc = &m_pLocalFilesDesc[i];

		m_LocalFilesMgr.Register( pDesc->m_szFilename, pDesc );
	}
	return TRUE;
}

