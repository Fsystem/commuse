#ifndef __MainDlg_H
#define __MainDlg_H
class MainDlg : public jkBaseDialog
{
public:
	virtual void OnInitDialog();
	virtual LRESULT OnDropFiles(HDROP hDrop);
protected:
private:
	HWND hTree;
};
#endif //__MainDlg_H