#include "stdafx.h"
#include "WebBrowser/XWebBrowser2.h"
#include <Shellapi.h> // CommandLineToArgvW
#include <mshtmdid.h>

#define WND_CLASSNAME		TEXT("Browser Example")

class EventCallback : public IXWebNotifyCallback2
{
private:
	CXWebBrowser2 *web;
public:
	EventCallback(CXWebBrowser2 *wb)
	{
		web = wb;
	}

	virtual ~EventCallback()
	{

	}
protected:
	virtual VOID OnBeforeNavigate(BOOL IsFrame, BSTR URL, BOOL &IsCancel)
	{
		// 将要打开一个网页时触发
		// 修改IsCancel=TRUE可以取消此次操作
	}

	virtual BOOL OnAmbientDownloadControl(LONG &nFlags)
	{
		// 设置下载控制标志
		// 若不修改, 返回FALSE, 使用默认值
		// 若要自定义, 修改nFlags后, 返回TRUE

		// 特别注意, 修改这个值会覆盖Silent属性
		// 若需要, 请附加DLCTL_SILENT标志
		
		// 例子: 只下载文字和图片, 且禁止ActiveX插件
		nFlags = DLCTL_SILENT|DLCTL_DLIMAGES|DLCTL_NO_DLACTIVEXCTLS|DLCTL_NO_RUNACTIVEXCTLS;
		return TRUE;
	}

	virtual VOID OnDocumentComplete(BSTR URL)
	{
		// 页面元素加载完成后触发
	}

	virtual VOID OnNavigateComplete(BSTR URL)
	{
		// 整个页面加载完毕后触发
	}

	virtual NW_TYPE OnNewWindow(BSTR URL)
	{
		// 要弹窗时触发
		// 参阅NW_TYPE定义

		// PS: 不太好使, 嘿嘿
		return NW_DEFAULT;
	}
};

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)  
{
	static CXWebBrowser2 *wb;
	static EventCallback *wbe;

	switch (uMsg)
	{
	case WM_CREATE:
		{
			if (CXWebBrowser2::CreateInstance(hWnd, wb))
			{
				int argc;
				WCHAR** argv;

				wbe = new EventCallback(wb);
				wb->SetCallback2(wbe);
				argv = CommandLineToArgvW(GetCommandLineW(), &argc);
				wb->Silent(TRUE);
				if (argc > 1)
				{
					wb->Navigate(argv[1]);
				}
				else
				{
					wb->Navigate(L"www.baidu.com");
				}
			}
		}
		break;
	case WM_CLOSE:
		{
			CXWebBrowser2::DeleteInstance(wb);
			delete wbe;
			PostQuitMessage(0);
		}
		break;
	case WM_SIZE:
		{
			switch (wParam)
			{
			case SIZE_MAXSHOW:
			case SIZE_RESTORED:
			case SIZE_MAXIMIZED:
				{
					RECT rc;

					GetClientRect(hWnd, &rc);
					wb->SetBrowserRect(&rc);
				}
				break;
			default:
				break;
			}
		}
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return S_OK;
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WindowProc;
	wc.lpszClassName = WND_CLASSNAME;
	RegisterClassEx(&wc);

	CXWebBrowser2::SetIEVersion(CXWebBrowser2::IE10);

	if (HWND hWnd = CreateWindowEx(0, WND_CLASSNAME, TEXT("Ole WebBrowser"), WS_OVERLAPPEDWINDOW, 10, 10, 1024, 768, HWND_DESKTOP, NULL, hInstance, NULL))
	{
		MSG msg;

		ShowWindow(hWnd, SW_SHOW);  
		UpdateWindow(hWnd);
		while (GetMessage(&msg, 0, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return 0;
}

