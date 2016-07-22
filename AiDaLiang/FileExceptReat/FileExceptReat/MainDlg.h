#ifndef __MainDlg_H
#define __MainDlg_H
class MainDlg : public jkBaseDialog
{
public:
	virtual LRESULT MessageHandle(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	LRESULT OnInitDialog();
	LRESULT OnCommand(int nId);
	LRESULT OnDropFiles(HDROP hd);
	LRESULT OnRButtonUp(HWND hCtrl,LPARAM lp);
	//
	void OnCancel();
	void OnOk();
	void OnMenuRetain();
	void OnMenuClearList();

	//
	void UpdateCompFileListControl();
	void UpdateRetainFile();
private:
	std::vector<std::string> listCompFile;
	int mRetainFile;
	WNDPROC mListOldFunc;
};
#endif //__MainDlg_H