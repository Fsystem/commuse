#include <stdafx.h>
#include "ProtectorEngine.h"

#include "StagetyInclude.h"

#include "../resource.h"

//-------------------------------------------------------------------------------

//驱动回调
static BOOL WINAPI Sys_Filter_fun(UINT type, PVOID lpata, DWORD size_len)
{
	return StagetyManager::Instance().HandleAction(type,lpata,size_len);
}
//-------------------------------------------------------------------------------
ProtectorEngine::ProtectorEngine()
{
	GetModuleFileName(NULL, mModulePath, MAX_PATH);
	_tcsrchr(mModulePath,'\\')[0]=0;

	mHDllModule = NULL;
	mEngineSink = NULL;

}

void ProtectorEngine::StartEngine(IActionResultDelegate* pEngineSink)
{
	mEngineSink = pEngineSink;

	mEventBlock = CreateEvent(NULL,FALSE,FALSE,NULL);
	ResetEvent(mEventBlock);

	//上报服务器信息
	//ReportManager::Instance().SetWorkServerInfo(netserver_ip.c_str(),netserver_port);
	//ReportManager::Instance().SetKernelServerInfo(coreserver_ip.c_str(),coreserver_port_second);
	ReportManager::Instance().StartService();

	//初始化策略管理器
	StagetyManager::Instance().BuildAllStagety(this);

	//加载驱动
	if(LoadProtectDriver()==FALSE) 
	{
		LOGEVEN(TEXT("ERROR:保护驱动加载失败\n"));
		return;
	}
}

void ProtectorEngine::StopEngine()
{
	if (mHDllModule)
	{
		mOperPorcessStatus(FALSE);
		mOperMsgFilter(FALSE);
		mOperSysFilter(FALSE);

		FreeLibrary(mHDllModule);
		mHDllModule = NULL;
	}

	StagetyManager::Instance().ResetData();

	ReportManager::Instance().StopService();

	if(mEventBlock)
	{
		SetEvent(mEventBlock);
		CloseHandle(mEventBlock);
		mEventBlock = NULL;
	}
}

void ProtectorEngine::Block()
{
	WaitForSingleObject(mEventBlock,INFINITE);
}

void ProtectorEngine::UnBlock()
{
	SetEvent(mEventBlock);
}


BOOL ProtectorEngine::LoadProtectDriver()
{
	TCHAR strFile[MAX_PATH] = {0};
	_sntprintf(strFile,MAX_PATH,TEXT("%s/tmpsys.dll"),mModulePath);
	CheckAndExportFile(TEXT("DLL"), strFile, IDR_SYSIO);	
	mHDllModule = LoadLibrary(strFile);
	if( NULL != mHDllModule)
	{
		mRegCallBackFun		= (SYS_RegCallBackFun)GetProcAddress(mHDllModule, "RegCallBackFun");
		mOperMsgFilter		= (SYS_OperMsgFilter)GetProcAddress(mHDllModule, "OperMsgFilter");
		mOperSysFilter		= (SYS_OperSysFilter)GetProcAddress(mHDllModule, "OperSysFilter");
		mOperPorcessStatus	= (SYS_OperSysFilter)GetProcAddress(mHDllModule, "OperPorcessStatus");

		if( NULL == mRegCallBackFun||
			NULL == mOperMsgFilter||
			NULL == mOperSysFilter||
			NULL == mOperPorcessStatus)
		{
			FreeLibrary(mHDllModule);
			mHDllModule = NULL;
			return FALSE;
		}
	}

	if( !mRegCallBackFun(Sys_Filter_fun) )
	{
		LOGEVEN(TEXT("注册回调失败\n"));
	}
	mOperPorcessStatus(FALSE);
	Sleep(1000);
	mOperPorcessStatus(TRUE);

	LOGEVEN(TEXT(">>加载驱动成功\n" ));

	return TRUE;
}

void ProtectorEngine::ReloadProtectDriver()
{
	try
	{
		if (mHDllModule==NULL)
		{
			LoadProtectDriver();
			return;
		}

		if( NULL == mRegCallBackFun||
			mOperMsgFilter==NULL ||
			mOperSysFilter==NULL ||
			mOperPorcessStatus==NULL) throw 0;

		mOperPorcessStatus(FALSE);
		mOperMsgFilter(FALSE);
		mOperSysFilter(FALSE);
		mRegCallBackFun(NULL);

		Sleep(0);
		mRegCallBackFun(Sys_Filter_fun);
		mOperMsgFilter(TRUE);
		mOperSysFilter(TRUE);
		mOperPorcessStatus(TRUE);

		LOGEVEN(TEXT(">>重新加载驱动\n" ));
	}
	catch(...)
	{
		LOGEVEN(TEXT(">>重新加载驱动异常[%p],[%p],[%p],[%p]\n" ),
			mRegCallBackFun,mOperMsgFilter,mOperSysFilter,mOperPorcessStatus);
	}

}

void ProtectorEngine::OnHandleResultByStagety(ActionOperateResult* pResult)
{
	//无操作的表示打印日志
	//if (pResult->actionType == (WORD)enActionNull || pResult->operateType == (WORD)enOperateNull ) return;

	//1.调用客户端结果及操作结果
	if(mEngineSink) mEngineSink->OnHandleResultByStagety(pResult);

	//2.上报
	ReportOper(*pResult);

}

void ProtectorEngine::ReportOper(const ActionOperateResult& pResult)
{
	if (ActionOperateRecord::Instance().NeedReport(pResult))
	{
		//上报（操作系统不信任和厂商不信任）到核心服务器,所有操作到网吧服务器
		ReportManager::Instance().AddReport(pResult);
	}

}

//-------------------------------------------------------------------------------
//ADD by 
//#define MAX_LEN 8192
//string  CreateLogReportXml(Common_Info &info,PcInfo &pcInfo)
//{
//	char* pchReport = "<?xml version=\"1.0\" encoding=\"gb2312\" ?>\n \
//					  <warnInfo>\n \
//					  <host_name v=\"%s\" />\n\
//					  <mac v=\"%s\" />\n\
//					  <operdate v=\"\" />\n\
//					  <ip v=\"%s\" />\n\
//					  <opertype v=\"%d\" />\n\
//					  <disposetype v=\"%d\" />\n\
//					  <parentpid v=\"%s\" />\n\
//					  <subpid v=\"%s\" />\n\
//					  <parentpath v=\"%s\" />\n\
//					  <subpath v=\"%s\" />\n\
//					  <parentMD5 v=\"%s\" />\n\
//					  <subMD5 v=\"%s\" />\n\
//					  <dirveMD5 v=\"%s\" />\n\
//					  <drivePath v=\"%s\" />\n\
//					  <par_com v=\"%s\" />\n\
//					  <par_filedesc v=\"%s\" />\n\
//					  <par_procdesc v=\"%s\" />\n\
//					  <sub_com v=\"%s\" />\n\
//					  <sub_filedesc v=\"%s\" />\n\
//					  <sub_procdesc v=\"%s\" />\n\
//					  <driver_com v=\"%s\" />\n\
//					  <driver_filedesc v=\"%s\" />\n\
//					  <driver_procdesc v=\"%s\" />\n\
//					  <operateMethod v=\"%d%d\" />\n";
//
//	string strParentID = "";
//	string strSubID = "";
//	char chContent[MAX_LEN] = {0};
//
//	CToolOper tool_oper;
//	std::string parent_file_desc = "";
//	std::string parent_file_proc = "";
//	std::string sub_file_desc = "";
//	std::string sub_file_proc = "";
//	std::string driver_file_desc;
//	std::string driver_file_proc;
//	if (!info.parent_path.empty())
//	{
//		VERSION_INFO_LIST parentInfoList;
//		tool_oper.ReadFileVersionInfo(info.parent_path, parentInfoList);
//		parent_file_desc = parentInfoList[FILEDESCRIPTION];
//		parent_file_proc = parentInfoList[PRODUCTNAME];
//	}
//	if (!info.sub_path.empty())
//	{
//		VERSION_INFO_LIST subInfoList;
//		tool_oper.ReadFileVersionInfo(info.sub_path, subInfoList);
//		sub_file_desc = subInfoList[FILEDESCRIPTION];
//		sub_file_proc = subInfoList[PRODUCTNAME];
//	}
//	string strDriverPath = "";
//	if (1 == info.type)
//	{
//		VERSION_INFO_LIST driverInfoList;
//		tool_oper.ReadFileVersionInfo(info.data.drive_info.driver_path, driverInfoList);
//		driver_file_desc = driverInfoList[FILEDESCRIPTION];
//		driver_file_proc = driverInfoList[PRODUCTNAME];
//		strDriverPath = info.data.drive_info.driver_path;
//	}
//	wsprintfA(chContent,pchReport,pcInfo.strPcHostName.c_str(),pcInfo.strPcMac.c_str(),pcInfo.strPcIp.c_str(),info.detail_type,info.is_trust,
//		strParentID.c_str(),strSubID.c_str(),info.parent_path.c_str(),info.sub_path.c_str(),info.parent_md5.c_str(),info.sub_md5.c_str(),info.dirver_md5.c_str(),
//		strDriverPath.c_str(),info.signer_parent.c_str(),parent_file_desc.c_str(),parent_file_proc.c_str(),info.signer_sub.c_str(),
//		sub_file_desc.c_str(),sub_file_proc.c_str(),info.signer_driver.c_str(),driver_file_desc.c_str(),driver_file_proc.c_str(),info.trust_sign[0], info.trust_sign[1]);
//	string strRtn = chContent;
//	return strRtn;
//}
//
//DWORD _stdcall send_operate_log(LPVOID lpParam)
//{
//	CDebugLog log;
//	CCRC crc;
//	CToolOper tool_oper;
//	CDeviceOper oper;
//	VERSION_INFO_LIST list;
//	char ip[32] = {0};
//	std::string mac;
//	std::string ip_ref;
//	mac = S_CW2A(oper.GetMac(ip_ref).c_str());
//
//	yCUdp udp_server;
//	char pchhostname[256] = {0};  
//	gethostname(pchhostname,256);
//	udp_server.GetNameToIP(pchhostname, ip);
//
//	PcInfo pcInfo;
//	pcInfo.strPcIp = ip;
//	pcInfo.strPcHostName =pchhostname;
//	pcInfo.strPcMac = mac;
//	while (true)
//	{
//		int size = g_queue_drive.size();
//		while(size != 0)
//		{
//			g_lock_queue_.Lock();
//			Common_Info info = g_queue_drive.front();
//			g_lock_queue_.Unlock();
//
//			std::string send_buff_;
//			std::string crc_string;
//
//			string strReport = CreateLogReportXml(info,pcInfo);
//			send_buff_ = strReport;
//			strReport += "</warnInfo>";
//			CNetOper net_oper;
//			int type = NET_TYPE;
//
//			//OutputDebugStringA(send_buff.c_str());
//
//			CDebugLog log;
//			log.SaveLog(1, "c:\\oper.log", "%s\n", strReport.c_str());
//			std::string str = net_oper.NetOperForUdp(CLIENT_REPORT_WARN_Q, strReport.c_str(), type);
//
//			//新增上报日志信息到核心服务器
//			send_buff_ += "<crc v=\"";
//			std::string::size_type pos_begin;
//			std::string::size_type pos_crc;
//			pos_begin = strReport.find("<parentpath");
//			crc_string = strReport.substr(pos_begin, strReport.length() - pos_begin);
//			pos_crc    = crc_string.rfind('<');
//			crc_string = crc_string.substr(0, pos_crc);
//			ULONG crc_res = crc.GetStrCrc_Key(const_cast<char*>(crc_string.c_str()), crc_string.length());
//
//			char crc[MAX_PATH] ={0};
//			sprintf(crc, "%u", crc_res);
//
//			send_buff_ += crc;
//			send_buff_ += "\" />\n";
//
//			send_buff_ += "<wangid v=\"";
//			send_buff_ += g_wangba_id;
//			send_buff_ += "\" />\n";
//			send_buff_ += "</warnInfo>";
//
//			//add log files
//			log.SaveLog(1, "c:\\oper.log", "发送的内容：%s\n", send_buff_.c_str());
//			log.SaveLog(1, "c:\\oper.log", "生成CRC的部分：%s\n", crc_string.c_str());
//			log.SaveLog(1, "c:\\oper.log", "生成CRC的值：%u\n", crc_res);
//			//add log files
//
//			type = CORE_TYPE_SECOND;
//			std::string str_;
//			if (!info.is_trust)
//				str_ = net_oper.NetOper(CLIENT_REPORT_WARN_CORE_Q, send_buff_, type);
//			//新增上报日志信息到核心服务器
//			//CDebugLog log;
//			//log.SaveLog(1, "c:\\core_server.log", "%s\n", send_buff_.c_str());
//			g_lock_queue_.Lock();
//			g_queue_drive.pop();
//			size = g_queue_drive.size();
//			g_lock_queue_.Unlock();
//		}
//		Sleep(1000);
//	}
//	return 0;
//}