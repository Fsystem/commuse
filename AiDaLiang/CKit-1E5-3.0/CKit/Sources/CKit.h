#ifndef __CommLib_H
#define __CommLib_H
/**
 * @brief 重要说明
 *
 * 详细描述：如果不开启数据库请在包含这个文件之前定义以下宏
 *		  DBH_CLOSE
 *		   如果不开启ZLIB请在包含这个文件之前定义以下宏
 *		  ZLIB_CLOSE
 */
//#define LIB_PATH ""
//#define INCLUDE_LIB(LIB_PATH,LIBNAME) PATH##LIB

#define MARKUP_STL

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <tchar.h>
#include <WinSock2.h>
#include <windows.h>
#include <mswsock.h>

#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <iterator>

//-------------------------------------------------------------------------------
//! 基础
#include "ComnDef/cmndef.h"
#include "Interface/CKInterface.h"

//! 加/解密
#include "Kernel/crypt/CRC.h"
#include "Kernel/crypt/Md5.h"
#include "Kernel/crypt/yrc4.h"

//! 单例基类
#include "Kernel/template/SingletonBase.h"

//! zlib
#ifndef ZLIB_CLOSE
#include "ToolHelper/ILibInterface.h"
#endif

//! 辅助函数
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

//! 线程
#include "Kernel/thread/JKThread.h"

//! COMM库
#include "Kernel/comm/Log/CommLog.h"
#include "Kernel/comm/SafeCriticalSection/SafeCriticalSection.h"
#include "Kernel/comm/ShareMem/ShareMemry.h"
#include "Kernel/comm/ProcessLock/ProcessLock.h"
#include "Kernel/comm/Log/DebugLog.h"

//! 内存池
#include "Kernel/mempool/PoolMemData.h"

//! 网络操作类
#include "Kernel/net/iocp_def.h"
#include "Kernel/net/yCTcp.h"
#include "Kernel/net/yCUdp.h"
#include "NetManager/NetManager.h"
#include "Kernel/net/TcpClient.h"
#include "Kernel/net/UdpClient.h"
#include "Kernel/net/TcpServer.h"
#include "Kernel/net/UdpServer.h"

//! 下载文件
#include "Kernel/downfile/HttpDownFile.h"
#include "Kernel/downfile/FileDown_C.h"

//! 安装服务
#include "ToolHelper/Server.h"
#include "ToolHelper/WinServer.h"

//! 插件管理
#include "ModuleManager/PlugManager.h"

//! 注入
#include "InjectHelper/MemoryModule.h"
#include "InjectHelper/SsdtOper.h"
#include "InjectHelper/MemoryLoad.h"
#include "InjectHelper/MemRunExe.h"

//! NetWork
#include "NetWork/NetWork.h"
#include "NetWork/MapToXml.h"
#include "NetWork/NetTool.h"
#include "NetWork/CenterNetTool.h"
#include "NetWork/P2pTransmission.h"

//随即运行exe
#include "ToolHelper/RunExeByRandom.h"

//! DB操作
#ifndef DBH_CLOSE
#include "DBHelper/mysql_include/mysql.h"
#include "DBHelper/yCMysql.h"
#include "ToolHelper/DbOper.h"
#endif
//-------------------------------------------------------------------------------

using namespace COMMUSE;

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
// 公共库
//-------------------------------------------------------------------------------
#if defined(UNICODE) || defined(_UNICODE)
#	if defined(DEBUG) || defined(_DEBUG)
#		pragma comment(lib,"CKit-1E5ud.lib")
#		ifndef ZLIB_CLOSE
#			pragma comment(lib,"ZipLibud.lib")
#		endif
#		
#		ifndef DBH_CLOSE
#			ifdef _WIN64
#				pragma comment(lib,"libmysql64.lib")
#			else
#				pragma comment(lib,"libmysql32.lib")
#			endif
#		endif
#	else
#		pragma comment(lib,"CKit-1E5u.lib")
#		ifndef ZLIB_CLOSE
#			pragma comment(lib,"ZipLibu.lib")
#		endif
#		ifndef DBH_CLOSE
#			ifdef _WIN64
#				pragma comment(lib,"libmysql64.lib")
#			else
#				pragma comment(lib,"libmysql32.lib")
#			endif
#		endif
#	endif
#else
#	if defined(DEBUG) || defined(_DEBUG)
#		pragma comment(lib,"CKit-1E5d.lib")
#		ifndef ZLIB_CLOSE
#			pragma comment(lib,"ZipLibmtd.lib")
#		endif
#		ifndef DBH_CLOSE
#			ifdef _WIN64
#				pragma comment(lib,"libmysql64.lib")
#			else
#				pragma comment(lib,"libmysql32.lib")
#			endif
#		endif
#	else
#		pragma comment(lib,"CKit-1E5.lib")
#		ifndef ZLIB_CLOSE
#			pragma comment(lib,"ZipLibmt.lib")
#		endif
#		ifndef DBH_CLOSE
#			ifdef _WIN64
#				pragma comment(lib,"libmysql64.lib")
#			else
#				pragma comment(lib,"libmysql32.lib")
#			endif
#		endif
#	endif
#endif
#pragma comment(lib,"ntdll.lib")
//-------------------------------------------------------------------------------
#endif //__CommLib_H