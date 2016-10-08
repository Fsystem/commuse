#ifndef __MainDlg_H
#define __MainDlg_H
class MainDlg : public jkBaseDialog
{
public:
	virtual void OnInitDialog();
protected:
private:
	HWND hIe;
};
#endif //__MainDlg_H