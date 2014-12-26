#pragma once
#include <Windows.h>
//#include "../../../Public/COMM_USE/Log/uxilog.h"

typedef void (CALLBACK* UXTIMERCALLBACK)(DWORD,void*);

#include <map>
#include <vector>

#define G_UXTimerQueue (CUXTimer::GetInstance())

//--------------------------------------------------------------------------------------
typedef struct tegTIMERINFO 
{
	HANDLE				hEvent;
	DWORD				dwEventID;
	UXTIMERCALLBACK		callback;
	void*				pEvent;
	void*				pThis;
	//HANDLE				hCmpEvent;
}TIMERINFO ;

typedef std::map<DWORD,TIMERINFO*> TIMERQUEUE;
typedef std::vector<TIMERINFO*> TIMERQUEUEDELETE;

//-------------------------------------------------------------------------------------
class CUXTimer
{
public:
	CUXTimer();
	virtual ~CUXTimer();
	void SetTimer(DWORD dwIdEvent,DWORD dwTime,UXTIMERCALLBACK callBack,void* p);
	void KillTimer(DWORD dwIdEvent);

	static CUXTimer& GetInstance()
	{
		static CUXTimer u;
		return u;
	}

	inline TIMERQUEUE& GetTimerQueue()
	{
		return m_timer_queue;
	};
	
	inline HANDLE GetTimerQueueHandle()
	{
		return m_hTimerQueue;
	};

	inline DWORD GetIdEvent()
	{
		InterlockedIncrement((LONG*)&m_longIdEvent);		
		if(m_longIdEvent==UINT_MAX)
			m_longIdEvent = 1;
		return (DWORD)m_longIdEvent;
	}
protected:
private:
	HANDLE				m_hTimerQueue;
	TIMERQUEUE			m_timer_queue;
	TIMERQUEUEDELETE	m_del_timer_queue;
	HANDLE				m_hGuardThd;

private:
	unsigned long m_longIdEvent;
 	static void WINAPI TimerFunc(void*,bool);
	static unsigned int __stdcall GuardThd(PVOID p);
public:
	CRITICAL_SECTION m_cs_timer_queue;
	CRITICAL_SECTION m_cs_del_timer_queue;
	static CUXTimer* m_pThis;
};
//--------------------------------------------------------------------------------------