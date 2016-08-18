#include "stdafx.h"
#include "MyControlSite.h"

BEGIN_INTERFACE_MAP(CMyControlSite, COleControlSite)    
	INTERFACE_PART(CMyControlSite, IID_IOleCommandTarget, OleCommandTarget)  
END_INTERFACE_MAP()    



CMyControlSite::~CMyControlSite(void)  
{  
}  

HRESULT CMyControlSite::XOleCommandTarget::Exec    
	(const GUID* pguidCmdGroup, DWORD nCmdID,    
	DWORD nCmdexecopt, VARIANTARG* pvaIn, VARIANTARG* pvaOut )    
{    
	HRESULT hr = OLECMDERR_E_NOTSUPPORTED;    
	//return S_OK;    
	if (pguidCmdGroup && IsEqualGUID(*pguidCmdGroup, CGID_DocHostCommandHandler))    
	{    

		switch (nCmdID)     
		{    

		case OLECMDID_SHOWSCRIPTERROR:    
			{    
				IHTMLDocument2*             pDoc = NULL;    
				IHTMLWindow2*               pWindow = NULL;    
				IHTMLEventObj*              pEventObj = NULL;    
				BSTR                        rgwszNames[5] =     
				{     
					SysAllocString(L"errLine"),    
					SysAllocString(L"errCharacter"),    
					SysAllocString(L"errCode"),    
					SysAllocString(L"errMsg"),    
					SysAllocString(L"errUrl")    
				};    
				DISPID                      rgDispIDs[5];    
				VARIANT                     rgvaEventInfo[5];    
				DISPPARAMS                  params;    
				BOOL                        fContinueRunningScripts = false;  //修改此处为false禁止脚本错误提示  

				params.cArgs = 0;    
				params.cNamedArgs = 0;    

				hr = pvaIn->punkVal->QueryInterface(IID_IHTMLDocument2, (void **) &pDoc);        

				hr = pDoc->get_parentWindow(&pWindow);    
				pDoc->Release();    

				hr = pWindow->get_event(&pEventObj);    

				for (int i = 0; i < 5; i++)     
				{      

					hr = pEventObj->GetIDsOfNames(IID_NULL, &rgwszNames[i], 1,     
						LOCALE_SYSTEM_DEFAULT, &rgDispIDs[i]);    

					hr = pEventObj->Invoke(rgDispIDs[i], IID_NULL,    
						LOCALE_SYSTEM_DEFAULT,    
						DISPATCH_PROPERTYGET, &params, &rgvaEventInfo[i],    
						NULL, NULL);    
					//可以在此记录错误信息                    //必须使用SysFreeString来释放SysAllocString分配的内存,SysAllocString在分配的内存中记录了字符的长度  
					SysFreeString(rgwszNames[i]);    
				}    

				// At this point, you would normally alert the user with     
				// the information about the error, which is now contained    
				// in rgvaEventInfo[]. Or, you could just exit silently.    

				(*pvaOut).vt = VT_BOOL;    
				if (fContinueRunningScripts)    
				{    
					// 在页面中继续执行脚本   
					(*pvaOut).boolVal = VARIANT_TRUE;    
				}    
				else   
				{    
					// 停止在页面中执行脚本    
					(*pvaOut).boolVal = VARIANT_FALSE;       
				}     
				break;    
			}    
		default:    
			hr =OLECMDERR_E_NOTSUPPORTED;   
			break;    
		}    
	}    
	else   
	{    
		hr = OLECMDERR_E_UNKNOWNGROUP;  
	}    
	return (hr);    
}    


ULONG FAR EXPORT CMyControlSite::XOleCommandTarget::AddRef()     
{     
	METHOD_PROLOGUE(CMyControlSite, OleCommandTarget)     
		return pThis->ExternalAddRef();     
}     


ULONG FAR EXPORT CMyControlSite::XOleCommandTarget::Release()     
{     
	METHOD_PROLOGUE(CMyControlSite, OleCommandTarget)     
		return pThis->ExternalRelease();     
}     

HRESULT FAR EXPORT CMyControlSite::XOleCommandTarget::QueryInterface(REFIID riid, void **ppvObj)     
{     
	METHOD_PROLOGUE(CMyControlSite, OleCommandTarget)     
		HRESULT hr = (HRESULT)pThis->ExternalQueryInterface(&riid, ppvObj);     
	return hr;     
}    

STDMETHODIMP CMyControlSite::XOleCommandTarget::QueryStatus(     
	/* [unique][in] */ const GUID __RPC_FAR *pguidCmdGroup,     
	/* [in] */ ULONG cCmds,     
	/* [out][in][size_is] */ OLECMD __RPC_FAR prgCmds[ ],     
	/* [unique][out][in] */ OLECMDTEXT __RPC_FAR *pCmdText     
	)     
{     
	METHOD_PROLOGUE(CMyControlSite, OleCommandTarget)     
		return OLECMDERR_E_NOTSUPPORTED;     
}  