/** 32λ����ע�루32��64λdll����32λ���̺�64λ����
*   @FileName  : jkInjectDll.h
*   @Author    : Double Sword(doublesword.jack@gmail.com)
*   @date      : 2016-5-25
*   @Copyright : Copyright belong to Author and 9u2,CO.LTD
*   @RefDoc    : ����32λ�����п��ã�64λ������ʧЧ
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