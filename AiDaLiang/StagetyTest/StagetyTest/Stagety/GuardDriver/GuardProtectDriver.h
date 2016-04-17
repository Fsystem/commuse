/** 守护驱动，保持驱动的正常运行
*   @FileName  : GuardProtectDriver.h
*   @Author    : Double Sword(doublesword.jack@gmail.com)
*   @date      : 2016-4-15
*   @Copyright : Copyright belong to Author and 9u2,CO.LTD
*   @RefDoc    : --
*/

#ifndef __GuardProtectDriver_H
#define __GuardProtectDriver_H
class GuardProtectDriver
{
public:
	GuardProtectDriver();
	virtual ~GuardProtectDriver();

public:
	void StartGuard();
	void StopGuard();

	//进入驱动回调
	void IntoDriver();
	//驱动回调注入检测,true-我们自己的检测，不要拦截判断
	bool InjectionCheck(DWORD dwParantProcessId,DWORD dwChildProcessId);
	//离开驱动回调
	void OutDriver();

private:
	void GuardThread(void* pThis);
	void CheckDriver(DWORD dwCurProcessId,TCHAR * szPath);

protected:
	bool		m_bInCheckDriverRunning;			//正在检测驱动是否有效标志
	DWORD		m_dwCheckProcessId;
	//DWORD		m_dwCheckCount;
	bool		m_bIncallback;
	bool		m_bRunThread;						//线程是否在运行
};
#endif //__GuardProtectDriver_H