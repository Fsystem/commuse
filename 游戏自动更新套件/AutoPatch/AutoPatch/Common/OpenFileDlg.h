#ifndef _openfiledlg_h
#define _openfiledlg_h
#include <windows.h>
char* OpenFileDialog( const char* filter, const char* ext );
char *BrowseBox( HWND hWnd, char *s, int length ); 
#endif