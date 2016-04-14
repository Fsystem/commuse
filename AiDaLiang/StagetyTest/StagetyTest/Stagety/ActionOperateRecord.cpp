#include <StdAfx.h>
#include "StagetyInclude.h"
#include "ActionOperateRecord.h"
#include "helper/FunctionHelper.h"

BOOL ActionOperateRecord::RecordProcessOperate(const ActionOperateResult & operate)
{
	DWORD dwKey = GetActionKey(operate.parantProcess.szProcessPath.c_str(),
		operate.childProcess.szProcessPath.c_str(),operate.operateType);

	if(dwKey == 0)
	{
		LOGEVEN(TEXT("error:²úÉúCRC´íÎó\n"));
		return FALSE;
	}

	mOperateRecord[dwKey].push_back(operate);

	return (mOperateRecord[dwKey].size()==1);
}
void ActionOperateRecord::ReportWinTrustFile(ProcessInfoStagety* pProcessInfo)
{
	JKThread::Start(&ActionOperateRecord::ReportWinTrustFileThread,pProcessInfo);
}

void ActionOperateRecord::ReportWinTrustFileThread(void* pProcessInfo)
{
	ProcessInfoStagety mProcessInfo = *(ProcessInfoStagety*)pProcessInfo;

	std::string strXml="<?xml version=\"1.0\" encoding=\"gb2312\"?>\r\n<r>";
	char szTemp[512]={0};	
	sprintf(szTemp,"<file_crc v=\"%u\"/>\r\n",mProcessInfo.unCrc);
	strXml += string(szTemp);
	sprintf(szTemp,"<filename_crc v=\"%u\"/>\r\n", GetFileCrc(mProcessInfo.szProcessPath.c_str()));
	strXml += string(szTemp);
	sprintf(szTemp,"<filename v=\"%s\"/>\r\n", mProcessInfo.szProcessPath.c_str());
	strXml += string(szTemp);
	sprintf(szTemp,"<trust_sign v=\"%d\"/>\r\n", mProcessInfo.cbVerifyResult);
	strXml += string(szTemp);
	sprintf(szTemp,"<company v=\"%s\"/>\r\n", mProcessInfo.szSign.c_str());
	strXml += string(szTemp);

	strXml += "</r>\r\n";
	//CReportModule::ReportData(strRet, 58);
}
