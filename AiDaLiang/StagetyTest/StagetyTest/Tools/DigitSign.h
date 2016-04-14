#ifndef __DigitSign_H
#define __DigitSign_H
#include <Windows.h>

BOOL CheckFileTrust(const char* filename);

DWORD get_sign_info(const char *filename, TCHAR* serial, TCHAR* issuser, TCHAR* signer);

#endif //__DigitSign_H