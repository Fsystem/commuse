#ifndef __MainClient_H
#define __MainClient_H
class MainClient : public jkBaseWindow
{
public:
	void ResizeClient(HWND hPrant,RECT rcClient);
	virtual void OnCreate(LPCREATESTRUCT pStruct);
	//virtual HBRUSH OnCtrlColor();
	//virtual void OnSize(UINT nSizeCmd,int width,int height);
	virtual void OnPaint(HDC hdc);
protected:
private:
};

#endif //__MainClient_H