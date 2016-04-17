#include <stdafx.h>
#include "GuardProtectDriver.h"
#include "../StagetyInclude.h"

#define	CHECK_PROCESS_NAME	TEXT("aqcheck.exe")

GuardProtectDriver::GuardProtectDriver()
{
	m_bRunThread = false;
	StopGuard();
}

GuardProtectDriver::~GuardProtectDriver()
{
	StopGuard();
}

void GuardProtectDriver::StartGuard()
{
	if(m_bRunThread) StopGuard();

	m_bInCheckDriverRunning = false;
	m_dwCheckProcessId = 0;
	//m_dwCheckCount = 0;
	m_bIncallback = false;

	m_bRunThread = true;
	JKThread::Start(&GuardProtectDriver::GuardThread,this);
}

void GuardProtectDriver::StopGuard()
{
	m_bInCheckDriverRunning = false;
	m_dwCheckProcessId = 0;
	//m_dwCheckCount = 0;
	m_bIncallback = false;

	if (m_bRunThread)
	{
		m_bRunThread = false;
		Sleep(100);
	}
}

//���������ص�
void GuardProtectDriver::IntoDriver()
{
	m_bIncallback = true;
}

//�����ص�ע����
bool GuardProtectDriver::InjectionCheck(DWORD dwParantProcessId,DWORD dwChildProcessId)
{
	bool bTip = false;

	if(m_bInCheckDriverRunning)
	{
		DWORD dwMainProcessId = ::GetCurrentProcessId();

		if(dwChildProcessId == m_dwCheckProcessId && dwParantProcessId == dwMainProcessId)
		{
			bTip = true;
		}
		else if(dwChildProcessId == dwMainProcessId && dwParantProcessId == m_dwCheckProcessId)
		{
			bTip = true;
		}
		if(bTip)
		{
			m_bInCheckDriverRunning = false;
			LOGEVEN(TEXT(">>>�����ػ��̣߳������ɣ���������\n") );
		}
	}

	return bTip;
}

//�뿪�����ص�
void GuardProtectDriver::OutDriver()
{
	m_bIncallback = false;
}

void GuardProtectDriver::GuardThread(void* pThis)
{
	GuardProtectDriver* mThis = (GuardProtectDriver*)pThis;

	DWORD dwMainProcessId = ::GetCurrentProcessId();
	TCHAR szPath[MAX_PATH]={0};
	::GetModuleFileName(GetModuleHandle(NULL),szPath,MAX_PATH);
	_tcsrchr(szPath,'\\')[0]=0;

	TCHAR strFile[MAX_PATH] = {0};
	_sntprintf(strFile,MAX_PATH,TEXT("%s/%s"),szPath,CHECK_PROCESS_NAME);
	CheckAndExportFile(TEXT("EXE"), strFile, IDR_AQCheck);	

	while(mThis->m_bRunThread)
	{
		LOGEVEN(TEXT(">>>�ػ�������������ά����...\n"));

		try
		{
			if(mThis == NULL) throw 0;

			mThis->CheckDriver(dwMainProcessId,szPath);
		}
		catch(...)
		{
			LOGEVEN(TEXT(">>>exception:GuardProtectDriver::GuardThread\n"));
		}

		Sleep(5000);
	}

	JKThread::Stop();
}

void GuardProtectDriver::CheckDriver(DWORD dwCurProcessId,TCHAR * szPath)
{
	//�������ص�������,��ʾ������������
	if(m_bIncallback) return;

	if(m_bInCheckDriverRunning)
	{
		PROTECTOR_ENGINE.ReloadProtectDriver();
		m_bInCheckDriverRunning = false;
	}
	else
	{
		m_bInCheckDriverRunning = true;

		TCHAR szFilePath[MAX_PATH]={0};
		_sntprintf(szFilePath,MAX_PATH,TEXT("%s/%s %u"),szPath, CHECK_PROCESS_NAME, dwCurProcessId);

		PROCESS_INFORMATION processInformation = {0};
		STARTUPINFO startupInfo                = {0};
		startupInfo.cb                         = sizeof(startupInfo);

		// Create the process
		BOOL result = CreateProcess(NULL, szFilePath, 
			NULL, NULL, FALSE, 
			NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, 
			NULL, NULL, &startupInfo, &processInformation);
		if(!result)
		{
			LOGEVEN(TEXT(">>>�����ػ��̣߳�����������ʧ��.\n"));
			return;
		}

		m_dwCheckProcessId = processInformation.dwProcessId;
// 		HANDLE hProcess = OpenProcess(0x1F0FFF, FALSE, processInformation.dwProcessId);
// 		if (hProcess == NULL || hProcess == INVALID_HANDLE_VALUE) 
// 		{
// 			LOGEVEN(TEXT(">>>�����ػ��̣߳��򿪼�����ʧ��.\n"));
// 			CloseHandle( processInformation.hThread );
// 			CloseHandle( processInformation.hProcess );
// 
// 			return;
// 		}
// 
// 		CloseHandle(hProcess);
		CloseHandle( processInformation.hThread );
		CloseHandle( processInformation.hProcess );
		LOGEVEN(TEXT(">>>�����ػ��̣߳����Ҽ�⿪ʼ.\n"));
	}
}