/** 上报模块
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

	/** 上传数据到网吧服务器
	*   @Author   : Double sword
	*   @Params   : 	
			
	*   @Return   :				
	*   @Date     :	2016-4-14
	*/
	void ReportToWorkServer(int nMsg,string sXml);

	/** 上传数据到核心服务器
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
	char szWorkServerIp[64];						//网吧服务器IP
	unsigned short usWorkServerPort;				//网吧服务器端口

	char szKernelServerIp[64];						//核心服务器Ip
	unsigned short usKernelServerPort;				//核心服务器端口

	PcInfo mDeviceInfo;								//设备相关信息
	CCritiSection mLock;							//数据表锁
	CCritiSection mLockWinTrust;					//数据表锁
	list<ActionOperateResult> mReportList;			//上报数据表
	list<ProcessInfoStagety> mReportListWinTrust;	//WinTrust上报数据表
	bool mRun;
};

#endif //__ReportManager_H