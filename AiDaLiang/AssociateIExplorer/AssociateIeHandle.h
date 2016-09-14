#ifndef __AssociateIeHandle_H
#define __AssociateIeHandle_H
#include <ExDisp.h>

class AssociateIeHandle : public IDispatch
{
public:
	AssociateIeHandle();
	AssociateIeHandle(IWebBrowser2* pIe);
	virtual ~AssociateIeHandle();
public:
	void AssociateIePtr(IWebBrowser2* pIe);
	BOOL StartService();
	void StopService();

	IWebBrowser2* GetIEFromHWnd(HWND hIEParantWindow,LPCSTR szClassWndName=NULL);
	//virtual void ProcessHtml();
	//virtual void ProcessHtmlElem();

	//ÊÂ¼þº¯Êý
	void __stdcall BeforeNavigate2(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers, VARIANT_BOOL* Cancel);
	void __stdcall DocumentComplete(IDispatch *pDisp, VARIANT &URL);
	void __stdcall DownloadBegin();
	void __stdcall DownloadComplete();
	void __stdcall ProgressChange(int Progress, int ProgressMax);
	void __stdcall NewWindow3(IDispatch *pDisp,VARIANT_BOOL* Cancel,LONG dwFlags ,BSTR bstrUrlContext ,BSTR bstrUrl );
public:
	// IUnknown
	STDMETHOD(QueryInterface)(REFIID riid, void **ppv);
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();

	// IDispatch
	STDMETHOD(GetTypeInfoCount)(UINT * pit);
	STDMETHOD(GetTypeInfo)(UINT it,LCID lcid,ITypeInfo **ppti);
	STDMETHOD(GetIDsOfNames)(REFIID riid,
		OLECHAR ** pNames,
		UINT nNames,
		LCID lcid,
		DISPID * pdispids);

	STDMETHOD(Invoke)(DISPID id,
		REFIID riid,
		LCID lcid,
		WORD wFlags,
		DISPPARAMS *pd,
		VARIANT * pVarResult,
		EXCEPINFO * pe,
		UINT *pu);
protected:
	IWebBrowser2* mAssociateIePtr;
	IConnectionPoint* mIeConnectionPoint;
	DWORD mCookie;
private:
private:
	LONG mRef;
};

extern void TestIe();
#endif //__AssociateIeHandle_H