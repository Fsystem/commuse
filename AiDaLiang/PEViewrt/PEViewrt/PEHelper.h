#ifndef __PEHelper_H
#define __PEHelper_H

class PEHelper
{
public:
	PEHelper();
public:
	HWND m_tree;
	WCHAR m_AgentId[64];
public:
	BOOL LoadPeFile(LPCTSTR lpszFileName, LPTSTR errormsg);
	void LoadResTable(LPBYTE lpBaseAddress, PIMAGE_NT_HEADERS pNtHeaders, DWORD rva);
	HTREEITEM AddChildNode(HTREEITEM hParent, LPVOID lpBaseAddress, 
		PIMAGE_NT_HEADERS pNtHeaders, DWORD tableAddress, 
		PIMAGE_RESOURCE_DIRECTORY_ENTRY pEntry, int depth,int nResType);
protected:
private:
	
};

#endif //__PEHelper_H