#include "stdafx.h"
#include <atlbase.h>
#include <ExDispid.h>
#include "XWebBrowser2.h"
#include <mshtmdid.h>

CXWebBrowser2::CXWebBrowser2(void)
{
}

CXWebBrowser2::~CXWebBrowser2(void)
{
	
}

BOOL CXWebBrowser2::InitInstance(HWND hWnd)
{
	m_Callback2 = NULL;
	if (CXWebBrowser::InitInstance(hWnd))
	{
		GetWebObject()->QueryInterface<IDispatch>(&m_Dispatch);
		return TRUE;
	}
	return FALSE;
}

VOID CXWebBrowser2::UninitInstance()
{
	/*
	MSG m;

	if (!IsCompleted())
	{
		Stop();
		while (!IsCompleted())
		{
			LONGLONG TickCount = GetTickCount64();

			while (PeekMessage(&m, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&m);
				DispatchMessage(&m);
				if (GetTickCount64() - TickCount > 3 * 1000)
					break;
			}
		}
	}
	*/
	if (!IsCompleted())
		Stop();
	m_Dispatch->Release();
	CXWebBrowser::UninitInstance();
}

BOOL CXWebBrowser2::CreateInstance(HWND hWnd, CXWebBrowser2* &wb2)
{
	CXWebBrowser2 *NewObject;

	if (NewObject = new CXWebBrowser2)
	{
		if (NewObject->InitInstance(hWnd))
		{
			wb2 = NewObject;
			return TRUE;
		}
		delete NewObject;
	}
	return FALSE;
}

VOID CXWebBrowser2::DeleteInstance(CXWebBrowser2* &wb2)
{
	if (wb2 != NULL)
	{
		wb2->UninitInstance();
		delete wb2;
		wb2 = NULL;
	}
}

HRESULT STDMETHODCALLTYPE CXWebBrowser2::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
	if (m_Callback2 == NULL)
		return E_NOTIMPL;
	switch (dispIdMember)
	{
	case DISPID_BEFORENAVIGATE2:
		{
			VARIANT *vURL = pDispParams->rgvarg[5].pvarVal;
			VARIANT_BOOL *vb = pDispParams->rgvarg[0].pboolVal;
			IDispatch *Invoker = pDispParams->rgvarg[6].pdispVal;
			BOOL IsCancel = (*vb != VARIANT_FALSE);

			m_Callback2->OnBeforeNavigate((Invoker != m_Dispatch), CComBSTR(vURL->bstrVal), IsCancel);
			*vb = (IsCancel ? VARIANT_TRUE : VARIANT_FALSE);
		}
		break;
	case DISPID_DOCUMENTCOMPLETE:
		{
			VARIANT *v;
			IDispatch *Invoker;

			v = pDispParams->rgvarg[0].pvarVal;
			Invoker = pDispParams->rgvarg[1].pdispVal;
			if (Invoker == m_Dispatch)
			{
				m_Callback2->OnNavigateComplete(v->bstrVal);
			}
			else
			{
				m_Callback2->OnDocumentComplete(v->bstrVal);
			}
		}
		break;
	case DISPID_NEWWINDOW3:
		{
			BSTR URL = pDispParams->rgvarg[0].bstrVal;
			VARIANT_BOOL *vb = pDispParams->rgvarg[3].pboolVal;
			IDispatch** lppDispatch = pDispParams->rgvarg[4].ppdispVal;

			switch (m_Callback2->OnNewWindow(CComBSTR(URL)))
			{
			case IXWebNotifyCallback2::NW_DEFAULT:
				break;
			case IXWebNotifyCallback2::NW_CANCEL:
				*vb = VARIANT_TRUE;
				break;
			case IXWebNotifyCallback2::NW_LIMIT:
				{
					m_Dispatch->AddRef();
					*lppDispatch = m_Dispatch;
				}
				break;
			default:
				break;
			}
		}
		break;
	case DISPID_AMBIENT_DLCONTROL:
		{
			VARIANT_BOOL b = FALSE;
			GetWebObject()->get_Silent(&b);

			LONG nFlags;

			nFlags = 0;
			if (m_Callback2->OnAmbientDownloadControl(nFlags))
			{
				pVarResult->vt = VT_I4;
				pVarResult->lVal = nFlags|(b?DLCTL_SILENT:0);
			}
		}
		break;
	default:
		return E_NOTIMPL;
	}
	return S_OK;
}

VOID CXWebBrowser2::SetCallback2(IXWebNotifyCallback2 *cb)
{
	m_Callback2 = cb;
}

BOOL CXWebBrowser2::Silent(BOOL Enabled)
{
	return SUCCEEDED(GetWebObject()->put_Silent(Enabled ? VARIANT_TRUE : VARIANT_FALSE));
}

BOOL CXWebBrowser2::Navigate(WCHAR *Url)
{
	return SUCCEEDED(GetWebObject()->Navigate2(&CComVariant(CComBSTR(Url)), 
		&CComVariant(navNoReadFromCache|navNoWriteToCache), NULL, NULL, NULL));
}

BOOL CXWebBrowser2::Navigate(UINT nRes)
{
	WCHAR szFile[MAX_PATH];
	WCHAR NavURL[MAX_PATH];

	GetModuleFileNameW(NULL, szFile, _countof(szFile));
	wsprintfW(NavURL, L"res://%s/%d", szFile, nRes);
	return Navigate(NavURL);
}

BOOL CXWebBrowser2::Stop()
{
	return SUCCEEDED(GetWebObject()->Stop());
}

BOOL CXWebBrowser2::Refresh()
{
	return SUCCEEDED(GetWebObject()->Refresh());
}

BOOL CXWebBrowser2::GoBack()
{
	return SUCCEEDED(GetWebObject()->GoBack());
}

BOOL CXWebBrowser2::GoForward()
{
	return SUCCEEDED(GetWebObject()->GoForward());
}

BOOL CXWebBrowser2::IsIdle()
{
	BOOL IsOk;
	VARIANT_BOOL b;

	IsOk = FALSE;
	if (SUCCEEDED(GetWebObject()->get_Busy(&b)))
	{
		IsOk = (b == 0);
	}
	return IsOk;
}

BOOL CXWebBrowser2::IsCompleted()
{
	BOOL IsOk;
	READYSTATE rs;

	IsOk = FALSE;
	if (SUCCEEDED(GetWebObject()->get_ReadyState(&rs)))
	{
		IsOk = (rs == READYSTATE_COMPLETE);
	}
	return IsOk;
}

BOOL CXWebBrowser2::RefHtmlDocument(IHTMLDocument* &HtmlDoc)
{
	BOOL IsOk;
	IDispatch *Dispatch;

	IsOk = FALSE;
	if (SUCCEEDED(GetWebObject()->get_Document(&Dispatch)) && (Dispatch != NULL))
	{
		IsOk = SUCCEEDED(Dispatch->QueryInterface(IID_IHTMLDocument, (PVOID*)&HtmlDoc));
		Dispatch->Release();
	}
	return IsOk;
}

BOOL CXWebBrowser2::RefHtmlDocument2(IHTMLDocument2* &HtmlDoc2)
{
	BOOL IsOk;
	IHTMLDocument* HtmlDoc;

	IsOk = FALSE;
	if (RefHtmlDocument(HtmlDoc))
	{
		IsOk = SUCCEEDED(HtmlDoc->QueryInterface(IID_IHTMLDocument2, (PVOID*)&HtmlDoc2));
		HtmlDoc->Release();
	}
	return IsOk;
}

BOOL CXWebBrowser2::RefHtmlDocument3(IHTMLDocument3* &HtmlDoc3)
{
	BOOL IsOk;
	IHTMLDocument* HtmlDoc;

	IsOk = FALSE;
	if (RefHtmlDocument(HtmlDoc))
	{
		IsOk = SUCCEEDED(HtmlDoc->QueryInterface(IID_IHTMLDocument3, (PVOID*)&HtmlDoc3));
		HtmlDoc->Release();
	}
	return IsOk;
}

BOOL CXWebBrowser2::CompareVariant(CONST VARIANT &v1, CONST VARIANT &v2)
{
	BOOL IsEqualVariant;

	IsEqualVariant = FALSE;
	if (v1.vt == v2.vt)
	{
		switch (v1.vt)
		{
		case VT_BSTR:
			{
				if ((v1.bstrVal != NULL) && (v2.bstrVal != NULL))
				{
					IsEqualVariant = (lstrcmpiW(v1.bstrVal, v2.bstrVal) == 0);
				}
			}
			break;
		case VT_I4:
			IsEqualVariant = (v1.lVal == v2.lVal);
			break;
		default:
			{
#ifdef _DEBUG
				DebugBreak();
#else
				OutputDebugString(TEXT("Unknown Type Compared.\r\n"));
#endif
			}
			break;
		}
	}
	return IsEqualVariant;
}

VOID CXWebBrowser2::DerefAllElementsFromList(XLIST_ELEMENTS &EleList)
{
	while (!EleList.empty())
	{
		IDispatch *Item;

		Item = EleList.back();
		EleList.pop_back();

		Item->Release();
	}
}

VOID CXWebBrowser2::RefAllElementsToList(XLIST_ELEMENTS &EleList)
{
	IHTMLDocument2 *HtmlDoc2;

	if (RefHtmlDocument2(HtmlDoc2))
	{
		IHTMLElementCollection *Elements;

		if (SUCCEEDED(HtmlDoc2->get_all(&Elements)))
		{
			RefElementsToList(EleList, Elements);
			Elements->Release();
		}
		HtmlDoc2->Release();
	}
}

VOID CXWebBrowser2::RefElementsToList(XLIST_ELEMENTS &EleList, IHTMLElementCollection *Elements)
{
	LONG nItems;

	if (SUCCEEDED(Elements->get_length(&nItems)))
	{
		for (LONG i = 0; i < nItems; i++)
		{
			IDispatch *Item;

			if (SUCCEEDED(Elements->item(CComVariant(i), CComVariant(), &Item)))
			{
				IHTMLFrameBase2 *HtmlFrame;

				Item->AddRef();
				EleList.push_back(Item);
				// 是不是iframe?
				if (SUCCEEDED(Item->QueryInterface(IID_IHTMLFrameBase2, (void**)&HtmlFrame)))
				{
					IHTMLWindow2 *HtmlFrameWindow;

					if (SUCCEEDED(HtmlFrame->get_contentWindow(&HtmlFrameWindow)))
					{
						IHTMLDocument2 *HtmlFrameDoc2;

						if (SUCCEEDED(HtmlFrameWindow->get_document(&HtmlFrameDoc2)))
						{
							IHTMLElementCollection *HtmlFrameElements;

							if (SUCCEEDED(HtmlFrameDoc2->get_all(&HtmlFrameElements)))
							{
								RefElementsToList(EleList, HtmlFrameElements);
								HtmlFrameElements->Release();
							}
							HtmlFrameDoc2->Release();
						}
						HtmlFrameWindow->Release();
					}
					HtmlFrame->Release();
				}
				Item->Release();
			}
		}
	}
}

VOID CXWebBrowser2::RefAllScriptToList(XLIST_ELEMENTS &EleList)
{
	IHTMLDocument2 *HtmlDoc2;

	if (RefHtmlDocument2(HtmlDoc2))
	{
		IDispatch *ScriptObject;

		if (SUCCEEDED(HtmlDoc2->get_Script(&ScriptObject)))
		{
			ScriptObject->AddRef();
			EleList.push_back(ScriptObject);
			
			ScriptObject->Release();
		}
		IHTMLElementCollection *Elements;

		if (SUCCEEDED(HtmlDoc2->get_all(&Elements)))
		{
			RefScriptToList(EleList, Elements);
			Elements->Release();
		}
		HtmlDoc2->Release();
	}
}

VOID CXWebBrowser2::RefScriptToList(XLIST_ELEMENTS &EleList, IHTMLElementCollection *Elements)
{
	LONG nItems;

	if (SUCCEEDED(Elements->get_length(&nItems)))
	{
		for (LONG i = 0; i < nItems; i++)
		{
			IDispatch *Item;

			if (SUCCEEDED(Elements->item(CComVariant(i), CComVariant(), &Item)))
			{
				IHTMLFrameBase2 *HtmlFrame;

				// 是不是iframe?
				if (SUCCEEDED(Item->QueryInterface(IID_IHTMLFrameBase2, (void**)&HtmlFrame)))
				{
					IHTMLWindow2 *HtmlFrameWindow;

					if (SUCCEEDED(HtmlFrame->get_contentWindow(&HtmlFrameWindow)))
					{
						IHTMLDocument2 *HtmlFrameDoc2;

						if (SUCCEEDED(HtmlFrameWindow->get_document(&HtmlFrameDoc2)))
						{
							IDispatch *FrameScriptObject;

							if (SUCCEEDED(HtmlFrameDoc2->get_Script(&FrameScriptObject)))
							{
								FrameScriptObject->AddRef();
								EleList.push_back(FrameScriptObject);

								FrameScriptObject->Release();
							}
							IHTMLElementCollection *HtmlFrameElements;

							if (SUCCEEDED(HtmlFrameDoc2->get_all(&HtmlFrameElements)))
							{
								RefScriptToList(EleList, HtmlFrameElements);
								HtmlFrameElements->Release();
							}
							HtmlFrameDoc2->Release();
						}
						HtmlFrameWindow->Release();
					}
					HtmlFrame->Release();
				}
				Item->Release();
			}
		}
	}
}

BOOL CXWebBrowser2::GetElementValueEx(IDispatch *Item, LPCWSTR Name, VARIANT &Value)
{
	HRESULT hr;
	LCID Lcid;
	DISPID DispId;

	hr = E_FAIL;
	Lcid = GetUserDefaultLCID();
	if (SUCCEEDED(Item->GetIDsOfNames(IID_NULL, &CComBSTR(Name), 1, Lcid, &DispId)))
	{
		DISPPARAMS DispParams = {NULL, NULL, 0, 0};

		hr = Item->Invoke(DispId, 
			IID_NULL, 
			Lcid, 
			DISPATCH_PROPERTYGET,
			&DispParams, 
			&Value, NULL, NULL);
	}
	return SUCCEEDED(hr);
}

BOOL CXWebBrowser2::SetElementValueEx(IDispatch *Item, LPCWSTR Name, VARIANT &Value)
{
	HRESULT hr;
	LCID Lcid;
	DISPID DispId;

	hr = E_FAIL;
	Lcid = GetUserDefaultLCID();
	if (SUCCEEDED(Item->GetIDsOfNames(IID_NULL, &CComBSTR(Name), 1, Lcid, &DispId)))
	{
		DISPPARAMS DispParams;
		DISPID DispIdNamedArgs;

		DispIdNamedArgs = DISPID_PROPERTYPUT;
		DispParams.cArgs = 1;
		DispParams.cNamedArgs = 1;
		DispParams.rgdispidNamedArgs = &DispIdNamedArgs;
		DispParams.rgvarg = &Value;
		hr = Item->Invoke(DispId, 
			IID_NULL, 
			Lcid, 
			DISPATCH_PROPERTYPUT,
			&DispParams, 
			&CComVariant(), NULL, NULL);
	}
	return SUCCEEDED(hr);
}

VOID CXWebBrowser2::BuildElementListByFilterEx(CONST XLIST_ELEMENTS &From, XLIST_ELEMENTS &To, LPCWSTR PropName, CONST VARIANT &PropValue)
{
	for (size_t i = 0; i < From.size(); i++)
	{
		CComPtr<IDispatch> Item(From[i]);
		CComVariant v;

		if (GetElementValueEx(Item, PropName, v) && CompareVariant(PropValue, v))
		{
			IDispatch *t = From[i];

			t->AddRef();
			To.push_back(t);
			t = NULL;
		}
	}
}

VOID CXWebBrowser2::BuildElementListByFilter(XLIST_ELEMENTS &EleList, LPCWSTR PropName, CONST VARIANT &PropValue)
{
	XLIST_ELEMENTS AllItems;

	RefAllElementsToList(AllItems);
	BuildElementListByFilterEx(AllItems, EleList, PropName, PropValue);
	DerefAllElementsFromList(AllItems);
}

BOOL CXWebBrowser2::IsElementExistEx(LPCWSTR tagName, LPCWSTR PropName, CONST VARIANT &PropValue)
{
	LONG nCount;
	XLIST_ELEMENTS xle1, xle2;

	nCount = 0;
	BuildElementListByFilter(xle1, L"tagName", CComVariant(CComBSTR(tagName)));
	BuildElementListByFilterEx(xle1, xle2, PropName, PropValue);
	for (size_t i = 0; i < xle2.size(); i++)
		nCount++;
	DerefAllElementsFromList(xle2);
	DerefAllElementsFromList(xle1);
	return (nCount > 0);
}

BOOL CXWebBrowser2::ClickElementEx(LPCWSTR tagName, LPCWSTR PropName, CONST VARIANT &PropValue)
{
	BOOL IsOk;
	XLIST_ELEMENTS xle1, xle2;

	IsOk = FALSE;
	BuildElementListByFilter(xle1, L"tagName", CComVariant(CComBSTR(tagName)));
	BuildElementListByFilterEx(xle1, xle2, PropName, PropValue);
	for (size_t i = 0; i < xle2.size(); i++)
	{
		CComPtr<IDispatch> Item(xle2[i]);
		CComPtr<IHTMLElement> e;

		if (SUCCEEDED(Item->QueryInterface<IHTMLElement>(&e)))
		{
			if (SUCCEEDED(e->click()))
			{
				IsOk = TRUE;
				break;
			}
		}
	}
	DerefAllElementsFromList(xle2);
	DerefAllElementsFromList(xle1);
	return IsOk;
}

BOOL CXWebBrowser2::SetElementTextEx(LPCWSTR tagName, LPCWSTR PropName, CONST VARIANT &PropValue, LPCWSTR NewText)
{
	LONG nCount;
	XLIST_ELEMENTS xle1, xle2;

	nCount = 0;
	BuildElementListByFilter(xle1, L"tagName", CComVariant(CComBSTR(tagName)));
	BuildElementListByFilterEx(xle1, xle2, PropName, PropValue);
	for (size_t i = 0; i < xle2.size(); i++)
	{
		if (SetElementValueEx(xle2[i], L"value", CComVariant(CComBSTR(NewText))))
			nCount++;
	}
	DerefAllElementsFromList(xle2);
	DerefAllElementsFromList(xle1);
	return (nCount > 0);
}

BOOL CXWebBrowser2::InvokeScript(LPCWSTR FuncName)
{
	BOOL IsOk;
	XLIST_ELEMENTS EleList;

	IsOk = FALSE;
	RefAllScriptToList(EleList);
	for (size_t i = 0; i < EleList.size(); i++)
	{
		CComDispatchDriver cdd(EleList[i]);

		if (SUCCEEDED(cdd.Invoke0(CComBSTR(FuncName))))
		{
			IsOk = TRUE;
			break;
		}
	}
	DerefAllElementsFromList(EleList);
	return IsOk;
}

