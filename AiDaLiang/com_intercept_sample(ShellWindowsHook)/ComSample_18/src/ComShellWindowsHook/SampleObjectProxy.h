#pragma once


class ATL_NO_VTABLE CSampleObjectProxy :
    public ATL::CComObjectRootEx<ATL::CComMultiThreadModel>,
	public ATL::CComCoClass<CSampleObjectProxy, &CLSID_ShellWindows>,
	public ATL::IDispatchImpl<IShellWindows, &IID_IShellWindows>
{
public:
    CSampleObjectProxy();

    DECLARE_NO_REGISTRY()

    BEGIN_COM_MAP(CSampleObjectProxy)
	    COM_INTERFACE_ENTRY(IShellWindows)
	    COM_INTERFACE_ENTRY(IDispatch)
    END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

public:
	virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
		/* [retval][out] */ __RPC__out long *Count) {return m_OriginalObject->get_Count(Count);};

	virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Item( 
		/* [optional][in] */ VARIANT index,
		/* [retval][out] */ __RPC__deref_out_opt IDispatch **Folder){
			return m_OriginalObject->Item(index,Folder);
	};

	virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE _NewEnum( 
		/* [retval][out] */ __RPC__deref_out_opt IUnknown **ppunk) {
			return m_OriginalObject->_NewEnum(ppunk);
	}

	virtual /* [hidden][helpstring] */ HRESULT STDMETHODCALLTYPE Register( 
		/* [in] */ __RPC__in_opt IDispatch *pid,
		/* [in] */ long hwnd,
		/* [in] */ int swClass,
		/* [out] */ __RPC__out long *plCookie) 
	{
		return m_OriginalObject->Register(pid,hwnd,swClass,plCookie);
	}

	virtual /* [hidden][helpstring] */ HRESULT STDMETHODCALLTYPE RegisterPending( 
		/* [in] */ long lThreadId,
		/* [in] */ __RPC__in VARIANT *pvarloc,
		/* [in] */ __RPC__in VARIANT *pvarlocRoot,
		/* [in] */ int swClass,
		/* [out] */ __RPC__out long *plCookie) 
	{
		return m_OriginalObject->RegisterPending(lThreadId,pvarloc,pvarlocRoot,swClass,plCookie);
	}

	virtual /* [hidden][helpstring] */ HRESULT STDMETHODCALLTYPE Revoke( 
		/* [in] */ long lCookie) 
	{
		return m_OriginalObject->Revoke(lCookie);
	}

	virtual /* [hidden][helpstring] */ HRESULT STDMETHODCALLTYPE OnNavigate( 
		/* [in] */ long lCookie,
		/* [in] */ __RPC__in VARIANT *pvarLoc) 
	{
		return m_OriginalObject->OnNavigate(lCookie,pvarLoc);
	}

	virtual /* [hidden][helpstring] */ HRESULT STDMETHODCALLTYPE OnActivated( 
		/* [in] */ long lCookie,
		/* [in] */ VARIANT_BOOL fActive)
	{
		return m_OriginalObject->OnActivated(lCookie,fActive);
	}

	virtual /* [hidden][helpstring] */ HRESULT STDMETHODCALLTYPE FindWindowSW( 
		/* [in] */ __RPC__in VARIANT *pvarLoc,
		/* [in] */ __RPC__in VARIANT *pvarLocRoot,
		/* [in] */ int swClass,
		/* [out] */ __RPC__out long *phwnd,
		/* [in] */ int swfwOptions,
		/* [retval][out] */ __RPC__deref_out_opt IDispatch **ppdispOut) 
	{
		return m_OriginalObject->FindWindowSW(pvarLoc,pvarLocRoot,swClass,phwnd,swfwOptions,ppdispOut);
	}

	virtual /* [hidden][helpstring] */ HRESULT STDMETHODCALLTYPE OnCreated( 
		/* [in] */ long lCookie,
		/* [in] */ __RPC__in_opt IUnknown *punk) 
	{
		return m_OriginalObject->OnCreated(lCookie,punk);
	}

	virtual /* [hidden][helpstring] */ HRESULT STDMETHODCALLTYPE ProcessAttachDetach( 
		/* [in] */ VARIANT_BOOL fAttach) 
	{
		return m_OriginalObject->ProcessAttachDetach(fAttach);
	}

public:
    HRESULT static CreateInstance(IUnknown* original, REFIID riid, void **ppvObject);

public:
    //STDMETHOD(get_ObjectName)(BSTR* pVal);
    //STDMETHOD(put_ObjectName)(BSTR newVal);
    //STDMETHOD(DoWork)(LONG arg1, LONG arg2, LONG* result);

private:
    ATL::CComPtr<IShellWindows> m_OriginalObject;
    ATL::CComBSTR m_Name;
};
