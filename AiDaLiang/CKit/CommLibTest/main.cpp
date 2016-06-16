#include "stdafx.h"
#include <tchar.h>
#include <WindowsX.h>
#include <Windows.h>

#include "resource.h"

#include "FileMonitor/FileMonitor.h"

class Sink : public IFileMonitorDelegate
{
	HWND mMainDlg;
public: 
	BOOL start(HWND mainDlg)
	{
		mMainDlg = mainDlg;
		FileMonMgr::Instance().StartService(TRUE,TRUE,this);
		return TRUE;
	}

	virtual void OnFileChange(LPCSTR lpszFile,DWORD dwAction)
	{
		char szTmp[MAX_PATH];
		if(dwAction == FILE_ACTION_ADDED)
		{//创建
			sprintf(szTmp,"文件创建:%s\n",lpszFile);
		}
		else if (dwAction == FILE_ACTION_MODIFIED)
		{//修改
			sprintf(szTmp,"文件修改:%s\n",lpszFile);
		}
		else if (dwAction == FILE_ACTION_REMOVED)
		{//修改
			sprintf(szTmp,"文件删除:%s\n",lpszFile);
		}
		else if (dwAction == FILE_ACTION_RENAMED_OLD_NAME)
		{//修改
			sprintf(szTmp,"文件重命名:%s\n",lpszFile);
		}
		else if (dwAction == FILE_ACTION_RENAMED_NEW_NAME)
		{//修改
			sprintf(szTmp,"文件重命名(新):%s\n",lpszFile);
		}
		else
		{
			sprintf(szTmp,"未知文件操作(%d):%s\n",dwAction,lpszFile);
		}

		TSTD_STING tszTmp = A2TString(szTmp);
		ListBox_InsertString(GetDlgItem(mMainDlg,IDC_LIST1),ListBox_GetCount(GetDlgItem(mMainDlg,IDC_LIST1)),tszTmp.c_str());
	}
};

Sink gSink;

LRESULT OnFileChange(HWND hWnd, LPCSTR lpszFile,DWORD dwAction)
{
	char szTmp[MAX_PATH];
	if(dwAction == FILE_ACTION_ADDED)
	{//创建
		sprintf(szTmp,"文件创建:%s\n",lpszFile);
	}
	else if (dwAction == FILE_ACTION_MODIFIED)
	{//修改
		sprintf(szTmp,"文件修改:%s\n",lpszFile);
	}
	else if (dwAction == FILE_ACTION_REMOVED)
	{//修改
		sprintf(szTmp,"文件删除:%s\n",lpszFile);
	}
	else if (dwAction == FILE_ACTION_RENAMED_OLD_NAME)
	{//修改
		sprintf(szTmp,"文件重命名:%s\n",lpszFile);
	}
	else if (dwAction == FILE_ACTION_RENAMED_NEW_NAME)
	{//修改
		sprintf(szTmp,"文件重命名(新):%s\n",lpszFile);
	}
	else
	{
		sprintf(szTmp,"未知文件操作(%d):%s\n",dwAction,lpszFile);
	}

	TSTD_STING tszTmp = A2TString(szTmp);
	ListBox_InsertString(GetDlgItem(hWnd,IDC_LIST1),ListBox_GetCount(GetDlgItem(hWnd,IDC_LIST1)),tszTmp.c_str());

	return 0L;
}


void OnOk(HWND hWnd, int id, HWND hWndCtl, UINT codeNotify)
{
	gSink.start(hWnd);
}


BOOL Dlg_OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam)
{
	hMainWnd = hWnd;
	return TRUE;
}

void Dlg_OnCommand(HWND hWnd, int id, HWND hWndCtl, UINT codeNotify)
{
	switch(id)
	{
	case IDCANCEL:
		PostQuitMessage(0);
		break;
	case IDOK:
		OnOk(hWnd,id,hWndCtl,codeNotify);
		break;
	}
}

INT_PTR WINAPI Dlg_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	switch (uMsg) {
		HANDLE_MSG(hWnd, WM_INITDIALOG, Dlg_OnInitDialog);
		HANDLE_MSG(hWnd, WM_COMMAND,    Dlg_OnCommand);
	case WM_USER+1:
		OnFileChange(hWnd,(LPCSTR)wParam,(DWORD)lParam);
		break;
	}

	return(FALSE);
}

int WINAPI _tWinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPTSTR lpCmdLine, __in int nShowCmd )
{
	//测试插件
	PlugManager::Instance().LoadAllPlugin();

	while(!PlugManager::Instance().IsPlugRunning())Sleep(1000);

	PlugManager::Instance().OnHandleMessage(110,"你好hello",9);

	SetPrivilege();

	LOGEVENA("asdasdasd");
	DialogBox(hInstance,MAKEINTRESOURCE(IDD_DIALOG1),NULL,Dlg_Proc);
	return 0;
}

//
//TCHAR sz[] = TEXT("asdasd你好爱上大时代asdas,./");
//std::string a = T2AString(sz);
//std::wstring b = T2WString(sz);
////a = W2AString(sz);
////TSTD_STING pw = W2TString(sz);
//
//
//char sz1[] = "asdasd你好爱上大时代asdas,./好";
//std::wstring d = A2WString(sz1);
//TSTD_STING pc = A2TString(sz1);
//
//char* pFind = stristr("你好hahad好是j asdjl","是");
//pFind = stristr("你好hahad好是j asdjl","是j");
//pFind = stristr("你好hahad好是j asdjl","asD");
//
//WCHAR* pFindw = wcsistr(L"你好hahad好是j asdjl",L"是");
//pFindw = wcsistr(L"你好hahad好是j asdjl",L"是J");
//pFindw = wcsistr(L"你好hahad好是j asdjl",L"aSD");