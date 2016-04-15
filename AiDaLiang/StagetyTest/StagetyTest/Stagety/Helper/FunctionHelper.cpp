#include <stdafx.h>
#include "FunctionHelper.h"
#include "../StagetyInclude.h"
#include <windows.h>
#include <io.h>
#include <atlconv.h>

#include <Psapi.h>

#pragma comment(lib,"psapi")

//-------------------------------------------------------------------------------
DWORD	WINAPI SetDisableOrEnable(BOOL IsEnable,PVOID *OldValue);


//-------------------------------------------------------------------------------
typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS)(HANDLE,   PBOOL);   
LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(TEXT("kernel32")), "IsWow64Process");   
  
BOOL GetWOW64PlatformSupport()   
{   
    BOOL  bIsWow64   =   FALSE;   
    if   (NULL != fnIsWow64Process)   
    {   
       fnIsWow64Process(GetCurrentProcess(),&bIsWow64) ;
    }   

    return bIsWow64;  
}  

//-------------------------------------------------------------------------------
/*
通过一个地址取模块句柄
*/
HMODULE ModuleHandleByAddr(const void* ptrAddr)
{
	MEMORY_BASIC_INFORMATION info;
	::VirtualQuery(ptrAddr, &info, sizeof(info));
	return (HMODULE)info.AllocationBase;
}
/*
当前模块句柄
*/
HMODULE ThisModuleHandle()
{
	static HMODULE sInstance = ModuleHandleByAddr((void*)&ThisModuleHandle);
	return sInstance;
}

//-------------------------------------------------------------------------------
//提升权限
bool SetPrivilege()
{

	//定义结构体变量
	TOKEN_PRIVILEGES tkp;
	HANDLE hToken;
	if (!OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
	{
		//	MessageBox("OpenProcessToken failed!");
		DWORD err = GetLastError();
		//CDebugLog log;
		//log.SaveLog(1, "Safe_Client.log", "OpenProcessToken err = %d", err); 
		return false;
	}
	if(! LookupPrivilegeValue(NULL, SE_DEBUG_NAME,&tkp.Privileges[0].Luid))//获得本地机唯一的标识
	{ 
		//	AfxMessageBox("从本地计算机提取权限失败！");
		DWORD err = GetLastError();
		//CDebugLog log;
		//log.SaveLog(1, "Safe_Client.log", "LookupPrivilegeValue err = %d", err); 
		return false;
	}
	tkp.PrivilegeCount = 1;  
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,(PTOKEN_PRIVILEGES) NULL, 0); //调整获得的权限
	if (GetLastError() != ERROR_SUCCESS) 
	{
		DWORD err = GetLastError();
		//CDebugLog log;
		//log.SaveLog(1, "Safe_Client.log", "AdjustTokenPrivileges err = %d", err); 
		//	MessageBox("AdjustTokenPrivileges enable failed!");
		return false;
	}
	return true;
}

//获取操作Key
unsigned int GetActionKey(LPCSTR pParantPath,LPCSTR pChildPath,WORD operate)
{
	if(operate == enOperateNull) return 0;

	char szRandKey[1024]={0};
	sprintf(szRandKey,"%s%s%d",pParantPath, pChildPath, operate);

	CCRC crc;
	return crc.GetStrCrc_Key(szRandKey,strlen(szRandKey));
}

//-------------------------------------------------------------------------------
//微软数字签名
// typedef struct WINTRUST_FILE_INFO_  
// {  
//     DWORD   cbStruct;  
//     LPCWSTR pcwszFilePath;  
//     HANDLE  hFile;  
//     GUID*   pgKnownSubject;  
// } WINTRUST_FILE_INFO, *PWINTRUST_FILE_INFO;  
// 
// typedef struct _CRYPTOAPI_BLOB {  
//     DWORD   cbData;  
//     BYTE    *pbData;  
// } CRYPT_INTEGER_BLOB, *PCRYPT_INTEGER_BLOB,  
// CRYPT_UINT_BLOB, *PCRYPT_UINT_BLOB,  
// CRYPT_OBJID_BLOB, *PCRYPT_OBJID_BLOB,  
// CERT_NAME_BLOB, *PCERT_NAME_BLOB,  
// CERT_RDN_VALUE_BLOB, *PCERT_RDN_VALUE_BLOB,  
// CERT_BLOB, *PCERT_BLOB,  
// CRL_BLOB, *PCRL_BLOB,  
// DATA_BLOB, *PDATA_BLOB,                   // JEFFJEFF temporary (too generic)  
// CRYPT_DATA_BLOB, *PCRYPT_DATA_BLOB,  
// CRYPT_HASH_BLOB, *PCRYPT_HASH_BLOB,  
// CRYPT_DIGEST_BLOB, *PCRYPT_DIGEST_BLOB,  
// CRYPT_DER_BLOB, *PCRYPT_DER_BLOB,  
// CRYPT_ATTR_BLOB, *PCRYPT_ATTR_BLOB;  
// 
// typedef struct _CTL_USAGE {  
//     DWORD               cUsageIdentifier;  
//     LPSTR               *rgpszUsageIdentifier;      // array of pszObjId  
// } CTL_USAGE, *PCTL_USAGE,  
// CERT_ENHKEY_USAGE, *PCERT_ENHKEY_USAGE;  
// typedef struct _CRYPT_ALGORITHM_IDENTIFIER {  
//     LPSTR               pszObjId;  
//     CRYPT_OBJID_BLOB    Parameters;  
// } CRYPT_ALGORITHM_IDENTIFIER, *PCRYPT_ALGORITHM_IDENTIFIER;  
// typedef struct _CRYPT_ATTRIBUTE {  
//     LPSTR               pszObjId;  
//     DWORD               cValue;  
//     PCRYPT_ATTR_BLOB    rgValue;  
// } CRYPT_ATTRIBUTE, *PCRYPT_ATTRIBUTE;  
// typedef struct _CTL_ENTRY {  
//     CRYPT_DATA_BLOB     SubjectIdentifier;          // For example, its hash  
//     DWORD               cAttribute;  
//     PCRYPT_ATTRIBUTE    rgAttribute;                // OPTIONAL  
// } CTL_ENTRY, *PCTL_ENTRY;  
// typedef struct _CERT_EXTENSION {  
//     LPSTR               pszObjId;  
//     BOOL                fCritical;  
//     CRYPT_OBJID_BLOB    Value;  
// } CERT_EXTENSION, *PCERT_EXTENSION;  
// typedef struct _CTL_INFO {  
//     DWORD                       dwVersion;  
//     CTL_USAGE                   SubjectUsage;  
//     CRYPT_DATA_BLOB             ListIdentifier;     // OPTIONAL  
//     CRYPT_INTEGER_BLOB          SequenceNumber;     // OPTIONAL  
//     FILETIME                    ThisUpdate;  
//     FILETIME                    NextUpdate;         // OPTIONAL  
//     CRYPT_ALGORITHM_IDENTIFIER  SubjectAlgorithm;  
//     DWORD                       cCTLEntry;  
//     PCTL_ENTRY                  rgCTLEntry;         // OPTIONAL  
//     DWORD                       cExtension;  
//     PCERT_EXTENSION             rgExtension;        // OPTIONAL  
// } CTL_INFO, *PCTL_INFO;  
// typedef void *HCERTSTORE;  
// typedef void *HCRYPTMSG;  
// typedef struct _CTL_CONTEXT {  
//     DWORD                   dwMsgAndCertEncodingType;  
//     BYTE                    *pbCtlEncoded;  
//     DWORD                   cbCtlEncoded;  
//     PCTL_INFO               pCtlInfo;  
//     HCERTSTORE              hCertStore;  
//     HCRYPTMSG               hCryptMsg;  
//     BYTE                    *pbCtlContent;  
//     DWORD                   cbCtlContent;  
// } CTL_CONTEXT, *PCTL_CONTEXT;  
// typedef const CTL_CONTEXT *PCCTL_CONTEXT;  
// typedef struct WINTRUST_CATALOG_INFO_  
// {  
//     DWORD         cbStruct;  
//     DWORD         dwCatalogVersion;  
//     LPCWSTR       pcwszCatalogFilePath;  
//     LPCWSTR       pcwszMemberTag;  
//     LPCWSTR       pcwszMemberFilePath;  
//     HANDLE        hMemberFile;  
//     BYTE*         pbCalculatedFileHash;  
//     DWORD         cbCalculatedFileHash;  
// 	PCCTL_CONTEXT pcCatalogContext;  
// } WINTRUST_CATALOG_INFO, *PWINTRUST_CATALOG_INFO;  
// typedef struct WINTRUST_BLOB_INFO_  
// {  
//     DWORD   cbStruct;  
//     GUID    gSubject;  
//     LPCWSTR pcwszDisplayName;  
//     DWORD   cbMemObject;  
//     BYTE*   pbMemObject;  
//     DWORD   cbMemSignedMsg;  
//     BYTE*   pbMemSignedMsg;  
// } WINTRUST_BLOB_INFO, *PWINTRUST_BLOB_INFO;  
// typedef struct _CRYPT_ATTRIBUTES {  
//     IN DWORD                cAttr;  
//     IN PCRYPT_ATTRIBUTE     rgAttr;  
// } CRYPT_ATTRIBUTES, *PCRYPT_ATTRIBUTES;  
// typedef struct _CMSG_SIGNER_INFO {  
//     DWORD                       dwVersion;  
//     CERT_NAME_BLOB              Issuer;  
//     CRYPT_INTEGER_BLOB          SerialNumber;  
//     CRYPT_ALGORITHM_IDENTIFIER  HashAlgorithm;  
//     CRYPT_ALGORITHM_IDENTIFIER  HashEncryptionAlgorithm;  
//     CRYPT_DATA_BLOB             EncryptedHash;  
//     CRYPT_ATTRIBUTES            AuthAttrs;  
//     CRYPT_ATTRIBUTES            UnauthAttrs;  
// } CMSG_SIGNER_INFO, *PCMSG_SIGNER_INFO;  
// typedef struct WINTRUST_SGNR_INFO_  
// {  
//     DWORD             cbStruct;  
//     LPCWSTR           pcwszDisplayName;  
//     CMSG_SIGNER_INFO* psSignerInfo;  
//     DWORD             chStores;  
//     HCERTSTORE*       pahStores;  
// } WINTRUST_SGNR_INFO, *PWINTRUST_SGNR_INFO;  
// typedef struct _CRYPT_BIT_BLOB {  
//     DWORD   cbData;  
//     BYTE    *pbData;  
//     DWORD   cUnusedBits;  
// } CRYPT_BIT_BLOB, *PCRYPT_BIT_BLOB;  
// typedef struct _CERT_PUBLIC_KEY_INFO {  
//     CRYPT_ALGORITHM_IDENTIFIER    Algorithm;  
//     CRYPT_BIT_BLOB                PublicKey;  
// } CERT_PUBLIC_KEY_INFO, *PCERT_PUBLIC_KEY_INFO;  
// typedef struct _CERT_INFO {  
//     DWORD                       dwVersion;  
//     CRYPT_INTEGER_BLOB          SerialNumber;  
//     CRYPT_ALGORITHM_IDENTIFIER  SignatureAlgorithm;  
//     CERT_NAME_BLOB              Issuer;  
//     FILETIME                    NotBefore;  
//     FILETIME                    NotAfter;  
//     CERT_NAME_BLOB              Subject;  
//     CERT_PUBLIC_KEY_INFO        SubjectPublicKeyInfo;  
//     CRYPT_BIT_BLOB              IssuerUniqueId;  
//     CRYPT_BIT_BLOB              SubjectUniqueId;  
//     DWORD                       cExtension;  
//     PCERT_EXTENSION             rgExtension;  
// } CERT_INFO, *PCERT_INFO;  
// typedef struct _CERT_CONTEXT {  
//     DWORD                   dwCertEncodingType;  
//     BYTE                    *pbCertEncoded;  
//     DWORD                   cbCertEncoded;  
//     PCERT_INFO              pCertInfo;  
//     HCERTSTORE              hCertStore;  
// } CERT_CONTEXT, *PCERT_CONTEXT;  
// typedef struct WINTRUST_CERT_INFO_  
// {  
//     DWORD         cbStruct;  
//     LPCWSTR       pcwszDisplayName;  
//     CERT_CONTEXT* psCertContext;  
//     DWORD         chStores;  
//     HCERTSTORE*   pahStores;  
//     DWORD         dwFlags;  
//     FILETIME*     psftVerifyAsOf;  
// } WINTRUST_CERT_INFO, *PWINTRUST_CERT_INFO;  
// typedef struct _WINTRUST_DATA  
// {  
//     DWORD  cbStruct;  
//     LPVOID pPolicyCallbackData;  
//     LPVOID pSIPClientData;  
//     DWORD  dwUIChoice;  
//     DWORD  fdwRevocationChecks;  
//     DWORD  dwUnionChoice;  
//     union  
//     {  
//         struct WINTRUST_FILE_INFO_*    pFile;  
//         struct WINTRUST_CATALOG_INFO_* pCatalog;  
//         struct WINTRUST_BLOB_INFO_*    pBlob;  
//         struct WINTRUST_SGNR_INFO_*    pSgnr;  
//         struct WINTRUST_CERT_INFO_*    pCert;  
//     } DUMMYUNIONNAMEX;  
//     DWORD  dwStateAction;  
//     HANDLE hWVTStateData;  
//     WCHAR* pwszURLReference;  
//     DWORD  dwProvFlags;  
//     DWORD  dwUIContext;  
// } WINTRUST_DATA, *PWINTRUST_DATA;  
// typedef struct CATALOG_INFO_   
// {  
//     DWORD cbStruct;  
//     WCHAR wszCatalogFile[MAX_PATH];  
// } CATALOG_INFO;  
// typedef HANDLE HCATADMIN;  
// typedef HANDLE HCATINFO;  
//  
// /* dwUIChoice */  
// #define WTD_UI_ALL                1  
// #define WTD_UI_NONE               2  
// #define WTD_UI_NOBAD              3  
// #define WTD_UI_NOGOOD             4  
// /* fdwRevocationChecks */  
// #define WTD_REVOKE_NONE           0  
// #define WTD_REVOKE_WHOLECHAIN     1  
// /* dwUnionChoice */  
// #define WTD_CHOICE_FILE           1  
// #define WTD_CHOICE_CATALOG        2  
// #define WTD_CHOICE_BLOB           3  
// #define WTD_CHOICE_SIGNER         4  
// #define WTD_CHOICE_CERT           5  
// #define WTD_STATEACTION_IGNORE           0  
// #define WTD_STATEACTION_VERIFY           1  
// #define WTD_STATEACTION_CLOSE            2  
// #define WTD_STATEACTION_AUTO_CACHE       3  
// #define WTD_STATEACTION_AUTO_CACHE_FLUSH 4  
// #define WTD_SAFER_FLAG                          0x00000100  
// #define WINTRUST_ACTION_GENERIC_VERIFY_V2 \
// { 0xaac56b,   0xcd44, 0x11d0, { 0x8c,0xc2,0x00,0xc0,0x4f,0xc2,0x95,0xee }}  
// 
// BOOL (WINAPI *CryptCATAdminAcquireContextX)(HCATADMIN*,const GUID*,DWORD);  
// BOOL (WINAPI *CryptCATAdminReleaseContextX)(HCATADMIN,DWORD);  
// BOOL (WINAPI *CryptCATAdminCalcHashFromFileHandleX)(HANDLE,DWORD*,BYTE*,DWORD);  
// HCATINFO  (WINAPI *CryptCATAdminEnumCatalogFromHashX)(HCATADMIN,BYTE*,DWORD,DWORD,HCATINFO*);  
// BOOL (WINAPI *CryptCATAdminReleaseCatalogContextX)(HCATADMIN,HCATINFO,DWORD);  
// BOOL (WINAPI *CryptCATCatalogInfoFromContextX)(HCATINFO,CATALOG_INFO*,DWORD);  
// LONG (WINAPI *WinVerifyTrustX)( HWND hwnd,GUID *ActionID,LPVOID  ActionData); 
// 
// BOOL InitFunc();
// 
// BOOL CheckFileTrust( LPCWSTR lpFileName )  
// {  
// 	if (CryptCATAdminAcquireContextX == NULL)
// 	{
// 		if(InitFunc() == FALSE) return FALSE;
// 	}
// 
//     BOOL bRet = FALSE;  
//     WINTRUST_DATA wd = { 0 };  
//     WINTRUST_FILE_INFO wfi = { 0 };  
//     WINTRUST_CATALOG_INFO wci = { 0 };  
//     CATALOG_INFO ci = { 0 };  
//     HCATADMIN hCatAdmin = NULL;  
// 	HANDLE hFile = NULL;  
//     DWORD dwCnt = 100;  
//     BYTE byHash[100];  
// 	
// 	DWORD dw = 0;  
// 	HCATINFO hCatInfo;  
// 	GUID action = WINTRUST_ACTION_GENERIC_VERIFY_V2;  
// 	HRESULT hr;  
// 	LPWSTR pszMemberTag = (LPWSTR)malloc(sizeof(WCHAR)*(dwCnt * 2 + 1));//new WCHAR[dwCnt * 2 + 1];  
// 	if ( !CryptCATAdminAcquireContextX( &hCatAdmin, NULL, 0 ) )  
//     {  
//         return FALSE;  
//     }  
//     hFile = CreateFileW( lpFileName, GENERIC_READ, FILE_SHARE_READ,  
//         NULL, OPEN_EXISTING, 0, NULL );  
//     if ( INVALID_HANDLE_VALUE == hFile )  
//     {  
//         CryptCATAdminReleaseContextX( hCatAdmin, 0 );  
//         return FALSE;  
//     }  
// 	//对整个文件做hash,采用标准SHA1_160bit算法  
//     CryptCATAdminCalcHashFromFileHandleX( hFile, &dwCnt, byHash, 0 );  
//     CloseHandle( hFile );  
// 	
// 	//将hash的结果 变换成unicode 例如0x8c -> 38 00 43 00  
//     for ( dw = 0; dw < dwCnt; ++dw )  
//     {  
//         wsprintfW( &pszMemberTag[dw * 2], L"%02X", byHash[dw] );  
//     }  
//     hCatInfo = CryptCATAdminEnumCatalogFromHashX( hCatAdmin,  byHash, dwCnt, 0, NULL );  
//     if ( NULL == hCatInfo )  
//     {  
//         wfi.cbStruct       = sizeof( WINTRUST_FILE_INFO );  
//         wfi.pcwszFilePath  = lpFileName;  
//         wfi.hFile          = NULL;  
//         wfi.pgKnownSubject = NULL;  
//         wd.cbStruct            = sizeof( WINTRUST_DATA );  
//         wd.dwUnionChoice       = WTD_CHOICE_FILE;  
// 		//     wd.pFile               = &wfi;  
// 		wd.DUMMYUNIONNAMEX.pFile      = &wfi;   
//         wd.dwUIChoice          = WTD_UI_NONE;  
//         wd.fdwRevocationChecks = WTD_REVOKE_NONE;  
//         wd.dwStateAction       = WTD_STATEACTION_IGNORE;  
//         wd.dwProvFlags         = WTD_SAFER_FLAG;  
//         wd.hWVTStateData       = NULL;  
//         wd.pwszURLReference    = NULL;  
//     }  
//     else  
//     {  
//         CryptCATCatalogInfoFromContextX( hCatInfo, &ci, 0 );  
//         wci.cbStruct             = sizeof( WINTRUST_CATALOG_INFO );  
//         wci.pcwszCatalogFilePath = ci.wszCatalogFile;  
//         wci.pcwszMemberFilePath  = lpFileName;  
//         wci.pcwszMemberTag       = pszMemberTag;  
//         wd.cbStruct            = sizeof( WINTRUST_DATA );  
//         wd.dwUnionChoice       = WTD_CHOICE_CATALOG;  
//         wd.DUMMYUNIONNAMEX.pCatalog            = &wci;  
//         wd.dwUIChoice          = WTD_UI_NONE;  
//         wd.fdwRevocationChecks = WTD_STATEACTION_VERIFY;  
//         wd.dwProvFlags         = 0;  
//         wd.hWVTStateData       = NULL;  
//         wd.pwszURLReference    = NULL;  
//     }  
// 	
//     hr  = WinVerifyTrustX( NULL, &action, &wd );  
// 	//关键在于跟踪WinVerifyTrust()的过程了  
//     bRet        = SUCCEEDED( hr );  
//     if ( NULL != hCatInfo )  
//     {  
//         CryptCATAdminReleaseCatalogContextX( hCatAdmin, hCatInfo, 0 );  
//     }  
//     CryptCATAdminReleaseContextX( hCatAdmin, 0 );  
// 	free(pszMemberTag);  
//     return bRet;  
// }  
// BOOL InitFunc()  
// {  
// 	HMODULE dllHandle = NULL;  
// 	dllHandle = LoadLibrary("wintrust.dll");  
// 	if (NULL == dllHandle)  
// 	{  
// 		printf("LoadLibrary wintrust.dll error!%d/n",GetLastError());  
// 		return FALSE;  
// 	}  
// 	(FARPROC &)CryptCATAdminAcquireContextX = GetProcAddress(dllHandle,TEXT("CryptCATAdminAcquireContext"));  
// 	(FARPROC &)CryptCATAdminReleaseContextX = GetProcAddress(dllHandle,TEXT("CryptCATAdminReleaseContext"));  
// 	(FARPROC &)CryptCATAdminCalcHashFromFileHandleX = GetProcAddress(dllHandle,TEXT("CryptCATAdminCalcHashFromFileHandle"));  
// 	(FARPROC &)CryptCATAdminEnumCatalogFromHashX = GetProcAddress(dllHandle,TEXT("CryptCATAdminEnumCatalogFromHash"));  
// 	(FARPROC &)CryptCATAdminReleaseCatalogContextX = GetProcAddress(dllHandle,TEXT("CryptCATAdminReleaseCatalogContext"));  
// 	(FARPROC &)CryptCATCatalogInfoFromContextX = GetProcAddress(dllHandle,TEXT("CryptCATCatalogInfoFromContext"));  
// 	(FARPROC &)WinVerifyTrustX = GetProcAddress(dllHandle,TEXT("WinVerifyTrust"));  
// 
// 	if (CryptCATAdminAcquireContextX == NULL  
// 		|| CryptCATAdminReleaseContextX == NULL  
// 		|| CryptCATAdminCalcHashFromFileHandleX == NULL  
// 		|| CryptCATAdminEnumCatalogFromHashX == NULL  
// 		|| CryptCATAdminReleaseCatalogContextX == NULL  
// 		|| CryptCATCatalogInfoFromContextX == NULL   
// 		|| WinVerifyTrustX == NULL)   
// 	{  
// 		//printf("GetProcAddress error 1%d/n",GetLastError());  
// 		return FALSE;  
// 	}  
// 	return TRUE;
// }

//-------------------------------------------------------------------------------
DWORD	WINAPI SetDisableOrEnable(BOOL IsEnable,PVOID *OldValue)
{
	typedef int (WINAPI *Wow64DisableWow64FsRedirectionFn)(PVOID *OldValue);
	typedef int (WINAPI *Wow64RevertWow64FsRedirectionFn)(PVOID OldValue);
	HMODULE hkernel = GetModuleHandle(_T("kernel32.dll"));
	Wow64DisableWow64FsRedirectionFn Disable_redirect = NULL;
	Wow64RevertWow64FsRedirectionFn Enable_redirect = NULL;

	if(hkernel == NULL)
	{
		return 0;
	}

	Disable_redirect = reinterpret_cast<Wow64DisableWow64FsRedirectionFn>(GetProcAddress(hkernel, "Wow64DisableWow64FsRedirection"));
	Enable_redirect = reinterpret_cast<Wow64RevertWow64FsRedirectionFn>(GetProcAddress(hkernel, "Wow64RevertWow64FsRedirection"));

	if (Disable_redirect == NULL || Enable_redirect == NULL)
	{
		return 0;
	}

	if (!IsEnable)
	{
		//	MessageBox(NULL,"init","sys",0+64);
		Disable_redirect(OldValue);
		//	MessageBox(NULL,"out","sys",0+64);
	}
	else
	{
		Enable_redirect(*OldValue);
	}
	return 0;
}

//-------------------------------------------------------------------------------

std::string GetFileMd5(const char * filename)
{
	std::string strRet;

	PVOID old_value = NULL;
	if(GetWOW64PlatformSupport()) SetDisableOrEnable(FALSE,&old_value);

	{
		if( NULL == filename)
		{
			return strRet;
		}
		
		FILE* fp = fopen(filename, "rb");
		if( NULL == fp)
		{
			DWORD err = GetLastError();
			return strRet;
		}
		fseek(fp, 0, SEEK_END);
		long nTotalLen = ftell(fp);
		fseek(fp, SEEK_SET, 0);
		char* pBuffer = new char[nTotalLen + 100];
		if( NULL == pBuffer)
		{
			fclose(fp);
			return "";
		}
		int nReadLen = 0;
		int nIndex = 0;
		while( nIndex < nTotalLen)
		{
			nReadLen = fread(&pBuffer[nIndex], 1, 4096, fp);
			nIndex += nReadLen;
			if(nReadLen < 1)
			{
				break;
			}
		}
		pBuffer[nIndex] = 0;
		fclose(fp);
		MD5 md5;
		md5.update(pBuffer, nIndex);
		delete [] pBuffer;
		strRet = md5.toString();
	}

	if(GetWOW64PlatformSupport()) SetDisableOrEnable(TRUE,&old_value);

	return strRet;
}

unsigned int GetFileCrc(const char * filename)
{
	unsigned int unRet = 0;

	PVOID old_value = NULL;
	if(GetWOW64PlatformSupport()) SetDisableOrEnable(FALSE,&old_value);

	{
		if( NULL == filename)
		{
			char szError[512]={0};
			_snprintf(szError,512,"error:文件名空\n");
			OutputDebugStringA(szError);
			return 0;
		}

		FILE* fp = fopen(filename, "rb");
		if( NULL == fp)
		{
			DWORD err = GetLastError();
			char szError[512]={0};
			_snprintf(szError,512,"error:[GetFileCrc]打开文件失败[%s][errno:%u]\n",filename,err);
			OutputDebugStringA(szError);
			return 0;
		}

		fseek(fp, 0, SEEK_END);
		long nTotalLen = ftell(fp);
		rewind(fp);

		char cbBuffer[8192];

		int nReadLen = 0;

		CCRC crc;
		while( (nReadLen=fread(cbBuffer, 1, 8192, fp)) > 0 )
		{
			unRet = crc.GetStrCrc_Key(cbBuffer,nReadLen);
		}

		fclose(fp);
	}

	if(GetWOW64PlatformSupport()) SetDisableOrEnable(TRUE,&old_value);

	return unRet;
}

// unsigned int GetFileCrc(const char * filename)
// {
// 	unsigned int unRet = 0;
// 
// 	HANDLE hand;///句柄
// 	hand=CreateFileA(filename,GENERIC_READ,///打开方式，可读
// 		FILE_SHARE_READ,//共享读
// 		NULL,OPEN_EXISTING,
// 		FILE_ATTRIBUTE_NORMAL,
// 		NULL);
// 	//没有打开
// 	if (hand==INVALID_HANDLE_VALUE) return 0;
// 
// 	LARGE_INTEGER m_file_len;
// 	BY_HANDLE_FILE_INFORMATION m_file_info;
// 	::GetFileInformationByHandle(hand,&m_file_info);
// 	m_file_len.LowPart=m_file_info.nFileSizeLow;
// 	m_file_len.HighPart=m_file_info.nFileSizeHigh;
// 		
// 	ULONG re;
// 	char buf[1024*8];
// 	CCRC crc;
// 	while(m_file_len.QuadPart)
// 	{
// 		memset(buf,0,1024*8);
// 		if(!ReadFile(hand,buf,1024*8,&re,0))
// 		{
// 			///读取文件失败
// 			return 0;
// 		}
// 		m_file_len.QuadPart=m_file_len.QuadPart-re;
// 		unRet = crc.GetStrCrc_Key((char*)buf,re);
// 	}
// 	CloseHandle(hand);
// 	
// 	return unRet;
// }


BOOL ExtractFile(LPCTSTR restype, int resid, LPCTSTR destpath,HMODULE hModule)
{
	HRSRC hRes;
	HGLOBAL hFileData;
	BOOL bResult = FALSE;

	hRes = FindResource(hModule, MAKEINTRESOURCE(resid), restype);
	if(hRes)
	{
		hFileData = LoadResource(hModule, hRes);
		if(hFileData)
		{
			DWORD dwSize = SizeofResource(hModule, hRes);
			if(dwSize)
			{
				HANDLE hFile = CreateFile( destpath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, 0);
				if(hFile != INVALID_HANDLE_VALUE)
				{
					DWORD dwWritten = 0;
					BOOL b = WriteFile(hFile, hFileData, dwSize, &dwWritten, NULL);
					if(b && dwWritten == dwSize)
					{
						CloseHandle(hFile);
						return TRUE;
					}

					CloseHandle(hFile);
				}
			}
		}
	} 
	return bResult;
}


void CheckAndExportFile(const TCHAR* pResName, const TCHAR * path,DWORD dwResource)
{
	if (_taccess(path,0) == -1)
	{
		ExtractFile(pResName, dwResource, path, ThisModuleHandle());
	}

}

//-------------------------------------------------------------------------------
//#pragma region 全局函数

BOOL DosPath2NtPath(LPTSTR pszDosPath, LPTSTR pszNtPath)  
{  
	TCHAR           szDriveStr[500];  
	TCHAR           szDrive[3];  
	TCHAR           szDevName[100];  
	INT             cchDevName;  
	INT             i;  

	//检查参数  
	if(!pszDosPath || !pszNtPath )  
		return FALSE;  

	//获取本地磁盘字符串  
	if(GetLogicalDriveStrings(sizeof(szDriveStr), szDriveStr))  
	{  
		for(i = 0; szDriveStr[i]; i += 4)  
		{  
			if(!lstrcmpi(&(szDriveStr[i]), _T("A:\\")) || !lstrcmpi(&(szDriveStr[i]), _T("B:\\")))  
				continue;  

			szDrive[0] = szDriveStr[i];  
			szDrive[1] = szDriveStr[i + 1];  
			szDrive[2] = '\0';  
			if(!QueryDosDevice(szDrive, szDevName, 100))//查询 Dos 设备名  
				return FALSE;

			_tcscat(szDevName,_T("\\"));
			cchDevName = lstrlen(szDevName);  
			if(_tcsnicmp(pszDosPath, szDevName, cchDevName) == 0)//命中  
			{  
				lstrcpy(pszNtPath, szDrive);//复制驱动器  
				lstrcat(pszNtPath, pszDosPath + cchDevName - 1);//复制路径  

				return TRUE;  
			}             
		}  
	}  
	lstrcpy(pszNtPath, pszDosPath);  
	return FALSE;  
}  

//获取进程路径名称
std::string GetProcessName(HANDLE hProcess)
{
	OSVERSIONINFO osvi;//定义OSVERSIONINFO数据结构对象
	memset(&osvi, 0, sizeof(OSVERSIONINFO));//开空间 
	osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);//定义大小 
	GetVersionEx (&osvi);//获得版本信息 

	if (osvi.dwMajorVersion >= 6)//os vista above
	{
		return GetWin7ProcessName(hProcess);
	}
	else
	{
		return GetXPProcessName(hProcess);
	}
}

//获得win7进程路径
std::string GetWin7ProcessName(HANDLE handle)
{
	std::string strRet="";

	USES_CONVERSION;

	typedef DWORD (__stdcall *ptr_fun)(HANDLE,LPTSTR,DWORD);
	HINSTANCE hInstance = LoadLibrary(TEXT("Psapi.dll") ); 
	if (hInstance != NULL)
	{
#ifdef UNICODE
		ptr_fun ptr = (ptr_fun)GetProcAddress(hInstance, "GetProcessImageFileNameW");
#else
		ptr_fun ptr = (ptr_fun)GetProcAddress(hInstance, "GetProcessImageFileNameA");
#endif

		if(ptr != NULL)  
		{
			TCHAR procName[MAX_PATH] = {0};
			DWORD dwRet = ptr(handle, procName, MAX_PATH);
			char procName_tmp[MAX_PATH] = {0};
			if( 0 == dwRet)
			{
				DWORD dwLastErr = ::GetLastError();
				dwRet = GetModuleFileNameEx(handle, NULL, procName, MAX_PATH);
				strRet = T2A(procName);
			}
			else
			{
				TCHAR cNtPath[MAX_PATH + 1] = {0};
				DosPath2NtPath(procName, cNtPath);
				strRet = T2A(cNtPath);
			}

			//DosPathToNtPath(procName, procName_tmp);
			FreeLibrary(hInstance);
		}
	}
	return strRet;
}

//获得winXP进程路径
std::string GetXPProcessName(HANDLE handle)
{
	std::string strRet="";

	USES_CONVERSION;

	typedef DWORD (__stdcall *ptr_fun)(HANDLE,LPTSTR,DWORD);
	HINSTANCE hInstance = LoadLibrary(TEXT("Psapi.dll") ); 
	if (hInstance != NULL)
	{
#ifdef UNICODE
		ptr_fun ptr = (ptr_fun)GetProcAddress(hInstance, "GetProcessImageFileNameW"); 
#else
		ptr_fun ptr = (ptr_fun)GetProcAddress(hInstance, "GetProcessImageFileNameA"); 
#endif

		if (ptr != NULL)
		{
			TCHAR procName[MAX_PATH] = {0};
			DWORD dwRet = ptr(handle, procName, MAX_PATH);
			char procName_tmp[MAX_PATH] = {0};
			if( 0 == dwRet)
			{
				dwRet = GetModuleFileNameEx(handle, NULL, procName, MAX_PATH);
				strRet = T2A(procName);
			}
			else
			{
				TCHAR cNtPath[MAX_PATH + 1] = {0};
				DosPath2NtPath(procName, cNtPath);
				strRet = T2A(cNtPath);
			}

			//DosPathToNtPath(procName, procName_tmp);
			FreeLibrary(hInstance);
		}
	}
	return strRet;
}

std::string change_dirver_path(std::string path)
{
	if (path.empty())
		return "";
	char src[MAX_PATH] = {0};
	char dest[MAX_PATH] = {0};
	std::string str_32_u = "system32";
	std::string str_32_l = "System32";

	std::string str_sysroot_ru = "\\SystemRoot\\System32\\";
	std::string str_sysroot_rl = "\\SystemRoot\\system32\\";

	std::string str_sysroot_au = "%SystemRoot%\\System32\\";
	std::string str_sysroot_al = "%SystemRoot%\\system32\\";

	std::string str_sysroot_ll = "%systemroot%\\system32\\";
	if (path.length() > 4)
	{
		if (path.substr(0 , 4).compare("\\??\\") == 0)
			return path.substr(4, path.length() - 4);
		else if(path.substr(0, str_32_u.length()).compare(str_32_u) == 0)
		{
			strcat(dest, "C:\\Windows\\system32");
			strcat(dest, path.substr(str_32_u.length(), path.length() - str_32_u.length()).c_str());
			return dest;
		}
		else if(path.substr(0, str_32_l.length()).compare(str_32_l) == 0)
		{
			strcat(dest, "C:\\Windows\\System32");
			strcat(dest, path.substr(str_32_l.length(), path.length() - str_32_l.length()).c_str());
			return dest;
		}
		else if(path.substr(0, str_sysroot_ru.length()).compare(str_sysroot_ru) == 0)
		{
			strcat(dest, "C:\\Windows\\System32\\");
			strcat(dest, path.substr(str_sysroot_ru.length(), path.length() - str_sysroot_ru.length()).c_str());
			return dest;
		}
		else if(path.substr(0, str_sysroot_rl.length()).compare(str_sysroot_rl) == 0)
		{
			strcat(dest, "C:\\Windows\\System32\\");
			strcat(dest, path.substr(str_sysroot_rl.length(), path.length() - str_sysroot_rl.length()).c_str());
			return dest;
		}
		else if(path.substr(0, str_sysroot_au.length()).compare(str_sysroot_au) == 0)
		{
			strcat(dest, "C:\\Windows\\System32\\");
			strcat(dest, path.substr(str_sysroot_au.length(), path.length() - str_sysroot_au.length()).c_str());
			return dest;
		}
		else if(path.substr(0, str_sysroot_al.length()).compare(str_sysroot_al) == 0)
		{
			strcat(dest, "C:\\Windows\\System32\\");
			strcat(dest, path.substr(str_sysroot_al.length(), path.length() - str_sysroot_al.length()).c_str());
			return dest;
		}
		else if(path.substr(0, str_sysroot_ll.length()).compare(str_sysroot_ll) == 0)
		{
			strcat(dest, "C:\\Windows\\System32\\");
			strcat(dest, path.substr(str_sysroot_al.length(), path.length() - str_sysroot_al.length()).c_str());
			return dest;
		}
		else
			return path;
	}
	else
		return path;
}