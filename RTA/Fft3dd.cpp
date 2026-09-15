#include "stdafx.h"
#include "Rta.h"
#include "FftWnd.h"
#include "FftDlg.h"

void CFftWnd::SetBitmap3dd(FFTWINDOW *pFftWindow)
{
	CFont *pFontOrg = m_dcMem.SelectObject(&m_Font);

	TEXTMETRIC tm;
	m_dcMem.GetTextMetrics(&tm);

	pFftWindow->m_nFrameLeft = tm.tmAveCharWidth * 5 + tm.tmAscent + 4;
	pFftWindow->m_nFrameTop = 5;
	pFftWindow->m_nFrameRight = pFftWindow->m_nWidth - (tm.tmAveCharWidth * 5 + 4);
	pFftWindow->m_nFrameBottom = pFftWindow->m_nHeight - (tm.tmAscent + 2) * 2 - 3;
	pFftWindow->m_nFrameWidth = pFftWindow->m_nFrameRight - pFftWindow->m_nFrameLeft;
	pFftWindow->m_nFrameHeight = pFftWindow->m_nFrameBottom - pFftWindow->m_nFrameTop;

	if (m_nViewMode != VM_SPLIT) {
		m_dx = pFftWindow->m_nFrameWidth / 5 / g_oSetData.Fft.nTimeDataNum;
		m_dy = m_dx * 5 / 3;
	} else {
		m_dx = pFftWindow->m_nFrameWidth / 8 / g_oSetData.Fft.nTimeDataNum;
		m_dy = m_dx;
	}
	int dx = m_dx * g_oSetData.Fft.nTimeDataNum;
	int dy = m_dy * g_oSetData.Fft.nTimeDataNum;

	m_dcMem.SetBkMode(TRANSPARENT);
	m_dcMem.SetTextColor(COLOR_TEXT);

	if (m_nViewMode != VM_SPLIT) {
		pFftWindow->m_nScaleWidth = pFftWindow->m_nFrameWidth;
		pFftWindow->m_nScaleHeight = pFftWindow->m_nFrameHeight;
		pFftWindow->m_nScaleWidth2 = pFftWindow->m_nScaleWidth - dx;
		pFftWindow->m_nScaleHeight2 = pFftWindow->m_nScaleHeight - dy;

		DrawScale3dd(pFftWindow, pFftWindow->m_nFrameLeft, pFftWindow->m_nFrameTop, pFftWindow->m_nFrameRight, pFftWindow->m_nFrameBottom, TRUE, m_nChannel == CH_STEREO ? "Lch / Rch" : NULL);
	} else {
		pFftWindow->m_nScaleWidth = pFftWindow->m_nFrameWidth;
		pFftWindow->m_nScaleHeight = pFftWindow->m_nFrameHeight / 2 - pFftWindow->m_nHeight / 50;
		pFftWindow->m_nScaleWidth2 = pFftWindow->m_nScaleWidth - dx;
		pFftWindow->m_nScaleHeight2 = pFftWindow->m_nScaleHeight - dy;

		DrawScale3dd(pFftWindow, pFftWindow->m_nFrameLeft, pFftWindow->m_nFrameTop, pFftWindow->m_nFrameRight, pFftWindow->m_nFrameTop + pFftWindow->m_nScaleHeight, FALSE, "Lch");
		DrawScale3dd(pFftWindow, pFftWindow->m_nFrameLeft, pFftWindow->m_nFrameBottom - pFftWindow->m_nScaleHeight, pFftWindow->m_nFrameRight, pFftWindow->m_nFrameBottom, TRUE, "Rch");
	}

	m_dcMem.SelectObject(pFontOrg);

	for (int i = 0; i < 2; i++) {
		FFTDATA &oFftData = m_aFftData[i];

		for (int j = 0; j < g_oSetData.Fft.nTimeDataNum; j++) {
			if (oFftData.m_p3ddPoint[j] == NULL)
				oFftData.m_p3ddPoint[j] = new POINT[pFftWindow->m_nScaleWidth2 + 2];

			oFftData.m_n3ddPointCount[j] = 0;
		}

		oFftData.m_n3ddCurrentPtr = 0;
	}

	if (m_p3ddWork == NULL)
		m_p3ddWork = new POINT[pFftWindow->m_nScaleWidth2 + 2];

	if (m_p3ddCheckTbl == NULL)
		m_p3ddCheckTbl = new int[pFftWindow->m_nScaleWidth];
}

void CFftWnd::DrawScale3dd(const FFTWINDOW *pFftWindow, int nLeft, int nTop, int nRight, int nBottom, BOOL bAxisX, LPCTSTR pText)
{
	int	nFreq, nLevel;
	int x, y;
	CString sText;
	CSize size;
	int nLeft1 = nLeft, nTop1 = nTop, nRight1 = nLeft + pFftWindow->m_nScaleWidth2, nBottom1 = nTop + pFftWindow->m_nScaleHeight2;
	int nLeft2 = nRight - pFftWindow->m_nScaleWidth2, nTop2 = nBottom - pFftWindow->m_nScaleHeight2, nRight2 = nRight, nBottom2 = nBottom;

	CPen *pPenOrg = m_dcMem.SelectObject(&m_penBlack);

	POINT point[6];
	point[0].x = nLeft2;
	point[0].y = nBottom2;
	point[1].x = nLeft1;
	point[1].y = nBottom1;
	point[2].x = nLeft1;
	point[2].y = nTop1;
	point[3].x = nRight1;
	point[3].y = nTop1;
	point[4].x = nRight2;
	point[4].y = nTop2;
	point[5].x = nRight2;
	point[5].y = nBottom2;
	m_dcMem.Polygon(point, 6);

	m_dcMem.SelectObject(&m_penGray);

	m_dcMem.MoveTo(nRight1, nBottom1);
	m_dcMem.LineTo(nLeft1, nBottom1);
	m_dcMem.MoveTo(nRight1, nBottom1);
	m_dcMem.LineTo(nRight1, nTop1);
	m_dcMem.MoveTo(nRight1, nBottom1);
	m_dcMem.LineTo(nRight2, nBottom2);

	m_dcMem.SetTextColor(COLOR_TEXT);
	m_dcMem.SetBkMode(TRANSPARENT);

	m_dcMem.SelectObject(&m_Font);

	if (g_oSetData.Fft.nFftScale == 0) {
		int step = GetLogScaleStep(m_nMinFreq);
		double fSpanFreq = m_fLogMaxFreq - m_fLogMinFreq;
		for (nFreq = GetScaleStartValue(m_nMinFreq, step); nFreq <= m_nMaxFreq; nFreq += GetLogScaleStep(nFreq)) {
			x =  nLeft2 + (int)((log((double)nFreq) - m_fLogMinFreq) * pFftWindow->m_nScaleWidth2 / fSpanFreq + 0.5);

			if (x > nLeft2 && x < nRight2) {
				m_dcMem.MoveTo(x, nBottom2 - 4);
				m_dcMem.LineTo(x, nBottom2);
			}

			if (bAxisX) {
				if (nFreq < 1000)
					sText.Format("%d", nFreq);
				else
					sText.Format("%dk", nFreq / 1000);
				if (sText[0] == '1' || sText[0] == '2' || sText[0] == '5') {
					size = m_dcMem.GetOutputTextExtent(sText);
					m_dcMem.TextOut(x - size.cx / 2, nBottom2 + 2, sText);
				}
			}
		}
	} else {
		int step = GetLinearScaleStep(m_nMaxFreq, m_nMinFreq, (int)(pFftWindow->m_nScaleWidth2 * 0.75));
		double fSpanFreq = m_nMaxFreq - m_nMinFreq;
		for (nFreq = GetScaleStartValue(m_nMinFreq, step); nFreq <= m_nMaxFreq; nFreq += step) {
			x = nLeft2 + (int)((nFreq - m_nMinFreq) * pFftWindow->m_nScaleWidth2 / fSpanFreq + 0.5);

			if (x > nLeft2 && x < nRight2) {
				m_dcMem.MoveTo(x, nBottom2 - 4);
				m_dcMem.LineTo(x, nBottom2);
			}

			if (bAxisX) {
				if (nFreq < 1000)
					sText.Format("%d", nFreq);
				else
					sText.Format("%gk", (double)nFreq / 1000);
				size = m_dcMem.GetOutputTextExtent(sText);
				m_dcMem.TextOut(x - size.cx / 2, nBottom2 + 2, sText);
			}
		}
	}

	int step = GetLinearScaleStep(m_nMaxLevel, m_nMinLevel, pFftWindow->m_nScaleHeight);
	double fSpanLevel = m_nMaxLevel - m_nMinLevel;
	for (nLevel = GetScaleStartValue(m_nMinLevel, step); nLevel <= m_nMaxLevel; nLevel += step) {
		sText.Format("%d", nLevel);
		size = m_dcMem.GetOutputTextExtent(sText);

		y = nTop + (int)((m_nMaxLevel - nLevel) * pFftWindow->m_nScaleHeight2 / fSpanLevel + 0.5);

		m_dcMem.MoveTo(nLeft, y);
		m_dcMem.LineTo(nLeft + 4, y);

		m_dcMem.TextOut(nLeft - size.cx - 2, y - size.cy / 2, sText);

		y = nTop2 + (int)((m_nMaxLevel - nLevel) * pFftWindow->m_nScaleHeight2 / fSpanLevel + 0.5);

		m_dcMem.MoveTo(nRight2, y);
		m_dcMem.LineTo(nRight2 - 4, y);

		m_dcMem.TextOut(nRight2 + 2, y - size.cy / 2, sText);
	}

	for (int i = 0; i < g_oSetData.Fft.nTimeDataNum; i++) {
		x = nLeft2 - m_dx * i;
		y = nBottom2 - m_dy * i;
		m_dcMem.MoveTo(x, y);
		m_dcMem.LineTo(x + 4, y);
	}

	double t = m_fTimeStep * g_oSetData.Fft.nTimeDataNum / g_oSetData.Fft.nTimeRes;
	if (t < 1.0)
		sText.Format("t=%.1fms", t * 1000);
	else
		sText.Format("t=%.3fs", t);
	size = m_dcMem.GetOutputTextExtent(sText);
	m_dcMem.TextOut(nLeft2 - size.cx - 20, nBottom2 - size.cy - 10, sText);

	if (bAxisX) {
		sText.LoadString(IDS_FREQUENCY);
		sText += " [Hz]";
		size = m_dcMem.GetOutputTextExtent(sText);
		m_dcMem.TextOut(nLeft2 + (pFftWindow->m_nScaleWidth2 - size.cx) / 2, pFftWindow->m_nHeight - size.cy, sText);
	}

	m_dcMem.SelectObject(&m_Font2);
	sText.LoadString(IDS_SPL);
	sText += " [dB]";
	size = m_dcMem.GetOutputTextExtent(sText);
	m_dcMem.TextOut(0, nTop + (pFftWindow->m_nScaleHeight2 + size.cx) / 2, sText);

	DrawNote(pText, nTop, nRight1);

	m_dcMem.SelectObject(pPenOrg);
}

void CFftWnd::GetWaveData3dd(const FFTWINDOW *pFftWindow)
{
	if (m_bLch)
		Calc3dd(pFftWindow, &m_aFftData[0]);

	if (m_bRch)
		Calc3dd(pFftWindow, &m_aFftData[1]);

	Disp3ddInfo(pFftWindow);
}

void CFftWnd::Calc3dd(const FFTWINDOW *pFftWindow, FFTDATA *pFftData)
{
	int nPtr = pFftData->m_n3ddCurrentPtr;
	POINT *pPoint = pFftData->m_p3ddPoint[nPtr];
	double *pFftPBuf = pFftData->m_oPowerSpecBuf.GetBuf();
	long x, y;
	long x2 = 0;
	double fStepY = double(pFftWindow->m_nScaleHeight2) / (m_nMinLevel - m_nMaxLevel);
	double fTmp1 = double(pFftWindow->m_nScaleWidth2) / (m_fLogMaxFreq - m_fLogMinFreq);
	double fTmp2 = double(pFftWindow->m_nScaleWidth2) / (m_nMaxFreq - m_nMinFreq);
	int nFftSize2 = m_nFftSize / 2;
	double fLevel = 0;
	double fLevel2 = pFftPBuf[0];
	int nLevelCounter = 0;
	int nPointCount = 0;
	int dx, xj;
	double dl;
	double t;

	pPoint[nPointCount].x = 0;
	pPoint[nPointCount].y = pFftWindow->m_nScaleHeight2;
	nPointCount++;

	for (int i = 0; i < nFftSize2; i++) {
		if (g_oSetData.Fft.nFftScale == 0)
			x = long((m_pLogFreqTbl[i] - m_fLogMinFreq) * fTmp1 + 0.5);
		else
			x = long((i * m_fFreqStep - m_nMinFreq) * fTmp2 + 0.5);

		fLevel += pFftPBuf[i];
		nLevelCounter++;

		if (x != x2) {
			fLevel /= nLevelCounter;

			if (fLevel2 == 0)
				fLevel2 = fLevel;

			dx = x - x2;
			dl = fLevel - fLevel2;
			for (int j = 0; j < dx; j++) {
				xj = x2 + j;
				if (xj >= 0 && xj < pFftWindow->m_nScaleWidth2) {
					pPoint[nPointCount].x = xj;
					t = fLevel2 + dl * j / dx;
					if (t > 0) {
						y = long((dB10(t) - m_nMaxLevel) * fStepY + 0.5);
						if (y > pFftWindow->m_nScaleHeight2)
							y = pFftWindow->m_nScaleHeight2;
					} else
						y = pFftWindow->m_nScaleHeight2;
					pPoint[nPointCount].y = y;
					nPointCount++;
				}
			}

			x2 = x;
			fLevel2 = fLevel;
			fLevel = 0;
			nLevelCounter = 0;
		}
	}

	pPoint[nPointCount].x = pFftWindow->m_nScaleWidth2 - 1;
	pPoint[nPointCount].y = pFftWindow->m_nScaleHeight2;
	nPointCount++;

	pFftData->m_n3ddPointCount[nPtr] = nPointCount;

	if (++(pFftData->m_n3ddCurrentPtr) >= g_oSetData.Fft.nTimeDataNum)
		pFftData->m_n3ddCurrentPtr = 0;
}

void CFftWnd::Disp3ddInfo(const FFTWINDOW *pFftWindow)
{
	if (m_bLch)
		Disp3ddInfoSub(pFftWindow, &m_aFftData[0], WM_FREQ_LEVEL_LEFT);

	if (m_bRch)
		Disp3ddInfoSub(pFftWindow, &m_aFftData[1], WM_FREQ_LEVEL_RIGHT);
}

void CFftWnd::Disp3ddInfoSub(const FFTWINDOW *pFftWindow, FFTDATA *pFftData, int nMessageId)
{
	CString sFreq, sLevel;

	if (pFftData->m_fFftAllPower != 0)
		sLevel.Format("%.2lf", dB10(pFftData->m_fFftAllPower));
	sFreq = "ALL";

	if (pFftWindow->m_bMasterWindow)
		m_pFftDlg->SendMessage(nMessageId, (WPARAM)(LPCSTR)sFreq, (LPARAM)(LPCSTR)sLevel);
	else
		pFftWindow->m_pWnd->SendMessage(nMessageId, (WPARAM)(LPCSTR)sFreq, (LPARAM)(LPCSTR)sLevel);
}

void CFftWnd::Paint3dd(const FFTWINDOW *pFftWindow, int nChannel)
{
	if (nChannel == 0) {
		m_dcMem2.SelectObject(&m_penLeft);
		Paint3ddSub(pFftWindow, &m_aFftData[0]);
	} else {
		m_dcMem2.SelectObject(&m_penRight);
		Paint3ddSub(pFftWindow, &m_aFftData[1]);
	}
}

void CFftWnd::Paint3ddSub(const FFTWINDOW *pFftWindow, FFTDATA *pFftData)
{
	int i, j, nPtr, nPointCount, n;
	long x, y;
	POINT *pPoint;

	for (i = 0; i < pFftWindow->m_nScaleWidth; i++)
		m_p3ddCheckTbl[i] = pFftWindow->m_nScaleHeight;

	nPtr = pFftData->m_n3ddCurrentPtr;
	for (i = 0; i < g_oSetData.Fft.nTimeDataNum; i++) {
		n = g_oSetData.Fft.nTimeDataNum - i;
		if (g_oSetData.Fft.nTimeDir != 0) {
			if (--nPtr < 0)
				nPtr = g_oSetData.Fft.nTimeDataNum - 1;
		}

		nPointCount = pFftData->m_n3ddPointCount[nPtr];
		pPoint = pFftData->m_p3ddPoint[nPtr] + nPointCount - 1;
		for (j = 0; j < nPointCount - 1; j++) {
			x = pPoint->x + m_dx * n;
			y = pPoint->y + m_dy * n;
			if (m_p3ddCheckTbl[x] > y)
				m_p3ddCheckTbl[x] = y;

			m_p3ddWork[j].x = x;
			m_p3ddWork[j].y = m_p3ddCheckTbl[x];

			pPoint--;
		}
		m_p3ddWork[j].x = pPoint->x + m_dx * n;
		m_p3ddWork[j].y = pPoint->y + m_dy * n;

		if (nPointCount != 0)
			m_dcMem2.Polyline(m_p3ddWork, nPointCount);

		if (g_oSetData.Fft.nTimeDir == 0) {
			if (++nPtr >= g_oSetData.Fft.nTimeDataNum)
				nPtr = 0;
		}
	}
}
