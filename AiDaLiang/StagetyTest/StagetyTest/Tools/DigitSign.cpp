#include "stdafx.h"

#include "DigitSign.h"
#include <iostream>
#include <atlconv.h>

#include <wincrypt.h>
#include <wintrust.h>
#include <stdio.h>
#include <SoftPub.h>
#include <Mscat.h>

#include <atlconv.h>

#include <xlocale>
using std::locale;

#include <iostream>
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "WinTrust.lib")

//-------------------------------------------------------------------------------

#define ENCODING (X509_ASN_ENCODING | PKCS_7_ASN_ENCODING)

typedef struct {
	LPWSTR lpszProgramName;
	LPWSTR lpszPublisherLink;
	LPWSTR lpszMoreInfoLink;
} SPROG_PUBLISHERINFO, *PSPROG_PUBLISHERINFO;

LPWSTR AllocateAndCopyWideString(LPCWSTR inputString);
BOOL GetProgAndPublisherInfo(PCMSG_SIGNER_INFO pSignerInfo,	PSPROG_PUBLISHERINFO Info);
BOOL GetCertificateInfo(PCCERT_CONTEXT pCertContext, std::string& serial, std::wstring& issuser, std::wstring& signer);
BOOL GetDateOfTimeStamp(PCMSG_SIGNER_INFO pSignerInfo, SYSTEMTIME *st);
BOOL GetTimeStampSignerInfo(PCMSG_SIGNER_INFO pSignerInfo, PCMSG_SIGNER_INFO *pCounterSignerInfo);


//C++ 验证微软数字签名   
BOOL CheckFileTrust_Private(const char* filename)
{
	USES_CONVERSION;
	LPCWSTR lpFileName = A2W(filename);

	BOOL bRet = FALSE;
	WINTRUST_DATA wd = { 0 };
	WINTRUST_FILE_INFO wfi = { 0 };
	WINTRUST_CATALOG_INFO wci = { 0 };
	CATALOG_INFO ci = { 0 };
	HCATADMIN hCatAdmin = NULL;
	if ( !CryptCATAdminAcquireContext( &hCatAdmin, NULL, 0 ) )
	{
		return FALSE;
	}
	HANDLE hFile = CreateFileW( lpFileName, GENERIC_READ, FILE_SHARE_READ,
		NULL, OPEN_EXISTING, 0, NULL );
	if ( INVALID_HANDLE_VALUE == hFile )
	{
		CryptCATAdminReleaseContext( hCatAdmin, 0 );
		return FALSE;
	}
	DWORD dwCnt = 100;
	BYTE byHash[100];
	CryptCATAdminCalcHashFromFileHandle( hFile, &dwCnt, byHash, 0 );
	CloseHandle( hFile );
	LPWSTR pszMemberTag = new WCHAR[dwCnt * 2 + 1];
	for ( DWORD dw = 0; dw < dwCnt; ++dw )
	{
		wsprintfW( &pszMemberTag[dw * 2], L"%02X", byHash[dw] );
	}
	HCATINFO hCatInfo = CryptCATAdminEnumCatalogFromHash( hCatAdmin,
		byHash, dwCnt, 0, NULL );
	if ( NULL == hCatInfo )
	{
		wfi.cbStruct       = sizeof( WINTRUST_FILE_INFO );
		wfi.pcwszFilePath  = lpFileName;
		wfi.hFile          = NULL;
		wfi.pgKnownSubject = NULL;

		wd.cbStruct            = sizeof( WINTRUST_DATA );
		wd.dwUnionChoice       = WTD_CHOICE_FILE;
		wd.pFile               = &wfi;
		wd.dwUIChoice          = WTD_UI_NONE;
		wd.fdwRevocationChecks = WTD_REVOKE_NONE;
		wd.dwStateAction       = WTD_STATEACTION_IGNORE;
		wd.dwProvFlags         = WTD_SAFER_FLAG;
		wd.hWVTStateData       = NULL;
		wd.pwszURLReference    = NULL;
	}
	else
	{
		CryptCATCatalogInfoFromContext( hCatInfo, &ci, 0 );
		wci.cbStruct             = sizeof( WINTRUST_CATALOG_INFO );
		wci.pcwszCatalogFilePath = ci.wszCatalogFile;
		wci.pcwszMemberFilePath  = lpFileName;
		wci.pcwszMemberTag       = pszMemberTag;

		wd.cbStruct            = sizeof( WINTRUST_DATA );
		wd.dwUnionChoice       = WTD_CHOICE_CATALOG;
		wd.pCatalog            = &wci;
		wd.dwUIChoice          = WTD_UI_NONE;
		wd.fdwRevocationChecks = WTD_STATEACTION_VERIFY;
		wd.dwProvFlags         = 0;
		wd.hWVTStateData       = NULL;
		wd.pwszURLReference    = NULL;
	}
	GUID action = WINTRUST_ACTION_GENERIC_VERIFY_V2;
	HRESULT hr  = WinVerifyTrust( NULL, &action, &wd );
	bRet        = SUCCEEDED( hr );

	if ( NULL != hCatInfo )
	{
		CryptCATAdminReleaseCatalogContext( hCatAdmin, hCatInfo, 0 );
	}
	CryptCATAdminReleaseContext( hCatAdmin, 0 ); 
	delete[] pszMemberTag;
	return bRet;
}

BOOL CheckFileTrust(const char* filename)
{
	BOOL bRet = FALSE;
	locale loc = locale::global(locale(""));
	bRet = CheckFileTrust_Private(filename);
	locale::global(loc);
	return bRet;
}

BOOL GetCertificateInfo(PCCERT_CONTEXT pCertContext, TCHAR* serial, TCHAR* issuser, TCHAR* signer)
{
	BOOL fReturn = FALSE;
	LPTSTR szName = NULL;
	DWORD dwData;

	if(!serial || !issuser || !signer) return FALSE;

	__try
	{
		// Print Serial Number.
		//_tprintf(_T("Serial Number: "));
		dwData = pCertContext->pCertInfo->SerialNumber.cbData;
		TCHAR serial_number[128] = {0};
		for (DWORD n = 0; n < dwData; n++)
		{
			//char tmp[5] = {0};
			_sntprintf(serial_number,128,TEXT("%s%02x"),serial_number, pCertContext->pCertInfo->SerialNumber.pbData[dwData - (n + 1)]);
			//_tprintf(_T("%02x "),
			//	pCertContext->pCertInfo->SerialNumber.pbData[dwData - (n + 1)]);
			//strcat(serial_number, tmp);
			if (n != dwData - 1) _tcscat(serial_number, TEXT(" "));
		}
		_tcscpy(serial , serial_number);
		//_tprintf(_T("buff is : %x"), serial_number);
		//_tprintf(_T("\n"));
		// Get Issuer name size.
		if (!(dwData = CertGetNameString(pCertContext, 
			CERT_NAME_SIMPLE_DISPLAY_TYPE,
			CERT_NAME_ISSUER_FLAG,
			NULL,
			NULL,
			0)))
		{
			//_tprintf(_T("CertGetNameString failed.\n"));
			__leave;
		}

		// Allocate memory for Issuer name.
		szName = (LPTSTR)LocalAlloc(LPTR, dwData * sizeof(TCHAR));
		if (!szName)
		{
			//_tprintf(_T("Unable to allocate memory for issuer name.\n"));
			__leave;
		}

		// Get Issuer name.
		if (!(CertGetNameString(pCertContext, 
			CERT_NAME_SIMPLE_DISPLAY_TYPE,
			CERT_NAME_ISSUER_FLAG,
			NULL,
			szName,
			dwData)))
		{
			//_tprintf(_T("CertGetNameString failed.\n"));
			__leave;
		}

		// print Issuer name.
		//_tprintf(_T("Issuer Name: %s\n"), szName);
		_tcscpy(issuser , szName);
		// 		wchar_t issuser_name[256] = {0};
		// 		wcsncpy(issuser_name, szName , dwData);
		// 		int len_issuser_name = lstrlen(issuser_name);
		// 		char *issuser_name_a = new char[len_issuser_name * 2 +1];
		// 		memset(issuser_name_a , 0 , len_issuser_name * 2 +1);
		// 		wcstombs(issuser_name_a, issuser_name, len_issuser_name);
		// 		delete []issuser_name_a;
		LocalFree(szName);
		szName = NULL;

		// Get Subject name size.
		if (!(dwData = CertGetNameString(pCertContext, 
			CERT_NAME_SIMPLE_DISPLAY_TYPE,
			0,
			NULL,
			NULL,
			0)))
		{
			//_tprintf(_T("CertGetNameString failed.\n"));
			__leave;
		}

		// Allocate memory for subject name.
		szName = (LPTSTR)LocalAlloc(LPTR, dwData * sizeof(TCHAR));
		if (!szName)
		{
			//_tprintf(_T("Unable to allocate memory for subject name.\n"));
			__leave;
		}

		// Get subject name.
		if (!(CertGetNameString(pCertContext, 
			CERT_NAME_SIMPLE_DISPLAY_TYPE,
			0,
			NULL,
			szName,
			dwData)))
		{
			//_tprintf(_T("CertGetNameString failed.\n"));
			__leave;
		}

		// Print Subject Name.
		//_tprintf(_T("Subject Name: %s\n"), szName);
		_tcscpy(signer , szName);
		// 		wchar_t sign_name[128] = {0};
		// 		wcsncpy(sign_name, szName, dwData);
		// 		int len = lstrlenW(sign_name);
		// 		char *sign_name_a = new char[len * 2 +1];
		// 		memset(sign_name_a , 0 , len * 2 +1);
		// 		wcstombs(sign_name_a, sign_name, len);
		// 		delete []sign_name_a;
		fReturn = TRUE;
	}
	__finally
	{
		if (szName != NULL) LocalFree(szName);
	}
	return fReturn;
}

LPWSTR AllocateAndCopyWideString(LPCWSTR inputString)
{
	LPWSTR outputString = NULL;

	outputString = (LPWSTR)LocalAlloc(LPTR,
		(wcslen(inputString) + 1) * sizeof(WCHAR));
	if (outputString != NULL)
	{
		lstrcpyW(outputString, inputString);
	}
	return outputString;
}

BOOL GetProgAndPublisherInfo(PCMSG_SIGNER_INFO pSignerInfo, PSPROG_PUBLISHERINFO Info)
{
	BOOL fReturn = FALSE;
	PSPC_SP_OPUS_INFO OpusInfo = NULL;  
	DWORD dwData;
	BOOL fResult;

	__try
	{
		// Loop through authenticated attributes and find
		// SPC_SP_OPUS_INFO_OBJID OID.
		for (DWORD n = 0; n < pSignerInfo->AuthAttrs.cAttr; n++)
		{           
			if (lstrcmpA(SPC_SP_OPUS_INFO_OBJID, 
				pSignerInfo->AuthAttrs.rgAttr[n].pszObjId) == 0)
			{
				// Get Size of SPC_SP_OPUS_INFO structure.
				fResult = CryptDecodeObject(ENCODING,
					SPC_SP_OPUS_INFO_OBJID,
					pSignerInfo->AuthAttrs.rgAttr[n].rgValue[0].pbData,
					pSignerInfo->AuthAttrs.rgAttr[n].rgValue[0].cbData,
					0,
					NULL,
					&dwData);
				if (!fResult)
				{
					//_tprintf(_T("CryptDecodeObject failed with %x\n"),
					//	GetLastError());
					__leave;
				}

				// Allocate memory for SPC_SP_OPUS_INFO structure.
				OpusInfo = (PSPC_SP_OPUS_INFO)LocalAlloc(LPTR, dwData);
				if (!OpusInfo)
				{
					//_tprintf(_T("Unable to allocate memory for Publisher Info.\n"));
					__leave;
				}

				// Decode and get SPC_SP_OPUS_INFO structure.
				fResult = CryptDecodeObject(ENCODING,
					SPC_SP_OPUS_INFO_OBJID,
					pSignerInfo->AuthAttrs.rgAttr[n].rgValue[0].pbData,
					pSignerInfo->AuthAttrs.rgAttr[n].rgValue[0].cbData,
					0,
					OpusInfo,
					&dwData);
				if (!fResult)
				{
					//_tprintf(_T("CryptDecodeObject failed with %x\n"),
					//	GetLastError());
					__leave;
				}

				// Fill in Program Name if present.
				if (OpusInfo->pwszProgramName)
				{
					Info->lpszProgramName =
						AllocateAndCopyWideString(OpusInfo->pwszProgramName);
				}
				else
					Info->lpszProgramName = NULL;

				// Fill in Publisher Information if present.
				if (OpusInfo->pPublisherInfo)
				{

					switch (OpusInfo->pPublisherInfo->dwLinkChoice)
					{
					case SPC_URL_LINK_CHOICE:
						Info->lpszPublisherLink =
							AllocateAndCopyWideString(OpusInfo->pPublisherInfo->pwszUrl);
						break;

					case SPC_FILE_LINK_CHOICE:
						Info->lpszPublisherLink =
							AllocateAndCopyWideString(OpusInfo->pPublisherInfo->pwszFile);
						break;

					default:
						Info->lpszPublisherLink = NULL;
						break;
					}
				}
				else
				{
					Info->lpszPublisherLink = NULL;
				}

				// Fill in More Info if present.
				if (OpusInfo->pMoreInfo)
				{
					switch (OpusInfo->pMoreInfo->dwLinkChoice)
					{
					case SPC_URL_LINK_CHOICE:
						Info->lpszMoreInfoLink =
							AllocateAndCopyWideString(OpusInfo->pMoreInfo->pwszUrl);
						break;

					case SPC_FILE_LINK_CHOICE:
						Info->lpszMoreInfoLink =
							AllocateAndCopyWideString(OpusInfo->pMoreInfo->pwszFile);
						break;

					default:
						Info->lpszMoreInfoLink = NULL;
						break;
					}
				}               
				else
				{
					Info->lpszMoreInfoLink = NULL;
				}

				fReturn = TRUE;

				break; // Break from for loop.
			} // lstrcmp SPC_SP_OPUS_INFO_OBJID                 
		} // for 
	}
	__finally
	{
		if (OpusInfo != NULL) LocalFree(OpusInfo);      
	}

	return fReturn;
}

BOOL GetDateOfTimeStamp(PCMSG_SIGNER_INFO pSignerInfo, SYSTEMTIME *st)
{   
	BOOL fResult;
	FILETIME lft, ft;   
	DWORD dwData;
	BOOL fReturn = FALSE;

	// Loop through authenticated attributes and find
	// szOID_RSA_signingTime OID.
	for (DWORD n = 0; n < pSignerInfo->AuthAttrs.cAttr; n++)
	{           
		if (lstrcmpA(szOID_RSA_signingTime, 
			pSignerInfo->AuthAttrs.rgAttr[n].pszObjId) == 0)
		{               
			// Decode and get FILETIME structure.
			dwData = sizeof(ft);
			fResult = CryptDecodeObject(ENCODING,
				szOID_RSA_signingTime,
				pSignerInfo->AuthAttrs.rgAttr[n].rgValue[0].pbData,
				pSignerInfo->AuthAttrs.rgAttr[n].rgValue[0].cbData,
				0,
				(PVOID)&ft,
				&dwData);
			if (!fResult)
			{
				//_tprintf(_T("CryptDecodeObject failed with %x\n"),
				//	GetLastError());
				break;
			}

			// Convert to local time.
			FileTimeToLocalFileTime(&ft, &lft);
			FileTimeToSystemTime(&lft, st);

			fReturn = TRUE;

			break; // Break from for loop.

		} //lstrcmp szOID_RSA_signingTime
	} // for 

	return fReturn;
}

BOOL GetTimeStampSignerInfo(PCMSG_SIGNER_INFO pSignerInfo, PCMSG_SIGNER_INFO *pCounterSignerInfo)
{   
	PCCERT_CONTEXT pCertContext = NULL;
	BOOL fReturn = FALSE;
	BOOL fResult;       
	DWORD dwSize;   

	__try
	{
		*pCounterSignerInfo = NULL;

		// Loop through unathenticated attributes for
		// szOID_RSA_counterSign OID.
		for (DWORD n = 0; n < pSignerInfo->UnauthAttrs.cAttr; n++)
		{
			if (lstrcmpA(pSignerInfo->UnauthAttrs.rgAttr[n].pszObjId, 
				szOID_RSA_counterSign) == 0)
			{
				// Get size of CMSG_SIGNER_INFO structure.
				fResult = CryptDecodeObject(ENCODING,
					PKCS7_SIGNER_INFO,
					pSignerInfo->UnauthAttrs.rgAttr[n].rgValue[0].pbData,
					pSignerInfo->UnauthAttrs.rgAttr[n].rgValue[0].cbData,
					0,
					NULL,
					&dwSize);
				if (!fResult)
				{
					//_tprintf(_T("CryptDecodeObject failed with %x\n"),
					//	GetLastError());
					__leave;
				}

				// Allocate memory for CMSG_SIGNER_INFO.
				*pCounterSignerInfo = (PCMSG_SIGNER_INFO)LocalAlloc(LPTR, dwSize);
				if (!*pCounterSignerInfo)
				{
					//_tprintf(_T("Unable to allocate memory for timestamp info.\n"));
					__leave;
				}

				// Decode and get CMSG_SIGNER_INFO structure
				// for timestamp certificate.
				fResult = CryptDecodeObject(ENCODING,
					PKCS7_SIGNER_INFO,
					pSignerInfo->UnauthAttrs.rgAttr[n].rgValue[0].pbData,
					pSignerInfo->UnauthAttrs.rgAttr[n].rgValue[0].cbData,
					0,
					(PVOID)*pCounterSignerInfo,
					&dwSize);
				if (!fResult)
				{
					//_tprintf(_T("CryptDecodeObject failed with %x\n"),
					//	GetLastError());
					__leave;
				}

				fReturn = TRUE;

				break; // Break from for loop.
			}           
		}
	}
	__finally
	{
		// Clean up.
		if (pCertContext != NULL) CertFreeCertificateContext(pCertContext);
	}

	return fReturn;
}

DWORD get_sign_info_private(const char *filename , TCHAR* serial, TCHAR* issuser, TCHAR* signer)
{
	if(!serial || !issuser || !signer ) return 0;

	WCHAR szFileName[MAX_PATH]; 
	HCERTSTORE hStore = NULL;
	HCRYPTMSG hMsg = NULL; 
	PCCERT_CONTEXT pCertContext = NULL;
	BOOL fResult;   
	DWORD dwEncoding, dwContentType, dwFormatType;
	PCMSG_SIGNER_INFO pSignerInfo = NULL;
	PCMSG_SIGNER_INFO pCounterSignerInfo = NULL;
	DWORD dwSignerInfo;
	CERT_INFO CertInfo;     
	SPROG_PUBLISHERINFO ProgPubInfo;
	SYSTEMTIME st;

	ZeroMemory(&ProgPubInfo, sizeof(ProgPubInfo));
	__try
	{
		
		// if (argc != 2)
		// {
		// ////_tprintf(_T("Usage: SignedFileInfo <filename>\n"));
		// return 0;
		// }

		// #ifdef UNICODE
		// /*lstrcpynW(szFileName, argv[1], MAX_PATH);*/
		// lstrcpynW(szFileName, filename, MAX_PATH);
		// #else
		if (mbstowcs(szFileName, filename, MAX_PATH) == -1)
		{
			//printf("Unable to convert to unicode.\n");
			__leave;
		}
		/*#endif*/

		// Get message handle and store handle from the signed file.
		fResult = CryptQueryObject(CERT_QUERY_OBJECT_FILE,
			szFileName,
			CERT_QUERY_CONTENT_FLAG_PKCS7_SIGNED_EMBED,
			CERT_QUERY_FORMAT_FLAG_BINARY,
			0,
			&dwEncoding,
			&dwContentType,
			&dwFormatType,
			&hStore,
			&hMsg,
			NULL);
		if (!fResult)
		{
			////_tprintf(_T("CryptQueryObject failed with %x\n"), GetLastError());
			DWORD err = GetLastError();
			__leave;
		}

		// Get signer information size.
		fResult = CryptMsgGetParam(hMsg, 
			CMSG_SIGNER_INFO_PARAM, 
			0, 
			NULL, 
			&dwSignerInfo);
		if (!fResult)
		{
			////_tprintf(_T("CryptMsgGetParam failed with %x\n"), GetLastError());
			__leave;
		}

		// Allocate memory for signer information.
		pSignerInfo = (PCMSG_SIGNER_INFO)LocalAlloc(LPTR, dwSignerInfo);
		if (!pSignerInfo)
		{
			////_tprintf(_T("Unable to allocate memory for Signer Info.\n"));
			__leave;
		}

		// Get Signer Information.
		fResult = CryptMsgGetParam(hMsg, 
			CMSG_SIGNER_INFO_PARAM, 
			0, 
			(PVOID)pSignerInfo, 
			&dwSignerInfo);
		if (!fResult)
		{
			////_tprintf(_T("CryptMsgGetParam failed with %x\n"), GetLastError());
			__leave;
		}

		// Get program name and publisher information from 
		// signer info structure.
		if (GetProgAndPublisherInfo(pSignerInfo, &ProgPubInfo))
		{
			if (ProgPubInfo.lpszProgramName != NULL)
			{
				wprintf(L"Program Name : %s\n",
					ProgPubInfo.lpszProgramName);
			}

			if (ProgPubInfo.lpszPublisherLink != NULL)
			{
				wprintf(L"Publisher Link : %s\n",
					ProgPubInfo.lpszPublisherLink);
			}

			if (ProgPubInfo.lpszMoreInfoLink != NULL)
			{
				wprintf(L"MoreInfo Link : %s\n",
					ProgPubInfo.lpszMoreInfoLink);
			}
		}

		//_tprintf(_T("\n"));

		// Search for the signer certificate in the temporary 
		// certificate store.
		CertInfo.Issuer = pSignerInfo->Issuer;
		CertInfo.SerialNumber = pSignerInfo->SerialNumber;

		pCertContext = CertFindCertificateInStore(hStore,
			ENCODING,
			0,
			CERT_FIND_SUBJECT_CERT,
			(PVOID)&CertInfo,
			NULL);
		if (!pCertContext)
		{
			//_tprintf(_T("CertFindCertificateInStore failed with %x\n"),
			//GetLastError());
			__leave;
		}

		// Print Signer certificate information.
		//_tprintf(_T("Signer Certificate:\n\n"));        
		GetCertificateInfo(pCertContext, serial, issuser, signer);
		//_tprintf(_T("\n"));

		// Get the timestamp certificate signerinfo structure.
		if (GetTimeStampSignerInfo(pSignerInfo, &pCounterSignerInfo))
		{
			// Search for Timestamp certificate in the temporary
			// certificate store.
			CertInfo.Issuer = pCounterSignerInfo->Issuer;
			CertInfo.SerialNumber = pCounterSignerInfo->SerialNumber;

			if (pCertContext != NULL)
			{
				CertFreeCertificateContext(pCertContext);
				pCertContext = NULL;
			}
			pCertContext = CertFindCertificateInStore(hStore,
				ENCODING,
				0,
				CERT_FIND_SUBJECT_CERT,
				(PVOID)&CertInfo,
				NULL);
			if (!pCertContext)
			{
				//_tprintf(_T("CertFindCertificateInStore failed with %x\n"),
				//GetLastError());
				__leave;
			}

			// Print timestamp certificate information.
			//_tprintf(_T("TimeStamp Certificate:\n\n"));
			//GetCertificateInfo(pCertContext);
			//_tprintf(_T("\n"));

			// Find Date of timestamp.
			if (GetDateOfTimeStamp(pCounterSignerInfo, &st))
			{
				//_tprintf(_T("Date of TimeStamp : %02d/%02d/%04d %02d:%02d\n"),
				//st.wMonth,
				//st.wDay,
				//st.wYear,
				//st.wHour,
				//st.wMinute);
			}
			//_tprintf(_T("\n"));
		}
	}
	__finally
	{               
		// Clean up.
		if (ProgPubInfo.lpszProgramName != NULL)
			LocalFree(ProgPubInfo.lpszProgramName);
		if (ProgPubInfo.lpszPublisherLink != NULL)
			LocalFree(ProgPubInfo.lpszPublisherLink);
		if (ProgPubInfo.lpszMoreInfoLink != NULL)
			LocalFree(ProgPubInfo.lpszMoreInfoLink);

		if (pSignerInfo != NULL) LocalFree(pSignerInfo);
		if (pCounterSignerInfo != NULL) LocalFree(pCounterSignerInfo);
		if (pCertContext != NULL) CertFreeCertificateContext(pCertContext);
		if (hStore != NULL) CertCloseStore(hStore, 0);
		if (hMsg != NULL) CryptMsgClose(hMsg);
	}

	return 0;
}

DWORD get_sign_info(const char *filename , TCHAR* serial, TCHAR* issuser, TCHAR* signer)
{
	DWORD dwErr = 0;
// 	TCHAR szSerial[128]={0};
// 	TCHAR szIssUser[512]={0};
// 	TCHAR szSigner[512]={0};
	locale loc = locale::global(locale(""));
	dwErr = get_sign_info_private(filename,serial,issuser,signer);
	locale::global(loc);

// 	USES_CONVERSION;
// 	strncpy(serial,T2A(szSerial),128);
// 	strncpy(issuser,T2A(szIssUser),512);
// 	strncpy(signer,T2A(szSigner),512);

	return dwErr;
}

//DWORD get_sign_info(const char *filename , std::string& serial, std::wstring& issuser, std::wstring& signer)
//{
//	DWORD dwRet = 0;
//	std::wstring sIssUser,sSigner;
//	dwRet = get_sign_info_private(filename,serial,sIssUser,sSigner);
//	USES_CONVERSION;
//	issuser = W2A(sIssUser.c_str());
//	signer = W2A(sSigner.c_str());
//
//	return dwRet;
//}