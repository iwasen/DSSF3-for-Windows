// RemarkWnd.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "RemarkWnd.h"
#include "comres.h"

#define XMARGIN		6
#define YMARGIN		3
#define XPOS		10
#define YPOS		11
#define LINELENGTH	30

/////////////////////////////////////////////////////////////////////////////
// CRemarkWnd

CRemarkWnd::CRemarkWnd()
{
	m_bMoving = FALSE;
}

BEGIN_MESSAGE_MAP(CRemarkWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRemarkWnd メッセージ ハンドラ

BOOL CRemarkWnd::Create(CWnd *pParent)
{
	if (!CreateEx(WS_EX_DLGMODALFRAME, ::AfxRegisterWndClass(0), NULL, pParent == NULL ? WS_POPUP : WS_CHILD, 0, 0, 0, 0, pParent->GetSafeHwnd(), NULL))
		return FALSE;

	CDC *pDC = GetDC();
	m_dcMem.CreateCompatibleDC(pDC);
	ReleaseDC(pDC);

	return TRUE;
}

void CRemarkWnd::OnPaint()
{
	CPaintDC dc(this);

	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
	dc.BitBlt(0, 0, m_nWidth, m_nHeight, &m_dcMem, 0, 0, SRCCOPY);
	m_dcMem.SelectObject(pBitmapOrg);
}

void CRemarkWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	WINDOWPLACEMENT wpl;

	GetWindowPlacement(&wpl);
	m_Rect = wpl.rcNormalPosition;
	m_Point = point;

	m_bMoving = TRUE;
	SetCapture();

	CWnd::OnLButtonDown(nFlags, point);
}

void CRemarkWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bMoving = FALSE;
	ReleaseCapture();

	CWnd::OnLButtonUp(nFlags, point);
}

void CRemarkWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bMoving && point != m_Point) {
		m_Rect += point - m_Point;
		SetWindowPos(NULL, m_Rect.left, m_Rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		UpdateWindow();
	}

	CWnd::OnMouseMove(nFlags, point);
}

void CRemarkWnd::DispRemarks(const REMARK_INFO *pRemark, int nX, int nY, int nFontPoint)
{
	CSize size;
	int i, y;
	CFont font, *pFontOrg;
	CBitmap *pBitmapOrg;
	CPen *pPenOrg;
	CString str;
	CWnd *pParent = GetParent();

	str.LoadString(IDS_DEFAULTFONT);
	font.CreatePointFont(nFontPoint, str, &m_dcMem);
	pFontOrg = m_dcMem.SelectObject(&font);
	m_dcMem.SetBkMode(TRANSPARENT);
	m_dcMem.SetTextColor(RGB(64, 64, 64));

	int cx = 0;
	int cy = 0;
	for (i = 0; i < pRemark->nRemark; i++) {
		size = m_dcMem.GetOutputTextExtent(pRemark->remarks[i].text);
		if (pRemark->remarks[i].pen != NULL)
			size.cx += LINELENGTH;
		if (size.cx > cx)
			cx = size.cx;
		cy += size.cy;
#ifdef _LANG_ENG
		cy -= 2;
#endif
	}

	m_nWidth = cx + XMARGIN * 2;
	m_nHeight = cy + YMARGIN * 2;

	CDC *pDC = GetDC();
	m_bitmapMem.DeleteObject();
	m_bitmapMem.CreateCompatibleBitmap(pDC, m_nWidth, m_nHeight);
	pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
	ReleaseDC(pDC);

	CBrush brush;
	brush.CreateSolidBrush(RGB(255, 255, 225));
	CRect rect(0, 0, m_nWidth, m_nHeight);
	m_dcMem.FillRect(rect, &brush);

	y = YMARGIN;
#ifdef _LANG_ENG
		y -= 1;
#endif
	for (i = 0; i < pRemark->nRemark; i++) {
		size = m_dcMem.GetOutputTextExtent(pRemark->remarks[i].text);

		if (pRemark->remarks[i].pen != NULL) {
			pPenOrg = m_dcMem.SelectObject(pRemark->remarks[i].pen);
			m_dcMem.MoveTo(XMARGIN, y + size.cy / 2);
			m_dcMem.LineTo(XMARGIN + LINELENGTH - 5, y + size.cy / 2);
			m_dcMem.SelectObject(pPenOrg);
			m_dcMem.TextOut(XMARGIN + LINELENGTH, y, pRemark->remarks[i].text);
		} else
			m_dcMem.TextOut(XMARGIN, y, pRemark->remarks[i].text);

		y += size.cy;
#ifdef _LANG_ENG
		y -= 2;
#endif
	}

	m_dcMem.SelectObject(pFontOrg);
	m_dcMem.SelectObject(pBitmapOrg);

	CalcWindowRect(rect);
	UINT nFlags;
	if (pParent != NULL) {
		CRect rect2;
		pParent->GetClientRect(rect2);
		nX = rect2.right - rect.Width() - XPOS;
		nY = YPOS;
		nFlags = SWP_NOZORDER | SWP_SHOWWINDOW;
	} else
		nFlags = SWP_SHOWWINDOW;
	SetWindowPos(&wndTopMost, nX, nY, rect.Width(), rect.Height(), nFlags);

	Invalidate();
}
