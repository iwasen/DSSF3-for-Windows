// DstLevelWnd.cpp : 実装ファイル
//

#include "stdafx.h"
#include "Rta.h"
#include "DstLevelWnd.h"
#include "Spline.h"

// CDstLevelWnd

BEGIN_MESSAGE_MAP(CDstLevelWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CDstLevelWnd メッセージ ハンドラ

void CDstLevelWnd::Initialize()
{
	CString str;

	CDC *pDC = GetDC();
	m_dcMem.CreateCompatibleDC(pDC);
	ReleaseDC(pDC);

	m_penBlack.CreatePen(PS_SOLID, 0, COLOR_BLACK);
	m_penGray.CreatePen(PS_SOLID, 0, COLOR_GRAY);
	m_penLightGray.CreatePen(PS_SOLID, 0, COLOR_LIGHTGRAY);
	m_brushWhite.CreateSolidBrush(RGB(255, 255, 255));

	LOGFONT font{
		.lfHeight = CDpi::OriginalDpi(90, 88),
		.lfCharSet = DEFAULT_CHARSET
	};
	str.LoadString(IDS_DEFAULTFONT);
	strcpy_s(font.lfFaceName, str);
	m_Font.CreatePointFontIndirect(&font, &m_dcMem);
	font.lfEscapement = 900;
	m_Font2.CreatePointFontIndirect(&font, &m_dcMem);

	Initialize2();

	SetBitmap(g_oSetData.Dst.nLevelStart, g_oSetData.Dst.nLevelEnd);
}

void CDstLevelWnd::Initialize2()
{
	RECT rect;
	GetClientRect(&rect);

	m_nWidth = rect.right;
	m_nHeight = rect.bottom;

	CDC *pDC = GetDC();
	m_bitmapMem.CreateCompatibleBitmap(pDC, m_nWidth, m_nHeight);
	ReleaseDC(pDC);

	CFont *pFontOrg = m_dcMem.SelectObject(&m_Font);
	TEXTMETRIC tm;
	m_dcMem.GetTextMetrics(&tm);
	m_dcMem.SelectObject(pFontOrg);

	m_nScaleLeft = tm.tmAveCharWidth * 4 + tm.tmHeight + 6;
	m_nScaleTop = 10;
	m_nScaleRight = m_nWidth - 10;
	m_nScaleBottom = m_nHeight - ((tm.tmHeight + 1) * 2 + 4);
	m_nScaleWidth = m_nScaleRight - m_nScaleLeft;
	m_nScaleHeight = m_nScaleBottom - m_nScaleTop;

	m_Rgn.CreateRectRgn(0, m_nScaleTop, m_nWidth, m_nScaleBottom + 1);
}

void CDstLevelWnd::Uninitialize2()
{
	m_bitmapMem.DeleteObject();
	m_Rgn.DeleteObject();
}

void CDstLevelWnd::Resize()
{
	Uninitialize2();
	Initialize2();
}

void CDstLevelWnd::SetBitmap(int nLevelStart, int nLevelEnd)
{
	CFont *pFontOrg = m_dcMem.SelectObject(&m_Font);

	DrawScaleDst(m_nScaleLeft, m_nScaleTop, m_nScaleRight, m_nScaleBottom, nLevelStart, nLevelEnd);

	m_dcMem.SelectObject(pFontOrg);
}

void CDstLevelWnd::DrawScaleDst(int nLeft, int nTop, int nRight, int nBottom, int nLevelStart, int nLevelEnd)
{
	int x, y;
	int i;
	const int nWidth = nRight - nLeft;
	const int nHeight = nBottom - nTop;
	CPen *pPenOrg;
	CSize size;
	CString sText;
	double fTHD;
	int nTHD;
	CString sUnit;

	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
	pPenOrg = m_dcMem.SelectObject(&m_penBlack);

	CRect rect(0, 0, m_nWidth, m_nHeight);
	m_dcMem.FillRect(rect, &m_brushWhite);

	m_dcMem.SetTextColor(COLOR_TEXT);
	m_dcMem.SetBkMode(TRANSPARENT);

	m_dcMem.SelectObject(&m_Font);

	const int nLevelMin = min(nLevelStart, nLevelEnd);
	const int nLevelMax = max(nLevelStart, nLevelEnd);

	if (nLevelMin != nLevelMax) {
		const int nStep = GetLinearScaleStep(nLevelMax, nLevelMin, (int)(m_nScaleWidth * 0.6));
		for (int nLevel = GetScaleStartValue(nLevelMin, nStep); nLevel <= nLevelMax; nLevel += nStep) {
			x =  (int)((nLevel - nLevelMin) * m_nScaleWidth / (nLevelMax - nLevelMin) + m_nScaleLeft + 0.5);

			sText.Format("%d", nLevel);
			size = m_dcMem.GetOutputTextExtent(sText);
			m_dcMem.TextOut(x - size.cx / 2, m_nScaleBottom + 3, sText);

			if (x > m_nScaleLeft && x < m_nScaleRight) {
				m_dcMem.SelectObject(m_penGray);
				m_dcMem.MoveTo(x, m_nScaleTop);
				m_dcMem.LineTo(x, m_nScaleBottom);
			}
		}
	}

	sText.LoadString(IDS_LEVEL);
	sText += " [dB]";
	size = m_dcMem.GetOutputTextExtent(sText);
	m_dcMem.TextOut(nLeft + (nWidth - size.cx) / 2, m_nHeight - size.cy - 2, sText);

	if (g_oSetData.Dst.nScaleMode == 0) {
		const double fMinTHD = pow(10.0, g_oSetData.Dst.nScaleMin / 20);
		const double fMaxTHD = pow(10.0, g_oSetData.Dst.nScaleMax / 20);
		const int nDist = g_oSetData.Dst.nScaleMax - g_oSetData.Dst.nScaleMin;
		if (nDist != 0) {
			double fStep;
			for (fTHD = fMinTHD, i = 0; fTHD <= fMaxTHD * 1.0001; fTHD += fStep, i++) {
				y = m_nScaleBottom - (int)((dB20((double)fTHD) - g_oSetData.Dst.nScaleMin) * m_nScaleHeight / nDist);

				if (i % 9 == 0) {
					m_dcMem.SelectObject(&m_penGray);
					sText.Format("%1g", fTHD * 100);
					size = m_dcMem.GetOutputTextExtent(sText);
					m_dcMem.TextOut(nLeft - size.cx - 2, y - size.cy / 2, sText);
				} else
					m_dcMem.SelectObject(&m_penLightGray);

				if (y > m_nScaleTop && y < m_nScaleBottom) {
					m_dcMem.MoveTo(m_nScaleLeft + 1, y);
					m_dcMem.LineTo(m_nScaleRight, y);
				}

				fStep = pow(10.0, (int)(log10(fTHD) + 0.0001) - 1);
			}
		}
		sUnit = "[%]";
	} else {
		const int nDist = g_oSetData.Dst.nScaleMax - g_oSetData.Dst.nScaleMin;
		if (nDist != 0) {
			const int nStep = GetLinearScaleStep(g_oSetData.Dst.nScaleMax, g_oSetData.Dst.nScaleMin, m_nScaleHeight);
			for (nTHD = g_oSetData.Dst.nScaleMin, i = 0; nTHD <= g_oSetData.Dst.nScaleMax; nTHD += nStep, i++) {
				y = m_nScaleBottom - (int)((nTHD - g_oSetData.Dst.nScaleMin) * m_nScaleHeight / nDist);

				if (i % 2 == 0) {
					m_dcMem.SelectObject(&m_penGray);
					sText.Format("%d", nTHD);
					size = m_dcMem.GetOutputTextExtent(sText);
					m_dcMem.TextOut(nLeft - size.cx - 2, y - size.cy / 2, sText);
				} else
					m_dcMem.SelectObject(&m_penLightGray);

				if (y > m_nScaleTop && y < m_nScaleBottom) {
					m_dcMem.MoveTo(m_nScaleLeft, y);
					m_dcMem.LineTo(m_nScaleRight, y);
				}
			}
		}
		sUnit = "[dB]";
	}

	m_dcMem.SelectObject(&m_penBlack);
	CGdiObject *pBrushOrg = m_dcMem.SelectStockObject(NULL_BRUSH);
	m_dcMem.Rectangle(m_nScaleLeft, m_nScaleTop, m_nScaleRight + 1, m_nScaleBottom + 1);
	m_dcMem.SelectObject(pBrushOrg);

	m_dcMem.SelectObject(&m_Font2);
	sText.LoadString(IDS_DISTORTION);
	sText += " " + sUnit;
	size = m_dcMem.GetOutputTextExtent(sText);
	m_dcMem.TextOut(2, nTop + (nHeight + size.cx) / 2, sText);

	m_dcMem.SelectObject(pPenOrg);
	m_dcMem.SelectObject(pBitmapOrg);
}

void CDstLevelWnd::DispGraph(const double *pLeftDst, const double *pRightDst, const double *pLevel, int nLevelCount, int nLevelStart, int nLevelEnd, int nLevelPoint, BOOL bSetBitmap, COLORREF colorLeft, COLORREF colorRight)
{
	CPen *pPenOrg;
	CBrush *pBrushOrg;

	if (bSetBitmap)
		SetBitmap(nLevelStart, nLevelEnd);

	m_dcMem.SelectClipRgn(&m_Rgn);

	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);

	if (pLeftDst != NULL) {
		CPen penLeft;
		penLeft.CreatePen(PS_SOLID, 2, colorLeft);
		pPenOrg = m_dcMem.SelectObject(&penLeft);

		CBrush brushLeft;
		brushLeft.CreateSolidBrush(colorLeft);
		pBrushOrg = m_dcMem.SelectObject(&brushLeft);

		DrawLine(pLeftDst, pLevel, nLevelCount, nLevelStart, nLevelEnd, nLevelPoint);

		m_dcMem.SelectObject(pBrushOrg);
		m_dcMem.SelectObject(pPenOrg);
	}

	if (pRightDst != NULL) {
		CPen penRight;
		penRight.CreatePen(PS_SOLID, 2, colorRight);
		pPenOrg = m_dcMem.SelectObject(&penRight);

		CBrush brushRight;
		brushRight.CreateSolidBrush(colorRight);
		pBrushOrg = m_dcMem.SelectObject(&brushRight);

		DrawLine(pRightDst, pLevel, nLevelCount, nLevelStart, nLevelEnd, nLevelPoint);

		m_dcMem.SelectObject(pBrushOrg);
		m_dcMem.SelectObject(pPenOrg);
	}

	m_dcMem.SelectObject(pBitmapOrg);

	m_dcMem.SelectClipRgn(NULL);

	Invalidate(FALSE);
}

void CDstLevelWnd::DrawLine(const double *pData, const double *pLevel, int nLevelCount, int nLevelStart, int nLevelEnd, int nLevelPoint)
{
	int i;
	int x, y;
	int nDotSize;
	const int nLevelMin = min(nLevelStart, nLevelEnd);
	const int nLevelMax = max(nLevelStart, nLevelEnd);
	const double yStep = (double)m_nScaleHeight / (g_oSetData.Dst.nScaleMin - g_oSetData.Dst.nScaleMax);
	POINT *pPoint = new POINT[nLevelCount];

	nDotSize = (m_nScaleWidth / nLevelPoint < 10) ? 2 : 3;

	int nPointCnt = 0;
	for (i = 0; i < nLevelCount; i++) {
		if (pLevel[i] + 0.5 >= nLevelMin && pLevel[i] - 0.5 <= nLevelMax) {
			x = (int)(((pLevel[i] - nLevelMin) * m_nScaleWidth) / (nLevelMax - nLevelMin) + m_nScaleLeft + 0.5);
			y = (int)((dB10(pData[i]) - g_oSetData.Dst.nScaleMax) * yStep + 0.5) + m_nScaleTop;

			if (g_oSetData.Dst.bLevelMarker)
				m_dcMem.Ellipse(x - 2, y - 2, x + nDotSize, y + nDotSize);

			pPoint[i].x = x;
			pPoint[i].y = y;
			nPointCnt++;
		}
	}

	if (g_oSetData.Dst.bLevelSpline && nPointCnt >= 3) {
		CSpline spl;
		double *xp = new double[nPointCnt];
		double *yp = new double[nPointCnt];

		if (pPoint[0].x <= pPoint[1].x) {
			for (i = 0; i < nPointCnt; i++) {
				xp[i] = pPoint[i].x;
				yp[i] = pPoint[i].y;
			}
		} else {
			for (i = 0; i < nPointCnt; i++) {
				xp[i] = pPoint[nPointCnt - i - 1].x;
				yp[i] = pPoint[nPointCnt - i - 1].y;
			}
		}

		spl.MakeTable(xp, yp, nPointCnt);

		delete [] yp;
		delete [] xp;

		const int x1 = min(pPoint[0].x, pPoint[nPointCnt - 1].x);
		const int x2 = max(pPoint[0].x, pPoint[nPointCnt - 1].x);
		const int nSpline = x2 - x1;

		POINT *pSpline = new POINT[nSpline];
		for (i = 0; i < nSpline; i++) {
			pSpline[i].x = x1 + i;
			pSpline[i].y = (LONG)spl.Spline(pSpline[i].x);
		}

		m_dcMem.Polyline(pSpline, nSpline);

		delete [] pSpline;
	} else
		m_dcMem.Polyline(pPoint, nPointCnt);

	delete [] pPoint;
}

void CDstLevelWnd::OnPaint()
{
	CPaintDC dc(this);

	if (m_dcMem.m_hDC != NULL) {
		CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
		dc.BitBlt(0, 0, m_nWidth, m_nHeight, &m_dcMem, 0, 0, SRCCOPY);
		m_dcMem.SelectObject(pBitmapOrg);
	}
}

BOOL CDstLevelWnd::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void CDstLevelWnd::CsvOutput(CCsvFile &oCsvFile, const double *pLeftDst, const double *pRightDst, const double *pLevel, int nLevelCount)
{
	oCsvFile.SetData("THD");
	oCsvFile.SetData("Level Sweep");
	oCsvFile.Output();

	oCsvFile.SetData("Level[dB]");
	if (g_oSetData.Dst.nScaleMode == 0) {
		if (pRightDst == NULL)
			oCsvFile.SetData("THD[%]");
		else {
			oCsvFile.SetData("THD-L[%]");
			oCsvFile.SetData("THD-R[%]");
		}
	} else {
		if (pRightDst == NULL)
			oCsvFile.SetData("THD[dB]");
		else {
			oCsvFile.SetData("THD-L[dB]");
			oCsvFile.SetData("THD-R[dB]");
		}
	}
	oCsvFile.Output();

	for (int i = 0; i < nLevelCount; i++) {
		oCsvFile.SetData(*pLevel++, "%.1f");

		if (g_oSetData.Dst.nScaleMode == 0) {
			if (pLeftDst != NULL)
				oCsvFile.SetData(sqrt(pLeftDst[i]) * 100);

			if (pRightDst != NULL)
				oCsvFile.SetData(sqrt(pRightDst[i]) * 100);
		} else {
			if (pLeftDst != NULL)
				oCsvFile.SetData(dB10(pLeftDst[i]));

			if (pRightDst != NULL)
				oCsvFile.SetData(dB10(pRightDst[i]));
		}

		oCsvFile.Output();
	}
}

HBITMAP CDstLevelWnd::GetBitmap()
{
	return m_bitmapMem;
}
