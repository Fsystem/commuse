#pragma once  
#include "afxocc.h"  
#include "Mshtml.h"//Ӧ�ü������ͷ�ļ�  
#include "Mshtmhst.h"//���Ҳ��  
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