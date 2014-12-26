class CCoolButton  : public CButton
{


public:

	enum ButtonStatus {NORMAL,HOVER,DOWN};

	CBitmap m_bmpNormal, m_bmpHover, m_bmpDown;

	ButtonStatus m_Status, m_PrevStatus;

	char logs[30];

	CCoolButton();

	void SetStatus(ButtonStatus bs);
	

public:
	CCoolButton(int nIDNormal, int nIDHover, int nIDDown);


	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);


protected:

	BOOL IsInRect(CRect &rect, CPoint &point);

	afx_msg	void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
};