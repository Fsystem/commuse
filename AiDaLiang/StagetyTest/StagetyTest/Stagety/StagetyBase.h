/** 策略处理接口
*   @FileName  : IStagety.h
*   @Author    : Double Sword(doublesword.jack@gmail.com)
*   @date      : 2016-4-7
*   @Copyright : Copyright belong to Author and 9u2,CO.LTD
*   @RefDoc    : --
*/

#ifndef __StagetyBase_H
#define __StagetyBase_H

/************************************************************************/
/*! 行为类型 */
/************************************************************************/
enum ActionType
{
	enActionNull=-1,//未设置的行为
	enActionDriver,	//驱动行为类型
	enActioProcess,	//进程行为类型
	enActionTypeMax,
};

/************************************************************************/
/*! 操作类型 */
/************************************************************************/
enum OperateType
{
	enOperateNull=-1,										//未设置的操作
	enLoadDriver,											//加载驱动
	enTryTerminateProcess	= 0x01,							//尝试结束进程
	enWritrProcess			= 0x02,							//尝试注入进程
	enMessageExit			= 0x04,							//向目标进程发送退出消息
	enCreateProcess			= 0x08,							//创建目标进程
	enTerminateProcess		= 0x10,							//结束进程
};

//信任消息
//static TCHAR* gszOperateDescrier[]= \
//{
//	TEXT("未设置的操作"),
//	TEXT("加载驱动"),
//	TEXT("尝试结束进程"),
//	TEXT("尝试注入进程"),
//	TEXT("发送退出消息"),
//	TEXT("创建进程"),
//	TEXT("结束进程"),
//};

/************************************************************************/
/*! 策略种类 */
/************************************************************************/
enum SafeStagetyType
{
	enAppStagety,											//应用程序策略
	enCompanyStagety,										//公司策略
	enDriverStagety,										//驱动策略
	enGameStagety,											//游戏策略
	enMaxStagety,
};


/************************************************************************/
/*! 信任方式 */                                                            
/************************************************************************/
enum TrustKind
{
	enTrustNull = 0,						//未初始化信任
	//enNotTrust,							//非信任程序
	enTrustByMd5AppUser,					//MD5信任
	enTrustByMd5AppSys,						//MD5信任
	enTrustByCompanyUser,					//厂商信任
	enTrustByCompanySys,					//厂商信任
	enTrustByDigalSignCompany,				//数字签名信任
	enTrustByMd5DriverUser,					//驱动信任
	enTrustByMd5DriverSys,					//驱动信任
	enTrustByOS,							//操作系统信任
	
	enTrustNotByMd5AppUser,					//MD5信任
	enTrustNotByMd5AppSys,					//MD5信任
	enTrustNotByCompanyUser,				//厂商信任
	enTrustNotByCompanySys,					//厂商信任
	enTrustNotByDigalSignCompany,			//数字签名信任
	enTrustNotByMd5DriverUser,				//驱动信任
	enTrustNotByMd5DriverSys,				//驱动信任
	enTrustNotByOS,							//操作系统信任
	enTrustNotByCompanyName,				//公司名称不信任

	enTrustKindMax,
};

//信任消息
static TCHAR* gszTrustDescrier[enTrustKindMax]= \
{
	TEXT("初始化状态"),
	TEXT("用户MD5信任"),
	TEXT("系统MD5信任"),
	TEXT("用户厂商信任"),
	TEXT("系统厂商信任"),
	TEXT("厂商数字签名信任"),
	TEXT("用户驱动信任"),
	TEXT("系统驱动信任"),
	TEXT("操作系统信任"),

	TEXT("用户MD5不信任"),
	TEXT("系统MD5不信任"),
	TEXT("用户厂商不信任"),
	TEXT("系统厂商不信任"),
	TEXT("厂商数字签名不信任"),
	TEXT("用户驱动不信任"),
	TEXT("系统驱动不信任"),
	TEXT("操作系统不信任"),
	TEXT("厂商不信任"),
};

//白名单验证
bool VerifyTrust(TrustKind trust_kind);
//黑名单验证
bool VerifyNoTrust(TrustKind trust_kind);

//-------------------------------------------------------------------------------
// 策略的数据结构
//-------------------------------------------------------------------------------
/*!
* 应用程序策略数据原型
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
* 公司策略数据原型
*/
typedef struct CompanyStagetyData
{
	std::string		name;
	std::string		number;
	unsigned int	is_trust;
	unsigned int	orient_status;
	std::string		desc;
	unsigned int	is_define;     //0:system 1:define
	std::string		picture;        //代表是哪张图片
}CompanyStagetyData;

/*!
* 驱动策略数据原型
*/
typedef struct DriverStagetyData
{
	std::string md5;
	std::string name;
	unsigned int is_trust;
	unsigned int orient_status;
}DriverStagetyData;

/*!
* 游戏策略数据原型
*/
typedef struct GameStagetyData
{

}GameStagetyData;

/*!
* 操作系统策略数据原型
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
// 以下为程序需要使用的数据结构
//-------------------------------------------------------------------------------

/*!
* 进程信息
*/
typedef struct ProcessInfoStagety
{
	DWORD					dwPid;				//进程ID
	TrustKind				cbVerifyResult;		//参考TrustKind
	unsigned int			unCrc;				//文件的CRC
	std::string				szProcessPath;		//进程路径
	std::string				szSign;				//文件签名
	std::string				szMd5;				//md5
}ProcessInfoStagety;

/*!
* 进程操作结果
*/
typedef struct ActionOperateResult
{
	ProcessInfoStagety		parantProcess;		//父进程
	ProcessInfoStagety		childProcess;		//子进程-被父进程操作的进程
	DWORD					dwTimeStamp;		//操作的时间戳
	ActionType				actionType;			//参考ActionType枚举
	OperateType				operateType;		//参考OperateType枚举
	TCHAR					szDescriber[1024];	//描述信息
	int						nTrusted;			//0-系统放行 1-系统阻止 2-用户放行 3-用户阻止
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
// 策略基类
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
	virtual int HandleDriver(LPCSTR pOldPath, LPCSTR pDriverName)=0; //处理驱动 0-信任 其他为不信任
	virtual int HandleProcess(ActionType action,OperateType optType, ProcessInfoStagety* parant, ProcessInfoStagety* child)=0;//处理进程 0-信任 其他为不信任
protected:
	virtual std::string GetOperateInfoDescribe(DWORD dwActionOperate);
	virtual bool VerifyProcess(ProcessInfoStagety* pProcessInfo);				//验证进程，通过以下策略
	virtual TrustKind VerifyByUserApp(ProcessInfoStagety* pProcessInfo);		//验证用户App策略
	virtual TrustKind VerifyBySysApp(ProcessInfoStagety* pProcessInfo);			//验证系统App策略
	virtual TrustKind VerifyByUserCompany(ProcessInfoStagety* pProcessInfo);	//验证用户厂商策略
	virtual TrustKind VerifyBySysCompany(ProcessInfoStagety* pProcessInfo);		//验证系统厂商策略
	virtual TrustKind VerifyByUserDriver(ProcessInfoStagety* pProcessInfo);		//验证用户驱动策略
	virtual TrustKind VerifyBySysDriver(ProcessInfoStagety* pProcessInfo);		//验证系统驱动策略
	//virtual TrustKind VerifyByUserGame(ProcessInfoStagety* pProcessInfo);		//验证用户游戏策略
	//virtual TrustKind VerifyBySysGame(ProcessInfoStagety* pProcessInfo);		//验证系统游戏策略
	virtual TrustKind VerifyByDigitSign(ProcessInfoStagety* pProcessInfo);		//验证数字签名
	virtual TrustKind VerifyByOS(ProcessInfoStagety* pProcessInfo);				//操作系统信任
	virtual TrustKind VerifyByADL(ProcessInfoStagety* pProcessInfo);			//通过WinTrustFile之后的判断
	virtual TrustKind VerifyBySysCompanyName(ProcessInfoStagety* pProcessInfo);	//最后通过是否有厂商名称验证

protected:
	ActionType						mActionType;					//行为类型
	OperateType						mOperateType;					//操作类型
	//应用策略表
	AppStageList					mListAppStagetyData;			//app.dat的数据列表
	AppStageList					mListAppUserStagetyData;		//app_user.dat的数据列表
	//公司策略表
	CompanyStageList				mListCompanyStagetyData;		//company.dat的数据列表
	CompanyStageList				mListCompanyUserStagetyData;	//company_user.dat的数据列表
	//驱动策略
	DriverStageList					mListDriverStagetyData;			//驱动策略
	DriverStageList					mListDriverUserStagetyData;		//用户驱动策略
	//ADL策略
	ADLStagetyList					mListADLStagetyData;			//ADL策略
};


//-------------------------------------------------------------------------------
// 以下为程序策略处理完毕后消息
//-------------------------------------------------------------------------------

/*! 策略通知 */
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

	ActionType mActionType;		//行为类型
	OperateType mOperateType;	//操作类型
private:
	unsigned int uRefCount;
};

//行为处理结果相应接口
struct IActionResultDelegate
{
	virtual void OnHandleResultByStagety(ActionOperateResult* pResult) = 0;
};
 


#endif //__StagetyBase_H