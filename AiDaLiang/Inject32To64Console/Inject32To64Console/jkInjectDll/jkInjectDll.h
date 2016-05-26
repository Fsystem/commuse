/** 32位进程注入（32、64位dll）到32位进程和64位进程
*   @FileName  : jkInjectDll.h
*   @Author    : Double Sword(doublesword.jack@gmail.com)
*   @date      : 2016-5-25
*   @Copyright : Copyright belong to Author and 9u2,CO.LTD
*   @RefDoc    : 仅在32位程序中可用，64位程序中失效
*/

#ifndef __jkInjectDll_H
#define __jkInjectDll_H

#ifdef UNICODE
#define jkInjectDll jkInjectDllW
#else
#define jkInjectDll jkInjectDllA
#endif

BOOL jkInjectDllA(const char* szPath,const char* szInjectExe);
BOOL jkInjectDllW(const wchar_t* szPath,const wchar_t* szInjectExe);

#endif //__jkInjectDll_H