#include "stdafx.h"
#include "MainDlg.h"

void MainDlg::OnInitDialog()
{
	hTree = GetCtrl(IDC_TREE1);
	memset(szFileName,0,sizeof szFileName);
}

LRESULT MainDlg::OnDropFiles(HDROP hDrop)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//TCHAR szFile[MAX_PATH]={0};
	TCHAR szError[1024]={0};
	::DragQueryFile(hDrop,0,szFileName,MAX_PATH);
	PEHelper pe;
	pe.m_tree = hTree;
	if(!pe.LoadPeFile(szFileName,szError))
	{
		MessageBox(mHwnd,szError,NULL,MB_OK);
	}

	return __super::OnDropFiles(hDrop);
}

LRESULT MainDlg::OnCommand( WPARAM w,LPARAM l )
{
	switch(w)
	{
	case IDC_BN_MODIFY_AGENTID:
		{
			TCHAR szAgentId[MAX_PATH];
			if (szFileName[0] == 0)
			{
				MessageBox(mHwnd,TEXT("请拖拽文件到此"),TEXT("提示"),64);
				return 0L;
			}

			GetWindowText(GetCtrl(IDC_EDIT_AGENTID),szAgentId,MAX_PATH);

			if (_tcslen(szAgentId)<3)
			{
				MessageBox(mHwnd,TEXT("agenid错误"),TEXT("提示"),64);
				return 0L;
			}
			
			TCHAR szError[1024]={0};
			
			PEHelper pe;
			wcsncpy(pe.m_AgentId,T2WString(szAgentId).c_str(),CountArr(pe.m_AgentId)); 
			if(!pe.LoadPeFile(szFileName,szError))
			{
				OutputDebugString(szError);
			}
			break;
		}
	}

	return 0L;
}
