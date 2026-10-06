// WaveEditWnd.cpp : 実装ファイル
//

#include "stdafx.h"
#include "RTA.h"
#include "WaveEditWnd.h"
#include "WaveEditManual.h"
#include "Spline.h"

#define COLOR_DATA	RGB(0, 0, 255)
#define COLOR_SEL	RGB(255, 0, 0)

// CWaveEditWnd

CWaveEditWnd::CWaveEditWnd()
{
	m_bInitialized = FALSE;
	m_nMouseDown = 0;
	m_fStartX = 0;
	m_fDispX = 0;
	m_fMaxY = 0;
	m_fMinY = 0;
}

BEGIN_MESSAGE_MAP(CWaveEditWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(IDM_ADD, &CWaveEditWnd::OnAdd)
	ON_COMMAND(IDM_DELETE, &CWaveEditWnd::OnDelete)
END_MESSAGE_MAP()

// CWaveEditWnd メッセージ ハンドラ

void CWaveEditWnd::OnPaint()
{
	CPaintDC dc(this); // 描画用のデバイス コンテキスト

	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
	dc.BitBlt(0, 0, m_nWidth, m_nHeight, &m_dcMem, 0, 0, SRCCOPY);
	m_dcMem.SelectObject(pBitmapOrg);
}

BOOL CWaveEditWnd::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void CWaveEditWnd::Initialize(CWaveEditManual *pWaveEditManual)
{
	if (m_bInitialized)
		return;

	m_bInitialized = TRUE;

	m_pWaveEditManual = pWaveEditManual;

	RECT rect;
	GetClientRect(&rect);

	m_nWidth = rect.right;
	m_nHeight = rect.bottom;

	CDC *pDC = GetDC();
	m_dcMem.CreateCompatibleDC(pDC);
	m_bitmapMem.CreateCompatibleBitmap(pDC, m_nWidth, m_nHeight);
	ReleaseDC(pDC);

	m_penData.CreatePen(PS_SOLID, 0, COLOR_LEFT);
	m_penSel.CreatePen(PS_SOLID, 0, RGB(255, 128, 128));
	m_penBlack.CreatePen(PS_SOLID, 0, RGB(0, 0, 0));
	m_penGray.CreatePen(PS_SOLID, 0, RGB(128, 128, 128));
	m_penLightGray.CreatePen(PS_SOLID, 0, RGB(192, 192, 192));

	m_brushWhite.CreateSolidBrush(RGB(255, 255, 255));
	m_brushData.CreateSolidBrush(COLOR_DATA);
	m_brushSel.CreateSolidBrush(COLOR_SEL);

	CString str;
	LOGFONT font{
		.lfHeight = CDpi::OriginalDpi(90, 88),
		.lfCharSet = DEFAULT_CHARSET
	};
	str.LoadString(IDS_DEFAULTFONT);
	strcpy_s(font.lfFaceName, str);
	m_Font.CreatePointFontIndirect(&font, &m_dcMem);
	font.lfEscapement = 900;
	m_Font2.CreatePointFontIndirect(&font, &m_dcMem);

	m_dcMem.SetBkMode(TRANSPARENT);
	m_dcMem.SetTextColor(RGB(64, 64, 64));

	CFont *pFontOrg = m_dcMem.SelectObject(&m_Font);
	TEXTMETRIC tm;
	m_dcMem.GetTextMetrics(&tm);
	m_dcMem.SelectObject(pFontOrg);

	m_nScaleLeft = tm.tmAveCharWidth * 5 + tm.tmHeight;
	m_nScaleTop = 10;
	m_nScaleRight = m_nWidth - 5;
	m_nScaleBottom = m_nHeight - ((tm.tmHeight + 1) * 2 + 2);
	m_nScaleWidth = m_nScaleRight - m_nScaleLeft;
	m_nScaleHeight = m_nScaleBottom - m_nScaleTop;
	m_nScaleCenter = m_nScaleTop + m_nScaleHeight / 2;

	m_Rgn.CreateRectRgn(m_nScaleLeft, m_nScaleTop, m_nScaleRight + 1, m_nScaleBottom + 1);
}

void CWaveEditWnd::SetBitmap(double fStartX, double fDispX, double fMaxY, double fMinY)
{
	CRect rect;
	CString sText;
	CSize size;
	int x, y;
	double step, t, t1, t2;
	double fValue;
	int ns;
	const double fRangeY = fMaxY - fMinY;

	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
	CPen *pPenOrg = m_dcMem.SelectObject(&m_penBlack);
	CFont *pFontOrg = m_dcMem.SelectObject(&m_Font);

	m_nScaleZero = m_nScaleTop + m_nScaleHeight * fMaxY / fRangeY;

	rect.SetRect(0, 0, m_nWidth, m_nHeight);
	m_dcMem.FillRect(rect, &m_brushWhite);

	rect.SetRect(m_nScaleLeft, m_nScaleTop, m_nScaleRight, m_nScaleBottom);
	m_dcMem.SelectStockObject(NULL_BRUSH);
	m_dcMem.Rectangle(rect);

	t1 = fRangeY / 3 + fRangeY / 1000;
	step = pow(10.0, floor(log10(t1)));
	t2 = t1 / step;
	if (t2 < 2) {
		step *= 0.5;
		ns = 2;
	} else if (t2 < 5)
		ns = 2;
	else
		ns = 5;

	if (step == 0)
		step = 1;

	for (fValue = floor(fMinY / step) * step; fValue <= fMaxY + step * 1e-7; fValue += step) {
		y = (int)(m_nScaleZero - fValue * m_nScaleHeight / fRangeY + 0.5);

		if (y <= m_nScaleBottom) {
			if (int(fabs(fValue) / step + 0.5) % ns == 0) {
				if (y > m_nScaleTop && y < m_nScaleBottom) {
					m_dcMem.SelectObject(&m_penGray);
					m_dcMem.MoveTo(m_nScaleLeft + 1, y);
					m_dcMem.LineTo(m_nScaleRight - 1, y);
				}
				if (fabs(fValue) < step / 10)
					fValue = 0;
				sText.Format("%g", fValue);
				size = m_dcMem.GetOutputTextExtent(sText);
				m_dcMem.TextOut(m_nScaleLeft - size.cx - 2, y - size.cy / 2, sText);
			} else {
				if (y > m_nScaleTop && y < m_nScaleBottom) {
					m_dcMem.SelectObject(&m_penLightGray);
					m_dcMem.MoveTo(m_nScaleLeft + 1, y);
					m_dcMem.LineTo(m_nScaleRight - 1, y);
				}
			}
		}
	}

	t1 = fDispX / 7;
	step = pow(10.0, floor(log10(t1)));
	t2 = t1 / step;
	if (t2 < 2) {
		step *= 0.5;
		ns = 2;
	} else if (t2 < 5)
		ns = 2;
	else
		ns = 5;

	if (step == 0)
		step = 1;

	for (t = floor(fStartX / step) * step; t <= fStartX + fDispX + step * 1e-7; t += step) {
		x = m_nScaleLeft + int((t - fStartX) * m_nScaleWidth / fDispX + 0.5);
		if (x >= m_nScaleLeft) {
			if (int(t / step + 0.5) % ns == 0) {
				if (x > m_nScaleLeft && x < m_nScaleRight) {
					m_dcMem.SelectObject(&m_penGray);
					m_dcMem.MoveTo(x, m_nScaleTop + 1);
					m_dcMem.LineTo(x, m_nScaleBottom - 1);
				}
				sText.Format("%g", t);
				size = m_dcMem.GetOutputTextExtent(sText);
				m_dcMem.TextOut(x - size.cx / 2, m_nScaleBottom + 2, sText);
			} else {
				if (x > m_nScaleLeft && x < m_nScaleRight) {
					m_dcMem.SelectObject(&m_penLightGray);
					m_dcMem.MoveTo(x, m_nScaleTop + 1);
					m_dcMem.LineTo(x, m_nScaleBottom - 1);
				}
			}
		}
	}

	sText = "X";
	size = m_dcMem.GetOutputTextExtent(sText);
	m_dcMem.TextOut(m_nScaleLeft + (m_nScaleWidth - size.cx) / 2, m_nHeight - (size.cy + 1), sText);

	m_dcMem.SelectObject(&m_Font2);
	sText = "Y";
	size = m_dcMem.GetOutputTextExtent(sText);
	m_dcMem.TextOut(1, m_nScaleTop + (m_nScaleHeight + size.cx) / 2, sText);

	m_dcMem.SelectObject(pBitmapOrg);
	m_dcMem.SelectObject(pPenOrg);
	m_dcMem.SelectObject(pFontOrg);
}

void CWaveEditWnd::DispGraph(double fStartX, double fDispX, const WaveFormManual *pWaveForm, double fMaxY, double fMinY, int nSelPoint)
{
	int i;
	int x, y;
	const double fLevelRange = (fMaxY - fMinY);

	m_fStartX = fStartX;
	m_fDispX = fDispX;
	m_fMaxY = fMaxY;
	m_fMinY = fMinY;

	SetBitmap(fStartX, fDispX, fMaxY, fMinY);

	if (pWaveForm == NULL) {
		Invalidate(FALSE);
//		UpdateWindow();
		return;
	}

	m_dcMem.SelectClipRgn(&m_Rgn);

	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
	CPen *pPenOrg = m_dcMem.SelectObject(&m_penData);
	CBrush *pBrushOrg = m_dcMem.SelectObject(&m_brushData);

	POINT *pPoint = new POINT[pWaveForm->nDataNum];

	for (i = 0; i < pWaveForm->nDataNum; i++) {
		pPoint[i].x = (int)((pWaveForm->vertex[i].x - fStartX) * m_nScaleWidth / fDispX + 0.5) + m_nScaleLeft;
		pPoint[i].y = (int)(m_nScaleZero - pWaveForm->vertex[i].y * m_nScaleHeight / fLevelRange + 0.5);
	}

	if (pWaveForm->bSmoothing) {
		const int nPoint = pWaveForm->nDataNum;
		CSpline spl;
		double *xp = new double[nPoint];
		double *yp = new double[nPoint];

		for (i = 0; i < nPoint; i++) {
			xp[i] = pPoint[i].x;
			yp[i] = pPoint[i].y;
		}

		spl.MakeTable(xp, yp, nPoint);

		delete [] yp;
		delete [] xp;

		POINT *pSpline = new POINT[m_nScaleWidth];
		for (i = 0; i < m_nScaleWidth; i++) {
			pSpline[i].x = m_nScaleLeft + i;
			pSpline[i].y = (LONG)(spl.Spline(pSpline[i].x) + 0.5);
		}

		m_dcMem.Polyline(pSpline, m_nScaleWidth);

		delete [] pSpline;
	} else {
		if (pWaveForm->nDataNum >= 2)
			m_dcMem.Polyline(pPoint, pWaveForm->nDataNum);
	}

	for (i = 0; i < pWaveForm->nDataNum; i++) {
		x = pPoint[i].x;
		y = pPoint[i].y;

		if (i == nSelPoint) {
			m_dcMem.SelectObject(&m_penSel);
			m_dcMem.SelectObject(&m_brushSel);
			m_dcMem.Ellipse(x - 2, y - 2, x + 3, y + 3);
			m_dcMem.SelectObject(&m_penData);
			m_dcMem.SelectObject(&m_brushData);
		} else
			m_dcMem.Ellipse(x - 2, y - 2, x + 3, y + 3);
	}

	delete [] pPoint;

	m_dcMem.SelectClipRgn(NULL);

	m_dcMem.SelectObject(pBitmapOrg);
	m_dcMem.SelectObject(pPenOrg);
	m_dcMem.SelectObject(pBrushOrg);

	Invalidate(FALSE);
//	UpdateWindow();
}

void CWaveEditWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_nMouseDown == 0) {
		m_nMouseDown = 1;
		SetCapture();

		m_pWaveEditManual->MouseDown(GetVertexX(point.x), GetVertexY(point.y));
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void CWaveEditWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_nMouseDown != 0) {
		m_nMouseDown = 0;
		ReleaseCapture();
	}

	CWnd::OnLButtonUp(nFlags, point);
}

void CWaveEditWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_nMouseDown != 0) {
		if (m_nMouseDown == 1)
			m_nMouseDown++;
		else
			m_pWaveEditManual->MouseMove(GetVertexX(point.x), GetVertexY(point.y));
	}

	CWnd::OnMouseMove(nFlags, point);
}

float CWaveEditWnd::GetVertexX(int x) const
{
	return (float)((x - m_nScaleLeft) * m_fDispX / m_nScaleWidth + m_fStartX);
}

float CWaveEditWnd::GetVertexY(int y) const
{
	return (float)((m_nScaleZero - y) * (m_fMaxY - m_fMinY) / m_nScaleHeight);
}

void CWaveEditWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
	m_pointMenu = point;

	CMenu menu;
	menu.LoadMenu(IDR_WAVE_EDIT_MENU);
	CMenu* pPopup = menu.GetSubMenu(0);
	if (m_pWaveEditManual->GetEditMode() == EDIT_MODE_ADD)
		pPopup->EnableMenuItem(IDM_ADD, MF_GRAYED);
	if (m_pWaveEditManual->GetSelPoint() == -1)
		pPopup->EnableMenuItem(IDM_DELETE, MF_GRAYED);
	ClientToScreen(&point);
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);

	CWnd::OnRButtonDown(nFlags, point);
}

void CWaveEditWnd::OnAdd()
{
	m_pWaveEditManual->MouseAdd(GetVertexX(m_pointMenu.x), GetVertexY(m_pointMenu.y));
}

void CWaveEditWnd::OnDelete()
{
	m_pWaveEditManual->MouseDelete();
}
