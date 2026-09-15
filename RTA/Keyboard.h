#pragma once

class CKeyboard : public CWnd
{
public:
	CKeyboard();

	void SetMarker(int nOctave, int nKey);

protected:
	CRect m_KeyRect[88];
	int m_nOctave[88];
	int m_nKey[88];
	BOOL m_bKeyPress;
	BOOL m_bMarker;
	COLORREF m_MarkerColor;
	CRect m_MarkerRect;

	void NotifyMessage(CPoint point);

	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};

