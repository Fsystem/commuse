/** ���Դ���ӿ�
*   @FileName  : IStagety.h
*   @Author    : Double Sword(doublesword.jack@gmail.com)
*   @date      : 2016-4-7
*   @Copyright : Copyright belong to Author and 9u2,CO.LTD
*   @RefDoc    : --
*/

#ifndef __StagetyBase_H
#define __StagetyBase_H

/************************************************************************/
/*! ��Ϊ���� */
/************************************************************************/
enum ActionType
{
	enActionNull=-1,//δ���õ���Ϊ
	enActionDriver,	//������Ϊ����
	enActioProcess,	//������Ϊ����
	enActionTypeMax,
};

/************************************************************************/
/*! �������� */
/************************************************************************/
enum OperateType
{
	enOperateNull=-1,										//δ���õĲ���
	enLoadDriver,											//��������
	enTryTerminateProcess	= 0x01,							//���Խ�������
	enWritrProcess			= 0x02,							//����ע�����
	enMessageExit			= 0x04,							//��Ŀ����̷����˳���Ϣ
	enCreateProcess			= 0x08,							//����Ŀ�����
	enTerminateProcess		= 0x10,							//��������
};

//������Ϣ
//static TCHAR* gszOperateDescrier[]= \
//{
//	TEXT("δ���õĲ���"),
//	TEXT("��������"),
//	TEXT("���Խ�������"),
//	TEXT("����ע�����"),
//	TEXT("�����˳���Ϣ"),
//	TEXT("��������"),
//	TEXT("��������"),
//};

/************************************************************************/
/*! �������� */
/************************************************************************/
enum SafeStagetyType
{
	enAppStagety,											//Ӧ�ó������
	enCompanyStagety,										//��˾����
	enDriverStagety,										//��������
	enGameStagety,											//��Ϸ����
	enMaxStagety,
};


/************************************************************************/
/*! ���η�ʽ */                                                            
/************************************************************************/
enum TrustKind
{
	enTrustNull = 0,						//δ��ʼ������
	//enNotTrust,							//�����γ���
	enTrustByMd5AppUser,					//MD5����
	enTrustByMd5AppSys,						//MD5����
	enTrustByCompanyUser,					//��������
	enTrustByCompanySys,					//��������
	enTrustByDigalSignCompany,				//����ǩ������
	enTrustByMd5DriverUser,					//��������
	enTrustByMd5DriverSys,					//��������
	enTrustByOS,							//����ϵͳ����
	
	enTrustNotByMd5AppUser,					//MD5����
	enTrustNotByMd5AppSys,					//MD5����
	enTrustNotByCompanyUser,				//��������
	enTrustNotByCompanySys,					//��������
	enTrustNotByDigalSignCompany,			//����ǩ������
	enTrustNotByMd5DriverUser,				//��������
	enTrustNotByMd5DriverSys,				//��������
	enTrustNotByOS,							//����ϵͳ����
	enTrustNotByCompanyName,				//��˾���Ʋ�����

	enTrustKindMax,
};

//������Ϣ
static TCHAR* gszTrustDescrier[enTrustKindMax]= \
{
	TEXT("��ʼ��״̬"),
	TEXT("�û�MD5����"),
	TEXT("ϵͳMD5����"),
	TEXT("�û���������"),
	TEXT("ϵͳ��������"),
	TEXT("��������ǩ������"),
	TEXT("�û���������"),
	TEXT("ϵͳ��������"),
	TEXT("����ϵͳ����"),

	TEXT("�û�MD5������"),
	TEXT("ϵͳMD5������"),
	TEXT("�û����̲�����"),
	TEXT("ϵͳ���̲�����"),
	TEXT("��������ǩ��������"),
	TEXT("�û�����������"),
	TEXT("ϵͳ����������"),
	TEXT("����ϵͳ������"),
	TEXT("���̲�����"),
};

//��������֤
bool VerifyTrust(TrustKind trust_kind);
//��������֤
bool VerifyNoTrust(TrustKind trust_kind);

//-------------------------------------------------------------------------------
// ���Ե����ݽṹ
//-------------------------------------------------------------------------------
/*!
* Ӧ�ó����������ԭ��
*/
typedef struct AppStagetyData
{
	std::string signer;
	std::string serial;
	std::string filedesc;
	std::string path;
	std::string name;
	std::string md5;
	unsigned int is_trust;
}AppStagetyData;


/*!
* ��˾��������ԭ��
*/
typedef struct CompanyStagetyData
{
	std::string		name;
	std::string		number;
	unsigned int	is_trust;
	unsigned int	orient_status;
	std::string		desc;
	unsigned int	is_define;     //0:system 1:define
	std::string		picture;        //����������ͼƬ
}CompanyStagetyData;

/*!
* ������������ԭ��
*/
typedef struct DriverStagetyData
{
	std::string md5;
	std::string name;
	unsigned int is_trust;
	unsigned int orient_status;
}DriverStagetyData;

/*!
* ��Ϸ��������ԭ��
*/
typedef struct GameStagetyData
{

}GameStagetyData;

/*!
* ����ϵͳ��������ԭ��
*/
typedef struct ADLStagetyData
{
	unsigned int unCrcPath;
	unsigned int unCrcContent;
	std::string szName;
	TrustKind cbTrustKind;
	std::string szCompany;
}ADLStagetyData;

//-------------------------------------------------------------------------------
// ����Ϊ������Ҫʹ�õ����ݽṹ
//-------------------------------------------------------------------------------

/*!
* ������Ϣ
*/
typedef struct ProcessInfoStagety
{
	DWORD					dwPid;				//����ID
	TrustKind				cbVerifyResult;		//�ο�TrustKind
	unsigned int			unCrc;				//�ļ���CRC
	std::string				szProcessPath;		//����·��
	std::string				szSign;				//�ļ�ǩ��
	std::string				szMd5;				//md5
}ProcessInfoStagety;

/*!
* ���̲������
*/
typedef struct ActionOperateResult
{
	ProcessInfoStagety		parantProcess;		//������
	ProcessInfoStagety		childProcess;		//�ӽ���-�������̲����Ľ���
	DWORD					dwTimeStamp;		//������ʱ���
	ActionType				actionType;			//�ο�ActionTypeö��
	OperateType				operateType;		//�ο�OperateTypeö��
	TCHAR					szDescriber[1024];	//������Ϣ
	int						nTrusted;			//0-ϵͳ���� 1-ϵͳ��ֹ 2-�û����� 3-�û���ֹ
	ActionOperateResult()
	{
		parantProcess.cbVerifyResult=enTrustNull;
		parantProcess.szMd5="";
		parantProcess.szProcessPath="";
		parantProcess.szSign="";
		childProcess = parantProcess;
		dwTimeStamp = GetTickCount();
		actionType = enActionNull;
		operateType = enOperateNull;
		nTrusted = 1;
		memset(szDescriber,0,1024);
	}
}ActionOperateResult;

//-------------------------------------------------------------------------------
// ���Ի���
//-------------------------------------------------------------------------------


typedef std::list<AppStagetyData> AppStageList;
typedef std::list<CompanyStagetyData> CompanyStageList;
typedef std::list<DriverStagetyData> DriverStageList;
typedef std::list<ADLStagetyData> ADLStagetyList;
// typedef CList<AppStagetyData,AppStagetyData&> AppStageList;
// typedef CList<CompanyStagetyData,CompanyStagetyData&> CompanyStageList;
// typedef CList<DriverStagetyData,DriverStagetyData&> DriverStageList;

class IStagety
{
public:
	IStagety();
	virtual ~IStagety();
	virtual ActionType GetActionType()=0;
	virtual OperateType GetOperateType()=0;
	virtual int HandleDriver(LPCSTR pOldPath, LPCSTR pDriverName)=0; //�������� 0-���� ����Ϊ������
	virtual int HandleProcess(ActionType action,OperateType optType, ProcessInfoStagety* parant, ProcessInfoStagety* child)=0;//������� 0-���� ����Ϊ������
protected:
	virtual std::string GetOperateInfoDescribe(DWORD dwActionOperate);
	virtual bool VerifyProcess(ProcessInfoStagety* pProcessInfo);				//��֤���̣�ͨ�����²���
	virtual TrustKind VerifyByUserApp(ProcessInfoStagety* pProcessInfo);		//��֤�û�App����
	virtual TrustKind VerifyBySysApp(ProcessInfoStagety* pProcessInfo);			//��֤ϵͳApp����
	virtual TrustKind VerifyByUserCompany(ProcessInfoStagety* pProcessInfo);	//��֤�û����̲���
	virtual TrustKind VerifyBySysCompany(ProcessInfoStagety* pProcessInfo);		//��֤ϵͳ���̲���
	virtual TrustKind VerifyByUserDriver(ProcessInfoStagety* pProcessInfo);		//��֤�û���������
	virtual TrustKind VerifyBySysDriver(ProcessInfoStagety* pProcessInfo);		//��֤ϵͳ��������
	//virtual TrustKind VerifyByUserGame(ProcessInfoStagety* pProcessInfo);		//��֤�û���Ϸ����
	//virtual TrustKind VerifyBySysGame(ProcessInfoStagety* pProcessInfo);		//��֤ϵͳ��Ϸ����
	virtual TrustKind VerifyByDigitSign(ProcessInfoStagety* pProcessInfo);		//��֤����ǩ��
	virtual TrustKind VerifyByOS(ProcessInfoStagety* pProcessInfo);				//����ϵͳ����
	virtual TrustKind VerifyByADL(ProcessInfoStagety* pProcessInfo);			//ͨ��WinTrustFile֮����ж�
	virtual TrustKind VerifyBySysCompanyName(ProcessInfoStagety* pProcessInfo);	//���ͨ���Ƿ��г���������֤

protected:
	ActionType						mActionType;					//��Ϊ����
	OperateType						mOperateType;					//��������
	//Ӧ�ò��Ա�
	AppStageList					mListAppStagetyData;			//app.dat�������б�
	AppStageList					mListAppUserStagetyData;		//app_user.dat�������б�
	//��˾���Ա�
	CompanyStageList				mListCompanyStagetyData;		//company.dat�������б�
	CompanyStageList				mListCompanyUserStagetyData;	//company_user.dat�������б�
	//��������
	DriverStageList					mListDriverStagetyData;			//��������
	DriverStageList					mListDriverUserStagetyData;		//�û���������
	//ADL����
	ADLStagetyList					mListADLStagetyData;			//ADL����
};


//-------------------------------------------------------------------------------
// ����Ϊ������Դ�����Ϻ���Ϣ
//-------------------------------------------------------------------------------

/*! ����֪ͨ */
class NotifyStagety
{
public:
	static NotifyStagety* Builder(const ActionOperateResult& result,ActionType action,OperateType opt);
	static NotifyStagety* Builder();
	
	void Retain();
	void Release();

	void SetActionResult(const ActionOperateResult& result){mActionResult = result;};
	inline ActionOperateResult& GetActionResult(){return mActionResult;};
	inline void SetAtionOperate(ActionType action,OperateType opt){mActionType=action;mOperateType=opt;}
	inline ActionType GetActionType(){return mActionType;}
	inline OperateType GetOperateType(){return mOperateType;}
protected:
	NotifyStagety();
	virtual ~NotifyStagety();

	ActionOperateResult mActionResult;

	ActionType mActionType;		//��Ϊ����
	OperateType mOperateType;	//��������
private:
	unsigned int uRefCount;
};

//��Ϊ��������Ӧ�ӿ�
struct IActionResultDelegate
{
	virtual void OnHandleResultByStagety(ActionOperateResult* pResult) = 0;
};
 


#endif //__StagetyBase_H