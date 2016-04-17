#include <stdafx.h>
#include "ReportManager.h"
#include "../StagetyInclude.h"

// #include "../../drivers/tools/ToolOper.h"
// #include "../../drivers/tools/NetOper.h"
// #include "../../drivers/tools/DeviceOper.h"
// #include "../../drivers/net/yCUdp.h"

#include <atlconv.h>

#define MAX_LEN 8192

ReportManager::ReportManager()
{
	memset(szWorkServerIp,0,sizeof szWorkServerIp);
	memset(szKernelServerIp,0,sizeof szKernelServerIp);

	usWorkServerPort = 0;
	usKernelServerPort = 0;

	GetDeviceInfo();

	mRun = false;
}

ReportManager::~ReportManager()
{

}

void ReportManager::SetWorkServerInfo(const char* ip,unsigned short usPort)
{
	strncpy(szWorkServerIp,ip,sizeof(szWorkServerIp)-1);
	usWorkServerPort = usPort;
}

void ReportManager::SetKernelServerInfo(const char* ip,unsigned short usPort)
{
	strncpy(szKernelServerIp,ip,sizeof(szKernelServerIp)-1);
	usKernelServerPort = usPort;
}

void ReportManager::StartService()
{
	mRun = true;
	JKThread::Start(&ReportManager::SendReportToServerThread,this);
}

void ReportManager::StopService()
{
	mRun = false;
	Sleep(100);
}

void ReportManager::ReportToWorkServer(int nMsg,string sXml)
{
	//CNetOper netOper;

	//netOper.NetOperForUdp(nMsg, sXml, NET_TYPE);//发送消息到网吧服务端

}

void ReportManager::ReportToKernelServer(int nMsg,string sXml)
{
	//CNetOper netOper;

	//netOper.NetOper(nMsg, sXml, CORE_TYPE_SECOND);//发送消息到核心服务端
}

PcInfo ReportManager::GetDeviceInfo()
{
	PcInfo DeviceInfo;
// 	CDeviceOper oper;
// 	char ip[32] = {0};
// 	std::string mac;
// 	std::string ip_ref;
// 	mac = S_CW2A(oper.GetMac(ip_ref).c_str());
// 
// 	yCUdp udp_server;
// 	char pchhostname[256] = {0};  
// 	gethostname(pchhostname,256);
// 	udp_server.GetNameToIP(pchhostname, ip);
// 
// 	DeviceInfo.strPcIp = ip;
// 	DeviceInfo.strPcHostName =pchhostname;
// 	DeviceInfo.strPcMac = mac;

	return DeviceInfo;
}

//-------------------------------------------------------------------------------
string ReportManager::GetStrOperMethod(int iType)
{
	string strOperMethord = "";
	switch (iType)
	{
		case 0:
		{
			strOperMethord = "00";//系统允许
			break;
		}
		case 1:
		{
			strOperMethord = "01";//系统阻止
			break;
		}
		case 2:
		{
			strOperMethord = "10";//用户允许
			break;
		}
		case 3:
		{
			strOperMethord = "11";//用户阻止
			break;
		}
	default:
		{
			strOperMethord = "11";//用户阻止
			break;
		}
	}
	return strOperMethord;
}

string ReportManager::IntToString(int iValue)
{
	char ch[16] = {0};
	itoa(iValue, ch, 10);
	string strValue = ch;
	return strValue;
}

string  ReportManager::CreateLogReportXml(const ActionOperateResult &info)
{
// 	string strParentID = IntToString(info.parantProcess.dwPid);
// 	string strSubID = IntToString(info.childProcess.dwPid);
// 	char chContent[MAX_LEN] = {0};
// 
// 	CToolOper tool_oper;
// 	std::string parent_file_desc = "";
// 	std::string parent_file_proc = "";
// 	std::string sub_file_desc = "";
// 	std::string sub_file_proc = "";
// 	std::string driver_file_desc = "";
// 	std::string driver_file_proc = "";
// 	if (!info.parantProcess.szProcessPath.empty())
// 	{
// 		VERSION_INFO_LIST parentInfoList;
// 		tool_oper.ReadFileVersionInfo(info.parantProcess.szProcessPath, parentInfoList);
// 		parent_file_desc = parentInfoList[FILEDESCRIPTION];
// 		parent_file_proc = parentInfoList[PRODUCTNAME];
// 	}
// 	if (!info.childProcess.szProcessPath.empty())
// 	{
// 		VERSION_INFO_LIST subInfoList;
// 		tool_oper.ReadFileVersionInfo(info.childProcess.szProcessPath, subInfoList);
// 		sub_file_desc = subInfoList[FILEDESCRIPTION];
// 		sub_file_proc = subInfoList[PRODUCTNAME];
// 	}
// 	string strDriverPath = info.parantProcess.szProcessPath;
// 	/*if (1 == info.type)
// 	{
// 	VERSION_INFO_LIST driverInfoList;
// 	tool_oper.ReadFileVersionInfo(info.data.drive_info.driver_path, driverInfoList);
// 	driver_file_desc = driverInfoList[FILEDESCRIPTION];
// 	driver_file_proc = driverInfoList[PRODUCTNAME];
// 	strDriverPath = info.data.drive_info.driver_path;
// 	}*/
// 
// 	PcInfo pcInfo = GetDeviceInfo();
// 	
// 	string strOperMethord = GetStrOperMethod(info.nTrusted);
// 
// 	char* pchReport = "<?xml version=\"1.0\" encoding=\"gb2312\" ?>\n <warnInfo>\n \
// 					  <host_name v=\"%s\" />\n\
// 					  <mac v=\"%s\" />\n\
// 					  <operdate v=\"\" />\n\
// 					  <ip v=\"%s\" />\n\
// 					  <opertype v=\"%d\" />\n\
// 					  <disposetype v=\"%d\" />\n\
// 					  <parentpid v=\"%s\" />\n\
// 					  <subpid v=\"%s\" />\n\
// 					  <parentpath v=\"%s\" />\n\
// 					  <subpath v=\"%s\" />\n\
// 					  <parentMD5 v=\"%s\" />\n\
// 					  <subMD5 v=\"%s\" />\n\
// 					  <dirveMD5 v=\"%s\" />\n\
// 					  <drivePath v=\"%s\" />\n\
// 					  <par_com v=\"%s\" />\n\
// 					  <par_filedesc v=\"%s\" />\n\
// 					  <par_procdesc v=\"%s\" />\n\
// 					  <sub_com v=\"%s\" />\n\
// 					  <sub_filedesc v=\"%s\" />\n\
// 					  <sub_procdesc v=\"%s\" />\n\
// 					  <driver_com v=\"%s\" />\n\
// 					  <driver_filedesc v=\"%s\" />\n\
// 					  <driver_procdesc v=\"%s\" />\n\
// 					  <operateMethod v=\"%s\" />\n";
// 
// 	_snprintf(chContent, MAX_LEN, pchReport, 
// 		pcInfo.strPcHostName.c_str(), 
// 		pcInfo.strPcMac.c_str(), 
// 		//operdate没填值
// 		pcInfo.strPcIp.c_str(),
// 		info.operateType, 
// 		info.nTrusted, 
// 		strParentID.c_str(), 
// 		strSubID.c_str(), 
// 		info.parantProcess.szProcessPath.c_str(),
// 		info.childProcess.szProcessPath.c_str(),
// 		info.parantProcess.szMd5.c_str(), 
// 		info.childProcess.szMd5.c_str(),
// 		info.parantProcess.szMd5.c_str(),
// 		strDriverPath.c_str(),
// 		info.parantProcess.szSign.c_str(),
// 		parent_file_desc.c_str(),
// 		parent_file_proc.c_str(),
// 		info.childProcess.szSign.c_str(),
// 		sub_file_desc.c_str(),
// 		sub_file_proc.c_str(),
// 		info.parantProcess.szSign.c_str(),
// 		driver_file_desc.c_str(),
// 		driver_file_proc.c_str(),
// 		strOperMethord.c_str()
// 		);
// 
// 	string strRtn = chContent;
// 
// 	string strReport = strRtn;
// 	strReport += "</warnInfo>";
// 	CMarkup xml;
// 	USES_CONVERSION;
// 	if(xml.SetDoc(A2T(strReport.c_str()) ) == false )
// 	{
// 		LOGEVEN(TEXT("解析错误[%s]\n"),A2T(strReport.c_str()));
// 	}
// 
// 	return strRtn;

	return "";
}

void ReportManager::AddReport(const ActionOperateResult &info)
{
	LOCK lk(mLock);
	mReportList.push_back(info);
}

void ReportManager::AddWinTrustReport(const ProcessInfoStagety& processinfo)
{
	LOCK lk(mLockWinTrust);
	mReportListWinTrust.push_back(processinfo);
}

void ReportManager::SendReportToServerThread(void* p)
{
	ReportManager* pThis = (ReportManager*)p;
	while(pThis->mRun)
	{
		try
		{
			if(pThis == NULL) throw 0;

			pThis->ReportResult();

			pThis->ReportWinTrustResult();

			Sleep(1000);
		}
		catch(int)
		{

		}
		catch(...)
		{
			LOGEVEN(TEXT("exception:ReportManager::SendReportToServerThread\n"));
		}
	}
	
	JKThread::Stop();
}

void ReportManager::ReportResult()
{
// 	if (mReportList.size()>0)
// 	{
// 		ActionOperateResult info = mReportList.front();
// 		{
// 			LOCK lk(mLock);
// 			mReportList.pop_front();
// 		}
// 
// 		CNetOper netOper;
// 		std::string strCoreMsg = "";
// 		std::string crc_string;
// 		CCRC crc;
// 
// 		string strReport = CreateLogReportXml(info);
// 		strCoreMsg = strReport;
// 		strReport += "</warnInfo>";
// 
// 		netOper.NetOperForUdp(CLIENT_REPORT_WARN_Q, strReport, NET_TYPE);//发送消息到网吧服务端
// 
// 		//LOGEVEN(TEXT(">>>上报数据到网吧服务器\n")); OutputDebugStringA(strReport.c_str());
// 		if (info.nTrusted == 0 || info.nTrusted == 2) throw 0;
// 
// 		strCoreMsg += "<crc v=\"";//继续拼接发送到核心服务的消息
// 		std::string::size_type pos_begin;
// 		std::string::size_type pos_crc;
// 		pos_begin = strReport.find("<parentpath");
// 		crc_string = strReport.substr(pos_begin, strReport.length() - pos_begin);
// 		pos_crc    = crc_string.rfind('<');
// 		crc_string = crc_string.substr(0, pos_crc);
// 		ULONG crc_res = crc.GetStrCrc_Key(const_cast<char*>(crc_string.c_str()), crc_string.length());
// 
// 		char chcrc[MAX_PATH] ={0};
// 		sprintf(chcrc, "%u", crc_res);
// 
// 		strCoreMsg += chcrc;
// 		strCoreMsg += "\" />\n";
// 
// 		strCoreMsg += "<wangid v=\"";
// 		strCoreMsg += g_wangba_id;
// 		strCoreMsg += "\" />\n";
// 		strCoreMsg += "</warnInfo>";
// 
// 		netOper.NetOper(CLIENT_REPORT_WARN_CORE_Q, strCoreMsg, CORE_TYPE_SECOND);//发送消息到核心服务端
// 
// 		//LOGEVEN(TEXT(">>>上报数据到核心服务器\n"));OutputDebugStringA(strCoreMsg.c_str());
// 
// 	}
}

void ReportManager::ReportWinTrustResult()
{
// 	if(mReportListWinTrust.size()==0) return;
// 
// 	ProcessInfoStagety mProcessInfo = mReportListWinTrust.front();
// 	{
// 		LOCK lk(mLockWinTrust);
// 		mReportListWinTrust.pop_front();
// 	}
// 
// 	std::string strXml="<?xml version=\"1.0\" encoding=\"gb2312\"?>\r\n<r>";
// 	char szTemp[512]={0};	
// 	sprintf(szTemp,"<file_crc v=\"%u\"/>\r\n",mProcessInfo.unCrc);
// 	strXml += string(szTemp);
// 	sprintf(szTemp,"<filename_crc v=\"%u\"/>\r\n", GetFileCrc(mProcessInfo.szProcessPath.c_str()));
// 	strXml += string(szTemp);
// 	sprintf(szTemp,"<filename v=\"%s\"/>\r\n", mProcessInfo.szProcessPath.c_str());
// 	strXml += string(szTemp);
// 	sprintf(szTemp,"<trust_sign v=\"%d\"/>\r\n", mProcessInfo.cbVerifyResult);
// 	strXml += string(szTemp);
// 	sprintf(szTemp,"<company v=\"%s\"/>\r\n", mProcessInfo.szSign.c_str());
// 	strXml += string(szTemp);
// 
// 	strXml += "</r>\r\n";
// 
// 	CNetOper netOper;
// 	netOper.NetOperForUdp(58, strXml, NET_TYPE);//发送消息到网吧服务端

	//LOGEVEN(TEXT(">>>WinTrust信任数据到网吧服务器\n"));OutputDebugStringA(strXml.c_str());
}