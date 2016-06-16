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
		{//����
			sprintf(szTmp,"�ļ�����:%s\n",lpszFile);
		}
		else if (dwAction == FILE_ACTION_MODIFIED)
		{//�޸�
			sprintf(szTmp,"�ļ��޸�:%s\n",lpszFile);
		}
		else if (dwAction == FILE_ACTION_REMOVED)
		{//�޸�
			sprintf(szTmp,"�ļ�ɾ��:%s\n",lpszFile);
		}
		else if (dwAction == FILE_ACTION_RENAMED_OLD_NAME)
		{//�޸�
			sprintf(szTmp,"�ļ�������:%s\n",lpszFile);
		}
		else if (dwAction == FILE_ACTION_RENAMED_NEW_NAME)
		{//�޸�
			sprintf(szTmp,"�ļ�������(��):%s\n",lpszFile);
		}
		else
		{
			sprintf(szTmp,"δ֪�ļ�����(%d):%s\n",dwAction,lpszFile);
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
	{//����
		sprintf(szTmp,"�ļ�����:%s\n",lpszFile);
	}
	else if (dwAction == FILE_ACTION_MODIFIED)
	{//�޸�
		sprintf(szTmp,"�ļ��޸�:%s\n",lpszFile);
	}
	else if (dwAction == FILE_ACTION_REMOVED)
	{//�޸�
		sprintf(szTmp,"�ļ�ɾ��:%s\n",lpszFile);
	}
	else if (dwAction == FILE_ACTION_RENAMED_OLD_NAME)
	{//�޸�
		sprintf(szTmp,"�ļ�������:%s\n",lpszFile);
	}
	else if (dwAction == FILE_ACTION_RENAMED_NEW_NAME)
	{//�޸�
		sprintf(szTmp,"�ļ�������(��):%s\n",lpszFile);
	}
	else
	{
		sprintf(szTmp,"δ֪�ļ�����(%d):%s\n",dwAction,lpszFile);
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
	//���Բ��
	PlugManager::Instance().LoadAllPlugin();

	while(!PlugManager::Instance().IsPlugRunning())Sleep(1000);

	PlugManager::Instance().OnHandleMessage(110,"���hello",9);

	SetPrivilege();

	LOGEVENA("asdasdasd");
	DialogBox(hInstance,MAKEINTRESOURCE(IDD_DIALOG1),NULL,Dlg_Proc);
	return 0;
}

//
//TCHAR sz[] = TEXT("asdasd��ð��ϴ�ʱ��asdas,./");
//std::string a = T2AString(sz);
//std::wstring b = T2WString(sz);
////a = W2AString(sz);
////TSTD_STING pw = W2TString(sz);
//
//
//char sz1[] = "asdasd��ð��ϴ�ʱ��asdas,./��";
//std::wstring d = A2WString(sz1);
//TSTD_STING pc = A2TString(sz1);
//
//char* pFind = stristr("���hahad����j asdjl","��");
//pFind = stristr("���hahad����j asdjl","��j");
//pFind = stristr("���hahad����j asdjl","asD");
//
//WCHAR* pFindw = wcsistr(L"���hahad����j asdjl",L"��");
//pFindw = wcsistr(L"���hahad����j asdjl",L"��J");
//pFindw = wcsistr(L"���hahad����j asdjl",L"aSD");