// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
#include "../GlobalDef/GlobalDef.h"
#include "../CKit-lib/CKit.h"

#ifdef _DEBUG
#	ifdef UNICODE
#		pragma comment(lib,"../CKit-lib/Ckitud.lib")
#	else
#		pragma comment(lib,"../CKit-lib/Ckitd.lib")	
#	endif
#else
#	ifdef UNICODE
#		pragma comment(lib,"../CKit-lib/Ckitu.lib")
#	else
#		pragma comment(lib,"../CKit-lib/Ckit.lib")	
#	endif
#endif
