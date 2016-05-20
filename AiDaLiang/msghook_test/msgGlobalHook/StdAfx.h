// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__B55C98D9_B3A5_4B12_8DD6_64555E5240AF__INCLUDED_)
#define AFX_STDAFX_H__B55C98D9_B3A5_4B12_8DD6_64555E5240AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define _WIN32_WINNT	0x0501
#define WINVER			0x0501
// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

/*!
* 共享内存信息节点
* 1字节（bool是否可以读）|ShareMemNode-1|...|ShareMemNode-n| (最大10000个)
*/
#pragma pack(push,1)
typedef struct ShareMemNode
{
	DWORD		dwPid;							//进程ID
	bool		bTrust;							//是否信任
}ShareMemNode;
#pragma pack(pop)

#endif // !defined(AFX_STDAFX_H__B55C98D9_B3A5_4B12_8DD6_64555E5240AF__INCLUDED_)
