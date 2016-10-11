#pragma once
#include "XWebBrowser.h"
#include <vector>
#include <MsHTML.h>

class IXWebNotifyCallback2
{
	friend class CXWebBrowser2;
protected:
	virtual VOID OnBeforeNavigate(BOOL IsFrame, BSTR URL, BOOL &IsCancel){}
	virtual BOOL OnAmbientDownloadControl(LONG &nFlags){return FALSE;}
	virtual VOID OnDocumentComplete(BSTR URL){}
	virtual VOID OnNavigateComplete(BSTR URL){}
	enum NW_TYPE
	{
		// 默认, 弹窗
		NW_DEFAULT,
		// 禁止弹窗
		NW_CANCEL,
		// 禁止弹窗, 但在当前浏览器打开
		NW_LIMIT
	};
	virtual NW_TYPE OnNewWindow(BSTR URL){return NW_DEFAULT;}
};

// 浏览器基本框架, 提供基本接口与元素相关接口.
class CXWebBrowser2 : public CXWebBrowser
{
protected:
	CXWebBrowser2(void);
	virtual ~CXWebBrowser2(void);
private:
	IDispatch *m_Dispatch;
	IXWebNotifyCallback2 *m_Callback2;
protected:
	virtual BOOL InitInstance(HWND hWnd);
	virtual VOID UninitInstance();
public:
	static BOOL CreateInstance(HWND hWnd, CXWebBrowser2* &wb2);
	static VOID DeleteInstance(CXWebBrowser2* &wb2);
protected:
	virtual HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);
public:
	VOID SetCallback2(IXWebNotifyCallback2 *cb);
	BOOL Silent(BOOL Enabled);
	BOOL Navigate(WCHAR *Url);
	BOOL Navigate(UINT nRes);
	BOOL Stop();
	BOOL Refresh();
	BOOL GoBack();
	BOOL GoForward();
	BOOL IsIdle();
	BOOL IsCompleted();
protected:
	BOOL RefHtmlDocument(IHTMLDocument* &HtmlDoc);
	BOOL RefHtmlDocument2(IHTMLDocument2* &HtmlDoc2);
	BOOL RefHtmlDocument3(IHTMLDocument3* &HtmlDoc3);
	BOOL CompareVariant(CONST VARIANT &v1, CONST VARIANT &v2);
	typedef std::vector<IDispatch*> XLIST_ELEMENTS;
	VOID DerefAllElementsFromList(XLIST_ELEMENTS &EleList);
	VOID RefAllScriptToList(XLIST_ELEMENTS &EleList);
	VOID RefAllElementsToList(XLIST_ELEMENTS &EleList);
private:
	VOID RefScriptToList(XLIST_ELEMENTS &EleList, IHTMLElementCollection *Elements);
	VOID RefElementsToList(XLIST_ELEMENTS &EleList, IHTMLElementCollection *Elements);
	VOID BuildElementListByFilterEx(CONST XLIST_ELEMENTS &From, XLIST_ELEMENTS &To, LPCWSTR PropName, CONST VARIANT &PropValue);
protected:
	BOOL GetElementValueEx(IDispatch *Item, LPCWSTR Name, VARIANT &Value);
	BOOL SetElementValueEx(IDispatch *Item, LPCWSTR Name, VARIANT &Value);
	VOID BuildElementListByFilter(XLIST_ELEMENTS &EleList, LPCWSTR PropName, CONST VARIANT &PropValue);
public:
	BOOL IsElementExistEx(LPCWSTR tagName, LPCWSTR PropName, CONST VARIANT &PropValue);
	BOOL ClickElementEx(LPCWSTR tagName, LPCWSTR PropName, CONST VARIANT &PropValue);
	BOOL SetElementTextEx(LPCWSTR tagName, LPCWSTR PropName, CONST VARIANT &PropValue, LPCWSTR NewText);
	BOOL InvokeScript(LPCWSTR FuncName);
};

