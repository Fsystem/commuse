/** �ػ�������������������������
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

	//���������ص�
	void IntoDriver();
	//�����ص�ע����,true-�����Լ��ļ�⣬��Ҫ�����ж�
	bool InjectionCheck(DWORD dwParantProcessId,DWORD dwChildProcessId);
	//�뿪�����ص�
	void OutDriver();

private:
	void GuardThread(void* pThis);
	void CheckDriver(DWORD dwCurProcessId,TCHAR * szPath);

protected:
	bool		m_bInCheckDriverRunning;			//���ڼ�������Ƿ���Ч��־
	DWORD		m_dwCheckProcessId;
	//DWORD		m_dwCheckCount;
	bool		m_bIncallback;
	bool		m_bRunThread;						//�߳��Ƿ�������
};
#endif //__GuardProtectDriver_H