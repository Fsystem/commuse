#include "stdafx.h"
#include "MainDlg.h"
#include "AXContainer.h"

jkWebBroser gWeb;

class CIeEvent : public IWebNotifyCallback2
{
public:

protected:
	virtual HRESULT OnIeEventCallBack( DISPID dispIdMember ,REFIID riid,LCID lcid,WORD wFlags ,DISPPARAMS FAR* pDispParams ,VARIANT FAR* pVarResult ,EXCEPINFO FAR* pExcepInfo ,unsigned int FAR* puArgErr )
	{
		//throw std::logic_error("The method or operation is not implemented.");

		return E_NOTIMPL;
	}

};

void MainDlg::OnInitDialog()
{
	RECT rc;
	
	GetClientRect(mHwnd,&rc);

	gWeb.SetCallBack(new CIeEvent);
	gWeb.Create(mHwnd,rc);

	

	gWeb.GetWebObject()->put_Silent(VARIANT_TRUE);
	gWeb.GetWebObject()->Navigate(L"http://192.168.1.47/d/b.html",0,0,0,0);


	DWORD dwErr = GetLastError();

	void(0);
}