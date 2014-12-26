#ifndef _FILEPATH_H
#define _FILEPATH_H
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#define IS_FILE_EXT( path, ext ) ( _stricmp( CFilePath::GetFileNameExt( path ),ext ) == 0 ) 
class CFilePath
{
public:
	CFilePath();
	static BOOL IsExist( const char* pszPathName );
	static BOOL IsDirectory( const char* pszPathName );
	static BOOL MakeDirectory( const char* pszPathName );
	static DWORD GetFileSize( const char* pszPathName );

	//static const char* GetFileName();
	static const char* GetFileNameExt( const char* pszPathName );
	static const char* GetFileDir( const char* pszPathName );
	//static const char* GetDir();
	//static const char* GetPath();
	
	
	BOOL Split( const char* pszPathName );
	

	BOOL MakeDirectoryForFile( const char* pszPathName );

	const char* GetDrive();
	const char* GetDirectory();
	const char* GetFileName();
	const char* GetExt();


protected:
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

};

class CFileDesc
{
public:
	CFileDesc(void);
	~CFileDesc(void);


	int GetByteBitSum( BYTE byte )
	{
		int nCount = 0;
		for( int i = 0; i < 8; i++ )
		{
			if( byte & 0x01 )
				nCount++;
			byte >>= 1;
		}
		return nCount;

	}
	BOOL Create( const char* pszFilename, BOOL* pbProcess );

public:
	//std::string m_strFilename;		// 文件名
	char		m_szCRC[MAX_PATH];
	int			m_nIsZip;
	char        m_szZipFileName[MAX_PATH];
	char		m_szZipFilePath[MAX_PATH];
	char		m_szFilePath[MAX_PATH];
	char		m_szFilename[MAX_PATH];
	DWORD		m_dwSize;			// 文件大小
	DWORD		m_dwCheckSum;		// 文件所有byte的和
	DWORD		m_dwBitSum;			// 所有的bit的和

};

class CFileDescMgr
{
public:
	CFileDescMgr(){};
	~CFileDescMgr();

	void	Destroy();
	BOOL	CreateFileList( const char* pszFilename, std::vector<CFileDesc>& files );
protected:
	std::vector<CFileDesc*> m_files;
};
int LoadFileToMemory( const char* pszFilename, BYTE** ppBuffer );
void ReleaseFileMemory( BYTE** ppBuffer );
#endif