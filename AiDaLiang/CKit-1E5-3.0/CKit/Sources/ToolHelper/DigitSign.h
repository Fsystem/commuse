#ifndef __DigitSign_H
#define __DigitSign_H

class CDigitSign
{
public:
	static BOOL CheckFileTrust(LPCSTR filename,LPSTR pszCompany);
	static DWORD GetSignInfo(LPCSTR filename, LPTSTR serial, LPTSTR issuser, LPTSTR signer);
protected:
private:
};

#endif //__DigitSign_H