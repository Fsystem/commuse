#include "Stdafx.h"
#include "RegOprate.h"


#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383
#define MAX_DATA_LEN   32768

CRegOprate g_GlobalRegOpt;

//////////////////////////////////////////////////////////////////////////
//CRegOprate

CRegOprate::CRegOprate()
{
	hkParent=NULL;
}
CRegOprate::~CRegOprate()
{
	if (hkParent)
	{
		::RegCloseKey(hkParent);
		hkParent = NULL;
	}
}


bool CRegOprate::HasChild(HKEY hParent)
{
	bool bRes = false;
	DWORD cSubKeys = 0;
	if (hParent)
	{
		DWORD retCode = ::RegQueryInfoKey(
			hParent,					// key handle 
			NULL,						// buffer for class name 
			NULL,						// size of class string 
			NULL,						// reserved 
			&cSubKeys,					// number of subkeys 
			NULL,						// longest subkey size 
			NULL,						// longest class string 
			NULL,						// number of values for this key 
			NULL,						// longest value name 
			NULL,						// longest value data 
			NULL,						// security descriptor 
			NULL);						// last write time 
	}
	bRes = (cSubKeys>0);
	return bRes;
}

bool CRegOprate::QuerySubKey(HKEY hParent,LPCTSTR lpSubKey)
{
	bool bRes = false;
	//////////////////////////////////////////////////////////////////////////

	TCHAR    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
	DWORD    cbName;                   // size of name string 
	TCHAR    achClass[MAX_PATH] = TEXT("");  // buffer for class name 
	DWORD    cchClassName = MAX_PATH;  // size of class string 
	DWORD    cSubKeys=0;               // number of subkeys 
	DWORD    cbMaxSubKey;              // longest subkey size 
	DWORD    cchMaxClass;              // longest class string 
	DWORD    cValues;              // number of values for key 
	DWORD    cchMaxValue;          // longest value name 
	DWORD    cbMaxValueData;       // longest value data 
	DWORD    cbSecurityDescriptor; // size of security descriptor 
	FILETIME ftLastWriteTime;      // last write time 

	DWORD i, retCode; 

	// Get the class name and the value count. 
	retCode = RegQueryInfoKey(
		hParent,                    // key handle 
		achClass,                // buffer for class name 
		&cchClassName,           // size of class string 
		NULL,                    // reserved 
		&cSubKeys,               // number of subkeys 
		&cbMaxSubKey,            // longest subkey size 
		&cchMaxClass,            // longest class string 
		&cValues,                // number of values for this key 
		&cchMaxValue,            // longest value name 
		&cbMaxValueData,         // longest value data 
		&cbSecurityDescriptor,   // security descriptor 
		&ftLastWriteTime);       // last write time 

	// Enumerate the subkeys, until RegEnumKeyEx fails.

	if (cSubKeys)
	{
		printf( "\nNumber of subkeys: %d\n", cSubKeys);

		for (i=0; i<cSubKeys; i++) 
		{ 
			cbName = MAX_KEY_LENGTH;
			retCode = RegEnumKeyEx(hParent, i,
				achKey, 
				&cbName, 
				NULL, 
				NULL, 
				NULL, 
				&ftLastWriteTime); 
			if (retCode == ERROR_SUCCESS) 
			{
				_tprintf(TEXT("(%d) %s\n"), i+1, achKey);
				if ( !_tcscmp(lpSubKey,achKey) )
				{
					bRes = true;
					break;
				}
			}
		}
	} 
	//////////////////////////////////////////////////////////////////////////

	return bRes;
}

bool CRegOprate::QuerySubValue(HKEY hSub,LPCTSTR lpSubValue,LPVOID pData,LPDWORD pcData,DWORD dwType)
{
	bool bRes = false;
	//////////////////////////////////////////////////////////////////////////
	TCHAR    achClass[MAX_PATH] = TEXT("");  // buffer for class name 
	DWORD    cchClassName = MAX_PATH;  // size of class string 
	DWORD    cSubKeys=0;               // number of subkeys 
	DWORD    cbMaxSubKey;              // longest subkey size 
	DWORD    cchMaxClass;              // longest class string 
	DWORD    cValues;              // number of values for key 
	DWORD    cchMaxValue;          // longest value name 
	DWORD    cbMaxValueData;       // longest value data 
	DWORD    cbSecurityDescriptor; // size of security descriptor 
	FILETIME ftLastWriteTime;      // last write time 
	DWORD	 dwRegType;
	DWORD	 dwDataLen=MAX_DATA_LEN;
	BYTE	 aData[MAX_DATA_LEN];

	DWORD i, retCode; 

	TCHAR  achValue[MAX_VALUE_NAME]; 
	DWORD cchValue = MAX_VALUE_NAME; 

	// Get the class name and the value count. 
	retCode = RegQueryInfoKey(
		hSub,                    // key handle 
		achClass,                // buffer for class name 
		&cchClassName,           // size of class string 
		NULL,                    // reserved 
		&cSubKeys,               // number of subkeys 
		&cbMaxSubKey,            // longest subkey size 
		&cchMaxClass,            // longest class string 
		&cValues,                // number of values for this key 
		&cchMaxValue,            // longest value name 
		&cbMaxValueData,         // longest value data 
		&cbSecurityDescriptor,   // security descriptor 
		&ftLastWriteTime);       // last write time 

	// Enumerate the key values. 

	if (cValues) 
	{
		printf( "\nNumber of values: %d\n", cValues);

		for (i=0, retCode=ERROR_SUCCESS; i<cValues; i++) 
		{ 
			cchValue = MAX_VALUE_NAME; 
			achValue[0] = '\0'; 
			dwDataLen = MAX_DATA_LEN;
			dwRegType = REG_NONE;
			retCode = RegEnumValue(hSub, i, 
				achValue, 
				&cchValue, 
				NULL, 
				&dwRegType,
				aData,
				&dwDataLen);

			if (retCode == ERROR_SUCCESS ) 
			{ 
				_tprintf(TEXT("(%d) %s\n"), i+1, achValue); 
				if ( !_tcscmp(lpSubValue,achValue)  )
				{
					ASSERT(dwRegType == dwType);
					if ( dwRegType == dwType)
					{
						bRes = true;
						if(pcData) *pcData = dwDataLen;
						if(pData) memcpy(pData,aData,dwDataLen);
						break;
					}

				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	return bRes;
}


HKEY CRegOprate::CreateSubKey(HKEY hParent,LPCTSTR lpSubKey)
{
	HKEY hkRes  = NULL;
	if (hParent)
	{
		bool bRes= false;
		bRes = QuerySubKey(hParent,lpSubKey);
		if (!bRes)
		{
			//HKEY hkSub; 
			DWORD dwDisp; 

			// Create the event source as a subkey of the log. 
			LSTATUS lr = ::RegCreateKeyEx(hParent, lpSubKey,
				0, NULL, REG_OPTION_NON_VOLATILE,
				KEY_WRITE|KEY_READ, NULL, &hkRes, &dwDisp);
			if (ERROR_SUCCESS == lr)
			{
				bRes =true;
			}
			else 
			{
				bRes = false;
			}
		}
		else
		{
			::RegOpenKeyEx(hParent,lpSubKey,0,KEY_WRITE|KEY_READ,&hkRes);
		}
	}
	return hkRes;
}


// bool CRegOprate::SaveSubKey(HKEY hParent,LPCTSTR lpSubKey)
// {
// 	bool bRes = false;
// 	if(hParent)
// 	{
// 		LSTATUS lr = ::RegSaveKey(hParent,lpSubKey,NULL);
// 		if (ERROR_SUCCESS == lr || ERROR_ALREADY_EXISTS == lr)
// 		{
// 			bRes = true;
// 		}
// 		else
// 		{
// 			bRes = false;
// 		}
// 	}
// 	return bRes;
// }

bool CRegOprate::DeleteSubKey(HKEY hParent,LPCTSTR lpSubKey)
{
	bool bRes = false;
	if (hParent)
	{
		LSTATUS lr = ::RegDeleteKey(hParent,lpSubKey);

		if(ERROR_SUCCESS == lr)
		{
			bRes = true;
		}

		//bRes = QuerySubKey(hParent,lpSubKey);
		//if (bRes)
		//{
		//	LSTATUS lr = ::RegDeleteKeyEx(hParent,lpSubKey,KEY_WRITE,0);
		//	if(ERROR_SUCCESS == lr)
		//	{
		//		bRes = true;
		//	}
		//	else
		//	{
		//		bRes = false;
		//	}
		//}
	}
	return bRes;
}


bool CRegOprate::SaveSubValue(HKEY hSub,LPCTSTR lpSubValue,LPVOID pData,DWORD cData,DWORD dwType)
{
	bool bRes = false;
	if (hSub)
	{
		LSTATUS lr = ::RegSetValueEx(
			hSub,						// subkey handle 
			lpSubValue,					// value name 
			0,							// must be zero 
			dwType,						// value type 
			(LPBYTE) pData,				// pointer to value data 
			(DWORD) cData);				// data size
		if ( ERROR_SUCCESS == lr )
		{
			bRes = true;
		}
	}
	return bRes;
}

//
void CRegOprate::SetRegistry(LPCTSTR lpRegistry)
{
	hkParent	= NULL;
	LSTATUS lr = ::RegOpenKeyEx(HKEY_CURRENT_USER,lpRegistry,NULL,KEY_READ|KEY_WRITE,&hkParent);
	if( ERROR_SUCCESS == lr )
	{
		//::RegQueryInfoKey(hkCurUser,NULL,NULL,NULL,&dwItemCount,&dwMaxItemNameLen,NULL,NULL,NULL,NULL,NULL,NULL);
	}
}
//
void CRegOprate::WriteProfileString(  LPCTSTR lpAppName,  LPCTSTR lpKeyName,  LPCTSTR lpString )
{
	if (hkParent )
	{
		HKEY hkSub = CreateSubKey(hkParent,lpAppName);
		SaveSubValue(hkSub,lpKeyName,(LPVOID)lpString,_tcslen(lpString),REG_SZ);
		::RegCloseKey(hkSub);
	}
}
//
void CRegOprate::WriteProfileInt(  LPCTSTR lpAppName,  LPCTSTR lpKeyName,  INT nValue )
{
	if (hkParent )
	{
		HKEY hkSub = CreateSubKey(hkParent,lpAppName);
		SaveSubValue(hkSub,lpKeyName,&nValue,sizeof(DWORD),REG_DWORD);
		::RegCloseKey(hkSub);
	}
}
//
INT CRegOprate::GetProfileInt( LPCTSTR lpAppName,  LPCTSTR lpKeyName,  INT nDefault)
{
	INT iRes=nDefault;
	if (hkParent )
	{
		if (hkParent )
		{
			bool bRes = QuerySubKey(hkParent,lpAppName);
			if (bRes)
			{
				HKEY hkSub;
				LSTATUS lr = ::RegOpenKeyEx(hkParent,lpAppName,NULL,KEY_READ,&hkSub);
				if (ERROR_SUCCESS == lr)
				{
					DWORD dwSize = 0;
					bRes = QuerySubValue(hkSub,lpKeyName,(LPDWORD)&iRes,&dwSize,REG_DWORD);
					if (bRes)
					{

					}
					::RegCloseKey(hkSub);
				}
			}
		}
	}
	return iRes;
}
//
void CRegOprate::GetProfileString( LPCTSTR lpAppName,  LPCTSTR lpKeyName,  LPCTSTR lpDefault,  LPTSTR lpValue)
{
	if (hkParent )
	{
		if(!lpDefault)lpDefault=TEXT("");
		_tcscpy(lpValue,lpDefault);
		bool bRes = QuerySubKey(hkParent,lpAppName);
		if (bRes)
		{
			HKEY hkSub;
			LSTATUS lr = ::RegOpenKeyEx(hkParent,lpAppName,NULL,KEY_READ,&hkSub);
			if (ERROR_SUCCESS == lr)
			{
				DWORD dwSize = 0;
				bRes = QuerySubValue(hkSub,lpKeyName,lpValue,&dwSize,REG_SZ);
				if (bRes)
				{

				}
				::RegCloseKey(hkSub);
			}
		}
	}

}


