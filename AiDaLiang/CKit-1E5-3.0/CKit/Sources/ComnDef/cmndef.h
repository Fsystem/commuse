#ifndef __cmndef_H
#define __cmndef_H

//���ñ���
//#define STR_CLIENT_MASTER					TEXT("Client_Master.exe")
//#define STR_CLIENT_BUBBLE					TEXT("Small_Ball.exe")
//#define STR_CLIENT_PROTECT32				TEXT("MasterProtect.exe")
//#define STR_CLIENT_PROTECT64				TEXT("MasterProtect64b.exe")
//#define STR_CLIENT_TIP						TEXT("Tip_Window.exe")
//#define STR_DRIVER_PD_DLL					TEXT("tmpsys.dll")
//#define STR_DRIVER_FR_DLL					TEXT("tmpfilereg.dll")
//#define STR_MSG_PROTECT						TEXT("tmpsysprotect.dll")
//-------------------------------------------------------------------------------
// �¿�ܶ���
//-------------------------------------------------------------------------------

#define		NETWORK_PASSWROD	"dalyT;32*&$@!@@410kdtsdf2"		//����ͨ�ż��ܣ�����

#define		NETWORK_VER			1								//ͨ��Э��汾��

#define		UDP_PACK_LEN		(64*1024*1024)					//UDP������󳤶�

#define		UDP_RECV_TIME_OUT	10								//UDP�������ݳ�ʱʱ��

#define		ZIP_PASSWORD		"12edsfi2#R@fwe"				//ѹ����������

//-------------------------------------------------------------------------------
//���綨��
#define	HEAD_VER							1
#define	APP_VER								7
#define MAX_40K_BUFFER						40*1024
#define MSG_TAG_FLAG						0x20150801

#define STR_NET_KEY							"jdaf832rn,snde329r"
#define LEN_NET_KEY							strlen(STR_NET_KEY)

//����������
#define MUTEX_TIPWINDOW						TEXT("MUTEX_TIP_WINDOW")
#define MUTEX_CLIENTMASTER					TEXT("MUTEX_CLIENT_MASTER")

//�����Ч
#define IsValidHandle(h) (h!=INVALID_HANDLE_VALUE && h!=NULL)

//�ر��߳�
#define SAFE_STOP_THREAD(h) if(IsValidHandle(h)){ ::TerminateThread(h,0);CloseHandle(h); h=NULL; }
//�ر��¼�
#define SAFE_CLOSE_EVENT(ev) if(IsValidHandle(ev)){::CloseHandle(ev);ev=NULL;}
#define SAFE_CLOSE_HANDLE(ev) if(IsValidHandle(ev)){::CloseHandle(ev);ev=NULL;}

//���ú�
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

//�����������н�����
#define CREATE_SINGLE_PROCESS_LOCK(globalName) \
	HANDLE hMutex = CreateMutexA(NULL,FALSE,globalName);\
	if(hMutex && GetLastError() == ERROR_ALREADY_EXISTS) { CloseHandle(hMutex);return 0;}
//�رյ������н�����
#define CLOSE_SINGLE_PROCESS_LOCK if(hMutex){ CloseHandle(hMutex); hMutex = NULL; }

//ע�����
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
// �����������(���dll�������κ�������������Ϣ�����Բ��õ���)
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
// ҵ���������ӿ�
//-------------------------------------------------------------------------------
#define  IMP_BUSSINESSPLUGIN_EXPORT(clazz_pointer) extern "C" __declspec(dllexport) PVOID GetPlugProxy (){\
	return clazz_pointer;\
}

//-------------------------------------------------------------------------------
// �������ӿ�
//-------------------------------------------------------------------------------
enum enNetMethod
{
	enTCP,
	enUDP,
};

#endif //__cmndef_H