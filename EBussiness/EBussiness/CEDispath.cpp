#include "stdafx.h"
#include "CEDispath.h"
#include "EBussinessDlg.h"



//js call me(Function name)
CString cszCB_SetLoginState				= _T("SetLoginState");
//Function ID
#define	DISPID_CB_SetLoginState			1

//////////////////////////////////////////////////////////////////////////
// CEDispath
//////////////////////////////////////////////////////////////////////////

CEDispath::CEDispath()
{
	m_cRef = 0;
}

CEDispath::~CEDispath()
{
	
}

STDMETHODIMP CEDispath::QueryInterface(REFIID riid, void** ppv)
{
	*ppv = NULL;

	if(IID_IDispatch == riid)
	{
		*ppv = this;
	}

	if(NULL != *ppv)
	{
		((LPUNKNOWN)*ppv)->AddRef();
		return NOERROR;
	}

	return E_NOINTERFACE;
}


STDMETHODIMP_(ULONG) CEDispath::AddRef(void)
{
	return ++m_cRef;
}

STDMETHODIMP_(ULONG) CEDispath::Release(void)
{
	return --m_cRef;
}

STDMETHODIMP CEDispath::GetTypeInfoCount(UINT* /*pctinfo*/)
{
	return E_NOTIMPL;
}

STDMETHODIMP CEDispath::GetTypeInfo(
									/* [in] */ UINT /*iTInfo*/,
									/* [in] */ LCID /*lcid*/,
									/* [out] */ ITypeInfo** /*ppTInfo*/)
{
	return E_NOTIMPL;
}

STDMETHODIMP CEDispath::GetIDsOfNames(
									  /* [in] */ REFIID riid,
									  /* [size_is][in] */ OLECHAR** rgszNames,
									  /* [in] */ UINT cNames,
									  /* [in] */ LCID lcid,
									  /* [size_is][out] */ DISPID* rgDispId)
{
	HRESULT		hr		= NOERROR;
	UINT		i		= 0;
	CString		cszName	= rgszNames[i];

	for(i=0; i<cNames; ++i)
	{
		if(cszName == cszCB_SetLoginState)
		{
			rgDispId[i] = DISPID_CB_SetLoginState;
		}
		else
		{
			// One or more are unknown so set the return code accordingly
			hr = ResultFromScode(DISP_E_UNKNOWNNAME);
			rgDispId[i] = DISPID_UNKNOWN;
		}
	}
	return hr;
}

STDMETHODIMP CEDispath::Invoke(
							   /* [in] */ DISPID dispIdMember,
							   /* [in] */ REFIID /*riid*/,
							   /* [in] */ LCID /*lcid*/,
							   /* [in] */ WORD wFlags,
							   /* [out][in] */ DISPPARAMS* pDispParams,
							   /* [out] */ VARIANT* pVarResult,
							   /* [out] */ EXCEPINFO* /*pExcepInfo*/,
							   /* [out] */ UINT* puArgErr)
{
	//CMFCHtmlDlg*	pDlg	= (CMFCHtmlDlg*)theApp.m_pMainWnd;
	if(dispIdMember == DISPID_CB_SetLoginState)
	{
		if(wFlags & DISPATCH_PROPERTYGET)
		{
			if(pVarResult != NULL)
			{
				VariantInit(pVarResult);
				V_VT(pVarResult) = VT_BOOL;
				V_BOOL(pVarResult) = true;
			}
		}

		if(wFlags & DISPATCH_METHOD)
		{
			////VariantInit(pVarResult);
			//V_VT(pVarResult) = VT_BOOL;
			//V_BOOL(pVarResult) = true;
			int iArg1= pDispParams->rgvarg[0].intVal;
			((CEBussinessDlg*)AfxGetApp()->m_pMainWnd)->SetLoginState(iArg1);
		}
	}

	return S_OK;
}