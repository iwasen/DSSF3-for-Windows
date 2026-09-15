// IRAssistWnd.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Rta.h"
#include "IRAssistWnd.h"

/////////////////////////////////////////////////////////////////////////////
// CIRALevelWnd

CIRALevelWnd::CIRALevelWnd()
{
	m_nMarkerPos = 0;
	m_nRangeColor = 0;
	m_brushBkgnd.CreateSolidBrush(RGB(128, 128, 128));
}

BEGIN_MESSAGE_MAP(CIRALevelWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIRALevelWnd メッセージ ハンドラ
/////////////////////////////////////////////////////////////////////////////

void CIRALevelWnd::OnPaint()
{
	CPaintDC dc(this); // 描画用のデバイス コンテキスト

	dc.FillRect(&dc.m_ps.rcPaint, &m_brushBkgnd);

	if (IsWindowEnabled()) {
		int i, y;

		CRect rect;
		GetClientRect(rect);

		CPen *pPenOrg = dc.SelectObject(&m_penRangeColor[0]);

		for (y = rect.bottom; y >= m_nMarkerPos; y -= 2) {
			for (i = 0; i < m_nRangeColor; i++) {
				if (y > m_nRangeLevel[i]) {
					dc.SelectObject(m_penRangeColor[i]);
					break;
				}
			}

			dc.MoveTo(rect.left, y);
			dc.LineTo(rect.right, y);
		}
		dc.SelectObject(pPenOrg);
	}
}

BOOL CIRALevelWnd::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void CIRALevelWnd::SetRangeColor(const RANGE_COLOR *pRangeColor, int nRangeColor)
{
	CRect rect;
	GetClientRect(rect);

	for (int i = 0; i < nRangeColor; i++) {
		m_nRangeLevel[i] = (int)((rect.bottom - rect.bottom * pRangeColor[i].fLevel) / 2);
		m_penRangeColor[i].CreatePen(PS_SOLID, 0, pRangeColor[i].color);
	}

	m_nRangeColor = nRangeColor;
}

void CIRALevelWnd::SetMarker(double fMarkerPos)
{
	if (fMarkerPos > 1)
		fMarkerPos = 1;

	if (fMarkerPos < -1)
		fMarkerPos = -1;

	CRect rect;
	GetClientRect(rect);

	m_nMarkerPos = (int)((rect.bottom - rect.bottom * fMarkerPos) / 2);

	Invalidate(FALSE);
}

// CIRAPositionWnd

CIRAPositionWnd::CIRAPositionWnd()
{
	m_nMarkerPos = 0;
	m_nMarkerColor = 0;
	m_brushBkgnd.CreateSolidBrush(RGB(128, 128, 128));
}

BEGIN_MESSAGE_MAP(CIRAPositionWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIRAPositionWnd メッセージ ハンドラ

void CIRAPositionWnd::OnPaint()
{
	CPaintDC dc(this); // 描画用のデバイス コンテキスト

	CRect rect;
	GetClientRect(rect);

	CRgn rgn;
	rgn.CreateRectRgn(rect.left, rect.top, rect.right, rect.bottom);
	dc.SelectClipRgn(&rgn);

	dc.FillRect(&dc.m_ps.rcPaint, &m_brushBkgnd);

	if (IsWindowEnabled()) {
		CBrush brush(m_nMarkerColor);

		POINT point[3];

		if (m_nLeftRight == 0) {
			point[0].x = 1;
			point[0].y = m_nMarkerPos - rect.right / 2;
			point[1].x = rect.right - 1;
			point[1].y = m_nMarkerPos;
			point[2].x = 1;
			point[2].y = m_nMarkerPos + rect.right / 2;
		} else {
			point[0].x = rect.right - 1;
			point[0].y = m_nMarkerPos - rect.right / 2;
			point[1].x = 1;
			point[1].y = m_nMarkerPos;
			point[2].x = rect.right - 1;
			point[2].y = m_nMarkerPos + rect.right / 2;
		}
		CBrush *pBrushOrg = dc.SelectObject(&brush);
		dc.Polygon(point, 3);
		dc.SelectObject(pBrushOrg);
	}
}

BOOL CIRAPositionWnd::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void CIRAPositionWnd::SetMarker(double fMarkerPos, COLORREF nMarkerColor, int nLeftRight)
{
	if (fMarkerPos > 1)
		fMarkerPos = 1;

	if (fMarkerPos < -1)
		fMarkerPos = -1;

	CRect rect;
	GetClientRect(rect);

	m_nMarkerPos = (int)((rect.bottom - rect.bottom * fMarkerPos) / 2);
	m_nMarkerColor = nMarkerColor;
	m_nLeftRight = nLeftRight;

	Invalidate(FALSE);
}
