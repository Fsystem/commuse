#include <StdAfx.h>
#include "StagetyInclude.h"
#include "StagetyBase.h"

#include <io.h>
#include <atlconv.h>

//-------------------------------------------------------------------------------
//��������֤
bool VerifyTrust(TrustKind trust_kind)
{																			
	bool bRet = (trust_kind>enTrustNull && trust_kind<enTrustNotByMd5AppUser); 
	return (bRet);																	
}
//��������֤
bool VerifyNoTrust(TrustKind trust_kind)
{																			
	bool bRet = (VerifyTrust(trust_kind)==false && trust_kind!= enTrustNull); 
	return (bRet);																	
}
//-------------------------------------------------------------------------------

#define StagetyFile_App				TEXT("app.dat")
#define StagetyFile_AppUser			TEXT("app_user.dat")

#define StagetyFile_Company			TEXT("company.dat")
#define StagetyFile_CompanyUser		TEXT("company_user.dat")

#define StagetyFile_Driver			TEXT("sys.dat")
#define StagetyFile_DriverUser		TEXT("sys_user.dat")

#define StagetyFile_Game			TEXT("game.dat")
#define StagetyFile_GameUser		TEXT("game_user.dat")


bool LoadAppStagety(LPCTSTR pszFileName,AppStageList& returnList)
{
	USES_CONVERSION;

	returnList.clear();

	TCHAR szAppDataFilePath[MAX_PATH]={0};
	::GetModuleFileName(NULL,szAppDataFilePath,MAX_PATH);
	_tcsrchr(szAppDataFilePath,'\\')[1]=0;
#pragma warning (push)
#pragma warning(disable : 4996)
	_sntprintf(szAppDataFilePath,MAX_PATH,TEXT("%setc\\%s"),szAppDataFilePath,pszFileName);
#pragma warning (pop)

	//�ж��ļ��Ƿ����
	if (_taccess(szAppDataFilePath,0) == -1)
	{
		//LOGEVEN(TEXT("[%s]������\n"),szAppDataFilePath);

		return true;
	}

	CMarkup parse;
	if (parse.Load(szAppDataFilePath))
	{
		if (parse.FindElem(_T("r")))
		{
			parse.IntoElem();
			while (parse.FindElem(_T("s")))
			{
				AppStagetyData xmlData;
				xmlData.signer    = string(T2A(parse.GetAttrib(_T("sign_man")).c_str()) );
				xmlData.serial    = string(T2A(parse.GetAttrib(_T("serial")).c_str()) );
				xmlData.filedesc  = string(T2A(parse.GetAttrib(_T("file_des")).c_str()) );
				xmlData.path      = string(T2A(parse.GetAttrib(_T("path")).c_str()) );
				xmlData.name      = string(T2A(parse.GetAttrib(_T("name")).c_str()) );
				xmlData.is_trust  = _ttoi(parse.GetAttrib(_T("trust")).c_str());
				xmlData.md5       = string(T2A(parse.GetAttrib(_T("md5")).c_str()) );
				returnList.push_back(xmlData);
			}

			return true;
		}//end if parse.FindElem
	}//end if parse.Load

	return false;
}

bool LoadCompanyStagety(LPCTSTR pszFileName,OUT CompanyStageList& returnList)
{
	USES_CONVERSION;

	returnList.clear();

	TCHAR szAppDataFilePath[MAX_PATH]={0};
	::GetModuleFileName(NULL,szAppDataFilePath,MAX_PATH);
	_tcsrchr(szAppDataFilePath,'\\')[1]=0;
#pragma warning (push)
#pragma warning(disable : 4996)
	_sntprintf(szAppDataFilePath,MAX_PATH,TEXT("%setc\\%s"),szAppDataFilePath,pszFileName);
#pragma warning (pop)

	//�ж��ļ��Ƿ����
	if (_taccess(szAppDataFilePath,0) == -1)
	{
		//LOGEVEN(TEXT("[%s]������\n"),szAppDataFilePath);
		return true;
	}

	CMarkup parse;
	if (parse.Load(szAppDataFilePath))
	{
		if (parse.FindElem(_T("r")))
		{
			parse.IntoElem();
			while (parse.FindElem(_T("s")))
			{
				CompanyStagetyData xmlData;
				xmlData.name			= string( T2A(parse.GetAttrib(_T("name")).c_str() ) );
				if (xmlData.name.empty()) continue;
				xmlData.number			= string( T2A(parse.GetAttrib(_T("number")).c_str() ) );
				xmlData.is_trust		= _ttoi(parse.GetAttrib(_T("trust")).c_str() );
				xmlData.orient_status	= xmlData.is_trust;
				xmlData.desc			= string( T2A( parse.GetAttrib(_T("RemarkName")).c_str() ) );
				xmlData.is_define		= 0;
				xmlData.picture			= string( T2A(parse.GetAttrib(_T("picture")).c_str() ) );

				returnList.push_back(xmlData);
			}

			return true;
		}//end if parse.FindElem
	}//end if parse.Load

	return false;
}

bool LoadDriverStagety(LPCTSTR pszFileName,OUT DriverStageList& returnList)
{
	USES_CONVERSION;

	returnList.clear();

	TCHAR szAppDataFilePath[MAX_PATH]={0};
	::GetModuleFileName(NULL,szAppDataFilePath,MAX_PATH);
	_tcsrchr(szAppDataFilePath,'\\')[1]=0;
#pragma warning (push)
#pragma warning(disable : 4996)
	_sntprintf(szAppDataFilePath,MAX_PATH,TEXT("%setc\\%s"),szAppDataFilePath,pszFileName);
#pragma warning (pop)

	//�ж��ļ��Ƿ����
	if (_taccess(szAppDataFilePath,0) == -1)
	{
		//LOGEVEN(TEXT("[%s]������\n"),szAppDataFilePath);

		return true;
	}

	CMarkup parse;
	if (parse.Load(szAppDataFilePath))
	{
		if (parse.FindElem(_T("r")))
		{
			parse.IntoElem();
			while (parse.FindElem(_T("s")))
			{
				DriverStagetyData xmlData;
				xmlData.md5				= string( T2A(parse.GetAttrib(_T("md5")).c_str() ) );
				if (xmlData.md5.empty()) continue;
				xmlData.name			= string( T2A(parse.GetAttrib(_T("name")).c_str() ) );
				xmlData.is_trust		= _ttoi(parse.GetAttrib(_T("trust")).c_str() ) ;
				xmlData.orient_status	= xmlData.is_trust;

				returnList.push_back(xmlData);
			}

			return true;
		}//end if parse.FindElem
	}//end if parse.Load

	return false;
}


//-------------------------------------------------------------------------------

IStagety::IStagety()
{
	//if (bLoadedStagetyFile == false)
	{
		bool bRet = true;
		bRet = LoadAppStagety(StagetyFile_App,mListAppStagetyData);
		bRet &= LoadAppStagety(StagetyFile_AppUser,mListAppUserStagetyData);
		
		bRet &= LoadCompanyStagety(StagetyFile_Company,mListCompanyStagetyData);
		bRet &= LoadCompanyStagety(StagetyFile_CompanyUser,mListCompanyUserStagetyData);
		
		bRet &= LoadDriverStagety(StagetyFile_Driver,mListDriverStagetyData);
		bRet &= LoadDriverStagety(StagetyFile_DriverUser,mListDriverUserStagetyData);

		//bLoadedStagetyFile = bRet;
	}

	mActionType = enActionNull;
	mOperateType = enOperateNull;
}
IStagety::~IStagety()
{

}

//int IStagety::HandleDriver(LPCSTR pOldPath, LPCSTR pDriverName)
//{
//	LOGEVEN(TEXT("�Ҳ�֪��Ӧ����δ���"));
//	return 0;
//}
//
//int IStagety::HandleProcess(ActionType action,OperateType optType,ProcessInfoStagety& prant,ProcessInfoStagety& child)
//{
//	LOGEVEN(TEXT("�Ҳ�֪��Ӧ����δ���"));
//	return 0;
//}

/*!
* ͨ����Ϊ��������Ϊ<<16 | ����������ó����������
*/
std::string IStagety::GetOperateInfoDescribe(DWORD dwActionOperate) 
{
	TCHAR szDescrie[1024]={0};

	WORD wActionType = (WORD)(((DWORD)dwActionOperate>>16)&0xffff);
	WORD wOperateType = (WORD)((DWORD)dwActionOperate&0xffff);

	switch(wOperateType)
	{
	case enOperateNull:
		_stprintf(szDescrie,TEXT("δ֪����"));
		break;
	case enLoadDriver:
		_stprintf(szDescrie,TEXT("������������"));
		break;
	case enTryTerminateProcess:
		_stprintf(szDescrie,TEXT("������ֹ���̲���"));
		break;
	case enWritrProcess:
		_stprintf(szDescrie,TEXT("����ע����̲���"));
		break;
	case enMessageExit:
		_stprintf(szDescrie,TEXT("���Է����˳���Ϣ����"));
		break;
	case enCreateProcess:
		_stprintf(szDescrie,TEXT("�������̲���"));
		break;
	case enTerminateProcess:
		_stprintf(szDescrie,TEXT("��ֹ���̲���"));
		break;
	}

	USES_CONVERSION;
	return T2A(szDescrie);
}

//��֤���̣�ͨ�����²���
bool IStagety::VerifyProcess(ProcessInfoStagety* pProcessInfo)
{
	if(pProcessInfo->cbVerifyResult != enTrustNull) return VerifyTrust(pProcessInfo->cbVerifyResult);

	//1.�û�Ӧ�ò�����֤
	TrustKind trustKind = VerifyByUserApp(pProcessInfo);
	if ( VerifyTrust( trustKind ) ) { return true;}			//���������
	if ( VerifyNoTrust( trustKind ) ) { return false;}      //���������

	//2.Ĭ��Ӧ�ò�����֤
	trustKind = VerifyBySysApp(pProcessInfo);
	if ( VerifyTrust( trustKind ) ) { return true;}
	if ( VerifyNoTrust( trustKind ) ) { return false;} 

	//3.����ϵͳ������֤
	trustKind = VerifyByOS(pProcessInfo);
	//if ( VerifyTrust( trustKind ) ) { return true;}
	if ( VerifyNoTrust( trustKind ) ) { return false;} 

	//4.�û���˾������֤
	trustKind = VerifyByUserCompany(pProcessInfo);
	if ( VerifyTrust( trustKind ) ) { return true;}   
	if ( VerifyNoTrust( trustKind ) ) { return false;} 

	//5.Ĭ�Ϲ�˾������֤
	trustKind = VerifyBySysCompany(pProcessInfo);
	if ( VerifyTrust( trustKind ) ) { return true;}
	if ( VerifyNoTrust( trustKind ) ) { return false;} 

	//6.�ж��Ƿ��г��̣��з��س��̲����Σ��޲���ϵͳ����
	trustKind = VerifyBySysCompanyName(pProcessInfo);
	if ( VerifyTrust( trustKind ) ) { return true;}
	if ( VerifyNoTrust( trustKind ) ) { return false;} 

	

	return false;

}

//��֤�û�App����
TrustKind IStagety::VerifyByUserApp(ProcessInfoStagety* pProcessInfo)
{
	TrustKind trustInfo = enTrustNull;

	if (pProcessInfo->szMd5.size()>0)
	{
		for (AppStageList::iterator it = mListAppUserStagetyData.begin(); it != mListAppUserStagetyData.end(); ++it)
		{
			if(stricmp( it->md5.c_str(),  pProcessInfo->szMd5.c_str() )==0) 
			{
				it->is_trust==1?trustInfo = enTrustByMd5AppUser:trustInfo=enTrustNotByMd5AppUser;
				break;
			}
		}
	}
	
// 	POSITION pos = mListAppUserStagetyData.GetHeadPosition();
// 
// 	while(pos != NULL)
// 	{
// 		AppStagetyData stagety = mListAppUserStagetyData.GetNext(pos);
// 
// 		if(stricmp( stagety.md5.c_str(),  pProcessInfo->szMd5.c_str() )==0) 
// 		{
// 			stagety.is_trust==1?trustInfo = enTrustByMd5AppUser:trustInfo=enTrustNotByMd5AppUser;
// 			break;
// 		}
// 	}

	pProcessInfo->cbVerifyResult = trustInfo;

	return trustInfo;
}
//��֤ϵͳApp����
TrustKind IStagety::VerifyBySysApp(ProcessInfoStagety* pProcessInfo)
{
	TrustKind trustInfo = enTrustNull;
	if(pProcessInfo==NULL) return trustInfo;

	if (pProcessInfo->szMd5.size()>0)
	{
		for (AppStageList::iterator it = mListAppStagetyData.begin(); it != mListAppStagetyData.end(); ++it)
		{
			if(stricmp( it->md5.c_str(),  pProcessInfo->szMd5.c_str() )==0) 
			{
				it->is_trust==1?trustInfo = enTrustByMd5AppSys:trustInfo=enTrustNotByMd5AppSys;
				break;
			}
		}
	}
	
// 	POSITION pos = mListAppStagetyData.GetHeadPosition();
// 
// 	while(pos != NULL)
// 	{
// 		AppStagetyData stagety = mListAppStagetyData.GetNext(pos);
// 
// 		if(stricmp(stagety.md5.c_str() , pProcessInfo->szMd5.c_str())==0) 
// 		{
// 			stagety.is_trust==1?trustInfo = enTrustByMd5AppSys:trustInfo=enTrustNotByMd5AppSys;
// 			break;
// 		}
// 	}

	pProcessInfo->cbVerifyResult = trustInfo;

	return trustInfo;
}
//��֤�û����̲���
TrustKind IStagety::VerifyByUserCompany(ProcessInfoStagety* pProcessInfo)
{
	TrustKind trustInfo = enTrustNull;
	if(pProcessInfo==NULL) return trustInfo;

	if (pProcessInfo->szSign.size()>0)
	{
		for (CompanyStageList::iterator it = mListCompanyUserStagetyData.begin(); it != mListCompanyUserStagetyData.end(); ++it)
		{
			if(stricmp( it->name.c_str(), pProcessInfo->szSign.c_str()) == 0 ) 
			{
				it->is_trust==1?trustInfo = enTrustByCompanyUser:trustInfo = enTrustNotByCompanyUser;
				break;
			}
		}
	}

// 	POSITION pos = mListCompanyUserStagetyData.GetHeadPosition();
// 
// 	while(pos != NULL)
// 	{
// 		CompanyStagetyData stagety = mListCompanyUserStagetyData.GetNext(pos);
// 
// 		if(stricmp( stagety.name.c_str(), pProcessInfo->szSign.c_str()) == 0 ) 
// 		{
// 			stagety.is_trust==1?trustInfo = enTrustByCompanyUser:trustInfo = enTrustNotByCompanyUser;
// 			break;
// 		}
// 	}

	pProcessInfo->cbVerifyResult = trustInfo;

	return trustInfo;
}
//��֤ϵͳ���̲���
TrustKind IStagety::VerifyBySysCompany(ProcessInfoStagety* pProcessInfo)
{
	TrustKind trustInfo = enTrustNull;
	if(pProcessInfo==NULL) return trustInfo;

	if (pProcessInfo->szSign.size()>0)
	{
		for (CompanyStageList::iterator it = mListCompanyStagetyData.begin(); it != mListCompanyStagetyData.end(); ++it)
		{
			if(stricmp( it->name.c_str(), pProcessInfo->szSign.c_str()) == 0 ) 
			{
				it->is_trust==1?trustInfo = enTrustByCompanySys:trustInfo = enTrustNotByCompanySys;
				break;
			}
		}
	}
	
// 	POSITION pos = mListCompanyStagetyData.GetHeadPosition();
// 
// 	while(pos != NULL)
// 	{
// 		CompanyStagetyData stagety = mListCompanyStagetyData.GetNext(pos);
// 
// 		if(stricmp( stagety.name.c_str(), pProcessInfo->szSign.c_str()) == 0 ) 
// 		{
// 			stagety.is_trust==1? trustInfo = enTrustByCompanySys:trustInfo = enTrustNotByCompanySys;
// 			break;
// 		}
// 	}

	pProcessInfo->cbVerifyResult = trustInfo;

	return trustInfo;
}
//��֤�û���������
TrustKind IStagety::VerifyByUserDriver(ProcessInfoStagety* pProcessInfo)
{
	TrustKind trustInfo = enTrustNull;
	if(pProcessInfo==NULL) return trustInfo;

	for(DriverStageList::iterator it = mListDriverUserStagetyData.begin(); it != mListDriverUserStagetyData.end(); ++it)
	{
		if(stricmp( it->md5.c_str(),  pProcessInfo->szMd5.c_str() )==0) 
		{
			it->is_trust==1?trustInfo = enTrustByMd5DriverUser:trustInfo=enTrustNotByMd5DriverUser;
			break;
		}
	}

	pProcessInfo->cbVerifyResult = trustInfo;

	return trustInfo;
}
//��֤ϵͳ��������
TrustKind IStagety::VerifyBySysDriver(ProcessInfoStagety* pProcessInfo)
{
	TrustKind trustInfo = enTrustNull;
	if(pProcessInfo==NULL) return trustInfo;

	for(DriverStageList::iterator it = mListDriverStagetyData.begin(); it != mListDriverStagetyData.end(); ++it)
	{
		if(stricmp( it->md5.c_str(),  pProcessInfo->szMd5.c_str() )==0) 
		{
			it->is_trust==1?trustInfo = enTrustByMd5DriverSys:trustInfo=enTrustNotByMd5DriverSys;
			break;
		}
	}

	pProcessInfo->cbVerifyResult = trustInfo;

	return trustInfo;
}
////��֤�û���Ϸ����
//TrustKind IStagety::VerifyUserGame(ProcessInfoStagety* pProcessInfo)
//{
//
//}
////��֤ϵͳ��Ϸ����
//TrustKind IStagety::VerifySysGame(ProcessInfoStagety* pProcessInfo)
//{
//
//}
//��֤����ǩ��
TrustKind IStagety::VerifyByDigitSign(ProcessInfoStagety* pProcessInfo)
{
	TrustKind trustInfo = enTrustNull;
	if(pProcessInfo==NULL) return trustInfo;

	if (pProcessInfo->szSign.size()>0)
	{
		trustInfo = enTrustByDigalSignCompany;
	}

	pProcessInfo->cbVerifyResult = trustInfo;

	return trustInfo;
}

//����ϵͳ����
TrustKind IStagety::VerifyByOS(ProcessInfoStagety* pProcessInfo)
{
	TrustKind trustInfo = enTrustNull;
	if(pProcessInfo==NULL) return trustInfo;

	if (CheckFileTrust(pProcessInfo->szProcessPath.c_str()))
	{
		trustInfo = enTrustByOS;
	}
	else
	{
		trustInfo = enTrustNotByOS;
	}

	pProcessInfo->cbVerifyResult = trustInfo;

	return trustInfo;
}

//���ͨ���Ƿ��г���������֤
TrustKind IStagety::VerifyBySysCompanyName(ProcessInfoStagety* pProcessInfo)
{
	TrustKind trustInfo = enTrustNull;
	if(pProcessInfo==NULL) return trustInfo;

	if (pProcessInfo->szSign.empty()) trustInfo = enTrustByOS;
	else trustInfo = enTrustNotByCompanyName;

	pProcessInfo->cbVerifyResult = trustInfo;

	return trustInfo;
}

//-------------------------------------------------------------------------------
// ���Դ���֪ͨ
//-------------------------------------------------------------------------------

NotifyStagety* NotifyStagety::Builder(const ActionOperateResult& result,ActionType action,OperateType opt)
{
	NotifyStagety* mThis = new NotifyStagety();
	mThis->SetActionResult(result);
	mThis->SetAtionOperate(action,opt);
	return mThis;
}

NotifyStagety* NotifyStagety::Builder()
{
	ActionOperateResult result;
	return Builder(result,enActionNull,enOperateNull);
}

void NotifyStagety::Retain()
{
	++uRefCount;
}

void NotifyStagety::Release()
{
	if(--uRefCount == 0)
	{
		//if (mData)
		//{
		//	delete mData;
		//	mData = NULL;
		//}

		delete this;
	}
}

//void NotifyStagety::SetData(LPVOID pData,unsigned int uDataSize)
//{
//	if (uDataSize == 0 || pData == NULL) return;
//
//	if (mData)
//	{
//		delete mData;
//		mData = NULL;
//	}
//
//	if (mData == NULL)
//	{
//		mData = new char[uDataSize];
//		if (mData)
//		{
//			memcpy(mData,pData,uDataSize);
//			mDataSize = uDataSize;
//		}
//	}
//}

NotifyStagety::NotifyStagety()
{
	//mData = NULL;
	//mDataSize = 0;
	uRefCount = 1;

}

NotifyStagety::~NotifyStagety()
{
	/*if (mData)
	{
		delete mData;
		mData = NULL;
	}*/
}