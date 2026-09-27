#include "stdafx.h"
#include "Rta.h"
#include "FftWnd.h"
#include "FftDlg.h"

void CFftWnd::SetBitmapSpg(FFTWINDOW *pFftWindow)
{
	CFont *pFontOrg = m_dcMem.SelectObject(&m_Font);

	TEXTMETRIC tm;
	m_dcMem.GetTextMetrics(&tm);

	pFftWindow->m_nFrameLeft = tm.tmAveCharWidth * 5 + tm.tmAscent + 3;
	pFftWindow->m_nFrameTop = 5;
	pFftWindow->m_nFrameRight = pFftWindow->m_nWidth - (22 + tm.tmAveCharWidth * 4);
	pFftWindow->m_nFrameBottom = pFftWindow->m_nHeight - (tm.tmAscent + 2) * 2 - 3;
	pFftWindow->m_nFrameWidth = pFftWindow->m_nFrameRight - pFftWindow->m_nFrameLeft;
	pFftWindow->m_nFrameHeight = pFftWindow->m_nFrameBottom - pFftWindow->m_nFrameTop;

	if (m_nViewMode != VM_SPLIT) {
		pFftWindow->m_nScaleWidth = pFftWindow->m_nFrameWidth;
		pFftWindow->m_nScaleHeight = pFftWindow->m_nFrameHeight;
		DrawScaleSpg(pFftWindow, pFftWindow->m_nFrameLeft, pFftWindow->m_nFrameTop, pFftWindow->m_nFrameRight, pFftWindow->m_nFrameBottom, TRUE, m_nChannel == CH_STEREO ? "Lch / Rch" : NULL);
	} else {
		pFftWindow->m_nScaleWidth = pFftWindow->m_nFrameWidth;
		pFftWindow->m_nScaleHeight = pFftWindow->m_nFrameHeight / 2 - pFftWindow->m_nHeight / 50;

		DrawScaleSpg(pFftWindow, pFftWindow->m_nFrameLeft, pFftWindow->m_nFrameTop, pFftWindow->m_nFrameRight, pFftWindow->m_nFrameTop + pFftWindow->m_nScaleHeight, FALSE, "Lch");
		DrawScaleSpg(pFftWindow, pFftWindow->m_nFrameLeft, pFftWindow->m_nFrameBottom - pFftWindow->m_nScaleHeight, pFftWindow->m_nFrameRight, pFftWindow->m_nFrameBottom, TRUE, "Rch");
	}

	DrawLevelScale(pFftWindow, pFftWindow->m_nFrameLeft, pFftWindow->m_nFrameTop, pFftWindow->m_nFrameRight, pFftWindow->m_nFrameBottom);

	m_dcMem.SelectObject(pFontOrg);

	if (m_pSpgLevel == NULL)
		m_pSpgLevel = new double[pFftWindow->m_nScaleHeight];

	for (int i = 0; i < 2; i++) {
		FFTDATA *pFftData = &m_aFftData[i];

		if (pFftData->m_pSpgBmp == NULL) {
			pFftData->m_pSpgBmp = new CBitmap;
			pFftData->m_pSpgBmp->CreateCompatibleBitmap(&m_dcMem, pFftWindow->m_nScaleWidth, pFftWindow->m_nScaleHeight);
		}
	}
}

void CFftWnd::DrawScaleSpg(const FFTWINDOW *pFftWindow, int nLeft, int nTop, int nRight, int nBottom, BOOL bAxisX, LPCTSTR pText)
{
	int	nFreq;
	int time;
	int x, y;
	int nWidth = nRight - nLeft;
	int nHeight = nBottom - nTop;
	CString sText;
	CSize size;

	CPen *pPenOrg = m_dcMem.SelectObject(&m_penBlack);
	CBrush *pBrushOrg = m_dcMem.SelectObject(&m_brushBlack);
	CFont *pFontOrg = m_dcMem.SelectObject(&m_Font);

	m_dcMem.Rectangle(nLeft - 1, nTop - 1, nRight + 1, nBottom + 1);

	m_dcMem.SetBkMode(TRANSPARENT);
	m_dcMem.SetTextColor(COLOR_TEXT);

	m_dcMem.SelectObject(m_penMidGray);

	for (time = 0; time <= g_oSetData.Fft.nTimeRange; time++) {
		x = nLeft + time * nWidth / g_oSetData.Fft.nTimeRange;

		if (x > nLeft && x < nRight) {
			m_dcMem.MoveTo(x, nTop);
			m_dcMem.LineTo(x, nBottom);
		}

		if (bAxisX) {
			sText.Format("%d", time);
			size = m_dcMem.GetOutputTextExtent(sText);
			m_dcMem.TextOut(x - size.cx / 2, nBottom + 2, sText);
		}
	}

	if (g_oSetData.Fft.nFftScale == 0) {
		int step = GetLogScaleStep(m_nMinFreq);
		double fSpanFreq = m_fLogMaxFreq - m_fLogMinFreq;
		for (nFreq = GetScaleStartValue(m_nMinFreq, step); nFreq <= m_nMaxFreq; nFreq += GetLogScaleStep(nFreq)) {
			y = nTop + (int)((m_fLogMaxFreq - log((double)nFreq)) * nHeight / fSpanFreq + 0.5);

			if (nFreq < 1000)
				sText.Format("%d", nFreq);
			else
				sText.Format("%gk", (double)nFreq / 1000);

			if (sText[0] == '1' || sText[0] == '2' || sText[0] == '5') {
				size = m_dcMem.GetOutputTextExtent(sText);
				m_dcMem.TextOut(nLeft - size.cx - 2, y - size.cy / 2, sText);
			}
		}
	} else {
		m_dcMem.SelectObject(m_penLightGray);

		int step = GetLinearScaleStep(m_nMaxFreq, m_nMinFreq, nHeight);
		double fSpanFreq = m_nMaxFreq - m_nMinFreq;
		for (nFreq = GetScaleStartValue(m_nMinFreq, step); nFreq <= m_nMaxFreq; nFreq += step) {
			y = nTop + (int)((m_nMaxFreq - nFreq) * nHeight / fSpanFreq + 0.5);
			if (nFreq < 1000)
				sText.Format("%d", nFreq);
			else
				sText.Format("%gk", (double)nFreq / 1000);
			size = m_dcMem.GetOutputTextExtent(sText);
			m_dcMem.TextOut(nLeft - size.cx - 2, y - size.cy / 2, sText);
		}
	}

	if (bAxisX) {
		sText.LoadString(IDS_TIME);
		sText += " [s]";
		size = m_dcMem.GetOutputTextExtent(sText);
		m_dcMem.TextOut(nLeft + (nWidth - size.cx) / 2, pFftWindow->m_nHeight - size.cy, sText);
	}

	m_dcMem.SelectObject(&m_Font2);
	sText.LoadString(IDS_FREQUENCY);
	sText += " [Hz]";
	size = m_dcMem.GetOutputTextExtent(sText);
	m_dcMem.TextOut(0, nTop + (nHeight + size.cx) / 2, sText);

	DrawNote(pText, nTop, nRight);

	m_dcMem.SelectObject(pFontOrg);
	m_dcMem.SelectObject(pBrushOrg);
	m_dcMem.SelectObject(pPenOrg);
}

void CFftWnd::DrawLevelScale(const FFTWINDOW *pFftWindow, int /*nLeft*/, int nTop, int nRight, int nBottom)
{
	int y;
	int nLevel;
	int nHeight = nBottom - nTop;
	int nLevelRange = m_nMaxLevel - m_nMinLevel;
	CString sText;
	CSize size;

	CPen *pPenOrg = m_dcMem.SelectObject(&m_penBlack);
	CBrush *pBrushOrg = m_dcMem.SelectObject(&m_brushBlack);
	CFont *pFontOrg = m_dcMem.SelectObject(&m_Font);

	m_dcMem.Rectangle(nRight + 7, nTop - 1, nRight + 17, nBottom + 1);

	m_dcMem.SetBkMode(TRANSPARENT);
	m_dcMem.SetTextColor(COLOR_TEXT);

	int step = GetLinearScaleStep(m_nMaxLevel, m_nMinLevel, nHeight * 3 / 5);
	for (nLevel = GetScaleStartValue(m_nMinLevel, step); nLevel <= m_nMaxLevel; nLevel += step) {
		y = nTop - (nLevel - m_nMaxLevel) * nHeight / nLevelRange;

		sText.Format("%d", nLevel);
		size = m_dcMem.GetOutputTextExtent(sText);
		m_dcMem.TextOut(nRight + 20, y - size.cy / 2, sText);
	}

	for (y = nTop; y <= nBottom; y++)
		m_dcMem.FillSolidRect(nRight + 8, y, 8, 1, GetLevelColor(m_nMaxLevel - (y - nTop) * nLevelRange / (double)nHeight));

	sText.LoadString(IDS_LEVEL);
	sText += " [dB]";
	size = m_dcMem.GetOutputTextExtent(sText);
	m_dcMem.TextOut(pFftWindow->m_nWidth - size.cx, pFftWindow->m_nHeight - size.cy, sText);

	m_dcMem.SelectObject(pFontOrg);
	m_dcMem.SelectObject(pBrushOrg);
	m_dcMem.SelectObject(pPenOrg);
}

COLORREF CFftWnd::GetLevelColor(double fLevel) const
{
	int r, g, b;

	fLevel = (fLevel - m_nMinLevel) / (m_nMaxLevel - m_nMinLevel);

	if (g_oSetData.Fft.nColorScale == 0) {
		if (fLevel >= 0.8) {
			r = 255;
			g = 255 - (int)((fLevel - 0.8) * 5 * 255 + 0.5);
			b = 0;
		} else if (fLevel >= 0.6) {
			r = (int)((fLevel - 0.6) * 5 * 255 + 0.5);
			g = 255;
			b = 0;
		} else if (fLevel >= 0.4) {
			r = 0;
			g = 255;
			b = 255 - (int)((fLevel - 0.4) * 5 * 255 + 0.5);
		} else if (fLevel >= 0.2) {
			r = 0;
			g = (int)((fLevel - 0.2) * 5 * 255 + 0.5);
			b = 255;
		} else {
			r = 0;
			g = 0;
			b = (int)(fLevel * 5 * 255 + 0.5);
		}
	} else
		r = g = b = (int)(fLevel * 255 + 0.5);

	if (r > 255)
		r = 255;
	else if (r < 0)
		r = 0;

	if (g > 255)
		g = 255;
	else if (g < 0)
		g = 0;

	if (b > 255)
		b = 255;
	else if (b < 0)
		b = 0;

	return RGB(r, g, b);
}

void CFftWnd::GetWaveDataSpg(const FFTWINDOW *pFftWindow)
{
	double fTimeStep = pFftWindow->m_nScaleWidth * m_fTimeStep2 / g_oSetData.Fft.nTimeRange;
	m_fSpgTimeStep += fTimeStep;
	int nTimeStep = (int)m_fSpgTimeStep;
	m_fSpgTimeStep -= nTimeStep;

	if (m_nViewMode == VM_OVERLAY) {
		CalcSpg(pFftWindow, &m_aFftData[0], nTimeStep);
	} else {
		if (m_bLch)
			CalcSpg(pFftWindow, &m_aFftData[0], nTimeStep);

		if (m_bRch)
			CalcSpg(pFftWindow, &m_aFftData[1], nTimeStep);
	}

	DispSpgInfo(pFftWindow);
}

void CFftWnd::CalcSpg(const FFTWINDOW *pFftWindow, FFTDATA *pFftData, int nTimeStep)
{
	CalcSpgSub(pFftWindow, pFftData->m_pPowerSpecBuf, pFftData->m_pSpgBmp, nTimeStep);
}

void CFftWnd::CalcSpgSub(const FFTWINDOW *pFftWindow, const double *pBuf, CBitmap *pBitmap, int nTimeStep)
{
	int i;
	int x, y;
	int y2 = 0;
	double fTmp1 = (double)pFftWindow->m_nScaleHeight / (m_fLogMaxFreq - m_fLogMinFreq);
	double fTmp2 = (double)pFftWindow->m_nScaleHeight / (m_nMaxFreq - m_nMinFreq);
	double fTmp3 = exp(0.5 / fTmp1);
	double nLevel = 0;
	int nFftSize2 = m_nFftSize / 2;
	int nCount = 0;
	int t1, t2, t3;

	memset(m_pSpgLevel, 0, sizeof(double) * pFftWindow->m_nScaleHeight);

	for (i = 1; i < nFftSize2; i++) {
		if (g_oSetData.Fft.nFftScale == 0)
			y = (int)((m_fLogMaxFreq - m_pLogFreqTbl[i]) * fTmp1 + 0.5);
		else
			y = (int)((m_nMaxFreq - m_fFreqStep * i) * fTmp2 + 0.5);

		if (i == 1)
			y2 = y;

		if (y != y2) {
			if (y >= 0 && y < pFftWindow->m_nScaleHeight) {
				if (nLevel == 0)
					m_pSpgLevel[y] = (double)m_nMinLevel;
				else {
					double tmp4, tmp5;

					if (g_oSetData.Fft.nFftScale == 0) {
						tmp4 = m_nMaxFreq / exp(y / fTmp1);
						tmp5 = tmp4 * fTmp3 - tmp4 / fTmp3;
					} else
						tmp5 = 1 / fTmp2;

					m_pSpgLevel[y] = dB10(nLevel / nCount * tmp5 / m_fFreqStep);
				}
			}

			y2 = y;
			nLevel = 0;
			nCount = 0;
		}

		nLevel += pBuf[i];
		nCount++;
	}

	for (i = 0; i < pFftWindow->m_nScaleHeight; i++) {
		if (m_pSpgLevel[i] == 0) {
			for (t1 = i; t1 >= 0 && m_pSpgLevel[t1] == 0; t1--)
				;
			for (t2 = i; t2 < pFftWindow->m_nScaleHeight && m_pSpgLevel[t2] == 0; t2++)
				;
			if (t1 < 0)
				m_pSpgLevel[i] = m_pSpgLevel[t2];
			else if (t2 >= pFftWindow->m_nScaleHeight)
				m_pSpgLevel[i] = m_pSpgLevel[t1];
			else {
				t3 = t2 - t1;
				m_pSpgLevel[i] = (m_pSpgLevel[t1] * (t2 - i) + m_pSpgLevel[t2] * (i - t1)) / t3;
			}
		}
	}

	CBitmap *pBitmapOrg = m_dcMem3.SelectObject(pBitmap);
	COLORREF color;
	int nScroll;

	if (g_oSetData.Fft.nTimeDir == 0) {
		x = pFftWindow->m_nScaleWidth - nTimeStep;
		nScroll = -nTimeStep;
	} else {
		x = 0;
		nScroll = nTimeStep;
	}

	CRect rect(0, 0, pFftWindow->m_nScaleWidth, pFftWindow->m_nScaleHeight);
	m_dcMem3.ScrollDC(nScroll, 0, rect, rect, NULL, NULL);

	for (y = 0; y < pFftWindow->m_nScaleHeight; y++) {
		color = GetLevelColor(m_pSpgLevel[y]);
		m_dcMem3.FillSolidRect(x, y, nTimeStep, 1, color);
	}

	m_dcMem3.SelectObject(pBitmapOrg);
}

void CFftWnd::DispSpgInfo(const FFTWINDOW *pFftWindow)
{
	if (m_bLch)
		DispSpgInfoSub(pFftWindow, &m_aFftData[0], WM_FREQ_LEVEL_LEFT);

	if (m_bRch)
		DispSpgInfoSub(pFftWindow, &m_aFftData[1], WM_FREQ_LEVEL_RIGHT);
}

void CFftWnd::DispSpgInfoSub(const FFTWINDOW *pFftWindow, FFTDATA *pFftData, int nMessageId)
{
	CString sFreq, sLevel;

	sLevel.Format("%.2lf", dB10(pFftData->m_fFftAllPower));
	sFreq = "ALL";

	if (pFftWindow->m_bMasterWindow)
		m_pFftDlg->SendMessage(nMessageId, (WPARAM)(LPCSTR)sFreq, (LPARAM)(LPCSTR)sLevel);
	else
		pFftWindow->m_pWnd->SendMessage(nMessageId, (WPARAM)(LPCSTR)sFreq, (LPARAM)(LPCSTR)sLevel);
}

void CFftWnd::PaintSpg(const FFTWINDOW *pFftWindow, int nChannel)
{
	if (nChannel == 1 && m_nViewMode == VM_OVERLAY)
		return;

	CBitmap *pBitmapOrg;
	if (nChannel == 0)
		pBitmapOrg = m_dcMem3.SelectObject(m_aFftData[0].m_pSpgBmp);
	else
		pBitmapOrg = m_dcMem3.SelectObject(m_aFftData[1].m_pSpgBmp);

	m_dcMem2.BitBlt(0, 0, pFftWindow->m_nScaleWidth, pFftWindow->m_nScaleHeight, &m_dcMem3, 0, 0, SRCPAINT);

	m_dcMem3.SelectObject(pBitmapOrg);
}

void CFftWnd::RedrawSpg(const FFTWINDOW *pFftWindow)
{
	int nTimeStep = (int)ceil(pFftWindow->m_nScaleWidth * m_fTimeStep2 / g_oSetData.Fft.nTimeRange);

	int nSize = m_bSpectrogramRingBufRound ? m_nSpectrogramRingBufNum : m_nSpectrogramRingBufPtr;
	int index = m_bSpectrogramRingBufRound ? m_nSpectrogramRingBufPtr : 0;
	double fTime = m_bSpectrogramRingBufRound ? 0 : (double)(m_nSpectrogramRingBufNum - m_nSpectrogramRingBufPtr) / m_nSpectrogramRingBufNum;
	for (int i = 0; i < nSize; i++) {
		if (++index >= m_nSpectrogramRingBufNum)
			index = 0;

		double *pDataL = m_aFftData[0].m_pSpectrogramRingBuf + m_nSpectrogramRingBufSize * index;
		double *pDataR = m_aFftData[1].m_pSpectrogramRingBuf + m_nSpectrogramRingBufSize * index;

		if (m_nViewMode == VM_OVERLAY) {
			RedrawSpgSub(pFftWindow, pDataL, m_aFftData[0].m_pSpgBmp, nTimeStep, fTime);
		} else {
			if (m_bLch)
				RedrawSpgSub(pFftWindow, pDataL, m_aFftData[0].m_pSpgBmp, nTimeStep, fTime);

			if (m_bRch)
				RedrawSpgSub(pFftWindow, pDataR, m_aFftData[1].m_pSpgBmp, nTimeStep, fTime);
		}

		fTime += (double)1 / m_nSpectrogramRingBufNum;
	}
}

void CFftWnd::RedrawSpgSub(const FFTWINDOW *pFftWindow, const double *pBuf, CBitmap *pBitmap, int nTimeStep, double fTime)
{
	int i;
	int x, y;
	int y2 = 0;
	double fTmp1 = (double)pFftWindow->m_nScaleHeight / (m_fLogMaxFreq - m_fLogMinFreq);
	double fTmp2 = (double)pFftWindow->m_nScaleHeight / (m_nMaxFreq - m_nMinFreq);
	double fTmp3 = exp(0.5 / fTmp1);
	double nLevel = 0;
	int nFftSize2 = m_nFftSize / 2;
	int nCount = 0;
	int t1, t2, t3;

	memset(m_pSpgLevel, 0, sizeof(double) * pFftWindow->m_nScaleHeight);

	for (i = 1; i < nFftSize2; i++) {
		if (g_oSetData.Fft.nFftScale == 0)
			y = (int)((m_fLogMaxFreq - m_pLogFreqTbl[i]) * fTmp1 + 0.5);
		else
			y = (int)((m_nMaxFreq - m_fFreqStep * i) * fTmp2 + 0.5);

		if (i == 1)
			y2 = y;

		if (y != y2) {
			if (y >= 0 && y < pFftWindow->m_nScaleHeight) {
				if (nLevel == 0)
					m_pSpgLevel[y] = (double)m_nMinLevel;
				else {
					double tmp4, tmp5;

					if (g_oSetData.Fft.nFftScale == 0) {
						tmp4 = m_nMaxFreq / exp(y / fTmp1);
						tmp5 = tmp4 * fTmp3 - tmp4 / fTmp3;
					} else
						tmp5 = 1 / fTmp2;

					m_pSpgLevel[y] = dB10(nLevel / nCount * tmp5 / m_fFreqStep);
				}
			}

			y2 = y;
			nLevel = 0;
			nCount = 0;
		}

		nLevel += pBuf[i];
		nCount++;
	}

	for (i = 0; i < pFftWindow->m_nScaleHeight; i++) {
		if (m_pSpgLevel[i] == 0) {
			for (t1 = i; t1 >= 0 && m_pSpgLevel[t1] == 0; t1--)
				;
			for (t2 = i; t2 < pFftWindow->m_nScaleHeight && m_pSpgLevel[t2] == 0; t2++)
				;
			if (t1 < 0)
				m_pSpgLevel[i] = m_pSpgLevel[t2];
			else if (t2 >= pFftWindow->m_nScaleHeight)
				m_pSpgLevel[i] = m_pSpgLevel[t1];
			else {
				t3 = t2 - t1;
				m_pSpgLevel[i] = (m_pSpgLevel[t1] * (t2 - i) + m_pSpgLevel[t2] * (i - t1)) / t3;
			}
		}
	}

	CBitmap *pBitmapOrg = m_dcMem3.SelectObject(pBitmap);
	COLORREF color;

	if (g_oSetData.Fft.nTimeDir == 0)
		x = (int)(pFftWindow->m_nScaleWidth * fTime);
	else
		x = pFftWindow->m_nScaleWidth - (int)(pFftWindow->m_nScaleWidth * fTime) - nTimeStep;

	for (y = 0; y < pFftWindow->m_nScaleHeight; y++) {
		color = GetLevelColor(m_pSpgLevel[y]);
		m_dcMem3.FillSolidRect(x, y, nTimeStep, 1, color);
	}

	m_dcMem3.SelectObject(pBitmapOrg);
}
