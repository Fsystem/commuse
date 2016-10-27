#include "stdafx.h"
#include <time.h>
#include "PKGBarServer.h"
//-------------------------------------------------------------------------------
std::string PKGCltConfig_Q::MakeBodyData()
{
	USES_XML;

	for ( auto it = vecProperNames.begin();it!=vecProperNames.end();it++)
	{
		ADD_XMLITEM(*it,"propername");
	}

	return GET_XML;
}

void PKGCltConfig_Q::ParseBodyData(std::string szXml)
{
	PARSE_XML_BEGIN;

	do 
	{
		std::string sItem;
		PARSE_ITEM_STDSTR(sItem,"propername");
		vecProperNames.push_back(sItem);

	} while (XML_LOOP_VERIFY);

	PARSE_XML_END;
}

//-------------------------------------------------------------------------------
std::string PKGCltConfig_A::MakeBodyData()
{
	USES_XML;

	ADD_XMLITEM(bAllowTipNoTrust,"allow_tipnotrust");
	ADD_XMLITEM(bAllowTipUnKnow,"allow_tipunknown");
	ADD_XMLITEM(bAllowTipAccount,"allow_tipaccount");
	ADD_XMLITEM(sAgent,"agent");
	ADD_XMLITEM(sBarId,"bar_id");

	//赵冬冬要去掉这个值 modify by lgw 20160519
	//PARSE_ITEM_NUMBER(bAllowStartClt,"allow_startup");
	ADD_XMLITEM(bSafeScan,"allow_safescan");
	ADD_XMLITEM(bShowTrayIcon,"allow_icon");
	ADD_XMLITEM(bStartAccelerateBall,"allow_ball");
	ADD_XMLITEM(bShowStartPage,"allow_page");
	ADD_XMLITEM(bAllowCloseClt,"allow_close");

	return GET_XML;
}

void PKGCltConfig_A::ParseBodyData(std::string szXml)
{
	PARSE_XML_BEGIN;

	PARSE_ITEM_NUMBER(bAllowTipNoTrust,"allow_tipnotrust");
	PARSE_ITEM_NUMBER(bAllowTipUnKnow,"allow_tipunknown");
	PARSE_ITEM_NUMBER(bAllowTipAccount,"allow_tipaccount");
	PARSE_ITEM_STDSTR(sAgent,"agent");
	PARSE_ITEM_STDSTR(sBarId,"bar_id");

	//赵冬冬要去掉这个值 modify by lgw 20160519
	//PARSE_ITEM_NUMBER(bAllowStartClt,"allow_startup");
	PARSE_ITEM_NUMBER(bSafeScan,"allow_safescan");
	PARSE_ITEM_NUMBER(bShowTrayIcon,"allow_icon");
	PARSE_ITEM_NUMBER(bStartAccelerateBall,"allow_ball");
	PARSE_ITEM_NUMBER(bShowStartPage,"allow_page");
	PARSE_ITEM_NUMBER(bAllowCloseClt,"allow_close");

	PARSE_XML_END;
}
//-------------------------------------------------------------------------------
std::string PKGBarStartClient_Q::MakeBodyData()
{
	USES_XML;
	ADD_XMLITEM(sOSVersion,"sysVer");
	ADD_XMLITEM(sIEVersion,"ieVer");
	ADD_XMLITEM(sFlashVersion,"flashVer");
	ADD_XMLITEM(sDiskId,"diskId");
	ADD_XMLITEM(sCltVersion,"clientVer");
	ADD_XMLITEM(dwStartSysTm,"sysOnTime");

	return GET_XML;
}

void PKGBarStartClient_Q::ParseBodyData(std::string szXml)
{
	PARSE_XML_BEGIN;

	PARSE_ITEM_STDSTR(sOSVersion,"sysVer");
	PARSE_ITEM_STDSTR(sIEVersion,"ieVer");
	PARSE_ITEM_STDSTR(sFlashVersion,"flashVer");
	PARSE_ITEM_STDSTR(sDiskId,"diskId");
	PARSE_ITEM_STDSTR(sCltVersion,"clientVer");
	PARSE_ITEM_NUMBER(dwStartSysTm,"sysOnTime");

	PARSE_XML_END;
}
//-------------------------------------------------------------------------------
PKGBarClientShakeHand_Q::PKGBarClientShakeHand_Q()
{
	bStatus = TRUE;
	bClientService = FALSE;
}

std::string PKGBarClientShakeHand_Q::MakeBodyData()
{
	USES_XML;

	ADD_XMLITEM(bStatus,"status");
	ADD_XMLITEM(bClientService,"client_s");

	return GET_XML;
}

void PKGBarClientShakeHand_Q::ParseBodyData(std::string szXml)
{
	PARSE_XML_BEGIN;

	PARSE_ITEM_NUMBER(bStatus,"status");
	PARSE_ITEM_NUMBER(bClientService,"client_s");

	PARSE_XML_END;
}
//-------------------------------------------------------------------------------
PKGBarClientShakeHand_A::PKGBarClientShakeHand_A()
{
	bStatus = FALSE;
}
std::string PKGBarClientShakeHand_A::MakeBodyData()
{
	USES_XML;

	ADD_XMLITEM(bStatus,"status");

	return GET_XML;
}

void PKGBarClientShakeHand_A::ParseBodyData(std::string szXml)
{
	PARSE_XML_BEGIN;

	PARSE_ITEM_NUMBER(bStatus,"status");

	PARSE_XML_END;
}
//-------------------------------------------------------------------------------

PKGBarAskClient_A::PKGBarAskClient_A()
{

}

std::string PKGBarAskClient_A::MakeBodyData()
{
	USES_XML;

	for (std::list<PKGClientInfo>::iterator it = clientlist.begin();
		it != clientlist.end();it++)
	{
		ADDSTART_CHILDNODE("p");

		ADD_XMLITEM((*it).strHostName,"name");
		ADD_XMLITEM((*it).dwLocalIp,"ip");
		ADD_XMLITEM((*it).strMac,"mac");
		ADD_XMLITEM((*it).dwKeyMacIP,"keymacip");
		ADD_XMLITEM((*it).dwKeyMacHostName,"keymacname");
		ADD_XMLITEM((*it).nState,"state");

		ADDEND_CHILDNODE("p");
	}

	return GET_XML;
}

void PKGBarAskClient_A::ParseBodyData(std::string szXml)
{
	PARSE_XML_BEGIN;
	PKGClientInfo procInfoEx;

	while(PARSE_XML_FINDELEM("p"))
	{
		PARSE_XML_INTOELEM;

		memset(&procInfoEx,0,sizeof(procInfoEx));
		PARSE_ITEM_STDSTR(procInfoEx.strHostName,"name");
		PARSE_ITEM_NUMBER(procInfoEx.dwLocalIp,"ip");
		PARSE_ITEM_STDSTR(procInfoEx.strMac,"mac");
		PARSE_ITEM_NUMBER(procInfoEx.dwKeyMacIP,"keymacip");
		PARSE_ITEM_NUMBER(procInfoEx.dwKeyMacHostName,"keymacname");
		PARSE_ITEM_NUMBER(procInfoEx.nState,"state");

		clientlist.push_back(procInfoEx);

		PARSE_XML_OUTOFELEM;
	}

	PARSE_XML_END;
}

//---------------------------------------------------------------------------------

PKGBarGetClientProcListTRUE_A::PKGBarGetClientProcListTRUE_A()
{

}

std::string PKGBarGetClientProcListTRUE_A::MakeBodyData()
{
	USES_XML;

	for (std::list<PKGProcInfo>::iterator it = proclist.begin();
		it != proclist.end();it++)
	{
		ADDSTART_CHILDNODE("p");

		ADD_XMLITEM((*it).name,"nm");
		ADD_XMLITEM((*it).pid,"pid");
		ADD_XMLITEM((*it).Desc,"Desc");
		ADD_XMLITEM((*it).Path,"path");

		ADDEND_CHILDNODE("p");
	}

	return GET_XML;
}

void PKGBarGetClientProcListTRUE_A::ParseBodyData(std::string szXml)
{
	PARSE_XML_BEGIN;
	PKGProcInfo procInfoEx;

	while(PARSE_XML_FINDELEM("p"))
	{
		PARSE_XML_INTOELEM;

		memset(&procInfoEx,0,sizeof(procInfoEx));
		PARSE_ITEM_STDSTR(procInfoEx.name,"nm");
		PARSE_ITEM_NUMBER(procInfoEx.pid,"pid");
		PARSE_ITEM_STDSTR(procInfoEx.Desc,"Desc");
		PARSE_ITEM_STDSTR(procInfoEx.Path,"path");

		proclist.push_back(procInfoEx);

		PARSE_XML_OUTOFELEM;
	}

	PARSE_XML_END;
}

//-------------------------------------------------------------------------------

PKGBarClientKillProc_Q::PKGBarClientKillProc_Q()
{
	pid = 0;
}

void PKGBarClientKillProc_Q::ParseBodyData(std::string szXml)
{
	PARSE_XML_BEGIN;

	PARSE_ITEM_NUMBER(pid,"pid");

	PARSE_XML_END;
}

std::string PKGBarClientKillProc_Q::MakeBodyData()
{
	USES_XML;

	ADD_XMLITEM(pid,"pid");

	return GET_XML;
}
//---------------------------------------------------------------------------------

PKGBarStopSafeSoft_Q::PKGBarStopSafeSoft_Q()
{
	optype = FALSE;
}

void PKGBarStopSafeSoft_Q::ParseBodyData(std::string szXml)
{
	PARSE_XML_BEGIN;

	PARSE_ITEM_NUMBER(optype,"optype");

	PARSE_XML_END;
}

std::string PKGBarStopSafeSoft_Q::MakeBodyData()
{
	USES_XML;

	ADD_XMLITEM(optype,"optype");

	return GET_XML;
}

//-------------------------------------------------------------------------------
PKGBarActionProcess_Q::PKGBarActionProcess_Q()
{
	wAction			= (WORD)-1;
	wOperation		= (WORD)-1;
	dwTimeStamp		= 0;
	memset(&parant,0,sizeof parant);
	memset(&child,0,sizeof child);
}

void PKGBarActionProcess_Q::ParseBodyData(std::string szXml)
{
	PARSE_XML_BEGIN;
	PARSE_ITEM_NUMBER(wAction,"action");
	PARSE_ITEM_NUMBER(wOperation,"opration");
	PARSE_ITEM_NUMBER(dwTimeStamp,"operate-time");

	PARSE_ITEM_NUMBER (parant.dwKey,"parant-key");
	PARSE_ITEM_NUMBER (parant.dwPid,"parant-pid");
	PARSE_ITEM_NUMBER (parant.unCrc,"parant-file-crc");
	PARSE_ITEM_CSTRING(parant.szTrustDes,"parant-trust-type",sizeof parant.szTrustDes);
	PARSE_ITEM_CSTRING(parant.szProcessName,"parant-name",sizeof parant.szProcessName);
	PARSE_ITEM_CSTRING(parant.szProcessPath,"parant-path",sizeof parant.szProcessPath);
	PARSE_ITEM_CSTRING(parant.szProcessDescribe,"parant-desc",sizeof parant.szProcessDescribe);
	PARSE_ITEM_CSTRING(parant.szSign,"parant-signer",sizeof parant.szSign);
	PARSE_ITEM_CSTRING(parant.szMd5,"parant-file-md5",sizeof parant.szMd5);

	PARSE_ITEM_NUMBER (child.dwKey,"child-key");
	PARSE_ITEM_NUMBER (child.dwPid,"child-pid");
	PARSE_ITEM_NUMBER (child.unCrc,"child-file-crc");
	PARSE_ITEM_CSTRING(child.szTrustDes,"child-trust-type",sizeof child.szTrustDes);
	PARSE_ITEM_CSTRING(child.szProcessName,"child-name",sizeof child.szProcessName);
	PARSE_ITEM_CSTRING(child.szProcessPath,"child-path",sizeof child.szProcessPath);
	PARSE_ITEM_CSTRING(child.szProcessDescribe,"child-desc",sizeof child.szProcessDescribe);
	PARSE_ITEM_CSTRING(child.szSign,"child-signer",sizeof child.szSign);
	PARSE_ITEM_CSTRING(child.szMd5,"child-file-md5",sizeof child.szMd5);

	PARSE_XML_END;
}

std::string PKGBarActionProcess_Q::MakeBodyData()
{
	USES_XML;

	ADD_XMLITEM(wAction,"action");
	ADD_XMLITEM(wOperation,"opration");
	ADD_XMLITEM(dwTimeStamp,"operate-time");

	ADD_XMLITEM (parant.dwKey,"parant-key");
	ADD_XMLITEM(parant.dwPid,"parant-pid");
	ADD_XMLITEM(parant.unCrc,"parant-file-crc");
	ADD_XMLITEM(parant.szTrustDes,"parant-trust-type");
	ADD_XMLITEM(parant.szProcessName,"parant-name");
	ADD_XMLITEM(parant.szProcessPath,"parant-path");
	ADD_XMLITEM(parant.szProcessDescribe,"parant-desc");
	ADD_XMLITEM(parant.szSign,"parant-signer");
	ADD_XMLITEM(parant.szMd5,"parant-file-md5");

	ADD_XMLITEM (child.dwKey,"child-key");
	ADD_XMLITEM(child.dwPid,"child-pid");
	ADD_XMLITEM(child.unCrc,"child-file-crc");
	ADD_XMLITEM(child.szTrustDes,"child-trust-type");
	ADD_XMLITEM(child.szProcessName,"child-name");
	ADD_XMLITEM(child.szProcessPath,"child-path");
	ADD_XMLITEM(child.szProcessDescribe,"child-desc");
	ADD_XMLITEM(child.szSign,"child-signer");
	ADD_XMLITEM(child.szMd5,"child-file-md5");

	return GET_XML;
}

//-------------------------------------------------------------------------------

PKGBarActionFile_Q::PKGBarActionFile_Q()
{
	wAction			= (WORD)-1;
	wOperation		= (WORD)-1;
	dwTimeStamp		= 0;
}

void PKGBarActionFile_Q::ParseBodyData(std::string szXml)
{
	PARSE_XML_BEGIN;

	PARSE_ITEM_NUMBER(wAction,"action");
	PARSE_ITEM_NUMBER(wOperation,"opration");
	PARSE_ITEM_NUMBER(dwTimeStamp,"operate-time");

	PARSE_ITEM_NUMBER (parant.dwKey,"parant-key");
	PARSE_ITEM_NUMBER (parant.dwPid,"parant-pid");
	PARSE_ITEM_NUMBER (parant.unCrc,"parant-file-crc");
	PARSE_ITEM_CSTRING(parant.szTrustDes,"parant-trust-type",sizeof parant.szTrustDes);
	PARSE_ITEM_CSTRING(parant.szProcessName,"parant-name",sizeof parant.szProcessName);
	PARSE_ITEM_CSTRING(parant.szProcessPath,"parant-path",sizeof parant.szProcessPath);
	PARSE_ITEM_CSTRING(parant.szProcessDescribe,"parant-desc",sizeof parant.szProcessDescribe);
	PARSE_ITEM_CSTRING(parant.szSign,"parant-signer",sizeof parant.szSign);
	PARSE_ITEM_CSTRING(parant.szMd5,"parant-file-md5",sizeof parant.szMd5);

	PARSE_ITEM_STDSTR(child,"path");

	PARSE_XML_END;
}

std::string PKGBarActionFile_Q::MakeBodyData()
{
	USES_XML;

	ADD_XMLITEM(wAction,"action");
	ADD_XMLITEM(wOperation,"opration");
	ADD_XMLITEM(dwTimeStamp,"operate-time");

	ADD_XMLITEM (parant.dwKey,"parant-key");
	ADD_XMLITEM(parant.dwPid,"parant-pid");
	ADD_XMLITEM(parant.unCrc,"parant-file-crc");
	ADD_XMLITEM(parant.szTrustDes,"parant-trust-type");
	ADD_XMLITEM(parant.szProcessName,"parant-name");
	ADD_XMLITEM(parant.szProcessPath,"parant-path");
	ADD_XMLITEM(parant.szProcessDescribe,"parant-desc");
	ADD_XMLITEM(parant.szSign,"parant-signer");
	ADD_XMLITEM(parant.szMd5,"parant-file-md5");

	ADD_XMLITEM(child,"path");

	return GET_XML;
}

//-------------------------------------------------------------------------------

PKGBarActionReg_Q::PKGBarActionReg_Q()
{
	wAction			= (WORD)-1;
	wOperation		= (WORD)-1;
	dwTimeStamp		= 0;
}

void PKGBarActionReg_Q::ParseBodyData(std::string szXml)
{
	PARSE_XML_BEGIN;

	PARSE_ITEM_NUMBER(wAction,"action");
	PARSE_ITEM_NUMBER(wOperation,"opration");
	PARSE_ITEM_NUMBER(dwTimeStamp,"operate-time");

	PARSE_ITEM_NUMBER (parant.dwKey,"parant-key");
	PARSE_ITEM_NUMBER (parant.dwPid,"parant-pid");
	PARSE_ITEM_NUMBER (parant.unCrc,"parant-file-crc");
	PARSE_ITEM_CSTRING(parant.szTrustDes,"parant-trust-type",sizeof parant.szTrustDes);
	PARSE_ITEM_CSTRING(parant.szProcessName,"parant-name",sizeof parant.szProcessName);
	PARSE_ITEM_CSTRING(parant.szProcessPath,"parant-path",sizeof parant.szProcessPath);
	PARSE_ITEM_CSTRING(parant.szProcessDescribe,"parant-desc",sizeof parant.szProcessDescribe);
	PARSE_ITEM_CSTRING(parant.szSign,"parant-signer",sizeof parant.szSign);
	PARSE_ITEM_CSTRING(parant.szMd5,"parant-file-md5",sizeof parant.szMd5);

	PARSE_ITEM_STDSTR(child,"path");

	PARSE_XML_END;
}

std::string PKGBarActionReg_Q::MakeBodyData()
{
	USES_XML;

	ADD_XMLITEM(wAction,"action");
	ADD_XMLITEM(wOperation,"opration");
	ADD_XMLITEM(dwTimeStamp,"operate-time");

	ADD_XMLITEM (parant.dwKey,"parant-key");
	ADD_XMLITEM(parant.dwPid,"parant-pid");
	ADD_XMLITEM(parant.unCrc,"parant-file-crc");
	ADD_XMLITEM(parant.szTrustDes,"parant-trust-type");
	ADD_XMLITEM(parant.szProcessName,"parant-name");
	ADD_XMLITEM(parant.szProcessPath,"parant-path");
	ADD_XMLITEM(parant.szProcessDescribe,"parant-desc");
	ADD_XMLITEM(parant.szSign,"parant-signer");
	ADD_XMLITEM(parant.szMd5,"parant-file-md5");

	ADD_XMLITEM(child,"path");

	return GET_XML;
}

//-------------------------------------------------------------------------------

PKGBarADLFileTrustInfo_Q::PKGBarADLFileTrustInfo_Q()
{
	dwFileCrc = 0;
	dwFileNameCrc = 0;
	cbTrustType = (BYTE)(-1);
}

void PKGBarADLFileTrustInfo_Q::ParseBodyData(std::string szXml)
{
	PARSE_XML_BEGIN;

	PARSE_ITEM_NUMBER(dwFileCrc,"file_crc");
	PARSE_ITEM_NUMBER(dwFileNameCrc,"filename_crc");
	PARSE_ITEM_NUMBER(cbTrustType,"trust_sign");
	PARSE_ITEM_STDSTR(sFilePath,"filename");
	PARSE_ITEM_STDSTR(sSigner,"company");

	PARSE_XML_END;
}

std::string PKGBarADLFileTrustInfo_Q::MakeBodyData()
{
	USES_XML;

	ADD_XMLITEM(dwFileCrc,"file_crc");
	ADD_XMLITEM(dwFileNameCrc,"filename_crc");
	ADD_XMLITEM(cbTrustType,"trust_sign");
	ADD_XMLITEM(sFilePath,"filename");
	ADD_XMLITEM(sSigner,"company");

	return GET_XML;
}

//-------------------------------------------------------------------------------
std::string PKGBarProcessListOnSysStart_Q::MakeBodyData()
{
	USES_XML;

	for (std::list<ProcessInfoEx>::iterator it = processList.begin();
		it != processList.end();it++)
	{
		ADDSTART_CHILDNODE("p");

		ADD_XMLITEM((*it).dwKey,"key");
		ADD_XMLITEM((*it).dwPid,"pid");
		ADD_XMLITEM((*it).dwParantPid,"parant-pid");
		ADD_XMLITEM((*it).dwStartTm,"start-time");
		ADD_XMLITEM((*it).unCrc,"file-crc");
		ADD_XMLITEM((*it).szProcessDescribe,"file-des");
		ADD_XMLITEM((*it).szProcessPath,"path");
		ADD_XMLITEM((*it).szSign,"signer");
		ADD_XMLITEM((*it).szMd5,"file-md5");

		ADDEND_CHILDNODE("p");
	}
	

	return GET_XML;
}

void PKGBarProcessListOnSysStart_Q::ParseBodyData(std::string szXml)
{
	PARSE_XML_BEGIN;
	ProcessInfoEx procInfoEx;

	while(PARSE_XML_FINDELEM("p"))
	{
		PARSE_XML_INTOELEM;

		memset(&procInfoEx,0,sizeof(procInfoEx));
		PARSE_ITEM_NUMBER(procInfoEx.dwKey,"key");
		PARSE_ITEM_NUMBER(procInfoEx.dwPid,"pid");
		PARSE_ITEM_NUMBER(procInfoEx.dwParantPid,"parant-pid");
		PARSE_ITEM_NUMBER(procInfoEx.dwStartTm,"start-time");
		PARSE_ITEM_NUMBER(procInfoEx.unCrc,"file-crc");
		PARSE_ITEM_CSTRING(procInfoEx.szProcessDescribe,"file-des",sizeof(procInfoEx.szProcessDescribe)-1);
		PARSE_ITEM_CSTRING(procInfoEx.szProcessPath,"path",sizeof(procInfoEx.szProcessPath)-1);
		PARSE_ITEM_CSTRING(procInfoEx.szSign,"signer",sizeof(procInfoEx.szSign)-1);
		PARSE_ITEM_CSTRING(procInfoEx.szMd5,"file-md5",sizeof(procInfoEx.szMd5)-1);

		processList.push_back(procInfoEx);

		PARSE_XML_OUTOFELEM;
	}

	PARSE_XML_END;
}
//-------------------------------------------------------------------------------
PKGBarReportCpuInfo_Q::PKGBarReportCpuInfo_Q()
{

}

void PKGBarReportCpuInfo_Q::ParseBodyData(std::string szXml)
{
	PARSE_XML_BEGIN;
	PKGBarCpuInfo cpuinfo;

	while(PARSE_XML_FINDELEM("rp"))
	{
		PARSE_XML_INTOELEM;

		memset(&cpuinfo,0,sizeof(cpuinfo));
		PARSE_ITEM_STDSTR(cpuinfo.pname,"pname");
		PARSE_ITEM_NUMBER(cpuinfo.pid,"pid");
		PARSE_ITEM_NUMBER(cpuinfo.cpu,"cpu");
		PARSE_ITEM_NUMBER(cpuinfo.mem,"mem");

		cpuinfo_list.push_back(cpuinfo);

		PARSE_XML_OUTOFELEM;
	}

	PARSE_XML_END;
}

std::string PKGBarReportCpuInfo_Q::MakeBodyData()
{
	USES_XML;

	for (std::list<PKGBarCpuInfo>::iterator it = cpuinfo_list.begin();
		it != cpuinfo_list.end();it++)
	{
		ADDSTART_CHILDNODE("rp");

		ADD_XMLITEM((*it).pname,"pname");
		ADD_XMLITEM((*it).pid,"pid");
		ADD_XMLITEM((*it).cpu,"cpu");
		ADD_XMLITEM((*it).mem,"mem");

		ADDEND_CHILDNODE("rp");
	}

	return GET_XML;
}

//-------------------------------------------------------------------------

std::string PKGBarPluginReport_Q::MakeBodyData()
{
	USES_XML;

	ADD_XMLITEM(nModuleId,"moduleid");
	ADD_XMLITEM(sLog,"txt");

	return GET_XML;
}

void PKGBarPluginReport_Q::ParseBodyData(std::string szXml)
{
	PARSE_XML_BEGIN;

	PARSE_ITEM_NUMBER(nModuleId,"moduleid");
	PARSE_ITEM_STDSTR(sLog,"txt");

	PARSE_XML_END;
}

//-------------------------------------------------------------------------------

PKGCloseOrOpenServer_A::PKGCloseOrOpenServer_A(PKGCommon_A* pDependObj):PKGCommon_A(pDependObj)
{
	nOperType = -1;
}

std::string PKGCloseOrOpenServer_A::MakeBodyData()
{
	USES_XML;

	ADD_XMLITEM(nOperType,"opertype");

	return GET_XML;
}

void PKGCloseOrOpenServer_A::ParseBodyData(std::string szXml)
{
	PARSE_XML_BEGIN;

	PARSE_ITEM_NUMBER(nOperType,"opertype");

	PARSE_XML_END;
}

//-------------------------------------------------------------------------------
PKGBarSendWhiteList_Q::PKGBarSendWhiteList_Q()
{

}  
std::string PKGBarSendWhiteList_Q::MakeBodyData()
{
	USES_XML;

	for (std::list<PKGWhiteInfo>::iterator it = whitelist.begin();
		it != whitelist.end();it++)
	{
		ADDSTART_CHILDNODE("whitelist");

		ADD_XMLITEM((*it).strProcName,"procname");
		ADD_XMLITEM((*it).dwProcMD5,"procmd5");
		ADD_XMLITEM((*it).dwProcCRC,"proccrc");

		ADDEND_CHILDNODE("white");
	}

	return GET_XML;
}
void PKGBarSendWhiteList_Q::ParseBodyData(std::string szXml)
{
	PARSE_XML_BEGIN;
	PKGWhiteInfo procInfoEx;

	while(PARSE_XML_FINDELEM("whitelist"))
	{
		PARSE_XML_INTOELEM;

		memset(&procInfoEx,0,sizeof(procInfoEx));
		PARSE_ITEM_STDSTR(procInfoEx.strProcName,"procname");
		PARSE_ITEM_NUMBER(procInfoEx.dwProcMD5,"procmd5");
		PARSE_ITEM_NUMBER(procInfoEx.dwProcCRC,"proccrc");

		whitelist.push_back(procInfoEx);

		PARSE_XML_OUTOFELEM;
	}

	PARSE_XML_END;
}
//-------------------------------------------------------------------------------
PKGBarClearLog_Q::PKGBarClearLog_Q()
{
	
}

void PKGBarClearLog_Q::ParseBodyData(std::string szXml)
{
	PARSE_XML_BEGIN;

	PARSE_ITEM_STDSTR(strLogPath,"logpath");

	PARSE_XML_END;
}

std::string PKGBarClearLog_Q::MakeBodyData()
{
	USES_XML;

	ADD_XMLITEM(strLogPath,"logpath");

	return GET_XML;
}