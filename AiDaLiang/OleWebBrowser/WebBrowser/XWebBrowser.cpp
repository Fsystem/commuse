#include "stdafx.h"
#include "XWebBrowser.h"
#include <Psapi.h>
#include <MsHTML.h>
#pragma comment(lib, "psapi.lib")

CXWebBrowser::CXWebBrowser()
{
}

CXWebBrowser::~CXWebBrowser(void)
{
	
}

BOOL CXWebBrowser::SetIEVersion(IE_VERSION v)
{
	BOOL IsOk;
	HKEY KeyHandle;
	TCHAR BaseName[64];

	IsOk = FALSE;
	GetModuleBaseName(GetCurrentProcess(), NULL, BaseName, _countof(BaseName));
	if (RegCreateKeyEx(HKEY_CURRENT_USER, 
		TEXT("Software\\Microsoft\\Internet Explorer\\Main\\FeatureControl\\FEATURE_BROWSER_EMULATION"), 
		0, NULL, REG_OPTION_VOLATILE, KEY_WRITE, NULL, &KeyHandle, NULL) == ERROR_SUCCESS)
	{
		DWORD dwValue;

		switch (v)
		{
		case IE7:
			dwValue = 7000;
			break;
		case IE8:
			dwValue = 8888;
			break;
		case IE9:
			dwValue = 9999;
			break;
		case IE10:
			dwValue = 10001;
			break;
		case IE11:
			dwValue = 11001;
			break;
		default:
			dwValue = 0;
			break;
		}
		if (dwValue)
			IsOk = (RegSetValueEx(KeyHandle, BaseName, 0, REG_DWORD, reinterpret_cast<LPBYTE>(&dwValue), sizeof(dwValue)) == ERROR_SUCCESS);
		RegCloseKey(KeyHandle);
	}
	return IsOk;
}

BOOL CXWebBrowser::InitInstance(HWND hWnd)
{
	if (hWnd && IsWindow(hWnd) && (OleInitialize(NULL) == S_OK))
	{
		m_RefCount = 0;
		m_Container = hWnd;
		if (SUCCEEDED(StgCreateDocfile(NULL, STGM_READWRITE|STGM_SHARE_EXCLUSIVE|STGM_DIRECT|STGM_CREATE, 0, &m_StorageObject)))
		{
			if (SUCCEEDED(OleCreate(CLSID_WebBrowser, IID_IOleObject, OLERENDER_DRAW, NULL, this, m_StorageObject, reinterpret_cast<LPVOID*>(&m_OleObject))))
			{
				OleSetContainedObject(m_OleObject, FALSE);
				if (SUCCEEDED(m_OleObject->QueryInterface(IID_IOleInPlaceObject, reinterpret_cast<LPVOID*>(&m_OleInPlaceObject))))
				{
					if (SUCCEEDED(m_OleObject->QueryInterface(IID_IWebBrowser2, (void**)&m_WebObject)))
					{
						RECT Rect;

						GetClientRect(m_Container, &Rect);
						if (SUCCEEDED(m_OleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, this, 0, m_Container, &Rect)))
						{
							if (SUCCEEDED(m_WebObject->QueryInterface(IID_IConnectionPointContainer, reinterpret_cast<LPVOID*>(&m_ConnectionPointContainer))))
							{
								if (SUCCEEDED(m_ConnectionPointContainer->FindConnectionPoint(DIID_DWebBrowserEvents2, &m_ConnectionPoint)))
								{
									if (SUCCEEDED(m_ConnectionPoint->Advise(reinterpret_cast<IUnknown*>(this), &m_dwCookie)))
									{
										return TRUE;
									}
									m_ConnectionPoint->Release();
								}
								m_ConnectionPointContainer->Release();
							}
						}
						m_WebObject->Release();
					}
					m_OleInPlaceObject->Release();
				}
				m_OleObject->SetClientSite(NULL);
				m_OleObject->Close(OLECLOSE_NOSAVE);
				CoDisconnectObject(m_OleObject, 0);
				m_OleObject->Release();
			}
			m_StorageObject->Release();
		}
	}
	return FALSE;
}

VOID CXWebBrowser::UninitInstance()
{
	// now it is must be idle
	// else will be crash
	m_ConnectionPoint->Unadvise(m_dwCookie);
	m_ConnectionPoint->Release();
	m_ConnectionPointContainer->Release();

	m_WebObject->ExecWB(OLECMDID_CLOSE, OLECMDEXECOPT_DONTPROMPTUSER, NULL, NULL);
	m_WebObject->Release();
	m_OleInPlaceObject->Release();

	m_OleObject->SetClientSite(NULL);
	m_OleObject->Close(OLECLOSE_NOSAVE);
	CoDisconnectObject(m_OleObject, 0);
	m_OleObject->Release();

	m_StorageObject->Release();
	OleUninitialize();
}

IWebBrowser2* CXWebBrowser::GetWebObject()
{
	return m_WebObject;
}

HRESULT CXWebBrowser::SetBrowserRect(LPRECT rc)
{
	return m_OleInPlaceObject->SetObjectRects(rc, rc);
}

//////////////////////////////////////////////////////////////////////////
// IUnKnown
ULONG STDMETHODCALLTYPE CXWebBrowser::AddRef(void)
{
	return InterlockedIncrement(&m_RefCount);
}

ULONG STDMETHODCALLTYPE CXWebBrowser::Release(void)
{
	return InterlockedDecrement(&m_RefCount);
}

HRESULT STDMETHODCALLTYPE CXWebBrowser::QueryInterface(REFIID riid, void **ppvObject)
{
	HRESULT hr;

	hr = E_NOINTERFACE;
	if (riid == IID_IUnknown)
	{
		*ppvObject = dynamic_cast<LPVOID>(this);
		hr = S_OK;
	}
	else if (riid == IID_IDispatch)
	{
		*ppvObject = dynamic_cast<IDispatch*>(this);
		hr = S_OK;
	}
	else if (riid == IID_IOleClientSite)
	{
		*ppvObject = dynamic_cast<IOleClientSite*>(this);
		hr = S_OK;
	}
	else if (riid == IID_IOleInPlaceSite)
	{
		*ppvObject = dynamic_cast<IOleInPlaceSite*>(this);
		hr = S_OK;
	}
	else if (riid == IID_IOleInPlaceUIWindow)
	{
		*ppvObject = dynamic_cast<IOleInPlaceUIWindow*>(this);
		hr = S_OK;
	}
	else if (riid == IID_IOleInPlaceFrame)
	{
		*ppvObject = dynamic_cast<IOleInPlaceFrame*>(this);
		hr = S_OK;
	}
	else if (riid == IID_IOleCommandTarget)
	{
		*ppvObject = dynamic_cast<IOleCommandTarget*>(this);
	}
	else if (riid == DIID_DWebBrowserEvents2)
	{
		*ppvObject = dynamic_cast<IDispatch*>(this);
//		*ppvObject = dynamic_cast<DWebBrowserEvents2*>(this);
		hr = S_OK;
	}
	else if (riid == IID_IDocHostUIHandler)
	{
		*ppvObject = dynamic_cast<IDocHostUIHandler*>(this);
		hr = S_OK;
	}
	if (SUCCEEDED(hr))
		AddRef();
	return hr;
}

//////////////////////////////////////////////////////////////////////////
// IDispatch
HRESULT STDMETHODCALLTYPE CXWebBrowser::GetTypeInfoCount(UINT *pctinfo)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CXWebBrowser::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CXWebBrowser::GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CXWebBrowser::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// IOleClientSite
HRESULT STDMETHODCALLTYPE CXWebBrowser::SaveObject(void)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CXWebBrowser::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CXWebBrowser::GetContainer(IOleContainer **ppContainer)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CXWebBrowser::ShowObject(void)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CXWebBrowser::OnShowWindow(BOOL fShow)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CXWebBrowser::RequestNewObjectLayout(void)
{
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// IOleWindow
HRESULT STDMETHODCALLTYPE CXWebBrowser::GetWindow(HWND *phwnd)
{
	if (m_Container)
	{
		*phwnd = m_Container;
		return S_OK;
	}
	else
	{
		return E_FAIL;
	}
}

HRESULT STDMETHODCALLTYPE CXWebBrowser::ContextSensitiveHelp(BOOL fEnterMode)
{
	return E_NOTIMPL;
}

//////////////////////////////////////////////////////////////////////////
// IOleInPlaceSite
HRESULT STDMETHODCALLTYPE CXWebBrowser::CanInPlaceActivate(void)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CXWebBrowser::OnInPlaceActivate(void)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CXWebBrowser::OnUIActivate(void)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CXWebBrowser::GetWindowContext(IOleInPlaceFrame **ppFrame, IOleInPlaceUIWindow **ppDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
	AddRef();
	*ppFrame = dynamic_cast<IOleInPlaceFrame*>(this);

	AddRef();
	*ppDoc = dynamic_cast<IOleInPlaceUIWindow*>(this);

	GetClientRect(m_Container, lprcPosRect);
	GetClientRect(m_Container, lprcClipRect);
	lpFrameInfo->cb = sizeof(OLEINPLACEFRAMEINFO);
	lpFrameInfo->hwndFrame = m_Container;
	lpFrameInfo->cAccelEntries = 0;
	lpFrameInfo->fMDIApp = FALSE;
	lpFrameInfo->haccel = NULL;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CXWebBrowser::Scroll(SIZE scrollExtant)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CXWebBrowser::OnUIDeactivate(BOOL fUndoable)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CXWebBrowser::OnInPlaceDeactivate(void)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CXWebBrowser::DiscardUndoState(void)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CXWebBrowser::DeactivateAndUndo(void)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CXWebBrowser::OnPosRectChange(LPCRECT lprcPosRect)
{
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// IOleInPlaceUIWindow
HRESULT STDMETHODCALLTYPE CXWebBrowser::GetBorder(LPRECT lprectBorder)
{
	GetClientRect(m_Container, lprectBorder);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CXWebBrowser::RequestBorderSpace(LPCBORDERWIDTHS pborderwidths)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CXWebBrowser::SetBorderSpace(LPCBORDERWIDTHS pborderwidths)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CXWebBrowser::SetActiveObject(IOleInPlaceActiveObject *pActiveObject,LPCOLESTR pszObjName)
{
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// IOleInPlaceFrame
HRESULT STDMETHODCALLTYPE CXWebBrowser::InsertMenus(HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CXWebBrowser::SetMenu(HMENU hmenuShared, HOLEMENU holemenu, HWND hwndActiveObject)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CXWebBrowser::RemoveMenus(HMENU hmenuShared)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CXWebBrowser::SetStatusText(LPCOLESTR pszStatusText)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CXWebBrowser::EnableModeless(BOOL fEnable)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CXWebBrowser::TranslateAccelerator(LPMSG lpmsg, WORD wID)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CXWebBrowser::QueryStatus( const GUID *pguidCmdGroup,ULONG cCmds,OLECMD prgCmds[ ],OLECMDTEXT *pCmdText )
{
	return OLECMDERR_E_NOTSUPPORTED; 
}

HRESULT STDMETHODCALLTYPE CXWebBrowser::Exec( const GUID *pguidCmdGroup, DWORD nCmdID,DWORD nCmdexecopt,VARIANT *pvaIn,VARIANT *pvaOut )
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
					SysAllocString(L"errorLine"), 
					SysAllocString(L"errorCharacter"), 
					SysAllocString(L"errorCode"), 
					SysAllocString(L"errorMessage"), 
					SysAllocString(L"errorUrl")
				};  
				DISPID                      rgDispIDs[5];  
				VARIANT                     rgvaEventInfo[5];  
				DISPPARAMS                  params;  
				BOOL                        fContinueRunningScripts = true;  

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
					//可以在此记录错误信息

					//必须使用SysFreeString来释放SysAllocString分配的内存,SysAllocString在分配的内存中记录了字符的长度
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

//////////////////////////////////////////////////////////////////////////
// IDocHostUIHandler
HRESULT STDMETHODCALLTYPE CXWebBrowser::ShowContextMenu(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved)
{
//	return S_OK;// 使用自定义菜单
	return E_NOTIMPL;// 使用默认菜单
}

HRESULT STDMETHODCALLTYPE CXWebBrowser::GetHostInfo(DOCHOSTUIINFO *pInfo)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CXWebBrowser::ShowUI(DWORD dwID, IOleInPlaceActiveObject *pActiveObject, IOleCommandTarget *pCommandTarget, IOleInPlaceFrame *pFrame, IOleInPlaceUIWindow *pDoc)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CXWebBrowser::HideUI(void)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CXWebBrowser::UpdateUI(void)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CXWebBrowser::OnDocWindowActivate(BOOL fActivate)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CXWebBrowser::OnFrameWindowActivate(BOOL fActivate)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CXWebBrowser::ResizeBorder(LPCRECT prcBorder, IOleInPlaceUIWindow *pUIWindow, BOOL fRameWindow)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CXWebBrowser::TranslateAccelerator(LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CXWebBrowser::GetOptionKeyPath(LPOLESTR *pchKey, DWORD dw)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CXWebBrowser::GetDropTarget(IDropTarget *pDropTarget, IDropTarget **ppDropTarget)
{
//	return S_OK;// 使用自定义拖拽
	return E_NOTIMPL;// 使用默认拖拽
}

HRESULT STDMETHODCALLTYPE CXWebBrowser::GetExternal(IDispatch **ppDispatch)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CXWebBrowser::TranslateUrl(DWORD dwTranslate, OLECHAR *pchURLIn, OLECHAR **ppchURLOut)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CXWebBrowser::FilterDataObject(IDataObject *pDO, IDataObject **ppDORet)
{
	return E_NOTIMPL;
}

