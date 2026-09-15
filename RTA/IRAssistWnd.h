#pragma once

#define MARKER_RANGE	100

struct RANGE_COLOR {
	double fLevel;
	COLORREF color;
};

/////////////////////////////////////////////////////////////////////////////
// CIRALevelWnd ウィンドウ

class CIRALevelWnd : public CWnd
{
public:
	CIRALevelWnd();

	void SetRangeColor(const RANGE_COLOR *pRangeColor, int nRangeColor);
	void SetMarker(double fMarkerPos);

protected:
	CBrush m_brushBkgnd;
	int m_nMarkerPos;
	int m_nRangeColor;
	int m_nRangeLevel[5];
	CPen m_penRangeColor[5];

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

/////////////////////////////////////////////////////////////////////////////
// CIRAPositionWnd ウィンドウ

class CIRAPositionWnd : public CWnd
{
public:
	CIRAPositionWnd();

	void SetMarker(double fMarkerPos, COLORREF nMarkerColor, int nLeftRight);

protected:
	int m_nMarkerPos;
	COLORREF m_nMarkerColor;
	CBrush m_brushBkgnd;
	int m_nLeftRight;

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
