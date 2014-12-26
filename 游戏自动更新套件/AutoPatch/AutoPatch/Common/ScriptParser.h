#ifndef _scriptparser_h
#define _scriptparser_h
#include <windows.h>
#include <vector>

class CScriptWords2 
{
	enum
	{
		eMaxWord = 32
	};
	struct OneWord
	{
		int		nStart;
		int		nLen;
		char*	pszStart;
		OneWord():nStart(0),nLen(0),pszStart(NULL){}
	};
	
public:
	CScriptWords2();
	~CScriptWords2();
	int		Parse( const char* pszScriptLine );
	//int		Parse2( const char* pszScriptLine );
	//int		GetWordCount(){ return (int)m_words.size(); }
	int		GetWordCount();
	const char*	GetWord( int i );
	static BOOL	IsNumber( const char* s );
	BOOL	IsNumber( int i );
	BOOL	Compare( int i, const char* s );
	//void	CombineWord1ToEnd( char* pszBuffer, int nBufferSize );
	BOOL	CombineWordFromXToEnd( int x, char* pszBuffer, int nBufferSize );
	BOOL	CreateFrom( int nStart, CScriptWords2* pSrcWord );
protected:
	//std::vector<std::string> m_words; 
	OneWord	m_words2[eMaxWord];
	int		m_nNumWord;
	char*	m_pszString;
	int		m_nLength;
};
#endif