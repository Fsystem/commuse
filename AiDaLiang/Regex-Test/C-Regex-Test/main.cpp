#include "stdafx.h"
#include <WindowsX.h>

#include "regex/regex.h"

extern HWND gMainWnd = NULL;

#define GET_CTRL_HWND(ID) GetDlgItem(hwnd,ID)
//-------------------------------------------------------------------------------
void CRegexSearch(std::string sPartten,std::string sContent)
{

}
//-------------------------------------------------------------------------------
BOOL Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	gMainWnd = hwnd;

	HWND hEdContent = GetDlgItem(hwnd,IDC_EDIT_CONTENT);
	//SetWindowLong(hEdContent,GWL_STYLE,GetWindowLong(hEdContent,GWL_STYLE)|ES_MULTILINE);
	std::string sPattern = "<a(?: [^>]*)+href=([^ >]*)(?: [^>]*)*>";
	std::string sHttp = "<td width=\"95\" class=\"wskf1_wz4\"><a href=\"http://go.sf999.ws/goto.asp?url=185.lanyueol.com\" target=\"_blank\" rel=\"nofollow\">点击查看</a></td>"
		"\r\n<td width=\"95\" class=\"wskf1_wz4\"><a href=\"http://go.sf999.ws/goto.asp?url=190.lanyueol.com\" target=\"_blank\" rel=\"nofollow\">点击查看</a></td>";

	SetWindowTextA(GET_CTRL_HWND(IDC_EDIT_CONTENT),sHttp.c_str());
	SetWindowTextA(GET_CTRL_HWND(IDC_EDIT_REGEX),sPattern.c_str());

	return TRUE;
}

void OnButtonOKClick(HWND hwnd,int id , HWND hwndCtrl , UINT codeNotify)
{
	HWND hEdCon = GET_CTRL_HWND(IDC_EDIT_CONTENT);
	HWND hEdResult = GET_CTRL_HWND(IDC_EDIT_RESULT);
	HWND hEdRegex = GET_CTRL_HWND(IDC_EDIT_REGEX);

	int nLen = 8192*10;
	char* szContent = new char[nLen];
	char szRegPattern[1024];
	szRegPattern[1023] = 0;
	if (szContent)
	{
		szContent[nLen-1] = 0;
		GetWindowTextA(hEdCon,szContent,nLen-1);
		GetWindowTextA(hEdRegex,szRegPattern,1023);

		RegOpt rg;
		auto vecRes = rg.GetRegexResult(szRegPattern,szContent);

		std::string sResult;
		for (auto it = vecRes.begin();it!=vecRes.end();it++)
		{
			sResult +=*it;
			sResult += "\r\n";
		}

		SetWindowTextA(hEdResult,sResult.c_str());

		delete[] szContent;
	}

}

void Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id)
	{
	case IDOK:
		{
			OnButtonOKClick(hwnd,id,hwndCtl,codeNotify);
			break;
		}
	case IDCANCEL:
		{
			EndDialog(hwnd,id);
			break;
		}
	}
}

INT_PTR CALLBACK DlgCallBack(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
		HANDLE_MSG(hDlg,WM_INITDIALOG,Cls_OnInitDialog);return TRUE;
		HANDLE_MSG(hDlg,WM_COMMAND,Cls_OnCommand);return TRUE;
	}
	return 0L;
}

int WINAPI _tWinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPTSTR lpCmdLine, __in int nShowCmd )
{
	DialogBox(hInstance,MAKEINTRESOURCE(IDD_DLG_MAIN),NULL,DlgCallBack);
	
	return 0;
}