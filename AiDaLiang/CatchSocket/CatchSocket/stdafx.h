// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

//sock
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <Windows.h>
#include <Iphlpapi.h>

//std
#include <fstream>
#include <sstream>
#include <strstream>
#include <iostream>
#include <string.h>
#include <list>
#include <vector>

//
#include "JKThread.h"
#include "SafeCriticalSection/SafeCriticalSection.h"
using namespace COMMUSE;
//
#include "RawSocket.h"
#include "HandleRawData.h"
// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�



//-------------------------------------------------------------------------------
extern DWORD GetPidBySocketLink(DWORD dwSrcIp,DWORD dwSrcPort,DWORD dwDesIp,DWORD dwDesPort);
extern USHORT ChechSum(USHORT *buffer, int size);
