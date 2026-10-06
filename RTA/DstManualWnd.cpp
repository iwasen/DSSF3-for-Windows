// DstManualWnd.cpp : 実装ファイル
//

#include "stdafx.h"
#include "Rta.h"
#include "DstManualWnd.h"

// CDstManualWnd

BEGIN_MESSAGE_MAP(CDstManualWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CDstManualWnd メッセージ ハンドラ

void CDstManualWnd::Initialize()
{
	CString str;

	CDC *pDC = GetDC();
	m_dcMem.CreateCompatibleDC(pDC);
	m_dcMem2.CreateCompatibleDC(pDC);
	ReleaseDC(pDC);

	m_penBlack.CreatePen(PS_SOLID, 0, COLOR_BLACK);
	m_penGray.CreatePen(PS_SOLID, 0, COLOR_GRAY);
	m_penLightGray.CreatePen(PS_SOLID, 0, COLOR_LIGHTGRAY);
	m_penLeft.CreatePen(PS_SOLID, 0, COLOR_LEFT);
	m_penRight.CreatePen(PS_SOLID, 0, COLOR_RIGHT);
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

	SetBitmap(g_oSetData.Dst.nChannel, g_oSetData.Dst.nMaxHarmonics);
}

void CDstManualWnd::Initialize2()
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
}

void CDstManualWnd::Uninitialize2()
{
	m_bitmapMem.DeleteObject();
}

void CDstManualWnd::Resize()
{
	Uninitialize2();
	Initialize2();
}

void CDstManualWnd::SetBitmap(int nChannel, int nMaxHarmonics)
{
	CFont *pFontOrg = m_dcMem.SelectObject(&m_Font);

	DrawScaleDst(m_nScaleLeft, m_nScaleTop, m_nScaleRight, m_nScaleBottom, nMaxHarmonics);

	m_dcMem.SelectObject(pFontOrg);

	const double xStep = (double)m_nScaleWidth / nMaxHarmonics;
	int xWidth;
	if (nChannel != 0)
		xWidth = (int)(xStep / 2.5);
	else
		xWidth = (int)(xStep / 1.4);
	LoadBimapStretch(m_bitmapLeft, IDB_RTA_OCT_LEFT, xWidth, m_nScaleHeight);
	LoadBimapStretch(m_bitmapRight, IDB_RTA_OCT_RIGHT, xWidth, m_nScaleHeight);
	LoadBimapStretch(m_bitmapLeftTotal, IDB_RTA_OCT_PEAK_LEFT, xWidth, m_nScaleHeight);
	LoadBimapStretch(m_bitmapRightTotal, IDB_RTA_OCT_PEAK_RIGHT, xWidth, m_nScaleHeight);
}

void CDstManualWnd::DrawScaleDst(int nLeft, int nTop, int nRight, int nBottom, int nMaxHarmonics)
{
	int y;
	int i;
	const int nWidth = nRight - nLeft;
	const int nHeight = nBottom - nTop;
	CPen *pPenOrg;
	CSize size;
	CString sText;
	CRect rect;
	double fStep;
	double fPos;
	double fTHD;
	int nTHD;
	CString sUnit;

	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
	pPenOrg = m_dcMem.SelectObject(&m_penBlack);

	rect.SetRect(0, 0, m_nWidth, m_nHeight);
	m_dcMem.FillRect(rect, &m_brushWhite);

	m_dcMem.SetTextColor(COLOR_TEXT);
	m_dcMem.SetBkMode(TRANSPARENT);

	m_dcMem.SelectObject(&m_Font);

	const int nData = nMaxHarmonics;
	fStep = (double)nWidth / nData;
	fPos = nLeft + fStep / 2;
	for (i = 0; i < nData; i++) {
		if (i == 0)
			sText = "THD";
		else
			sText.Format("%d", i + 1);

		size = m_dcMem.GetOutputTextExtent(sText);
		m_dcMem.TextOut((int)(fPos - size.cx / 2), m_nScaleBottom + 3, sText);
		fPos += fStep;
	}

	sText.LoadString(IDS_HARMONICS);
	size = m_dcMem.GetOutputTextExtent(sText);
	m_dcMem.TextOut(nLeft + (nWidth - size.cx) / 2, m_nHeight - size.cy - 2, sText);

	if (g_oSetData.Dst.nScaleMode == 0) {
		const double fMinTHD = pow(10.0, g_oSetData.Dst.nScaleMin / 20);
		const double fMaxTHD = pow(10.0, g_oSetData.Dst.nScaleMax / 20);
		const int nDist = g_oSetData.Dst.nScaleMax - g_oSetData.Dst.nScaleMin;
		if (nDist != 0) {
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

void CDstManualWnd::DispGraph(const double *pLeftDst, const double *pRightDst, int nChannel, int nMaxHarmonics)
{
	int i;
	CRect rect;
	double xPos;
	int xWidth;
	const double xStep = (double)m_nScaleWidth / nMaxHarmonics;
	const double yStep = (double)m_nScaleHeight / (g_oSetData.Dst.nScaleMin - g_oSetData.Dst.nScaleMax);

	SetBitmap(nChannel, nMaxHarmonics);

	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
	CBitmap *pBitmapOrg2 = m_dcMem2.SelectObject(&m_bitmapLeftTotal);
	CPen *pOenOrg = m_dcMem.SelectObject(&m_penLeft);

	if (nChannel != 0)
		xWidth = (int)(xStep / 2.5);
	else
		xWidth = (int)(xStep / 1.4);

	xPos = m_nScaleLeft + xStep / 2;

	if (nChannel == 0)
		xPos -= xWidth / 2;
	else
		xPos -= xWidth;

	if (pLeftDst != NULL) {
		for (i = 0; i < nMaxHarmonics; i++) {
			if (*pLeftDst != 0) {
				rect.left = (int)xPos;
				rect.top = (int)((dB10(*pLeftDst++) - g_oSetData.Dst.nScaleMax) * yStep + 0.5) + m_nScaleTop;
				rect.right = rect.left + xWidth;
				rect.bottom = m_nScaleBottom;

				if (rect.top < m_nScaleTop)
					rect.top = m_nScaleTop;

				if (i == 1)
					m_dcMem2.SelectObject(&m_bitmapLeft);

				if (rect.top < rect.bottom) {
					m_dcMem.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &m_dcMem2, 0, 0, SRCCOPY);

					m_dcMem.MoveTo(rect.left, rect.top);
					m_dcMem.LineTo(rect.right, rect.top);
				}
			}

			xPos += xStep;
		}
	}

	if (pRightDst != NULL) {
		xPos = m_nScaleLeft + xStep / 2;
		m_dcMem2.SelectObject(&m_bitmapRightTotal);
		m_dcMem.SelectObject(&m_penRight);

		for (i = 0; i < nMaxHarmonics; i++) {
			if (*pRightDst != 0) {
				rect.left = (int)xPos;
				rect.top = (int)((dB10(*pRightDst++) - g_oSetData.Dst.nScaleMax) * yStep + 0.5) + m_nScaleTop;
				rect.right = rect.left + xWidth;
				rect.bottom = m_nScaleBottom;

				if (rect.top < m_nScaleTop)
					rect.top = m_nScaleTop;

				if (i == 1)
					m_dcMem2.SelectObject(&m_bitmapRight);

				if (rect.top < rect.bottom) {
					m_dcMem.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &m_dcMem2, 0, 0, SRCCOPY);

					m_dcMem.MoveTo(rect.left, rect.top);
					m_dcMem.LineTo(rect.right, rect.top);
				}
			}

			xPos += xStep;
		}
	}

	m_dcMem.SelectObject(pBitmapOrg);
	m_dcMem2.SelectObject(pBitmapOrg2);
	m_dcMem.SelectObject(pOenOrg);

	Invalidate(FALSE);
}

void CDstManualWnd::OnPaint()
{
	CPaintDC dc(this);

	if (m_dcMem.m_hDC != NULL) {
		CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
		dc.BitBlt(0, 0, m_nWidth, m_nHeight, &m_dcMem, 0, 0, SRCCOPY);
		m_dcMem.SelectObject(pBitmapOrg);
	}
}

BOOL CDstManualWnd::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void CDstManualWnd::CsvOutput(CCsvFile &oCsvFile, const double *pLeftDst, const double *pRightDst, int nMaxHarmonics)
{
	oCsvFile.SetData("THD");
	oCsvFile.SetData("Manual");
	oCsvFile.Output();

	oCsvFile.SetData("Harmonics");
	if (g_oSetData.Dst.nScaleMode == 0) {
		if (pRightDst == NULL)
			oCsvFile.SetData("Level[%]");
		else {
			oCsvFile.SetData("Level-L[%]");
			oCsvFile.SetData("Level-R[%]");
		}
	} else {
		if (pRightDst == NULL)
			oCsvFile.SetData("Level[dB]");
		else {
			oCsvFile.SetData("Level-L[dB]");
			oCsvFile.SetData("Level-R[dB]");
		}
	}
	oCsvFile.Output();

	for (int i = 0; i < nMaxHarmonics; i++) {
		if (i == 0)
			oCsvFile.SetData("THD");
		else
			oCsvFile.SetData(i + 1);

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

HBITMAP CDstManualWnd::GetBitmap()
{
	return m_bitmapMem;
}
