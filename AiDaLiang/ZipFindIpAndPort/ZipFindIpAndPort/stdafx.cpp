// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// ZipFindIpAndPort.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"

// TODO: �� STDAFX.H ��
// �����κ�����ĸ���ͷ�ļ����������ڴ��ļ�������


void SendResultNotify(LPCSTR szFile,LPCSTR szFileName,DWORD dwResultCnt)
{
	IPC_Data data;
	memset(&data,0,sizeof(data));
	data.nResultCnt = dwResultCnt;
	if(szFile) strncpy(data.szFile,szFile,MAX_PATH);
	if(szFileName) strncpy(data.szRetFile,szFileName,MAX_PATH);
	data.bProcessed = dwResultCnt>0?true:false;

	COPYDATASTRUCT cds;
	cds.dwData = 1;
	cds.lpData = &data;
	cds.cbData = sizeof(data);

	if (gAnalysisMode == 0)
	{
		if (gParantHwnd)
		{
			::SendMessage(gParantHwnd,WM_COPYDATA,(WPARAM)gMainHwnd,(LPARAM)&cds);
		}
	}
	else
	{
		::SendMessage(gMainHwnd,WM_COPYDATA,(WPARAM)gMainHwnd,(LPARAM)&cds);
	}
}