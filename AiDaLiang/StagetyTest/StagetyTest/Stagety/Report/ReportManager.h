/** �ϱ�ģ��
*   @FileName  : ReportManager.h
*   @Author    : Double Sword(doublesword.jack@gmail.com)
*   @date      : 2016-4-14
*   @Copyright : Copyright belong to Author and 9u2,CO.LTD
*   @RefDoc    : --
*/

#ifndef __ReportManager_H
#define __ReportManager_H

#include <string>
#include <list>
#include "../SafeCriticalSection/SafeCriticalSection.h"

using std::string;
using std::list;
using namespace COMMUSE;

typedef struct _PcInfo 
{
	string strPcHostName;
	string strPcMac;
	string strPcIp;
}PcInfo;

class ReportManager
{
public:
	virtual ~ReportManager();
	static ReportManager& Instance(){static ReportManager r;return r;}

	void SetWorkServerInfo(const char* ip,unsigned short usPort);
	void SetKernelServerInfo(const char* ip,unsigned short usPort);

	void StartService();
	void StopService();

	/** �ϴ����ݵ����ɷ�����
	*   @Author   : Double sword
	*   @Params   : 	
			
	*   @Return   :				
	*   @Date     :	2016-4-14
	*/
	void ReportToWorkServer(int nMsg,string sXml);

	/** �ϴ����ݵ����ķ�����
	*   @Author   : Double sword
	*   @Params   : 	
			
	*   @Return   :				
	*   @Date     :	2016-4-14
	*/
	void ReportToKernelServer(int nMsg,string sXml);

	PcInfo GetDeviceInfo();

	string GetStrOperMethod(int iType);

	string IntToString(int iValue);

	string CreateLogReportXml(const ActionOperateResult &info);


	void AddReport(const ActionOperateResult &info);
	void AddWinTrustReport(const ProcessInfoStagety& processinfo);

protected:
	ReportManager();

protected:
	void SendReportToServerThread(void* p);
	void ReportResult();
	void ReportWinTrustResult();

protected:
	char szWorkServerIp[64];						//���ɷ�����IP
	unsigned short usWorkServerPort;				//���ɷ������˿�

	char szKernelServerIp[64];						//���ķ�����Ip
	unsigned short usKernelServerPort;				//���ķ������˿�

	PcInfo mDeviceInfo;								//�豸�����Ϣ
	CCritiSection mLock;							//���ݱ���
	CCritiSection mLockWinTrust;					//���ݱ���
	list<ActionOperateResult> mReportList;			//�ϱ����ݱ�
	list<ProcessInfoStagety> mReportListWinTrust;	//WinTrust�ϱ����ݱ�
	bool mRun;
};

#endif //__ReportManager_H