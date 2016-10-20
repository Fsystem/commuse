#ifndef __cmndef_H
#define __cmndef_H

//常用变量
//#define STR_CLIENT_MASTER					TEXT("Client_Master.exe")
//#define STR_CLIENT_BUBBLE					TEXT("Small_Ball.exe")
//#define STR_CLIENT_PROTECT32				TEXT("MasterProtect.exe")
//#define STR_CLIENT_PROTECT64				TEXT("MasterProtect64b.exe")
//#define STR_CLIENT_TIP						TEXT("Tip_Window.exe")
//#define STR_DRIVER_PD_DLL					TEXT("tmpsys.dll")
//#define STR_DRIVER_FR_DLL					TEXT("tmpfilereg.dll")
//#define STR_MSG_PROTECT						TEXT("tmpsysprotect.dll")
//-------------------------------------------------------------------------------
// 新框架定义
//-------------------------------------------------------------------------------

#define		NETWORK_PASSWROD	"dalyT;32*&$@!@@410kdtsdf2"		//网络通信加密，密码

#define		NETWORK_VER			1								//通信协议版本号

#define		UDP_PACK_LEN		(64*1024*1024)					//UDP包的最大长度

#define		UDP_RECV_TIME_OUT	10								//UDP接收数据超时时间

#define		ZIP_PASSWORD		"12edsfi2#R@fwe"				//压缩包，加密

//-------------------------------------------------------------------------------
//网络定义
#define	HEAD_VER							1
#define	APP_VER								7
#define MAX_40K_BUFFER						40*1024
#define MSG_TAG_FLAG						0x20150801

#define STR_NET_KEY							"jdaf832rn,snde329r"
#define LEN_NET_KEY							strlen(STR_NET_KEY)

//互斥锁名称
#define MUTEX_TIPWINDOW						TEXT("MUTEX_TIP_WINDOW")
#define MUTEX_CLIENTMASTER					TEXT("MUTEX_CLIENT_MASTER")

//句柄有效
#define IsValidHandle(h) (h!=INVALID_HANDLE_VALUE && h!=NULL)

//关闭线程
#define SAFE_STOP_THREAD(h) if(IsValidHandle(h)){ ::TerminateThread(h,0);CloseHandle(h); h=NULL; }
//关闭事件
#define SAFE_CLOSE_EVENT(ev) if(IsValidHandle(ev)){::CloseHandle(ev);ev=NULL;}
#define SAFE_CLOSE_HANDLE(ev) if(IsValidHandle(ev)){::CloseHandle(ev);ev=NULL;}

//常用宏
#define PropertyMember(Type,name) private: Type _##name;\
	public: void set##name(Type v){_##name=v;}\
			Type get##name(){return (Type)_##name;}

#define PropertyMemberRef(Type,name) private: Type _##name;\
	public: void set##name(Type v){_##name=v;}\
	Type& get##name(){return (Type)_##name;}

#define PropertyMemberReadOnly(Type,name) private: Type _##name;\
	public: Type get##name(){return (Type)_##name;}

#define PropertyMemberRefReadOnly(Type,name) private: Type _##name;\
	public: const Type& get##name(){return (Type)_##name;}

//创建单例运行进程锁
#define CREATE_SINGLE_PROCESS_LOCK(globalName) \
	HANDLE hMutex = CreateMutexA(NULL,FALSE,globalName);\
	if(hMutex && GetLastError() == ERROR_ALREADY_EXISTS) { CloseHandle(hMutex);return 0;}
//关闭单例运行进程锁
#define CLOSE_SINGLE_PROCESS_LOCK if(hMutex){ CloseHandle(hMutex); hMutex = NULL; }

//注入测试
#define INJECT_PROCESS_TEST(name) { DWORD dwInjectPId_ = GetProcessIdByName(name);\
	if (dwInjectPId_ != 0)\
{\
	HANDLE h = OpenProcess(PROCESS_ALL_ACCESS,FALSE,dwInjectPId_);\
	if (h)\
	{\
	CloseHandle(h);\
	}\
}}

//-------------------------------------------------------------------------------
// 插件导出函数(如果dll不处理任何宿主传来的消息，可以不用导出)
//-------------------------------------------------------------------------------
#define  PLUGIN_EXPORT_FUNCNAME "CreatePlugInterface"

#if _MSC_VER >= 1400
#	define  IMP_PLUGIN_EXPORT(classname,...) extern "C" __declspec(dllexport) void* CreatePlugInterface(){\
	static classname* pModuleInterface = NULL; if(pModuleInterface == NULL){\
	pModuleInterface = new classname( ##__VA_ARGS__ );\
		}\
		return pModuleInterface;\
	}
#endif

#define  IMP_PLUGIN_EXPORT_EX(clazz_pointer) extern "C" __declspec(dllexport) void* CreatePlugInterface(){\
	return clazz_pointer;\
}

typedef void* (WINAPI *PFN_CreatePlugInterface)();

//-------------------------------------------------------------------------------
// 业务插件导出接口
//-------------------------------------------------------------------------------
#define  IMP_BUSSINESSPLUGIN_EXPORT(clazz_pointer) extern "C" __declspec(dllexport) PVOID GetPlugProxy (){\
	return clazz_pointer;\
}

//-------------------------------------------------------------------------------
// 网络管理接口
//-------------------------------------------------------------------------------
enum enNetMethod
{
	enTCP,
	enUDP,
};

#endif //__cmndef_H