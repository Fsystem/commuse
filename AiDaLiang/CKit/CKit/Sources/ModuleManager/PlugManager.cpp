#include "stdafx.h"
#include "PlugManager.h"


#define PLUG_FILE         "plug.ini"

static std::map<std::string,PlugInfo>				mPlugMap;

PlugManager::PlugManager()
{
	mPlugCnt = 0;
	mThreadLoad = NULL;
	mLoadedAll = false;

	mSink = NULL;

	GetModuleFileNameA(NULL, mPlugDir, MAX_PATH);
	char *lpstr = strrchr(mPlugDir,'\\');
	lpstr[1] = 0;
}


PlugManager::~PlugManager()
{
	StopAllPlugin();
}

BOOL PlugManager::LoadCfg()
{
	mPlugMap.clear();
	char szPlugIniPath[MAX_PATH] = {0};
	sprintf(szPlugIniPath,"%s%s",mPlugDir,PLUG_FILE);

	mPlugCnt = GetPrivateProfileIntA("total", "plug_num", 0, szPlugIniPath);

	PlugInfo info;
	char szKey[64];

	for (int i = 0; i < mPlugCnt; i++)
	{
		sprintf(szKey, "plug%d", i + 1);

		info.hModule		= NULL;
		info.pPlugDelegate	= NULL;
		GetPrivateProfileStringA(szKey, "plug_name", "", info.szName, _countof(info.szName), szPlugIniPath);
		GetPrivateProfileStringA(szKey, "plug_desc", "", info.szPlugDescribe, _countof(info.szPlugDescribe), szPlugIniPath);
		GetPrivateProfileStringA(szKey, "plug_dir", "", info.szRelativePath, _countof(info.szRelativePath), szPlugIniPath);

		mPlugMap[info.szName] = info;
	}

	return TRUE;
}

BOOL PlugManager::LoadPlugin(LPCSTR szPlugName)
{
	if(szPlugName == NULL) return FALSE;

	auto itPlug = mPlugMap.find(szPlugName);
	if(itPlug == mPlugMap.end()) return FALSE;

	return LoadPlugin(itPlug->second);
}

BOOL PlugManager::LoadPlugin(PlugInfo& plugInfo)
{
	//if (plugInfo.hModule) return TRUE;
	
	if (plugInfo.hModule == NULL)
	{
		std::string sPlugFullPath = mPlugDir;
		sPlugFullPath += "plug\\";
		sPlugFullPath += plugInfo.szRelativePath;
		if(plugInfo.szRelativePath[0] != 0) sPlugFullPath += "\\";
		sPlugFullPath += plugInfo.szName;

		plugInfo.hModule = ::LoadLibraryA(sPlugFullPath.c_str());
		if (plugInfo.hModule == NULL ) return FALSE;
	}
	

	PFN_CreatePlugInterface fnCreateInterface = (PFN_CreatePlugInterface)GetProcAddress(plugInfo.hModule,PLUGIN_EXPORT_FUNCNAME);
	if(fnCreateInterface) 
	{
		plugInfo.pPlugDelegate = (IPluginInterface*)fnCreateInterface();

		if(plugInfo.pPlugDelegate == NULL)
		{
			goto FREELAB;
		}

		if(plugInfo.pPlugDelegate->OnInitPlugin(this) == FALSE )
		{
			plugInfo.pPlugDelegate->OnClosePlugin();
			goto FREELAB;
		}
	}

	return TRUE;

FREELAB:
	plugInfo.pPlugDelegate = NULL;

	//if (plugInfo.hModule)
	//{
	//	::FreeLibrary(plugInfo.hModule);
	//	plugInfo.hModule = NULL;
	//}

	return FALSE;
}

void PlugManager::FreePlugin(LPCSTR szPlugName)
{
	if(szPlugName == NULL) return ;
	auto itPlug = mPlugMap.find(szPlugName);
	if(itPlug == mPlugMap.end()) return ;

	FreePlugin(itPlug->second);
}

void PlugManager::FreePlugin(PlugInfo& plugInfo)
{
	if (plugInfo.pPlugDelegate)
	{
		plugInfo.pPlugDelegate->OnClosePlugin();
	}

	plugInfo.pPlugDelegate = NULL;

	//if (plugInfo.hModule)
	//{
	//	::FreeLibraryAndExitThread(plugInfo.hModule,0);
	//	//::FreeLibrary(plugInfo.hModule);
	//	plugInfo.hModule = NULL;
	//}
}

void PlugManager::LoadAllPlugin(IPluginParant* pSink)
{
	StopAllPlugin();
	mSink = pSink;
	mThreadLoad = (HANDLE)JKThread<PlugManager>::Start(&PlugManager::LoadPluginThread,this);
}

void PlugManager::StopAllPlugin()
{
	//WaitForSingleObject((HANDLE)JKThread<PlugManager>::Start(&PlugManager::FreePluginThread,this),INFINITE) ;
	FreePluginThread();
}

bool PlugManager::IsPlugRunning()
{
	return mLoadedAll;
}

void PlugManager::LoadPluginThread()
{
	LoadCfg();

	for (auto it = mPlugMap.begin() ; it != mPlugMap.end() ;it++)
	{
		LoadPlugin(it->second);
	}

	mThreadLoad = NULL;
	mLoadedAll = true;
}

void PlugManager::FreePluginThread()
{
	if (mThreadLoad)
	{
		SAFE_STOP_THREAD(mThreadLoad);
	}

	for (auto it = mPlugMap.begin() ; it != mPlugMap.end() ;it++)
	{
		FreePlugin(it->second);
	}

	mPlugMap.clear();
	mLoadedAll = false;
	mSink = NULL;
}

//宿主将消息投递给插件
void PlugManager::OnHandleMessage(DWORD dwCmd,PVOID pData,int nLen)
{
	for (auto it = mPlugMap.begin() ; it != mPlugMap.end() ;it++)
	{
		if(it->second.pPlugDelegate)
		{
			it->second.pPlugDelegate->OnHandleData(dwCmd,pData,nLen);
		}
	}
}

//插件将消息投递给宿主
void PlugManager::SendMessageToParant(DWORD dwCmd,PVOID pData,int nLen)
{
	//LOGEVENA("插件发消息了[%u]\n",dwCmd);
	if (mSink)
	{
		mSink->SendMessageToParant(dwCmd,pData,nLen);
	}
}

//
//void Plug_Manager::load_plug()
//{
//	m_plug_map.clear();
//	for (int i = 0; i < m_plug_num_; i++)
//	{
//		char current_path[PLUG_BUFFER] = {0};
//		GetModuleFileNameA(NULL, current_path, PLUG_BUFFER);
//		char *lpstr = strrchr(current_path,'\\');
//		if (lpstr != NULL)
//		{
//			*lpstr = '\0';
//		}
//		strcat(current_path, "\\plug\\");
//		strcat(current_path, m_plug_datas[i].dir.c_str());
//		strcat(current_path, "\\");
//		strcat(current_path, m_plug_datas[i].name.c_str());
//		HINSTANCE hInstance = LoadLibraryA(current_path); 
//		//HINSTANCE hInstance = ::LoadLibraryA("D:\\workspace_2015_11_30\\safeMaster\\03-trunk\\1.0\\client\\Client_Master\\bin\\plug\\module_01\\adl_module01.dll"); 
//		//m_plug_moudles.push_back(hInstance);
//		if (hInstance != 0)
//			m_plug_map.insert(std::make_pair<HINSTANCE,Plug_Info>(hInstance, m_plug_datas[i]));
//		else
//		{
//			//out log of dll failed;
//			DWORD err =  GetLastError();
//			LOGEVENA("%s %s= %d", m_plug_datas[i].name.c_str(), "插件加载失败", err);
//		}
//	}
//}
//
//void Plug_Manager::load_one_plug(const Plug_Info& info)
//{
//	plug_it_map it = m_plug_map.begin();
//	while (it != m_plug_map.end())
//	{
//		if (it->second.name.compare(info.name) == 0)
//			return;
//		it++;
//	}
//	std::string dll_file;
//	dll_file += m_plug_dir;
//	dll_file += info.dir;
//	dll_file += "\\";
//	dll_file += info.name;
//	HINSTANCE hInstance = LoadLibraryA(dll_file.c_str());
//	if (hInstance != 0)
//	{
//		m_plug_map.insert(std::make_pair(hInstance, info));
//	}
//	else
//	{
//		LOGEVENA("%s %s", info.name.c_str(), "重新加载插件失败");
//	}
//}
//
//void Plug_Manager::reload_plug()
//{
//	free_plug();
//	load_plug();
//}
//
//void Plug_Manager::free_plug()
//{
//	plug_it_map it = m_plug_map.begin();
//	while (it != m_plug_map.end())
//	{
//		if (it->second.name.compare("adl_module03.dll") == 0)
//		{
//			it++;
//			continue;
//		}
//		bool ret = ::FreeLibrary(it->first);
//		if (!ret)
//		{
//			//out log of failed Free Library,extend failed Free Libriay map;
//		}
//		it++;
//	}
//	m_plug_map.clear();
//}
//
//void Plug_Manager::free_one_plug(char* str)
//{
//	/*
//	for (int i = 0; i < m_plug_datas.size(); i++)
//	{
//		if (m_plug_datas[i].name.compare(str) == 0)
//		{
//			if (m_plug_moudles[i] == NULL) return;
//			FreeLibrary(m_plug_moudles[i]);
//			m_plug_moudles[i] = NULL;
//		}
//	}*/
//	plug_it_map it = m_plug_map.begin();
//	while (it != m_plug_map.end())
//	{
//		if (it->second.name.compare(str) != 0)
//		{
//			it++;
//			continue;
//		}
//		bool ret = FreeLibrary(it->first);
//		if (!ret)
//		{
//			//out log of failed Free Library
//			break;
//		}
//		else
//		{
//			m_plug_map.erase(it);
//			break;
//		}
//		it++;
//	}
//}
//
//
//
//Plug_Info Plug_Manager::find_one_plug(char* str)
//{
//	for (int i = 0; i < m_plug_datas.size(); i++)
//	{
//		if (m_plug_datas[i].name.compare(str) == 0)
//			return m_plug_datas[i];
//	}
//	return Plug_Info();
//}
//
//void  Plug_Manager::add_one_plug(char* str)
//{
//
//}
//
//void Plug_Manager::run_plug()
//{
//	read_plug_config();
//	load_plug();
//}
//
