#pragma once  
#include "afxocc.h"  
#include "Mshtml.h"//应该加入这个头文件  
#include "Mshtmhst.h"//这个也是  
class CMyControlSite :public COleControlSite  
{  
public:  
	CMyControlSite(COleControlContainer *pCntr):COleControlSite(pCntr) {}  
	~CMyControlSite(void);  
protected:  
	DECLARE_INTERFACE_MAP()    
	BEGIN_INTERFACE_PART(OleCommandTarget, IOleCommandTarget)    
		STDMETHOD(QueryStatus)(const GUID *pguidCmdGroup, ULONG cCmds, OLECMD prgCmds[], OLECMDTEXT *pCmdText);    
		STDMETHOD(Exec)(const GUID* pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt, VARIANTARG* pvaIn, VARIANTARG* pvaOut);    
	END_INTERFACE_PART(OleCommandTarget)    
};  