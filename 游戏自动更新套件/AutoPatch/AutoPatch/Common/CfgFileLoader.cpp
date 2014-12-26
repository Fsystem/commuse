#include "cfgfileloader.h"
#include "helper.h"
#include "FuncPerformanceLog.h"
#include "FilePath.h"
CCfgFileLoader::CCfgFileLoader(void)
{
	guardfunc;
	m_pCurrentSession = NULL;

	unguard;
}

CCfgFileLoader::~CCfgFileLoader(void)
{
	guardfunc;
	for( int i = 0; i < m_vectorWords.size(); i++ )
	{
		delete m_vectorWords[i];
	}
	for( int i = 0; i < m_sessions.size(); i++ )
	{
		delete m_sessions[i];
	}
	unguard;
}

BOOL CCfgFileLoader::OpenSession( const char* pszSessionName )
{
	guardfunc;
	m_pCurrentSession = (Session*)GetSessionMgr()->GetDstByName( pszSessionName );
	if( m_pCurrentSession )
		return TRUE;
	return FALSE;
	unguard;
}
BOOL CCfgFileLoader::OpenSession( int i )
{
	guardfunc;
	if( i < 0 || i > m_sessions.size() )
		return FALSE;
	m_pCurrentSession = m_sessions[i];
	return TRUE;
	unguard;
}
CScriptWords2* CCfgFileLoader::GetWords( const char* pszName )
{
	guardfunc;
	if( m_pCurrentSession )
		return (CScriptWords2*)m_pCurrentSession->m_wordsMgr.GetDstByName( pszName );
	return NULL;
	unguard;
}
int CCfgFileLoader::GetCurrentSessionLineCount()
{
	guardfunc;
	if( !m_pCurrentSession )
		return 0;
	return m_pCurrentSession->m_vectorWords.size();
	unguard;
}
const char* CCfgFileLoader::GetCurrentSessionName()
{
	guardfunc;
	if( !m_pCurrentSession )
		return "";
	return m_pCurrentSession->strName.c_str();
	unguard;
}
const char* CCfgFileLoader::GetLineKeyName( int nLine )
{
	guardfunc;
	if( !m_pCurrentSession )
		return "";

	if( nLine < 0 || nLine > m_pCurrentSession->m_vectorWords.size() )
		return "";
	return m_pCurrentSession->m_vectorWords[nLine]->GetWord( 0 );
	unguard;
}
const char* CCfgFileLoader::GetLineStringValue( int nLine )
{
	guardfunc;
	if( !m_pCurrentSession )
		return "";
	
	if( nLine < 0 || nLine > m_pCurrentSession->m_vectorWords.size() )
		return "";
	return m_pCurrentSession->m_vectorWords[nLine]->GetWord( 1 );
	unguard;
}
BOOL CCfgFileLoader::HasKey( const char* pszName )
{
	guardfunc;
	CScriptWords2* words = GetWords( pszName );
	if( !words )
		return FALSE;
	if( words->GetWord(1)[0] == 0 )
		return FALSE;
	return TRUE;
	unguard;
}
BOOL CCfgFileLoader::GetStringValue( const char* pszName, const char* pszDefault, char szBuffer[], int nBufferSize )
{
	guardfunc;
	if( HasKey( pszName ) )
	{
		HelperFunc::SafeNCpy( szBuffer, GetStringValue( pszName ), nBufferSize );
		return TRUE;
	}
	HelperFunc::SafeNCpy( szBuffer, pszDefault, nBufferSize );
	return FALSE;
	unguard;
}
const char* CCfgFileLoader::GetStringValue( const char* pszName )
{
	guardfunc;
	CScriptWords2* words = GetWords( pszName );
	if( !words )
		return "";
	return words->GetWord( 1 );
	unguard;
}
int	CCfgFileLoader::GetIntValue( const char* pszName )
{
	guardfunc;
	return atoi( GetStringValue( pszName ) );
	unguard;
}
float CCfgFileLoader::GetFloatValue( const char* pszName )
{
	guardfunc;
	return atof( GetStringValue( pszName ) );
	unguard;
}
int	CCfgFileLoader::ReadLine( char* pchLine, char szBuffer[], int nBufferSize )
{
	guardfunc;
	int i = 0;
	for( i = 0; i < nBufferSize; i++ )
	{
		if( pchLine[i] == 0 )
			break;
		if(	pchLine[i] == '\n' ||
			pchLine[i] == '\r' )
		{
			i++;
			break;
		}
		szBuffer[i] = pchLine[i];
	}
	szBuffer[i] = 0;
	return i;
	unguard;
}
BOOL CCfgFileLoader::LoadFileFromMemory( BYTE* pbyBuffer, int nBufferSize )
{
	guardfunc;
	int nOffset = 0;
	char buf[1024];
	while( true )
	//for( )
	{
		memset( buf, 0x00, sizeof(buf) );
		int nLen = ReadLine( (char*)&pbyBuffer[nOffset], buf, 1024 );
		if( nLen == 0 )
			break;
		nOffset += nLen+1;
		//int nLen = strlen( buf );
		for( int i = 0; i < nLen; i++ )
		{
			if( buf[i] == '=' )
				buf[i] = ' ';
		}
		CScriptWords2* words = new CScriptWords2;
		if( words->Parse( buf ) == 0 )
		{
			delete words;
			continue;
		}
		m_vectorWords.push_back( words );
		char s[256] = {0};
		strncpy(s, words->GetWord(0), sizeof(s)-1);
		BOOL bNewSession = FALSE;
		if( s[0] == '[' &&
			s[strlen(s)-1] == ']' )
		{
			// 去掉左右两个[]
			s[strlen(s)-1] = 0;
			char* pszSessionName = &s[1];
			Session* pSess = (Session*)m_sessionMgr.GetDstByName( pszSessionName );
			if( !pSess )
			{
				pSess = new Session;
				pSess->strName = pszSessionName;
				m_sessionMgr.Register( pszSessionName, pSess );
				m_sessions.push_back( pSess );
				m_pCurrentSession = pSess;
				bNewSession = TRUE;
			}
			else
			{
				pSess = new Session;
				pSess->strName = pszSessionName;
				//m_sessionMgr.Register( pszSessionName, pSess );
				m_sessions.push_back( pSess );
				m_pCurrentSession = pSess;
				bNewSession = TRUE;
			}
		}
		if( m_pCurrentSession )
		{
			if( words && !bNewSession )
			{
				m_pCurrentSession->m_wordsMgr.Register( words->GetWord( 0 ), words );
				//if( m_pCurrentSession->m_vectorWords.size() > 0 )

				m_pCurrentSession->m_vectorWords.push_back( words );
			}
		}
	}
	//fclose( fp );

	m_pCurrentSession = NULL;
	OpenSession( "main" );
	return TRUE;
	unguard;
}
BOOL CCfgFileLoader::LoadFromFile( const char* pszFilename )
{
	guardfunc;
	//BYTE* pbyBuffer = NULL;
	//int nFileSize = ::LoadFileToMemory( pszFilename, &pbyBuffer );
	//if( nFileSize == 0 )
	//	return FALSE;
	//BOOL bSucc = LoadFileFromMemory( pbyBuffer, nFileSize );
	//if( pbyBuffer )
	//	delete[] pbyBuffer;
	//return bSucc;
	FILE* fp = fopen( pszFilename, "r" );
	if( !fp )
		return FALSE;
	char buf[1024];
	while( fgets( buf, 1024, fp ) )
	{
		int nLen = strlen( buf );
		for( int i = 0; i < nLen; i++ )
		{
			if( buf[i] == '=' )
				buf[i] = ' ';
		}
		CScriptWords2* words = new CScriptWords2;
		if( words->Parse( buf ) == 0 )
		{
			delete words;
			continue;
		}
		m_vectorWords.push_back( words );
        char s[256] = {0};
		strncpy(s, words->GetWord(0), sizeof(s)-1);
		BOOL bNewSession = FALSE;
		if( s[0] == '[' &&
			s[strlen(s)-1] == ']' )
		{
			// 去掉左右两个[]
			s[strlen(s)-1] = 0;
			char* pszSessionName = &s[1];
			Session* pSess = (Session*)m_sessionMgr.GetDstByName( pszSessionName );
			if( !pSess )
			{
				pSess = new Session;
				pSess->strName = pszSessionName;
				m_sessionMgr.Register( pszSessionName, pSess );
				m_sessions.push_back( pSess );
				m_pCurrentSession = pSess;
				bNewSession = TRUE;
			}
			else
			{
				pSess = new Session;
				pSess->strName = pszSessionName;
				//m_sessionMgr.Register( pszSessionName, pSess );
				m_sessions.push_back( pSess );
				m_pCurrentSession = pSess;
				bNewSession = TRUE;
			}
		}
		if( m_pCurrentSession )
		{
			if( words && !bNewSession )
			{
				m_pCurrentSession->m_wordsMgr.Register( words->GetWord( 0 ), words );
				//if( m_pCurrentSession->m_vectorWords.size() > 0 )
				
				m_pCurrentSession->m_vectorWords.push_back( words );
			}
		}
	}
	fclose( fp );

	m_pCurrentSession = NULL;
	OpenSession( "main" );
	return TRUE;
	unguard;

}
BOOL CCfgFileLoader::LoadEncryptFile(const char* pszFilename )
{
	guardfunc;
	const char szDstFile[] = "temptjol.ini";
	long length = 0;
	FILE *fp = NULL;
	fp = fopen( pszFilename, "r" );

	if(!fp)
	{
		MessageBox( NULL, "当前目录中[cfg2d.ini]文件不存在!", "错误", MB_OK );
		return FALSE;
	}
	fseek( fp, 0, SEEK_END );
	length = ftell( fp );
	fseek( fp, 0, SEEK_SET );

	// Strange case, but good to handle up front.
	if ( length == 0 )
	{
		fclose( fp );
		MessageBox( NULL, "该文件没任何内容！", "错误", MB_OK );
		return FALSE ;
	}

	char* buf = new char[ length+1 ];
	buf[0] = 0;

	fread( buf, length, 1, fp );
	fclose( fp );
	for ( int n=0; n<length; n++ )
	{
		buf[n] ^= 0x12;
	}

	// write

	fp = fopen( szDstFile, "wb" );
	if ( fp )
	{
		fwrite( buf, length, 1, fp );
	}
	else
	{
		MessageBox( NULL, "目标文件属性只读，请更改属性！", "错误", MB_OK );
		return FALSE;
	}
	fclose(fp);
	CCfgFileLoader::LoadFromFile( szDstFile);
	DeleteFile(szDstFile);
	return TRUE;
	unguard;
}