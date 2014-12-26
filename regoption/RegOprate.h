#ifndef __RegOprate_H
#define __RegOprate_H

//注册表操作类


class CRegOprate
{
public:
	CRegOprate();
	~CRegOprate();
	//
	void SetRegistry(LPCTSTR lpRegistry);
	//
	void WriteProfileString(  LPCTSTR lpAppName,  LPCTSTR lpKeyName,  LPCTSTR lpString );
	//
	void WriteProfileInt(  LPCTSTR lpAppName,  LPCTSTR lpKeyName,  INT nValue );
	//
	INT GetProfileInt( LPCTSTR lpAppName,  LPCTSTR lpKeyName,  INT nDefault);
	//
	void GetProfileString( LPCTSTR lpAppName,  LPCTSTR lpKeyName,  LPCTSTR lpDefault,  LPTSTR lpValue);

protected:
	bool HasChild(HKEY hParent);
	bool QuerySubKey(HKEY hParent,LPCTSTR lpSubKey);
	bool QuerySubValue(HKEY hSub,LPCTSTR lpSubValue,LPVOID pData,LPDWORD pcData,DWORD dwType);
	HKEY CreateSubKey(HKEY hParent,LPCTSTR lpSubKey);
	//bool SaveSubKey(HKEY hParent,LPCTSTR lpSubKey);需要进程有SE_BACKUP_NAME privilege enabled
	bool DeleteSubKey(HKEY hParent,LPCTSTR lpSubKey);
	bool SaveSubValue(HKEY hSub,LPCTSTR lpSubValue,LPVOID pData,DWORD cData,DWORD dwType);

private:
	HKEY	hkParent;
};

extern CRegOprate g_GlobalRegOpt;

#endif //__RegOprate_H