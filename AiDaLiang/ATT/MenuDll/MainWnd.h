#include "..\ATT\src\ctrl\App.h"
#ifndef __MainWnd_H
#define __MainWnd_H
class MainWnd : public jkBaseWindow
{
public:
	//virtual void OnPaint(HDC hdc);
	//virtual BOOL OnEraseBackground(HDC hdc);
	virtual void OnCreate(LPCREATESTRUCT pStruct);
	virtual void OnDestroy();
protected:
	jkControl mEdit;
	jkControl mButton;
	jkControl mListBox;
	jkControl mComboBox;
	jkControl mListCtrl;
	jkControl mLabel;
	jkControl mTreeCtrl;
private:
};
#endif //__MainWnd_H