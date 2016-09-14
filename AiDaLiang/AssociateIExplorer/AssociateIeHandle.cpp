#include "stdafx.h"
#include "AssociateIeHandle.h"
#include <Windows.h>
#include <TlHelp32.h>
#include <map>

#include <atlbase.h>
#include <ExDispid.h>
#include <comutil.h>
#include <Objbase.h>
#include <process.h>

#include <mshtml.h>
#include <shlguid.h>

#pragma comment(lib,"comsuppw.lib")

AssociateIeHandle::AssociateIeHandle()
{
	mRef = 0;
	mCookie = 0;
	mAssociateIePtr = NULL;
	mIeConnectionPoint = NULL;

	CoInitialize(NULL);
}

AssociateIeHandle::AssociateIeHandle(IWebBrowser2* pIe)
{
	mRef = 0;
	mCookie = 0;
	mAssociateIePtr = pIe;
	mIeConnectionPoint = NULL;

	CoInitialize(NULL);
}

AssociateIeHandle::~AssociateIeHandle()
{
	StopService();
	CoUninitialize();
}

void AssociateIeHandle::AssociateIePtr(IWebBrowser2* pIe)
{
	mAssociateIePtr = pIe;
}

BOOL AssociateIeHandle::StartService()
{
	if(mCookie != 0) return TRUE;

	IConnectionPointContainer* pConnContainer = NULL;
	//获取连接点指针
	HRESULT hr = mAssociateIePtr->QueryInterface(IID_IConnectionPointContainer,(void**)&pConnContainer);
	if (SUCCEEDED(hr))
	{
		//查找事件连接点
		hr = pConnContainer->FindConnectionPoint(DIID_DWebBrowserEvents2, &mIeConnectionPoint);
		if (SUCCEEDED(hr))
		{
			//实现连接点地事件接收
			mIeConnectionPoint->Advise(this,&mCookie);
			pConnContainer->Release();

			CComVariant url = L"http://tv.sohu.com";

			CComVariant var;
			mAssociateIePtr->Navigate2(&url,&var,&var,&var,&var);
			return TRUE;
		}

		pConnContainer->Release();
	}

	return FALSE;
}

void AssociateIeHandle::StopService()
{
	if(mCookie == 0) return;

	mAssociateIePtr = NULL;
	if(mIeConnectionPoint)
	{
		mIeConnectionPoint->Unadvise(mCookie);
	}
	mCookie = 0;
}

void __stdcall AssociateIeHandle::BeforeNavigate2(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers, VARIANT_BOOL* Cancel)
{

}

void __stdcall AssociateIeHandle::DocumentComplete(IDispatch *pDisp, VARIANT &URL)
{
	IWebBrowser2 *po=(IWebBrowser2 *)pDisp;
	//Form1->Memo1->Lines->Add(po->LocationName);
	//Form1->Memo1->Lines->Add(URL.bstrVal);
}

void __stdcall AssociateIeHandle::DownloadBegin()
{
	//
}

void __stdcall AssociateIeHandle::DownloadComplete()
{
	//
}

void __stdcall AssociateIeHandle::ProgressChange(int Progress, int ProgressMax)
{
	//Form1->Memo1->Lines->Add((String)Progress+"  "+(String)ProgressMax);    //TEST
}

void __stdcall AssociateIeHandle::NewWindow3(IDispatch *pDisp,VARIANT_BOOL* Cancel,LONG dwFlags ,BSTR bstrUrlContext ,BSTR bstrUrl )
{

}

//-------------------------------------------------------------------------------
// IUnknown
STDMETHODIMP AssociateIeHandle::QueryInterface(REFIID riid, void **ppv)
{
	if(riid == IID_IDispatch)
		*ppv = static_cast<IDispatch *>(this);
	else if (riid == __uuidof(DWebBrowserEvents2))
	{
		*ppv = static_cast<IDispatch *>(this);
	}
	else if(riid == IID_IUnknown)
		*ppv = static_cast<IUnknown *>(this);
	else {
		*ppv = 0;
		//*ppv = static_cast<IDispatch *>(this);
		//return S_OK;
		return E_NOINTERFACE;
	}

	reinterpret_cast<IUnknown *>(*ppv)->AddRef();
	return S_OK;
}

STDMETHODIMP_(ULONG) AssociateIeHandle::AddRef()
{
	InterlockedIncrement(&mRef);
	return mRef;
}

STDMETHODIMP_(ULONG) AssociateIeHandle::Release()
{
	InterlockedDecrement(&mRef);
	ULONG res = mRef; 
	if(res == 0)
		delete this;
	return res;
}

// IDispatch
//此函数是重点，请结合程序并参考msdn理解其中参数的含义
//  DISPID_BEFORENAVIGATE2
// 	DISPID_COMMANDSTATECHANGE
// 	DISPID_DOCUMENTCOMPLETE
// 	DISPID_DOWNLOADBEGIN
// 	DISPID_DOWNLOADCOMPLETE
// 	DISPID_NAVIGATECOMPLETE2
// 	DISPID_NEWWINDOW2
// 	DISPID_ONFULLSCREEN
// 	DISPID_ONMENUBAR
// 	DISPID_ONQUIT
// 	DISPID_ONSTATUSBAR
// 	DISPID_ONTHEATERMODE
// 	DISPID_ONTOOLBAR
// 	DISPID_ONVISIBLE
// 	DISPID_PROGRESSCHANGE
// 	DISPID_PROPERTYCHANGE
// 	DISPID_STATUSTEXTCHANGE
// 	DISPID_TITLECHANGE 
STDMETHODIMP AssociateIeHandle::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid,
	WORD wFlags, DISPPARAMS *pDispParams,
	VARIANT * pVarResult, EXCEPINFO * pe, UINT *pu)
{
	HRESULT hr = S_OK;
	if (dispIdMember == DISPID_BEFORENAVIGATE2)
	{
		VARIANT_BOOL Cancel=false;
		IDispatch *pDisp=pDispParams->rgvarg[6].pdispVal;
		_variant_t URL=_variant_t(pDispParams->rgvarg[5].pvarVal->bstrVal);
		_variant_t Flags=_variant_t(pDispParams->rgvarg[4].pvarVal->iVal);
		_variant_t TargetFrameName=_variant_t(pDispParams->rgvarg[3].pvarVal->bstrVal);//!=NULL?pDispParams->rgvarg[3].pvarVal->bstrVal:TEXT("");//_variant_t(pDispParams->rgvarg[3].pvarVal->bstrVal?pDispParams->rgvarg[3].pvarVal->bstrVal:TEXT(""));
		_variant_t PostData=_variant_t(pDispParams->rgvarg[2].pvarVal->bstrVal);
		_variant_t Headers=_variant_t(pDispParams->rgvarg[1].pvarVal->bstrVal);
		BeforeNavigate2(pDisp, &URL, &Flags, &TargetFrameName, &PostData, &Headers, &Cancel);
		*pDispParams->rgvarg[0].pboolVal= Cancel? TRUE:FALSE;
	}
	if (dispIdMember == DISPID_DOCUMENTCOMPLETE)
	{
		_variant_t URL=_variant_t(pDispParams->rgvarg[0].pvarVal->bstrVal);
		DocumentComplete(pDispParams->rgvarg[1].pdispVal, URL);
	}
	if (dispIdMember == DISPID_DOWNLOADBEGIN)
	{
		DownloadBegin();
	}
	if (dispIdMember == DISPID_DOWNLOADCOMPLETE)
	{
		DownloadComplete();
	}
	if (dispIdMember == DISPID_PROGRESSCHANGE)
	{
		ProgressChange(pDispParams->rgvarg[1].lVal,pDispParams->rgvarg[0].lVal);
	}
	if (dispIdMember == DISPID_NEWWINDOW3)
	{
		NewWindow3(*pDispParams->rgvarg[4].ppdispVal,pDispParams->rgvarg[3].pboolVal,pDispParams->rgvarg[2].ulVal,pDispParams->rgvarg[1].bstrVal,pDispParams->rgvarg[0].bstrVal);
		//NewWindow3((pDispParams->rgvarg[3].pboolVal),(pDispParams->rgvarg->bstrVal));
	}

	return hr;

}

//为了简化程序，下面的接口函数都没有实现
STDMETHODIMP AssociateIeHandle::GetIDsOfNames(REFIID riid, OLECHAR ** pNames,
	UINT nNames, LCID lcid, DISPID * pdispids)
{
	return E_NOTIMPL;
}

STDMETHODIMP AssociateIeHandle::GetTypeInfo(UINT it,LCID lcid,ITypeInfo **ppti)
{
	return E_NOTIMPL;
}

STDMETHODIMP AssociateIeHandle::GetTypeInfoCount(UINT * pit)
{
	return E_NOTIMPL;
}

//-------------------------------------------------------------------------------
//BOOL CALLBACK WndChildProc(HWND hwnd, LPARAM lp)
//{
//	AssociateIeHandle* pThis = (AssociateIeHandle*)lp;
//	char szClass[MAX_PATH]={0};
//	GetClassNameA(hwnd,szClass,MAX_PATH);
//	if (stricmp(szClass,"Internet Explorer_Server") == 0)
//	{
//		pThis->
//		return TRUE;
//	}
//	return TRUE;
//}
IWebBrowser2* AssociateIeHandle::GetIEFromHWnd(HWND hIEParantWindow,LPCSTR szClassWndName)
{ 
	HWND hWnd  = NULL ;
	HWND hWndChild = NULL;
	BOOL bFind = FALSE;
	while( (hWndChild = GetWindow(hIEParantWindow,GW_CHILD)) )
	{
		bFind = FALSE;
		char szClass[MAX_PATH]={0}; 
		GetClassNameA(hWndChild,szClass,MAX_PATH);
		if (stricmp(szClass,"Internet Explorer_Server")==0)
		{
			break;
		}
		
		while( (hWndChild = GetWindow(hWndChild,GW_CHILD)) )
		{
			GetClassNameA(hWndChild,szClass,MAX_PATH);
			if (stricmp(szClass,"Internet Explorer_Server")==0)
			{
				bFind = TRUE;
				break;
			}
		}

		if(bFind)
		{
			break;
		}
	}

	if (bFind==FALSE)
	{
		MessageBoxA(NULL,"没有找到IE内核",NULL,MB_OK);
		hWndChild = NULL;
	}

	hWnd=hWndChild;
	// 我们需要显示地装载OLEACC.DLL,这样我们才知道有没有安装MSAA
	HINSTANCE hInst = LoadLibraryA( "OLEACC.DLL" );
	IWebBrowser2* pWebBrowser2=NULL;
	if ( hInst != NULL ){
		if ( hWnd != NULL ){

			LRESULT lRes;
			UINT nMsg = ::RegisterWindowMessage( _T("WM_HTML_GETOBJECT") );
			::SendMessageTimeout( hWnd, nMsg, 0L, 0L, SMTO_ABORTIFHUNG, 1000, (DWORD*)&lRes );

			LPFNOBJECTFROMLRESULT pfObjectFromLresult = (LPFNOBJECTFROMLRESULT)::GetProcAddress( hInst, "ObjectFromLresult" );
			if ( pfObjectFromLresult != NULL ){
				HRESULT hr;
				CComPtr<IHTMLDocument2>spDoc;

				hr=pfObjectFromLresult(lRes,IID_IHTMLDocument2,0,(void**)&spDoc);
				if ( SUCCEEDED(hr) ){

					CComPtr<IHTMLWindow2>spWnd2;
					CComPtr<IServiceProvider>spServiceProv;
					hr=spDoc->get_parentWindow ((IHTMLWindow2**)&spWnd2);
					if(SUCCEEDED(hr)){

						hr=spWnd2->QueryInterface (IID_IServiceProvider,(void**)&spServiceProv);
						if(SUCCEEDED(hr)){
							hr = spServiceProv->QueryService(SID_SWebBrowserApp,
								IID_IWebBrowser2,
								(void**)&pWebBrowser2);
						}
					}
				}
			}
		}

		::FreeLibrary(hInst);
	}
	else{//如果没有安装MSAA
		MessageBoxA(NULL,"Please Install Microsoft Active Accessibility","Error",MB_OK);
	}
	return pWebBrowser2;
}

//-------------------------------------------------------------------------------
BOOL IsExplorer(HWND hw)
{
	BOOL bRet = FALSE;

	DWORD pID;
	GetWindowThreadProcessId(hw, &pID);

	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if (INVALID_HANDLE_VALUE != hSnap)
	{
		PROCESSENTRY32 entry={sizeof(entry)};

		for (BOOL status = Process32First(hSnap,&entry);status;status = Process32Next(hSnap,&entry))
		{
			if ( _tcsicmp(TEXT("explorer.exe"),entry.szExeFile) == 0 && entry.th32ProcessID == pID)
			{
				bRet = TRUE;
				break;
			}
		}

		CloseHandle(hSnap);
	}

	return bRet;	
}

void CheckIeThread(void* p)
{
	(void)p;
	CoInitialize(NULL);

	IShellWindows* spShellWin = NULL;
	HRESULT hr ;
	hr = ::CoCreateInstance(CLSID_ShellWindows, NULL, CLSCTX_ALL, __uuidof(IShellWindows), (void**)&spShellWin);

	if(SUCCEEDED(hr))
	{ 
		std::map<HWND,AssociateIeHandle*> ieHandleMap;
		
		while(true)
		{
			Sleep(100);
			long nCount = 999;  // 取得浏览器实例个数(Explorer 和 IExplorer)
			spShellWin->get_Count( &nCount );

			if (nCount == 0 || nCount == 999)
			{
				continue ;
			}

			BOOL		nquit = FALSE;
			HWND		hwnd = NULL;

			for(int i = 0; i < nCount; i++)
			{
				CComPtr< IDispatch > spDispIE;
				hr = spShellWin->Item(CComVariant( (long)i ), &spDispIE );

				if ( FAILED(hr) || spDispIE == NULL) 
					continue;


				CComQIPtr<IWebBrowser2>	 spBrowser = spDispIE;

				hwnd = NULL;
				spBrowser->get_HWND((SHANDLE_PTR*)&hwnd);

				if (hwnd != NULL)
				{ 
					if (IsExplorer(hwnd))
					{
						continue;
					}
				}
				else continue;

				AssociateIeHandle* ieHandle = new AssociateIeHandle(spBrowser);
				auto it = ieHandleMap.find(hwnd);
				if (it == ieHandleMap.end())
				//{
				//	it->second->StopService();
				//	it->second = ieHandle;
				//	it->second->StartService();
				//}
				//else
				{
					ieHandleMap[hwnd] = ieHandle;
					ieHandleMap[hwnd]->StartService();
				}

			} 
		}
		spShellWin->Release();
	}
}



//TEST UNIT
void TestIe()
{
	_beginthread(CheckIeThread,0,NULL);
}