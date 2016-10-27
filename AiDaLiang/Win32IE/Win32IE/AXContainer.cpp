// AX.CPP
#include "stdafx.h"
#include <windows.h>
#include <comdef.h>
#include <exdisp.h>
#include <exdispid.h>
#include <oledlg.h>
#include "AXContainer.h"
#include <mshtmdid.h>


#pragma warning (disable: 4311)
#pragma warning (disable: 4312)
#pragma warning (disable: 4244)
#pragma warning (disable: 4800)


// AXClientSite class
// ------- Implement member functions
AXClientSite :: AXClientSite()
{
	refNum = 0;
	CalledCanInPlace = 0;
	InPlace = 0;
	mCallBack = NULL;
}

AXClientSite :: ~AXClientSite()
{
}


// IUnknown methods
STDMETHODIMP AXClientSite :: QueryInterface(REFIID iid,void**ppvObject)
{
	*ppvObject = 0;
	if (iid == IID_IOleClientSite)
		*ppvObject = (IOleClientSite*)this;
	if (iid == IID_IUnknown)
		*ppvObject = this;
	if (iid == IID_IAdviseSink)
		*ppvObject = (IAdviseSink*)this;
	if (iid == IID_IDispatch)
		*ppvObject = (IDispatch*)this;
// 	if (iid == __uuidof(DWebBrowserEvents2))
// 		*ppvObject = static_cast<IDispatch *>(this);
	if (ExternalPlace == false)
	{
		if (iid == IID_IOleInPlaceSite)
			*ppvObject = (IOleInPlaceSite*)this;
		if (iid == IID_IOleInPlaceFrame)
			*ppvObject = (IOleInPlaceFrame*)this;
		if (iid == IID_IOleInPlaceUIWindow)
			*ppvObject = (IOleInPlaceUIWindow*)this;
	}

	//* Log Call
	if (*ppvObject)
	{
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)  AXClientSite :: AddRef()
{
	refNum++;
	return refNum;
}

STDMETHODIMP_(ULONG)  AXClientSite :: Release()
{
	refNum--;
	return refNum;
}


// IOleClientSite methods
STDMETHODIMP AXClientSite :: SaveObject()
{
	return S_OK;
}

STDMETHODIMP AXClientSite :: GetMoniker(DWORD dwA,DWORD dwW,IMoniker**pm)
{
	*pm = 0;
	return E_NOTIMPL;
}

STDMETHODIMP AXClientSite :: GetContainer(IOleContainer**pc)
{
	*pc = 0;
	return E_FAIL;
}

STDMETHODIMP AXClientSite :: ShowObject()
{
	return S_OK;
}

STDMETHODIMP AXClientSite :: OnShowWindow(BOOL f)
{
	InvalidateRect(Window, 0, TRUE);
	InvalidateRect(Parent, 0, TRUE);
	return S_OK;
}

STDMETHODIMP AXClientSite :: RequestNewObjectLayout()
{
	return S_OK;
}

STDMETHODIMP_(void) AXClientSite :: OnViewChange(DWORD dwAspect,LONG lIndex)
{
}

STDMETHODIMP_(void) AXClientSite :: OnRename(IMoniker * pmk)
{
}

STDMETHODIMP_(void) AXClientSite :: OnSave()
{
}

STDMETHODIMP_(void) AXClientSite :: OnClose()
{
}


// IOleInPlaceSite methods
STDMETHODIMP AXClientSite :: GetWindow(HWND *p)
{
	*p = Window;
	return S_OK;
}

STDMETHODIMP AXClientSite :: ContextSensitiveHelp(BOOL)
{
	return E_NOTIMPL;
}


STDMETHODIMP AXClientSite :: CanInPlaceActivate()
{
	if (InPlace)
	{
		CalledCanInPlace = true;
		return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP AXClientSite :: OnInPlaceActivate()
{
	return S_OK;
}

STDMETHODIMP AXClientSite :: OnUIActivate()
{
	return S_OK;
}

STDMETHODIMP AXClientSite :: GetWindowContext(IOleInPlaceFrame** ppFrame,IOleInPlaceUIWindow **ppDoc,LPRECT r1,LPRECT r2,LPOLEINPLACEFRAMEINFO o)
{
	*ppFrame = (IOleInPlaceFrame*)this;
	AddRef();

	*ppDoc = NULL;
	GetClientRect(Window,r1);
	GetClientRect(Window,r2);
	o->cb = sizeof(OLEINPLACEFRAMEINFO);
	o->fMDIApp = FALSE;
	o->hwndFrame = Parent;
	o->haccel = 0;
	o->cAccelEntries = 0;

	return S_OK;

	/*AddRef();
	*ppFrame = dynamic_cast<IOleInPlaceFrame*>(this);

	AddRef();
	*ppDoc = dynamic_cast<IOleInPlaceUIWindow*>(this);

	GetClientRect(Window, r1);
	GetClientRect(Window, r2);
	o->cb = sizeof(OLEINPLACEFRAMEINFO);
	o->hwndFrame = Window;
	o->cAccelEntries = 0;
	o->fMDIApp = FALSE;
	o->haccel = NULL;
	return S_OK;*/
}

STDMETHODIMP AXClientSite :: Scroll(SIZE s)
{
	return E_NOTIMPL;
}

STDMETHODIMP AXClientSite :: OnUIDeactivate(int)
{
	return S_OK;
}

STDMETHODIMP AXClientSite :: OnInPlaceDeactivate()
{
	return S_OK;
}

STDMETHODIMP AXClientSite :: DiscardUndoState()
{
	return S_OK;
}

STDMETHODIMP AXClientSite :: DeactivateAndUndo()
{
	return S_OK;
}

STDMETHODIMP AXClientSite :: OnPosRectChange(LPCRECT)
{
	return S_OK;
}


// IOleInPlaceFrame methods
STDMETHODIMP AXClientSite :: GetBorder(LPRECT l)
{
	GetClientRect(Window,l);
	return S_OK;
}

STDMETHODIMP AXClientSite :: RequestBorderSpace(LPCBORDERWIDTHS b)
{
	return S_OK;
	//return E_NOTIMPL;
}

STDMETHODIMP AXClientSite :: SetBorderSpace(LPCBORDERWIDTHS b)
{
	return S_OK;
}

STDMETHODIMP AXClientSite :: SetActiveObject(IOleInPlaceActiveObject*pV,LPCOLESTR s)
{
	ax->Pao = pV;
	return S_OK;
}


STDMETHODIMP AXClientSite :: SetStatusText(LPCOLESTR t)
{
	return E_NOTIMPL;
}

STDMETHODIMP AXClientSite :: EnableModeless(BOOL f)
{
	return E_NOTIMPL;
}

STDMETHODIMP AXClientSite :: TranslateAccelerator(LPMSG,WORD)
{
	return E_NOTIMPL;
}


// IDispatch Methods
HRESULT _stdcall AXClientSite :: GetTypeInfoCount(
	unsigned int * pctinfo) {return E_NOTIMPL;}

HRESULT _stdcall AXClientSite :: GetTypeInfo(
	unsigned int iTInfo,
	LCID lcid,
	ITypeInfo FAR* FAR* ppTInfo) {return E_NOTIMPL;}

HRESULT _stdcall AXClientSite :: GetIDsOfNames(
	REFIID riid,
	OLECHAR FAR* FAR*,
	unsigned int cNames,
	LCID lcid,
	DISPID FAR* ) {return E_NOTIMPL;}


// Other Methods
void AXContainer :: Init(char* cls)
{
	wchar_t x[1000] = {0};
	MultiByteToWideChar(CP_ACP,0,cls,-1,x,1000);
	CLSIDFromString(x,&clsid);
	iid = (IID*)&IID_IOleObject;
	OleObject = 0;
	Storage = 0;
	View = 0;
	Data = 0;
	Unk = 0;
	Pao = 0;
	AdviseToken = 0;
	memset(DAdviseToken,0,sizeof(DAdviseToken));
	Site.ax = this;

	//////////////////////////////////////////////////////////////////////////
	mIWebBrowser = 0;
	mIeConnectionPoint = 0;
	mCookie = 0;
}

AXContainer :: AXContainer(char* cls)
{
	Init(cls);
}



void AXContainer :: Clean()
{
	if (Site.InPlace == true)
	{
		Site.InPlace = false;
		IOleInPlaceObject* iib = 0;
		if (OleObject)
			OleObject->QueryInterface(IID_IOleInPlaceObject,(void**)&iib);
		if (iib)
		{
			iib->UIDeactivate();
			iib->InPlaceDeactivate();
			iib->Release();
		}
	}

	if (AdviseToken && OleObject)
	{
		OleObject->Unadvise(AdviseToken);
		AdviseToken = 0;
	}
	if (Data)
	{
		for(int i = 0 ; i < 100 ; i++)
			if (DAdviseToken[i])
				Data->DUnadvise(DAdviseToken[i]);
		memset(DAdviseToken,0,sizeof(DAdviseToken));
	}

	if (mIeConnectionPoint)
	{
		mIeConnectionPoint->Unadvise(mCookie);
		mCookie = 0;
	}

	if (Pao) Pao->Release();
	if (Unk) Unk->Release();
	if (Data) Data->Release();
	if (View) View->Release();
	if (Storage) Storage->Release();
	if (OleObject) OleObject->Release();
	if(mIWebBrowser) mIWebBrowser->Release();
	if(mIeConnectionPoint) mIeConnectionPoint->Release();
	Unk = 0;
	Data = 0;
	View = 0;
	Storage = 0;
	OleObject = 0;
	mIWebBrowser = 0;
	mIeConnectionPoint = 0;
}

AXContainer :: ~AXContainer()
{
	Clean();
}

CLSID AXContainer :: GetCLSID()
{
	return clsid;
}

HRESULT _stdcall AXClientSite :: InsertMenus(HMENU h,LPOLEMENUGROUPWIDTHS x)
{
	/*      AX * t = (AX*)ax;
	if (t->AddMenu)
	{
	x->width[0] = 0;
	x->width[2] = 0;
	x->width[4] = 0;
	//InsertMenu(h,0,MF_BYPOSITION | MF_POPUP,(int)Menu,"test");
	return S_OK;
	}
	*/
	return E_NOTIMPL;
}
HRESULT _stdcall AXClientSite :: SetMenu(HMENU h,HOLEMENU hO,HWND hw)
{
	AXContainer * t = (AXContainer*)ax;
	/*      if (t->AddMenu)
	{
	if (!h && !hO)
	{
	//::SetMenu(Window,Menu);
	//DrawMenuBar(Window);
	::SetMenu(Parent,Menu);
	DrawMenuBar(Parent);
	return S_OK;
	}

	//::SetMenu(Window,h);
	//DrawMenuBar(Window);

	//HMENU hm = GetMenu(Parent);
	//AppendMenu(hm,MF_POPUP | MF_MENUBREAK,(int)h,0);
	//::SetMenu(Parent,hm);
	::SetMenu(Parent,h);
	DrawMenuBar(Parent);

	//hOleWindow = hw;
	//OleSetMenuDescriptor(hO,Window,hw,0,0);
	OleSetMenuDescriptor(hO,Parent,hw,0,0);

	return S_OK;
	}
	*/
	return E_NOTIMPL;
}

HRESULT _stdcall AXClientSite :: RemoveMenus(HMENU h)
{
	AXContainer * t = (AXContainer*)ax;
	if (t->AddMenu)
	{
		if (!h)
			return S_OK;

		int c = GetMenuItemCount(h);
		for (int i = c ; i >= 0 ; i--)
		{
			HMENU hh = GetSubMenu(h,i);
			if (hh == Menu)
				RemoveMenu(h,i,MF_BYPOSITION);
		}
		if (h == Menu)
			DestroyMenu(h);

		//DrawMenuBar(Window);
		DrawMenuBar(Parent);
		return S_OK;
	}
	return E_NOTIMPL;
}


HRESULT _stdcall AXClientSite :: Invoke(
	DISPID dispIdMember,
	REFIID riid,
	LCID lcid,
	WORD wFlags,
	DISPPARAMS FAR* pDispParams,
	VARIANT FAR* pVarResult,
	EXCEPINFO FAR* pExcepInfo,
	unsigned int FAR* puArgErr)
{
	if (mCallBack)
	{
		return mCallBack->OnIeEventCallBack(dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr);
	}
	/*switch(dispIdMember)
	{
	case DISPID_AMBIENT_DLCONTROL:
		{
			return S_OK;
		}
	case DISPID_NEWWINDOW3:
		{
			return S_OK;
		}
	}*/
	
	return E_NOTIMPL;
}


void _stdcall AXClientSite :: OnDataChange(FORMATETC *pFormatEtc,STGMEDIUM *pStgmed)
{
	// Notify our app that a change is being requested
	return;
}

// Window Procedure for AX control
LRESULT CALLBACK AXWndProc(HWND hh,UINT mm,WPARAM ww,LPARAM ll)
{
	if (mm == WM_CREATE)
	{
		char tit[1000] = {0};
		HRESULT hr;

		GetWindowTextA(hh,tit,1000);

		jkWebBroser* pWebHost = reinterpret_cast<jkWebBroser*>( ((LPCREATESTRUCT)ll)-> lpCreateParams ) ;

		AXContainer* ax;
		ax = new AXContainer(tit);

		SetWindowLong(hh,GWL_USERDATA,(LONG)ax);
		ax->Site.Window = hh;
		ax->Site.Parent = GetParent(hh);

		hr = StgCreateDocfile(0,STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_DIRECT | STGM_CREATE,0,&ax->Storage);
		ax->Site.Window = hh;

		REFIID rid = *ax->iid;
		hr = OleCreate(ax->GetCLSID(),rid,OLERENDER_DRAW,0,&ax->Site,ax->Storage,(void**)&ax->OleObject);

		if (!ax->OleObject)
		{
			delete ax;
			SetWindowLong(hh,GWL_USERDATA,0);
			return -1;
		}

		hr = OleSetContainedObject(ax->OleObject, TRUE);
		hr = ax->OleObject->Advise(&ax->Site,&ax->AdviseToken);
		hr = ax->OleObject->QueryInterface(IID_IViewObject,(void**)&ax->View);
		hr = ax->OleObject->QueryInterface(IID_IDataObject,(void**)&ax->Data);
		if (ax->View)
		hr = ax->View->SetAdvise(DVASPECT_CONTENT,0,&ax->Site);

		//IE事件
		hr = ax->OleObject->QueryInterface(IID_IWebBrowser2, (void**)&ax->mIWebBrowser);

		IConnectionPointContainer* pConnContainer = NULL;
		//获取连接点指针
		hr = ax->mIWebBrowser->QueryInterface(IID_IConnectionPointContainer,(void**)&pConnContainer);
		if (SUCCEEDED(hr))
		{
			//查找事件连接点
			hr = pConnContainer->FindConnectionPoint(DIID_DWebBrowserEvents2, &ax->mIeConnectionPoint);
			if (SUCCEEDED(hr))
			{
				//实现连接点地事件接收
				IUnknown* pIUnknown = NULL;
				ax->Site.QueryInterface(IID_IUnknown,(void**)&pIUnknown);
				ax->mIeConnectionPoint->Advise(pIUnknown,&ax->mCookie);
				pIUnknown->Release();
			}

			pConnContainer->Release();
		}

		if(pWebHost){
			pWebHost->SetIePtr(ax->mIWebBrowser);
			ax->Site.mCallBack = pWebHost->GetCallBack();
		}

		return 0;
	}



	if (mm == WM_DESTROY)
	{
		AXContainer* ax = (AXContainer*)GetWindowLong(hh,GWL_USERDATA);
		if (!ax)
			return 0;
		ax->Clean();
		return true;
	}

	if (mm == AX_SETDATAADVISE)
	{
		AXContainer* ax = (AXContainer*)GetWindowLong(hh,GWL_USERDATA);
		if (!ax)
			return 0;

		// 1.Enum available FORMATETC structures
		// 2.Set Data Advise specified to index ww
		if (!ax->Data)
			return 0;

		IEnumFORMATETC* ief = 0;
		int i = 0;

		FORMATETC fe;
		ax->Data->EnumFormatEtc((LPARAM)ll,&ief);
		if (!ief)
			return 0;
		for(;;)
		{
			HRESULT hr = ief->Next(1,&fe,0);
			if (hr != S_OK)
				break;
			if (ww == i)
				break;
			i++;
		}
		ief->Release();
		if (ww == -1)
			return i;

		if (ax->Data)
			ax->Data->DAdvise(&fe,0,&ax->Site,&ax->DAdviseToken[ww]);

		return true;
	}

	if (mm == AX_GETAXINTERFACE)
	{
		AXContainer* ax = (AXContainer*)GetWindowLong(hh,GWL_USERDATA);
		return (LONG)ax;
	}

	if (mm == AX_QUERYINTERFACE)
	{
		char* p = (char*)ww;
		AXContainer* ax = (AXContainer*)GetWindowLong(hh,GWL_USERDATA);
		if (!ax)
			return 0;
		return ax->OleObject->QueryInterface((REFIID)*p,(void**)ll);
	}

	if (mm == WM_LBUTTONDBLCLK)
	{
		PostMessage(hh,AX_INPLACE,1,0);
		return 0;
	}


	if (mm == AX_INPLACE)
	{
		AXContainer* ax = (AXContainer*)GetWindowLong(hh,GWL_USERDATA);
		if (!ax)
			return 0;
		if (!ax->OleObject)
			return 0;
		RECT rect;
		HRESULT hr;
		::GetClientRect(hh,&rect);

		if (ax->Site.InPlace == false && ww == 1) // Activate In Place
		{
			ax->Site.InPlace = true;
			ax->Site.ExternalPlace = ll;
			hr = ax->OleObject->DoVerb(OLEIVERB_INPLACEACTIVATE,0,&ax->Site,0,hh,&rect);
			InvalidateRect(hh,0,true);
			return 1;
		}

		if (ax->Site.InPlace == true && ww == 0) // Deactivate
		{
			ax->Site.InPlace = false;

			IOleInPlaceObject* iib;
			ax->OleObject->QueryInterface(IID_IOleInPlaceObject,(void**)&iib);
			if (iib)
			{
				iib->UIDeactivate();
				iib->InPlaceDeactivate();
				iib->Release();
				InvalidateRect(hh,0,true);
				return 1;
			}
		}
		return 0;
	}

	if (mm == WM_SIZE)
	{
		AXContainer* ax = (AXContainer*)GetWindowLong(hh,GWL_USERDATA);
		if (!ax)
			return 0;
		if (!ax->OleObject)
			return 0;

		DefWindowProc(hh,mm,ww,ll);

		if (ax->Site.InPlace == true)
		{
			SendMessage(hh,AX_INPLACE,0,0);
			InvalidateRect(hh,0,true);
			SendMessage(hh,AX_INPLACE,1,0);
		}

		IOleInPlaceObject* pl;
		ax->OleObject->QueryInterface(IID_IOleInPlaceObject,(void**)&pl);
		if (!pl)
			return 0;
		RECT r;
		GetClientRect(ax->Site.Window,&r);
		pl->SetObjectRects(&r,&r);
		pl->Release();
		return 0;
	}

	return DefWindowProc(hh,mm,ww,ll);
}

// Registration function
ATOM AXRegister()
{
	WNDCLASSEXA wC = {0};

	if ( GetClassInfoExA(GetModuleHandle(0),"AXContainer",&wC) )
	{
		return TRUE;
	}

	wC.cbSize = sizeof(wC);
	wC.style = CS_GLOBALCLASS | CS_DBLCLKS;
	wC.lpfnWndProc = AXWndProc;
	wC.cbWndExtra = 4;
	wC.hInstance = GetModuleHandle(0);
	wC.lpszClassName = "AXContainer";
	return RegisterClassExA(&wC);
}

jkWebBroser::jkWebBroser()
{
	mIePtr = NULL;
	mIeWnd = NULL;
}

jkWebBroser::~jkWebBroser()
{
	if (mIePtr)
	{
		mIePtr->Release();
		mIePtr = NULL;
	}

	if (IsWindow(mIeWnd))
	{
		DestroyWindow(mIeWnd);
		mIeWnd = NULL;
	}
}

//-------------------------------------------------------------------------------
BOOL jkWebBroser::Create( HWND hParantWnd,RECT rcWeb )
{
	if (!AXRegister())
		return FALSE;

	RECT rc = rcWeb;
	mIeWnd = ::CreateWindowExA(CS_HREDRAW | CS_VREDRAW,"AXContainer","{8856F961-340A-11D0-A96B-00C04FD705A2}"
		,WS_VISIBLE|WS_CHILD,rc.left,rc.top,rc.right-rc.left,rc.bottom-rc.top,hParantWnd,NULL,theJKApp.GetInstance(),(jkWebBroser*)this);
	if (mIeWnd)
	{
		SendMessage(mIeWnd,AX_INPLACE,1,0);

		SendMessage(mIeWnd,AX_QUERYINTERFACE,(WPARAM)&IID_IWebBrowser2,(LPARAM)&mIePtr);
	}

	return mIePtr != NULL;
}

void jkWebBroser::SetCallBack( IWebNotifyCallback2* callback )
{
	mCallBack = callback;
}

IWebNotifyCallback2* jkWebBroser::GetCallBack()
{
	return mCallBack;
}

IWebBrowser2* jkWebBroser::GetWebObject()
{
	return mIePtr;
}

// void jkWebBroser::SetAxContainer( AXContainer* container )
// {
// 	mAx = container;
// }
