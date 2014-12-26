#include <windows.h>
#include <tchar.h>
#include <shlobj.h>
#include "openfiledlg.h"
// #include "wkCore.h"

//#define new WKNEW
char* OpenFileDialog( const char* filter, const char* ext )
{
	OPENFILENAME ofn;
	memset( &ofn, 0, sizeof(ofn) );
	static TCHAR file[256];
	static TCHAR szFilepath[256];
	static TCHAR fileTitle[256];

	_tcscpy( file, TEXT(""));
	_tcscpy( fileTitle, TEXT(""));

	ofn.lStructSize       = sizeof(ofn);
	ofn.hwndOwner         = NULL;
	ofn.hInstance         = NULL;//m_hInstance;
	ofn.lpstrFilter       = filter;
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter    = 0L;
	ofn.nFilterIndex      = 1L;
	ofn.lpstrFile         = file;
	ofn.nMaxFile          = sizeof(file);
	ofn.lpstrFileTitle    = fileTitle;
	ofn.nMaxFileTitle     = sizeof(fileTitle);
	ofn.lpstrInitialDir   = NULL;
	ofn.nFileOffset       = 0;
	ofn.nFileExtension    = 0;
	ofn.lpstrDefExt       = TEXT(ext);
	ofn.lCustData         = 0;

	//ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
	if ( ! GetOpenFileName( &ofn) )
	{
		TCHAR s[40];
		DWORD dwErr = CommDlgExtendedError();
		if ( 0 != dwErr )
		{
			wsprintf( s, "GetOpenFileName failed with %x", dwErr );
			MessageBox( NULL, s, "TexWin", MB_OK | MB_SYSTEMMODAL );
		}
		return 0;
	}
	return ofn.lpstrFile;

}

char *BrowseBox( HWND hWnd, char *s, int length ) 
{
	if( length < MAX_PATH )
		return NULL;

	//char processdir[MAX_PATH];
	HWND hwndOwner = hWnd;
	LPCSTR  lpszTitle = "BrowseBox";
	//CString strSelIniDir;
	//CString strDirPathSelected;
	char strSelIniDir[MAX_PATH] = "";
	char strDirPathSelected[MAX_PATH] = "";

	/* Work's only if we're 95 capable or up*/ 
	BOOL bRe = FALSE;

	LPMALLOC pMalloc; 

	BROWSEINFO bi; 
	TCHAR pszBuffer[MAX_PATH];

	LPITEMIDLIST pidl = NULL;
	LPITEMIDLIST pidlIni = NULL;

	LPSHELLFOLDER ppshf = NULL;

	OLECHAR szOleChar[MAX_PATH];
	ULONG ulEaten, ulAttribs;

	//if(strSelIniDir.IsEmpty() == FALSE)
	{
		/* Get's the Shell's default allocator */ 
		if (SHGetMalloc(&pMalloc) != NOERROR) 
		{ 
			goto crash;
		}
		// Get's the Shell's desktop folder Interface
		if(! SUCCEEDED(SHGetDesktopFolder(&ppshf)) )
		{
			goto crash;
		}
		// Conver to UNICODE string
		MultiByteToWideChar(CP_ACP,
			MB_PRECOMPOSED,
			strSelIniDir,
			-1,
			szOleChar,
			sizeof(szOleChar));
		// 
		if(! SUCCEEDED(ppshf->ParseDisplayName(hwndOwner,NULL,szOleChar,
			&ulEaten,&pidlIni,&ulAttribs)))
			goto crash;
	}

	// Get help on BROWSEINFO struct - it's got all the bit settings 
	bi.hwndOwner = hwndOwner; 
	bi.pidlRoot = pidlIni;
	bi.pszDisplayName = pszBuffer; 
	bi.lpszTitle = lpszTitle; 
	bi.ulFlags = BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS 
		| BIF_RETURNONLYFSDIRS ; 
	bi.lpfn = NULL; bi.lParam = 0; 

	// This next call issues the dialog box
	if ((pidl = SHBrowseForFolder(&bi)) != NULL) 
	{ 
		if (SHGetPathFromIDList(pidl, pszBuffer)) 
		{ 
			//At this point pszBuffer contains the selected path 

			//strDirPathSelected = pszBuffer;
			strcpy( strDirPathSelected, pszBuffer );
			strcpy(s,pszBuffer);
			strcat(s,"\\");
		} 

		bRe = TRUE;
	} 

	return s;
crash:
	return NULL;
}



