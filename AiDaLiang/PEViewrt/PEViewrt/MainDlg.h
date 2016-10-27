#ifndef __MainDlg_H
#define __MainDlg_H
class MainDlg : public jkBaseDialog
{
public:
	virtual void OnInitDialog();
	virtual LRESULT OnDropFiles(HDROP hDrop);
	virtual LRESULT OnCommand(WPARAM w,LPARAM l);
protected:
private:
	HWND hTree;
	TCHAR szFileName[MAX_PATH];
};
#endif //__MainDlg_H