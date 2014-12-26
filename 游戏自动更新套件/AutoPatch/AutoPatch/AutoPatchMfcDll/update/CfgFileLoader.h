#pragma once
#include "MapStrToID.h"
#include "ScriptParser.h"
#include <vector>
using namespace std;
class CCfgFileLoader
{
public:
	struct Session
	{
		std::string strName;
		CMapStrToIDMgr	m_wordsMgr;
		std::vector<CScriptWords2*> m_vectorWords;
	};
public:
	CCfgFileLoader(void);
	~CCfgFileLoader(void);

	BOOL	LoadFromFile( const char* pszFilename );
	BOOL    LoadEncryptFile(const char* pszFilename );
	int		ReadLine( char* pchLine, char szBuffer[], int nBufferSize );
	BOOL	LoadFileFromMemory( BYTE* pbyBuffer, int nBufferSize );
	CScriptWords2* GetWords( const char* pszName );
	
	BOOL	OpenSession( const char* pszSessionName );
	BOOL	OpenSession( int i );
	CMapStrToIDMgr* GetSessionMgr(){ return &m_sessionMgr; }

	const char* GetStringValue( const char* pszName );
	BOOL	GetStringValue( const char* pszName, const char* pszDefault, char szBuffer[], int nBufferSize );
	int		GetIntValue( const char* pszName );
	float	GetFloatValue( const char* pszName );
	BOOL	HasKey( const char* pszName );

	const char* GetLineStringValue( int nLine );
	const char* GetLineKeyName( int nLine );

	int		GetSessionCount(){ return m_sessions.size(); }

	int		GetWordCount();

	int		GetCurrentSessionLineCount();
	const char* GetCurrentSessionName();
	
protected:
	std::vector<CScriptWords2*> m_vectorWords;
	Session*		m_pCurrentSession;
	CMapStrToIDMgr	m_sessionMgr;
	std::vector<Session*> m_sessions;
};
