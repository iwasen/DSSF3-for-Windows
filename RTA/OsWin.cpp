// OsWin.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Rta.h"
#include "OsWin.h"

#define COLOR_XY	RGB(0, 224, 64)

#pragma warning (disable : 4310)

/////////////////////////////////////////////////////////////////////////////
// COsWin

COsWin::COsWin()
{
	m_bInitialized = FALSE;
	m_hPenOrg = NULL;
	m_hFontOrg = NULL;
	m_hBitmapOrg = NULL;
	m_hBitmapOrg2 = NULL;
	m_hPenOrg2 = NULL;
	m_hBrushOrg2 = NULL;
	m_bTrigLevel = FALSE;
	m_nOverlayNum = 0;
	m_pOverlayBufL = NULL;
	m_pOverlayBufR = NULL;
	m_pOverlayBufXY = NULL;
	m_pOverlayCntL= NULL;
	m_pOverlayCntR= NULL;
	m_pOverlayCntXY= NULL;
	m_pPenLeft = NULL;
	m_pPenRight= NULL;
	m_pPenXY= NULL;
	m_pPosTbl = NULL;
	m_nOverlayWave = 0;
	m_nOverlayXY = 0;
	m_nChannel = 0;
	m_bLch = FALSE;
	m_bRch = FALSE;
}

BEGIN_MESSAGE_MAP(COsWin, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COsWin メッセージ ハンドラ

void COsWin::Initialize()
{
	if (m_bInitialized)
		return;

	m_bInitialized = TRUE;

	CDC *pDC = GetDC();
	m_dcMem.CreateCompatibleDC(pDC);
	m_dcMem2.CreateCompatibleDC(pDC);
	ReleaseDC(pDC);

	m_penBlack.CreatePen(PS_SOLID, 0, RGB(0, 0, 0));
	m_penGray.CreatePen(PS_SOLID, 0, RGB(128, 128, 128));
	m_penLightGray.CreatePen(PS_SOLID, 0, RGB(192, 192, 192));
	m_hPenOrg = (HPEN)m_dcMem.SelectObject(&m_penBlack)->m_hObject;

	m_penLeft.CreatePen(PS_SOLID, 0, COLOR_LEFT);
	m_penRight.CreatePen(PS_SOLID, 0, COLOR_RIGHT);
	m_hPenOrg2 = (HPEN)m_dcMem2.SelectObject(&m_penLeft)->m_hObject;

	m_penLeftZero.CreatePen(PS_SOLID, 0, COLOR_LEFT_ZERO);
	m_penRightZero.CreatePen(PS_SOLID, 0, COLOR_RIGHT_ZERO);

	m_brushWhite.CreateSolidBrush(RGB(255, 255, 255));
	m_brushGreen.CreateSolidBrush(RGB(0, 255, 0));
	m_hBrushOrg2 = (HBRUSH)m_dcMem2.SelectObject(&m_brushGreen)->m_hObject;

	CString str;
	str.LoadString(IDS_DEFAULTFONT);
	m_Font.CreatePointFont(90, str, &m_dcMem);
	m_hFontOrg = (HFONT)m_dcMem.SelectObject(&m_Font)->m_hObject;
	m_dcMem.SetBkMode(TRANSPARENT);
	m_dcMem.SetTextColor(RGB(64, 64, 64));

	Initialize2();
}

void COsWin::Initialize2()
{
	RECT rect;
	GetClientRect(&rect);

	m_nWidth = rect.right;
	m_nHeight = rect.bottom;

	m_nVDiv = 10;
	m_nScaleDiv = (m_nHeight - 60) / (m_nVDiv * 5) * 5;
	m_nHDiv = (m_nWidth - 40) / (m_nScaleDiv * 2) * 2;
	m_nScaleWidth = m_nScaleDiv * m_nHDiv;
	m_nScaleHeight = m_nScaleDiv * m_nVDiv;
	m_nScaleLeft = (m_nWidth - m_nScaleWidth) / 2;
	m_nScaleTop = (m_nHeight - m_nScaleHeight) * 2 / 3;
	m_nScaleRight = m_nScaleLeft + m_nScaleWidth;
	m_nScaleBottom = m_nScaleTop + m_nScaleHeight;
	m_nScaleHCenter = m_nScaleLeft + m_nScaleWidth / 2;
	m_nScaleVCenter = m_nScaleTop + m_nScaleHeight / 2;

	CDC *pDC = GetDC();
	m_bitmapMem.CreateCompatibleBitmap(pDC, m_nWidth, m_nHeight);
	m_hBitmapOrg = (HBITMAP)m_dcMem.SelectObject(&m_bitmapMem)->m_hObject;

	m_bitmapMem2.CreateCompatibleBitmap(pDC, m_nWidth, m_nHeight);
	m_hBitmapOrg2 = (HBITMAP)m_dcMem2.SelectObject(&m_bitmapMem2)->m_hObject;
	ReleaseDC(pDC);

	m_pPosTbl = new POSTBL[m_nWidth + 1]();

	m_Rgn.CreateRectRgn(m_nScaleLeft, 0, m_nScaleRight + 1, m_nHeight);

	SetBitmap();
	m_dcMem2.BitBlt(0, 0, m_nWidth, m_nHeight, &m_dcMem, 0, 0, SRCCOPY);
}

void COsWin::Uninitialize()
{
	Uninitialize2();

	if (m_hPenOrg != NULL)
		m_dcMem.SelectObject(CPen::FromHandle(m_hPenOrg));

	if (m_hFontOrg != NULL)
		m_dcMem.SelectObject(CFont::FromHandle(m_hFontOrg));

	if (m_hPenOrg2 != NULL)
		m_dcMem2.SelectObject(CPen::FromHandle(m_hPenOrg2));

	if (m_hBrushOrg2 != NULL)
		m_dcMem2.SelectObject(CBrush::FromHandle(m_hBrushOrg2));

	m_penBlack.DeleteObject();
	m_penGray.DeleteObject();
	m_penLightGray.DeleteObject();
	m_penLeft.DeleteObject();
	m_penRight.DeleteObject();
	m_penLeftZero.DeleteObject();
	m_penRightZero.DeleteObject();
	m_brushWhite.DeleteObject();
	m_brushGreen.DeleteObject();
	m_Font.DeleteObject();
	m_dcMem.DeleteDC();
	m_dcMem2.DeleteDC();

	m_bInitialized = FALSE;
}

void COsWin::Uninitialize2()
{
	if (m_hBitmapOrg != NULL)
		m_dcMem.SelectObject(CBitmap::FromHandle(m_hBitmapOrg));

	if (m_hBitmapOrg2 != NULL)
		m_dcMem2.SelectObject(CBitmap::FromHandle(m_hBitmapOrg2));

	m_bitmapMem.DeleteObject();
	m_bitmapMem2.DeleteObject();
	m_Rgn.DeleteObject();

	FreeOverlayBuf();

	if (m_pPosTbl != NULL) {
		delete [] m_pPosTbl;
		m_pPosTbl = NULL;
	}
}

void COsWin::Resize()
{
	Uninitialize2();
	Initialize2();
}

void COsWin::SetBitmap()
{
	CRect rect;
	int i;
	int x, y;

	rect.SetRect(0, 0, m_nWidth, m_nHeight);
	m_dcMem.FillRect(rect, &m_brushWhite);

	for (i = 1; i < m_nVDiv * 5; i++) {
		y = m_nScaleTop + i * m_nScaleDiv / 5;
		if (i == m_nVDiv / 2 * 5)
			m_dcMem.SelectObject(&m_penGray);
		else
			m_dcMem.SelectObject(&m_penLightGray);

		if (i % 5 == 0) {
			m_dcMem.MoveTo(m_nScaleLeft, y);
			m_dcMem.LineTo(m_nScaleRight, y);
		} else {
			m_dcMem.MoveTo(m_nScaleHCenter - 3, y);
			m_dcMem.LineTo(m_nScaleHCenter + 3, y);
		}
	}

	for (i = 1; i < m_nHDiv * 5; i++) {
		x = m_nScaleLeft + i * m_nScaleDiv / 5;
		if ((i - m_nHDiv / 2 * 5) % 25 == 0)
			m_dcMem.SelectObject(&m_penGray);
		else
			m_dcMem.SelectObject(&m_penLightGray);

		if (i % 5 == 0) {
			m_dcMem.MoveTo(x, m_nScaleTop);
			m_dcMem.LineTo(x, m_nScaleBottom);
		} else {
			m_dcMem.MoveTo(x, m_nScaleVCenter - 3);
			m_dcMem.LineTo(x, m_nScaleVCenter + 3);
		}
	}

	m_dcMem.SelectObject(&m_penBlack);
	CGdiObject *pBrushOrg = m_dcMem.SelectStockObject(NULL_BRUSH);
	m_dcMem.Rectangle(m_nScaleLeft, m_nScaleTop, m_nScaleRight, m_nScaleBottom);
	m_dcMem.SelectObject(pBrushOrg);
}

void COsWin::OnPaint()
{
	CPaintDC dc(this); // 描画用のデバイス コンテキスト

	dc.BitBlt(0, 0, m_nWidth, m_nHeight, &m_dcMem2, 0, 0, SRCCOPY);

	if (!g_oSetData.Os.bXY && g_oSetData.Os.bZeroLevel)
		DispCenterLine(dc);
}

void COsWin::DispCenterLine(CDC &dc)
{
	CPen *pPenOrg = dc.SelectObject(&m_penLeftZero);

	if (m_bLch) {
		int nLeftPos = m_nScaleVCenter - (g_oSetData.Os.nPosL * m_nScaleHeight / 100);
		dc.MoveTo(m_nScaleLeft, nLeftPos);
		dc.LineTo(m_nScaleRight, nLeftPos);
	}

	if (m_bRch) {
		int nRightPos = m_nScaleVCenter - (g_oSetData.Os.nPosR * m_nScaleHeight / 100);
		dc.SelectObject(&m_penRightZero);
		dc.MoveTo(m_nScaleLeft, nRightPos);
		dc.LineTo(m_nScaleRight, nRightPos);
	}

	dc.SelectObject(pPenOrg);
}

void COsWin::DispScreen(const double *pLeftData, const double *pRightData, double fTotal, int nStart, int nData,
					int nChannel, double fLevelL, double fLevelR, int nPosL, int nPosR, double fOffset)
{
	int i;
	int x;
	int y;
	POINT *pPoint;
	int nPoint;
	int nMin = 0, nMax = 0;
	int x2;
	int s, e;

	if ((m_pOverlayBufL == NULL && m_pOverlayBufR == NULL) || g_oSetData.Os.nOverlay != m_nOverlayWave || nChannel != m_nChannel) {
		m_nOverlayWave = g_oSetData.Os.nOverlay;
		m_nChannel = nChannel;
		m_bLch = (nChannel != CH_RIGHT_ONLY);
		m_bRch = (nChannel == CH_STEREO || nChannel == CH_RIGHT_ONLY);

		AllocOverlayWave();
		ResetOverlay();
	}

	int xMin = m_nScaleLeft + (int)((nStart + fOffset) * m_nScaleWidth / fTotal + 0.5);
	int xMax = m_nScaleLeft + (int)((nStart + nData + fOffset) * m_nScaleWidth / fTotal + 0.5);
	int yMin = m_nHeight;
	int yMax = 0;

	for (x = xMin; x <= xMax; x++) {
		if (x >= 0 && x < m_nWidth) {
			y = m_pPosTbl[x].yMin;
			if (y < yMin)
				yMin = y;

			y = m_pPosTbl[x].yMax;
			if (y > yMax)
				yMax = y;

			m_pPosTbl[x].yMin = m_nHeight;
			m_pPosTbl[x].yMax = 0;
		}
	}

	m_dcMem2.SelectClipRgn(&m_Rgn);

	if (g_oSetData.Os.nOverlay != 0 && yMax > yMin)
		m_dcMem2.BitBlt(xMin, yMin, xMax - xMin + 1, yMax - yMin + 1, &m_dcMem, xMin, yMin, SRCCOPY);

	if (nStart <= 0)
		m_nOverlayPos = (m_nOverlayPos + 1) % m_nOverlayNum;

	if (m_bLch) {
		pPoint = &m_pOverlayBufL[m_nOverlaySize * m_nOverlayPos];
		nPoint = (nStart <= 0) ? 0 : m_pOverlayCntL[m_nOverlayPos];

		x2 = 0;
		for (i = 0; i < nData && x2 <= m_nScaleRight; i++) {
			x = m_nScaleLeft + (int)((nStart + i + fOffset) * m_nScaleWidth / fTotal + 0.5);
			y = m_nScaleVCenter - ((int)(pLeftData[i] * m_nScaleDiv / fLevelL) + (m_nScaleHeight * nPosL / 100));

			if (x != x2) {
				if (x2 != 0) {
					pPoint[nPoint].x = x2;
					pPoint[nPoint].y = nMin;
					nPoint++;
					if (nMin != nMax) {
						pPoint[nPoint].x = x2;
						pPoint[nPoint].y = nMax;
						nPoint++;
					}
				}

				x2 = x;
				nMin = nMax = y;
			} else {
				if (y < nMin)
					nMin = y;
				if (y > nMax)
					nMax = y;
			}

			if (y < yMin)
				yMin = y;
			if (y > yMax)
				yMax = y;
		}

		m_pOverlayCntL[m_nOverlayPos] = nPoint;
	}

	if (m_bRch) {
		pPoint = &m_pOverlayBufR[m_nOverlaySize * m_nOverlayPos];
		nPoint = (nStart <= 0) ? 0 : m_pOverlayCntR[m_nOverlayPos];

		x2 = 0;
		for (i = 0; i < nData && x2 <= m_nScaleRight; i++) {
			x = m_nScaleLeft + (int)((nStart + i + fOffset) * m_nScaleWidth / fTotal + 0.5);
			y = m_nScaleVCenter - ((int)(pRightData[i] * m_nScaleDiv / fLevelR) + (m_nScaleHeight * nPosR / 100));

			if (x != x2) {
				if (x2 != 0) {
					pPoint[nPoint].x = x2;
					pPoint[nPoint].y = nMin;
					nPoint++;
					if (nMin != nMax) {
						pPoint[nPoint].x = x2;
						pPoint[nPoint].y = nMax;
						nPoint++;
					}
				}

				x2 = x;
				nMin = nMax = y;
			} else {
				if (y < nMin)
					nMin = y;
				if (y > nMax)
					nMax = y;
			}

			if (y < yMin)
				yMin = y;
			if (y > yMax)
				yMax = y;
		}

		m_pOverlayCntR[m_nOverlayPos] = nPoint;
	}

	int nPos = m_nOverlayPos;
	for (i = 0; i < m_nOverlayNum; i++) {
		nPos = (nPos + 1) % m_nOverlayNum;

		if (m_bLch) {
			nPoint = m_pOverlayCntL[nPos];
			pPoint = &m_pOverlayBufL[m_nOverlaySize * nPos];
			for (s = 1; s < nPoint - 1; s++) {
				if (pPoint[s].x >= xMin)
					break;
			}
			s--;
			for (e = s; e < nPoint; e++) {
				x = pPoint[e].x;
				y = pPoint[e].y;
				if (y < m_pPosTbl[x].yMin)
					m_pPosTbl[x].yMin = y;
				if (y > m_pPosTbl[x].yMax)
					m_pPosTbl[x].yMax = y;

				if (pPoint[e].x >= xMax)
					break;
			}

			m_dcMem2.SelectObject(&m_pPenLeft[i]);
			m_dcMem2.Polyline(&pPoint[s], e - s);
		}

		if (m_bRch) {
			nPoint = m_pOverlayCntR[nPos];
			pPoint = &m_pOverlayBufR[m_nOverlaySize * nPos];
			for (s = 1; s < nPoint - 1; s++) {
				if (pPoint[s].x >= xMin)
					break;
			}
			s--;
			for (e = s; e < nPoint; e++) {
				x = pPoint[e].x;
				y = pPoint[e].y;
				if (y < m_pPosTbl[x].yMin)
					m_pPosTbl[x].yMin = y;
				if (y > m_pPosTbl[x].yMax)
					m_pPosTbl[x].yMax = y;

				if (pPoint[e].x >= xMax)
					break;
			}

			m_dcMem2.SelectObject(&m_pPenRight[i]);
			m_dcMem2.Polyline(&pPoint[s], e - s);
		}
	}

	m_dcMem2.SelectClipRgn(NULL);

	RECT rect{
		.left = xMin - 1,
		.top = yMin,
		.right = xMax + 1,
		.bottom = yMax + 1
	};
	InvalidateRect(&rect, FALSE);
}

void COsWin::DispXY(const double *pData, int nData, double fLevelL, double fLevelR, int nPosL, int nPosR)
{
	int i, j;
	int x;
	int y;
	POINT *pPoint;
	int nPoint;
	int nPos;

	if (m_pOverlayBufXY == NULL || g_oSetData.Os.nOverlay != m_nOverlayXY || m_nOverlaySize != nData) {
		AllocOverlayXY(nData);
		ResetOverlay();
		m_nOverlayXY = g_oSetData.Os.nOverlay;
	}

	int xMin = m_nHeight;
	int xMax = 0;
	int yMin = m_nHeight;
	int yMax = 0;

	for (i = 0; i < m_nOverlayNum; i++) {
		pPoint = &m_pOverlayBufXY[i * m_nOverlaySize];
		for (j = 0; j < m_pOverlayCntXY[i]; j++) {
			x = pPoint[j].x;
			y = pPoint[j].y;
			if (x < xMin)
				xMin = x;
			if (x > xMax)
				xMax= x;
			if (y < yMin)
				yMin = y;
			if (y > yMax)
				yMax = y;
		}
	}

	if (g_oSetData.Os.nOverlay != 0 && yMax > yMin)
		m_dcMem2.BitBlt(xMin, yMin, xMax - xMin + 1, yMax - yMin + 1, &m_dcMem, xMin, yMin, SRCCOPY);

	m_nOverlayPos = (m_nOverlayPos + 1) % m_nOverlayNum;
	pPoint = &m_pOverlayBufXY[m_nOverlaySize * m_nOverlayPos];

	for (i = 0; i < nData; i++) {
		pPoint[i].x = x = m_nScaleHCenter + ((int)(*pData++ * m_nScaleDiv / fLevelL) + (m_nScaleHeight * nPosL / 100));
		pPoint[i].y = y = m_nScaleVCenter - ((int)(*pData++ * m_nScaleDiv / fLevelR) + (m_nScaleHeight * nPosR / 100));

		if (x < xMin)
			xMin = x;
		if (x > xMax)
			xMax= x;
		if (y < yMin)
			yMin = y;
		if (y > yMax)
			yMax = y;
	}

	m_pOverlayCntXY[m_nOverlayPos] = nData;

	nPos = m_nOverlayPos;
	for (i = 0; i < m_nOverlayNum; i++) {
		nPos = (nPos + 1) % m_nOverlayNum;
		nPoint = m_pOverlayCntXY[nPos];
		pPoint = &m_pOverlayBufXY[m_nOverlaySize * nPos];

		m_dcMem2.SelectObject(&m_pPenXY[i]);
		m_dcMem2.Polyline(pPoint, nPoint);
	}

	xMax++;
	yMax++;

	RECT rect{
		.left = min(xMin, xMin),
		.top = min(yMin, yMin),
		.right = max(xMax, xMax),
		.bottom = max(yMax, yMax)
	};
	InvalidateRect(&rect, FALSE);
}

void COsWin::ClearScreen()
{
	CRect rect;

	rect.SetRect(0, 0, m_nWidth, m_nScaleTop);
	m_dcMem.FillRect(rect, &m_brushWhite);

	m_dcMem2.BitBlt(0, 0, m_nWidth, m_nHeight, &m_dcMem, 0, 0, SRCCOPY);
	Invalidate(FALSE);

	ResetOverlay();
}

void COsWin::DispTriggerLevel(int nTrigLevel, int nChannel, BOOL bDraw)
{
	int y;

	if (m_bTrigLevel) {
		y = m_nScaleVCenter - m_nTrigLevel * m_nScaleHeight / 100;
		RECT rect{
			.left = m_nScaleLeft - 10,
			.top = y - 5,
			.right = m_nScaleLeft,
			.bottom = y + 5
		};
		m_dcMem2.BitBlt(rect.left, rect.top, 10, 10, &m_dcMem, rect.left, rect.top, SRCCOPY);
		InvalidateRect(&rect, FALSE);
	}

	if (bDraw) {
		y = m_nScaleVCenter - nTrigLevel * m_nScaleHeight / 100;
		RECT rect{
			.left = m_nScaleLeft - 10,
			.top = y - 5,
			.right = m_nScaleLeft,
			.bottom = y + 5
		};
		POINT point[3]{
			{
				.x = rect.left + 1,
				.y = rect.top + 1
			},{
				.x = rect.right - 1,
				.y = y
			},{
				.x = rect.left + 1,
				.y = rect.bottom - 1
			}
		};
		m_dcMem2.SelectObject(nChannel == 0 ? &m_penLeft : &m_penRight);
		m_dcMem2.SelectObject(&m_brushGreen);
		m_dcMem2.Polygon(point, 3);
		InvalidateRect(&rect, FALSE);
	}

	UpdateWindow();

	m_nTrigLevel = nTrigLevel;
	m_bTrigLevel = bDraw;
}

void COsWin::DispText(LPCTSTR text, int nLine)
{
	CRect rect;
	CSize size;

	size = m_dcMem.GetOutputTextExtent(text);
	int y = (size.cy + 2) * nLine + 6;

	rect.SetRect(m_nScaleLeft, y, m_nWidth, y + size.cy);
	m_dcMem.FillRect(rect, &m_brushWhite);

	m_dcMem.TextOut(m_nScaleLeft, y, text);

	m_dcMem2.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &m_dcMem, rect.left, rect.top, SRCCOPY);
	InvalidateRect(&rect, FALSE);
}

BOOL COsWin::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void COsWin::OnDestroy()
{
	Uninitialize();

	CWnd::OnDestroy();
}

void COsWin::ResetOverlay()
{
	m_nOverlayPos = 0;

	if (m_pOverlayCntL != NULL)
		MEMCLEAR(m_pOverlayCntL, m_nOverlayNum);

	if (m_pOverlayCntR != NULL)
		MEMCLEAR(m_pOverlayCntR, m_nOverlayNum);

	if (m_pOverlayCntXY != NULL)
		MEMCLEAR(m_pOverlayCntXY, m_nOverlayNum);

	m_dcMem2.BitBlt(m_nScaleLeft, 0, m_nScaleWidth, m_nHeight, &m_dcMem, m_nScaleLeft, 0, SRCCOPY);
	Invalidate(FALSE);
}

void COsWin::AllocOverlayWave()
{
	int i;
	int r, g, b;
	double t;

	FreeOverlayBuf();

	m_nOverlaySize = m_nScaleWidth * 2 + 2;
	m_nOverlayNum = max(g_oSetData.Os.nOverlay, 1);

	if (m_bLch) {
		m_pOverlayBufL = new POINT [m_nOverlaySize * m_nOverlayNum];
		m_pOverlayCntL = new int[m_nOverlayNum];

		m_pPenLeft = new CPen[m_nOverlayNum];
		for (i = 0; i < m_nOverlayNum; i++) {
			t = pow(2.0, (double)(i - m_nOverlayNum + 1) * 2.5 / m_nOverlayNum);
			r = 255 - (int)((255 - GetRValue(COLOR_LEFT)) * t);
			g = 255 - (int)((255 - GetGValue(COLOR_LEFT)) * t);
			b = 255 - (int)((255 - GetBValue(COLOR_LEFT)) * t);
			m_pPenLeft[i].CreatePen(PS_SOLID, 0, RGB(r, g, b));
		}
	}

	if (m_bRch) {
		m_pOverlayBufR = new POINT[m_nOverlaySize * m_nOverlayNum];
		m_pOverlayCntR= new int[m_nOverlayNum];

		m_pPenRight = new CPen[m_nOverlayNum];
		for (i = 0; i < m_nOverlayNum; i++) {
			t = pow(2.0, (double)(i - m_nOverlayNum + 1) * 2.5 / m_nOverlayNum);
			r = 255 - (int)((255 - GetRValue(COLOR_RIGHT)) * t);
			g = 255 - (int)((255 - GetGValue(COLOR_RIGHT)) * t);
			b = 255 - (int)((255 - GetBValue(COLOR_RIGHT)) * t);
			m_pPenRight[i].CreatePen(PS_SOLID, 0, RGB(r, g, b));
		}
	}
}

void COsWin::AllocOverlayXY(int nData)
{
	int r, g, b;
	double t;

	FreeOverlayBuf();

	m_nOverlaySize = nData;
	m_nOverlayNum = max(g_oSetData.Os.nOverlay, 1);

	m_pOverlayBufXY = new POINT [m_nOverlaySize * m_nOverlayNum];
	m_pOverlayCntXY = new int[m_nOverlayNum];

	m_pPenXY = new CPen[m_nOverlayNum];
	for (int i = 0; i < m_nOverlayNum; i++) {
		t = pow(2.0, (double)(i - m_nOverlayNum + 1) * 3 / m_nOverlayNum);
		r = 255 - (int)((255 - GetRValue(COLOR_XY)) * t);
		g = 255 - (int)((255 - GetGValue(COLOR_XY)) * t);
		b = 255 - (int)((255 - GetBValue(COLOR_XY)) * t);
		m_pPenXY[i].CreatePen(PS_SOLID, 0, RGB(r, g, b));
	}
}

void COsWin::FreeOverlayBuf()
{
	if (m_pOverlayBufL != NULL) {
		delete [] m_pOverlayBufL;
		m_pOverlayBufL = NULL;
	}

	if (m_pOverlayCntL != NULL) {
		delete [] m_pOverlayCntL;
		m_pOverlayCntL = NULL;
	}

	if (m_pOverlayBufR != NULL) {
		delete [] m_pOverlayBufR;
		m_pOverlayBufR = NULL;
	}

	if (m_pOverlayCntR != NULL) {
		delete [] m_pOverlayCntR;
		m_pOverlayCntR = NULL;
	}

	if (m_pOverlayBufXY != NULL) {
		delete [] m_pOverlayBufXY;
		m_pOverlayBufXY = NULL;
	}

	if (m_pOverlayCntXY != NULL) {
		delete [] m_pOverlayCntXY;
		m_pOverlayCntXY = NULL;
	}

	if (m_pPenLeft != NULL) {
		delete [] m_pPenLeft;
		m_pPenLeft = NULL;
	}

	if (m_pPenRight != NULL) {
		delete [] m_pPenRight;
		m_pPenRight = NULL;
	}

	if (m_pPenXY != NULL) {
		delete [] m_pPenXY;
		m_pPenXY = NULL;
	}
}

HBITMAP COsWin::GetBitmap()
{
	CDC *pDC = GetDC();
	CDC dc;
	CBitmap bmp;

	dc.CreateCompatibleDC(pDC);
	bmp.CreateCompatibleBitmap(pDC, m_nWidth, m_nHeight);
	CBitmap *pBitmapOrg = dc.SelectObject(&bmp);
	dc.BitBlt(0, 0, m_nWidth, m_nHeight, &m_dcMem2, 0, 0, SRCCOPY);
	DispCenterLine(dc);
	dc.SelectObject(pBitmapOrg);
	ReleaseDC(pDC);

	return (HBITMAP)bmp.Detach();
}
