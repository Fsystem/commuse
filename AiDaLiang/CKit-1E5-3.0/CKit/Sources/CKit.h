#ifndef __CommLib_H
#define __CommLib_H
#define MARKUP_STL

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <windows.h>
#include <tchar.h>

#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <iterator>

//-------------------------------------------------------------------------------
//! ����
#include "ComnDef/cmndef.h"
#include "Interface/CKInterface.h"

//! ��/����
#include "Kernel/crypt/CRC.h"
#include "Kernel/crypt/Md5.h"
#include "Kernel/crypt/yrc4.h"

//! ��������
#include "Kernel/template/SingletonBase.h"

//! ��������
#include "ToolHelper/FunctionHelper.h"
#include "ToolHelper/DeviceOper.h"
#include "ToolHelper/Markup.h"
#include "ToolHelper/NetOper.h"
#include "ToolHelper/Service.h"
#include "ToolHelper/ToolOper.h"
#include "ToolHelper/zip.h"
#include "ToolHelper/unzip.h"
#include "ToolHelper/MemLoadDll.h"
#include "ToolHelper/FileMonitor.h"
#include "ToolHelper/DigitSign.h"
#include "ToolHelper/yCMyseclock.h"

//! �߳�
#include "Kernel/thread/JKThread.h"

//! COMM��
#include "Kernel/comm/Log/CommLog.h"
#include "Kernel/comm/SafeCriticalSection/SafeCriticalSection.h"
#include "Kernel/comm/ShareMem/ShareMemry.h"
#include "Kernel/comm/ProcessLock/ProcessLock.h"
#include "Kernel/comm/Log/DebugLog.h"

//! ���������
#include "Kernel/net/yCTcp.h"
#include "Kernel/net/yCUdp.h"
#include "NetManager/NetManager.h"

//! �����ļ�
#include "Kernel/downfile/HttpDownFile.h"

//! ��װ����
#include "ToolHelper/Server.h"

//! �������
#include "ModuleManager/PlugManager.h"

//! ע��
#include "InjectHelper/MemoryModule.h"
#include "InjectHelper/SsdtOper.h"
#include "InjectHelper/MemoryLoad.h"

//�漴����exe
#include "ToolHelper/RunExeByRandom.h"
//-------------------------------------------------------------------------------

using namespace COMMUSE;
#endif //__CommLib_H