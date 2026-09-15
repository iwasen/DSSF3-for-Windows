#pragma once

struct REMARK_INFO {
	int nRemark;
	struct {
		CPen *pen;
		LPCTSTR text;
	} remarks[10];
};

class CRemarkWnd : public CWnd
{
public:
	CRemarkWnd();

	BOOL Create(CWnd *pParent);
	void DispRemarks(const REMARK_INFO *pRemark, int nX = 0, int nY = 0, int nFontPoint = 90);

protected:
	CDC m_dcMem;
	CBitmap m_bitmapMem;
	int m_nWidth;
	int m_nHeight;
	CRect m_Rect;
	CPoint m_Point;
	BOOL m_bMoving;

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
