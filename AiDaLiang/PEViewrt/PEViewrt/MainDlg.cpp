#include "stdafx.h"
#include "MainDlg.h"

void MainDlg::OnInitDialog()
{
	hTree = GetCtrl(IDC_TREE1);
}

LRESULT MainDlg::OnDropFiles(HDROP hDrop)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	TCHAR szFile[MAX_PATH]={0};
	TCHAR szError[1024]={0};
	::DragQueryFile(hDrop,0,szFile,MAX_PATH);
	PEHelper pe;
	pe.m_tree = hTree;
	if(!pe.LoadPeFile(szFile,szError))
	{
		MessageBox(mHwnd,szError,NULL,MB_OK);
	}

	return __super::OnDropFiles(hDrop);
}