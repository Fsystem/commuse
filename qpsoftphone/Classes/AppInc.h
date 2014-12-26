#ifndef __include_H
#define __include_H
#include "cocos2d.h"
#include <map>
#include <string>

using namespace cocos2d;
using namespace std;



#if	  CC_TARGET_PLATFORM == CC_PLATFORM_IOS
#include "../../cocos2dx/platform/third_party/ios/curl/curl.h"

#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_LINUX
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include "../../cocos2dx/platform/third_party/android/prebuilt/libcurl/include/curl/curl.h"

#define INVALID_SOCKET			(-1)
#define SOCKET_ERROR			(-1)

typedef int						SOCKET;


#elif CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
#include <io.h>
#include "../../cocos2dx/platform/third_party/win32/pthread/pthread.h"
#include "../../cocos2dx/platform/third_party/win32/curl/curl.h"
#pragma comment(lib,"WS2_32.lib")

#elif CC_TARGET_PLATFORM == CC_PLATFORM_MARMALADE
#pragma message "no support CC_PLATFORM_MARMALADE"

#elif CC_TARGET_PLATFORM == CC_PLATFORM_BADA
#pragma message "no support CC_PLATFORM_BADA"

#elif CC_TARGET_PLATFORM == CC_PLATFORM_BLACKBERRY
#pragma message "no support CC_PLATFORM_BLACKBERRY"

#elif CC_TARGET_PLATFORM == CC_PLATFORM_MAC
#include "../../cocos2dx/platform/third_party/ios/curl/curl.h"

#endif

#endif //__include_H