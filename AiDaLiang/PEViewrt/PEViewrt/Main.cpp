#include "stdafx.h"

//-------------------------------------------------------------------------------
INT APIENTRY _tWinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPTSTR lpCmdLine, __in int nShowCmd )
{
	if (__argc>2)
	{
		TCHAR szFile[MAX_PATH]={0};
		TCHAR szError[1024]={0};
		_tcscpy(szFile,__targv[1]);
		PEHelper pe;
		wcsncpy(pe.m_AgentId,T2WString(__targv[2]).c_str(),CountArr(pe.m_AgentId)); 
		if(!pe.LoadPeFile(szFile,szError))
		{
			OutputDebugString(szError);
		}

		return 0;
	}
	MainDlg wnd;
	
	wnd.Show(IDD_PEVIEWRT_DIALOG);
	

	//theJKApp.RunLoop();

	return 0;
}