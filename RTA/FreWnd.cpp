// FreWnd.cpp : 実装ファイル
//

#include "stdafx.h"
#include "Rta.h"
#include "FreWnd.h"

// CFreWnd

BEGIN_MESSAGE_MAP(CFreWnd, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()

// CFreWnd メッセージ ハンドラ

BOOL CFreWnd::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void CFreWnd::OnPaint()
{
	CPaintDC dc(this);

	if (m_dcMem.m_hDC != NULL) {
		CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
		dc.BitBlt(0, 0, m_nWidth, m_nHeight, &m_dcMem, 0, 0, SRCCOPY);
		m_dcMem.SelectObject(pBitmapOrg);
	}
}

void CFreWnd::Initialize(int nMode, int nFreqStart, int nFreqEnd)
{
	CString str;

	m_nMode = nMode;

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

	SetBitmap(nFreqStart, nFreqEnd);
}

void CFreWnd::Initialize2()
{
	CFont *pFontOrg = m_dcMem.SelectObject(&m_Font);
	TEXTMETRIC tm;
	m_dcMem.GetTextMetrics(&tm);
	m_dcMem.SelectObject(pFontOrg);

	RECT rect;
	GetClientRect(&rect);

	m_nWidth = rect.right;
	m_nHeight = rect.bottom;

	m_nScaleLeft = tm.tmAveCharWidth * 4 + tm.tmHeight + 6;
	m_nScaleTop = 10;
	m_nScaleRight = m_nWidth - 10;
	m_nScaleBottom = m_nHeight - ((tm.tmHeight + 1) * 2 + 4);
	m_nScaleWidth = m_nScaleRight - m_nScaleLeft;
	m_nScaleHeight = m_nScaleBottom - m_nScaleTop;

	CDC *pDC = GetDC();
	m_bitmapMem.CreateCompatibleBitmap(pDC, m_nWidth, m_nHeight);
	ReleaseDC(pDC);

	if (m_nMode != 2)
		m_Rgn.CreateRectRgn(0, m_nScaleTop, m_nWidth, m_nScaleBottom + 1);
	else
		m_Rgn.CreateRectRgn(m_nScaleLeft, m_nScaleTop, m_nScaleRight, m_nScaleBottom + 1);
}

void CFreWnd::Uninitialize2()
{
	m_bitmapMem.DeleteObject();
	m_Rgn.DeleteObject();
}

void CFreWnd::Resize()
{
	Uninitialize2();
	Initialize2();
}

void CFreWnd::SetBitmap(int nFreqStart, int nFreqEnd)
{
	DrawScale(m_nScaleLeft, m_nScaleTop, m_nScaleRight, m_nScaleBottom, nFreqStart, nFreqEnd);
}

void CFreWnd::DrawScale(int nLeft, int nTop, int nRight, int nBottom, int nFreqStart, int nFreqEnd)
{
	int x, y;
	const int nWidth = nRight - nLeft;
	const int nHeight = nBottom - nTop;
	CPen *pPenOrg;
	CSize size;
	CString sText;
	CRect rect;
	int nLevel;

	CFont *pFontOrg = m_dcMem.SelectObject(&m_Font);
	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
	pPenOrg = m_dcMem.SelectObject(&m_penBlack);

	rect.SetRect(0, 0, m_nWidth, m_nHeight);
	m_dcMem.FillRect(rect, &m_brushWhite);

	m_dcMem.SetTextColor(COLOR_TEXT);
	m_dcMem.SetBkMode(TRANSPARENT);

	m_dcMem.SelectObject(&m_Font);

	const int nFreqMin = min(nFreqStart, nFreqEnd);
	const int nFreqMax = max(nFreqStart, nFreqEnd);
	const double fLogFreqMin = log((double)nFreqMin);
	const double fLogFreqMax = log((double)nFreqMax);

	const double fDist = fLogFreqMax - fLogFreqMin;
	if (fDist != 0) {
		const int step = GetLogScaleStep(nFreqMin);
		for (int nFreq = GetScaleStartValue(nFreqMin, step); nFreq <= nFreqMax; nFreq += GetLogScaleStep(nFreq)) {
			x =  (int)((log((double)nFreq) - fLogFreqMin) * m_nScaleWidth / fDist + m_nScaleLeft + 0.5);

			if (nFreq < 1000)
				sText.Format("%d", nFreq);
			else
				sText.Format("%gk", (double)nFreq / 1000);
			if (sText[0] == '1' || sText[0] == '2' || sText[0] == '5') {
				size = m_dcMem.GetOutputTextExtent(sText);
				m_dcMem.TextOut(x - size.cx / 2, m_nScaleBottom + 3, sText);
			}

			if (x > m_nScaleLeft && x < m_nScaleRight) {
				m_dcMem.SelectObject(sText[0] == '1' ? m_penGray : m_penLightGray);
				m_dcMem.MoveTo(x, m_nScaleTop);
				m_dcMem.LineTo(x, m_nScaleBottom);
			}
		}
	}

	sText.LoadString(IDS_FREQUENCY);
	sText += " [Hz]";
	size = m_dcMem.GetOutputTextExtent(sText);
	m_dcMem.TextOut(nLeft + (nWidth - size.cx) / 2, m_nHeight - size.cy - 2, sText);

	const int nDist = g_oSetData.Fre.nMaxLevel - g_oSetData.Fre.nMinLevel;
	if (nDist > 0) {
		const int nStep = GetLinearScaleStep(g_oSetData.Fre.nMaxLevel, g_oSetData.Fre.nMinLevel, m_nScaleHeight);
		for (nLevel = GetScaleStartValue(g_oSetData.Fre.nMinLevel, nStep); nLevel <= g_oSetData.Fre.nMaxLevel; nLevel += nStep) {
			y = m_nScaleBottom - (int)((nLevel - g_oSetData.Fre.nMinLevel) * m_nScaleHeight / nDist);

			if (nLevel / 10 * 10 == nLevel) {
				m_dcMem.SelectObject(&m_penGray);
				sText.Format("%d", nLevel);
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

	m_dcMem.SelectObject(&m_penBlack);
	CGdiObject *pBrushOrg = m_dcMem.SelectStockObject(NULL_BRUSH);
	m_dcMem.Rectangle(m_nScaleLeft, m_nScaleTop, m_nScaleRight + 1, m_nScaleBottom + 1);
	m_dcMem.SelectObject(pBrushOrg);

	m_dcMem.SelectObject(&m_Font2);
	sText.LoadString(IDS_LEVEL);
	sText += " [dB]";
	size = m_dcMem.GetOutputTextExtent(sText);
	m_dcMem.TextOut(2, nTop + (nHeight + size.cx) / 2, sText);

	m_dcMem.SelectObject(pFontOrg);
	m_dcMem.SelectObject(pPenOrg);
	m_dcMem.SelectObject(pBitmapOrg);
}

void CFreWnd::DispGraph(const double *pLeftFre, const double *pRightFre, const double *pFreq, int nFreqCount, int nFreqStart, int nFreqEnd, int nFreqPoint, BOOL bSetBitmap, COLORREF colorLeft, COLORREF colorRight)
{
	CPen *pPenOrg;
	CBrush *pBrushOrg;

	if (bSetBitmap)
		SetBitmap(nFreqStart, nFreqEnd);

	m_dcMem.SelectClipRgn(&m_Rgn);

	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);

	if (pLeftFre != NULL) {
		CPen penLeft;
		penLeft.CreatePen(PS_SOLID, 2, colorLeft);
		pPenOrg = m_dcMem.SelectObject(&penLeft);

		CBrush brushLeft;
		brushLeft.CreateSolidBrush(colorLeft);
		pBrushOrg = m_dcMem.SelectObject(&brushLeft);

		if (m_nMode != 2)
			DrawLine(pLeftFre, pFreq, nFreqCount, nFreqStart, nFreqEnd, nFreqPoint);
		else
			DrawLine2(pLeftFre, pFreq, nFreqCount, nFreqStart, nFreqEnd);

		m_dcMem.SelectObject(pBrushOrg);
		m_dcMem.SelectObject(pPenOrg);
	}

	if (pRightFre != NULL) {
		CPen penRight;
		penRight.CreatePen(PS_SOLID, 2, colorRight);
		pPenOrg = m_dcMem.SelectObject(&penRight);

		CBrush brushRight;
		brushRight.CreateSolidBrush(colorRight);
		pBrushOrg = m_dcMem.SelectObject(&brushRight);

		if (m_nMode != 2)
			DrawLine(pRightFre, pFreq, nFreqCount, nFreqStart, nFreqEnd, nFreqPoint);
		else
			DrawLine2(pRightFre, pFreq, nFreqCount, nFreqStart, nFreqEnd);

		m_dcMem.SelectObject(pBrushOrg);
		m_dcMem.SelectObject(pPenOrg);
	}

	m_dcMem.SelectObject(pBitmapOrg);

	m_dcMem.SelectClipRgn(NULL);

	Invalidate(FALSE);
}

void CFreWnd::DrawLine(const double *pData, const double *pFreq, int nFreqCount, int nFreqStart, int nFreqEnd, int nFreqPoint)
{
	int x, y;
	int nDotSize;
	const int nFreqMin = min(nFreqStart, nFreqEnd);
	const int nFreqMax = max(nFreqStart, nFreqEnd);
	const double fLogFreqMin = log((double)nFreqMin);
	const double fLogFreqMax = log((double)nFreqMax);
	const double yStep = (double)m_nScaleHeight / (g_oSetData.Fre.nMinLevel - g_oSetData.Fre.nMaxLevel);
	POINT *pPoint = new POINT[nFreqCount];

	if (nFreqPoint != 0)
		nDotSize = (m_nScaleWidth / nFreqPoint < 10) ? 2 : 3;
	else
		nDotSize = 0;

	int nPointCnt = 0;
	int x2 = 0;
	int y2 = 0;
	for (int i = 0; i < nFreqCount; i++) {
		if (pFreq[i] + 0.5 >= nFreqMin && pFreq[i] - 0.5 <= nFreqMax) {
			x =  (int)((log((double)pFreq[i]) - fLogFreqMin) * m_nScaleWidth / (fLogFreqMax - fLogFreqMin) + m_nScaleLeft + 0.5);
			y = (int)((dB10(pData[i]) - g_oSetData.Fre.nMaxLevel) * yStep + 0.5) + m_nScaleTop;

			if (nDotSize > 0)
				m_dcMem.Ellipse(x - 2, y - 2, x + nDotSize, y + nDotSize);

			if (x != x2 || y != y2) {
				pPoint[nPointCnt].x = x;
				pPoint[nPointCnt].y = y;
				x2 = x;
				y2 = y;
				nPointCnt++;
			}
		}
	}

	m_dcMem.Polyline(pPoint, nPointCnt);

	delete [] pPoint;
}

void CFreWnd::DrawLine2(const double *pData, const double *pFreq, int nFreqCount, int nFreqStart, int nFreqEnd)
{
	int x, y;
	int nFreqMin = min(nFreqStart, nFreqEnd);
	int nFreqMax = max(nFreqStart, nFreqEnd);
	const double fLogFreqMin = log((double)nFreqMin);
	const double fLogFreqMax = log((double)nFreqMax);
	const double yStep = (double)m_nScaleHeight / (g_oSetData.Fre.nMinLevel - g_oSetData.Fre.nMaxLevel);
	POINT *pPoint = new POINT[nFreqCount];

	int nPointCnt = 0;
	int x2 = 0;
	double fData = 0;
	int nAddCount = 0;
	for (int i = 0; i < nFreqCount; i++) {
		x =  (int)((log((double)pFreq[i]) - fLogFreqMin) * m_nScaleWidth / (fLogFreqMax - fLogFreqMin) + m_nScaleLeft + 0.5);
		if (i == 0)
			x2 = x;
		else if (i == nFreqCount - 1) {
			x2 = x;
			x = 0;
		}

		fData += *pData++;
		nAddCount++;

		if (x != x2) {
			y = (int)((dB10(fData / nAddCount) - g_oSetData.Fre.nMaxLevel) * yStep + 0.5) + m_nScaleTop;
			pPoint[nPointCnt].x = x2;
			pPoint[nPointCnt].y = y;
			nPointCnt++;
			x2 = x;
			fData = 0;
			nAddCount = 0;
		}
	}

	m_dcMem.Polyline(pPoint, nPointCnt);

	delete [] pPoint;
}

void CFreWnd::CsvOutput(CCsvFile &oCsvFile, const double *pLeftData, const double *pRightData, const double *pFreq, int nFreqCount, LPCSTR pTitle)
{
	oCsvFile.SetData("Frequency Response");
	oCsvFile.SetData(pTitle);
	oCsvFile.Output();

	oCsvFile.SetData("Frequency[Hz]");
	if (pRightData == NULL)
		oCsvFile.SetData("Level[dB]");
	else {
		oCsvFile.SetData("Level-L[dB]");
		oCsvFile.SetData("Level-R[dB]");
	}
	oCsvFile.Output();

	for (int i = 0; i < nFreqCount; i++) {
		oCsvFile.SetData(*pFreq++, "%.1f");

		if (pLeftData != NULL)
			oCsvFile.SetData(dB10(pLeftData[i]), "%.2f");

		if (pRightData != NULL)
			oCsvFile.SetData(dB10(pRightData[i]), "%.2f");

		oCsvFile.Output();
	}
}

HBITMAP CFreWnd::GetBitmap()
{
	return m_bitmapMem;
}
