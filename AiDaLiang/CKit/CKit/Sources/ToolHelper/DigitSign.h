#ifndef __DigitSign_H
#define __DigitSign_H

class CDigitSign
{
public:
	static BOOL CheckFileTrust(LPCSTR filename,LPSTR pszCompany);
	static DWORD GetSignInfo(LPCSTR filename, LPSTR serial, LPSTR issuser, LPSTR signer);
protected:
private:
};

#endif //__DigitSign_H