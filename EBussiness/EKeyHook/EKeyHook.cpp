// EKeyHook.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "EKeyHook.h"
#include <process.h>
#include <assert.h>

//#define _WIN32_WINNT  0x0500				// ����ϵͳ�汾, ȷ������ʹ�õײ���̹���

#define KEY_DOWN_TIME_STAMP		0	//50MS
DWORD dwScanGunTimeStamp	= 20;

// ���ǵ���������һ��ʾ��
EKEYHOOK_API int nEKeyHook=0;

// ���ǵ���������һ��ʾ����
EKEYHOOK_API int fnEKeyHook(void)
{
	return 42;
}

CEKeyHook* CEKeyHook::m_pThis=NULL;

CEKeyHook::CEKeyHook()
{
	m_hHook=NULL;
	m_pThis=this;
	init();
	m_dwTimeStamp=GetMessageTime();
	dwScanGunTimeStamp=::GetPrivateProfileInt(TEXT("info"),TEXT("timestamp"),0,TEXT("./config.ini"));
	if (0==dwScanGunTimeStamp)
	{
		MessageBox(NULL,TEXT("ɨ�������ô���"),TEXT("����"),MB_OK|MB_ICONERROR);
		::PostQuitMessage(0);
	}
	return;
}

CEKeyHook::~CEKeyHook()
{
	m_hHook=NULL;
	
	StopMaskKeyboard();
	return;
}

void CEKeyHook::init()
{
	m_KeyCount=0;
	LOOPVAL(m_szKeyCode,_countof(m_szKeyCode),0);
}

BOOL CEKeyHook::StartMaskKeyboard()
{
	//installed return FALSE
	if (m_hHook != NULL) return FALSE;

	m_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, &CEKeyHook::LowLevelKeyboardProc, g_DllInstance, NULL);

	//installed is failed
	if (m_hHook == NULL) return FALSE;

	init();

	return TRUE;
}

void CEKeyHook::StopMaskKeyboard()
{
	// ж�ع���
	if(m_hHook)UnhookWindowsHookEx(m_hHook) ;
	m_hHook = NULL;
}

void CEKeyHook::SetKeyCallBackSink(IKeyboardSink* pSink)
{
	m_pKeySink=pSink;
}


// �ײ���̹��Ӻ���
LRESULT CALLBACK CEKeyHook::LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	KBDLLHOOKSTRUCT* pStruct = (KBDLLHOOKSTRUCT*)lParam;
	//g_key_code=pStruct->vkCode;
	if (nCode == HC_ACTION && WM_KEYDOWN == wParam )
	{
		bool bInput=false;
		//clear old time data
		if(dwScanGunTimeStamp<=pStruct->time-m_pThis->m_dwTimeStamp)
		{
			m_pThis->init();
			m_pThis->m_dwTimeStamp=pStruct->time;
			bInput=true;
		}

		m_pThis->m_szKeyCode[m_pThis->m_KeyCount++]=(char)pStruct->vkCode;
		
		if (!bInput)
		{//scan gun
			m_pThis->m_dwTimeStamp=pStruct->time;
			//LOGEVEN(TEXT(">>>>>>>::::%s\n"),m_pThis->m_szKeyCode);
			if (2<m_pThis->m_KeyCount && 0x0D==m_pThis->m_szKeyCode[m_pThis->m_KeyCount-1])
			{
				assert(m_pThis->m_pKeySink);
				if(m_pThis->m_pKeySink)
				{
					m_pThis->m_pKeySink->ScanGunCallBackData(m_pThis->m_szKeyCode,m_pThis->m_KeyCount);
					
				}
			}
			return TRUE;
		}
	}

	// send next sink
	return CallNextHookEx(m_pThis->m_hHook, nCode, wParam, lParam);
}


