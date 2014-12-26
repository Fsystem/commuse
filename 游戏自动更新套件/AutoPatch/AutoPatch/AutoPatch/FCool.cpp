#include "afxwin.h"
#include "FCool.h"



BEGIN_MESSAGE_MAP(CCoolButton,CButton)
ON_WM_LBUTTONUP()
ON_WM_LBUTTONDOWN()
ON_WM_MOUSEMOVE()
//ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
END_MESSAGE_MAP()


void CCoolButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetStatus(DOWN);
	SetCapture();
	CButton::OnLButtonDown(nFlags, point);

}


void CCoolButton::OnLButtonUp(UINT nFlags, CPoint point)
{

	CRect ButtonRect;

	SetStatus(NORMAL);
	ReleaseCapture();
	Invalidate();
	CButton::OnLButtonUp(nFlags, point);

	GetClientRect(&ButtonRect);
	
	if (m_PrevStatus == DOWN && IsInRect(ButtonRect,point))
		GetParent()->SendMessage(WM_COMMAND,MAKEWPARAM(GetDlgCtrlID(),0),(LPARAM) m_hWnd);


}

void CCoolButton::OnMouseMove(UINT nFlags, CPoint point)
{
	CRect ButtonRect;
	TRACKMOUSEEVENT tme;
	
	GetWindowRect(&ButtonRect);

	


	//sprintf(logs,"Button Rect(%d,%d,%d,%d), Mouse Point= (%d,%d)",ButtonRect.left,ButtonRect.right,
	//		ButtonRect.top, ButtonRect.bottom, point.x, point.y);


//	GetParent()->SetWindowText(logs);

	if (m_Status == DOWN) 
	{
		if (IsInRect(ButtonRect,point))
		{
			//SetStatus(DOWN);
		}
		else
		{
			SetStatus(HOVER);
			Invalidate(FALSE);
		}
	}
	else if (m_PrevStatus == DOWN && m_Status == HOVER)
	{
		SetStatus(DOWN);
	}
	else
	{
		if (IsInRect(ButtonRect,point))
		{
			//GetParent()->SetWindowText("END HOVER");
			SetStatus(HOVER);
			Invalidate(FALSE);
		}
		else
		{
			//GetParent()->SetWindowText("END NORMAL");
			SetStatus(NORMAL);
			//Invalidate();
		}

		
	}

	


	CButton::OnMouseMove(nFlags, point);


	tme.cbSize = sizeof(TRACKMOUSEEVENT);
	tme.dwFlags = TME_LEAVE;
	tme.hwndTrack = m_hWnd;

	_TrackMouseEvent(&tme);
	

}

void CCoolButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{

	//CButton::DrawItem(lpDrawItemStruct);
	CDC *pthisDC;
	CDC bmpDC;
	CBitmap *pOldBitmap;

	pthisDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	bmpDC.CreateCompatibleDC(pthisDC);

	switch (m_Status)
	{

	case NORMAL:
		pOldBitmap = bmpDC.SelectObject(&m_bmpNormal);
	//	GetParent()->SetWindowText("Normal");
		break;
	case HOVER:
		pOldBitmap = bmpDC.SelectObject(&m_bmpHover);
	//	GetParent()->SetWindowText("Hover");
		break;
	case DOWN:
		pOldBitmap = bmpDC.SelectObject(&m_bmpDown);
	//	GetParent()->SetWindowText("Down");
		break;
	default:
		pOldBitmap = bmpDC.SelectObject(&m_bmpNormal);
		break;
	}



	pthisDC->BitBlt(0,0,lpDrawItemStruct->rcItem.right-lpDrawItemStruct->rcItem.left,
		lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top,&bmpDC,0,0,SRCCOPY);


	bmpDC.SelectObject(pOldBitmap);

	bmpDC.DeleteDC();
	

}




CCoolButton::CCoolButton(int nIDNormal,int nIDHover, int nIDDown):m_Status(NORMAL)
{
	ASSERT(m_bmpNormal.LoadBitmap(nIDNormal));
	ASSERT(m_bmpHover.LoadBitmap(nIDHover));
	ASSERT(m_bmpDown.LoadBitmap(nIDDown));
}

void CCoolButton::OnMouseLeave(UINT nFlags, CPoint point)
{
	SetStatus(NORMAL);
	Invalidate();
}

CCoolButton::CCoolButton()
{

}


void CCoolButton::SetStatus(enum CCoolButton::ButtonStatus bs)
{

	m_PrevStatus = m_Status;
	m_Status = bs;

}

BOOL CCoolButton::IsInRect(CRect &rect, CPoint &point)
{

	return (point.x >= 0 && point.x <= (rect.right - rect.left) && point.y >= 0 && point.y <= (rect.bottom - rect.top))? TRUE : FALSE;

}

