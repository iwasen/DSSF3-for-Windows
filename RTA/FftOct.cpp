#include "stdafx.h"
#include "Rta.h"
#include "FftWnd.h"
#include "FftDlg.h"

#define COLOR_SELECT	RGB(255, 255, 128)
#define MIN_VALUE		-1000

void CFftWnd::SetBitmapOct(FFTWINDOW *pFftWindow)
{
	int i, j;
	double fFreqLow, fFreqCenter, fFreqHigh;

	CFont *pFontOrg = m_dcMem.SelectObject(&m_Font);

	TEXTMETRIC tm;
	m_dcMem.GetTextMetrics(&tm);

	if (g_oSetData.Fft.nOctaveBand == 0) {
		// 1/1オクターブ
		m_aFreqScale[0] = g_pFreqScale1[0];
		m_bOctScaleFlag[0] = 1;

		for (i = 0, j = 0; i < MAX_OCT1; i++) {
			fFreqLow = pow(2.0, i - 6.5) * 1000;
			if (fFreqLow >= m_fFreqStep) {
				m_fOctTbl[j] = fFreqLow;
				m_aFreqScale[++j] = g_pFreqScale1[i + 1];
				m_bOctScaleFlag[j] = 1;
			}
		}
		m_nOctBand = j;
		m_fOctTbl[j] = 1000000.0;		// ストッパ
	} else {
		// 1/3オクターブ
		const int nOct3 = g_nOctBandTbl[g_oSetData.Fft.nOctaveBand] / 3;
		const int nBandPerDecade = 10 * nOct3;

		BOOL bFirstBand = FALSE;
		m_aFreqScale[0] = g_pFreqScale3[0];
		m_bOctScaleFlag[0] = 0x03;

		for (i = 0, j = 0; i < MAX_OCT; i++) {
			fFreqLow = pow(10.0, (i - 0.5) / nBandPerDecade + 1.2);

			// 最初のデータの存在するバンドを取得
			if (!bFirstBand) {
				fFreqHigh = pow(10.0, (i + 0.5) / nBandPerDecade + 1.2);
				for (double fFreqMin = m_fFreqStep; fFreqMin < fFreqHigh; fFreqMin += m_fFreqStep) {
					if (fFreqMin >= fFreqLow) {
						bFirstBand = TRUE;
						break;
					}
				}
			}

			if (bFirstBand) {
				m_fOctTbl[j] = fFreqLow;
				if (i % nOct3 == 0) {
					const int n = i / nOct3 + 1;
					m_aFreqScale[++j] = g_pFreqScale3[n];
					m_bOctScaleFlag[j] = 0x01;
					if (n % 10 == 9)
						m_bOctScaleFlag[j] |= 0x02;
				} else {
					fFreqCenter = pow(10.0, (double)i / nBandPerDecade + 1.2);
					if (fFreqCenter < 1000)
						m_aFreqScale[++j].Format("%g", (double)(int)(fFreqCenter * 2 + 0.5) / 2);
					else
						m_aFreqScale[++j].Format("%.2fk", fFreqCenter / 1000);
					m_bOctScaleFlag[j] = 0;
				}
			}

			if (fFreqLow > m_nMaxFreq)
				break;
		}
		m_nOctBand = j;
		m_fOctTbl[j] = 1000000.0;		// ストッパ
	}

	pFftWindow->m_nFrameLeft = tm.tmAveCharWidth * 5 + tm.tmAscent + 4;
	pFftWindow->m_nFrameTop = 5;
	pFftWindow->m_nFrameRight = pFftWindow->m_nWidth - 7;
	pFftWindow->m_nFrameBottom = pFftWindow->m_nHeight - ((tm.tmAscent + 2) * 3) - 3;
	pFftWindow->m_nFrameWidth = pFftWindow->m_nFrameRight - pFftWindow->m_nFrameLeft;
	pFftWindow->m_nFrameHeight = pFftWindow->m_nFrameBottom - pFftWindow->m_nFrameTop;

	pFftWindow->m_nScaleWidth = pFftWindow->m_nFrameWidth;
	m_fOctBarStep = (double)pFftWindow->m_nScaleWidth / m_nOctBand;
	if (m_fOctBarStep - (int)m_fOctBarStep < 0.2)
		m_fOctBarStep = (int)m_fOctBarStep;

	if (m_nViewMode == VM_OVERLAY)
		m_nBarWidth = (int)(m_fOctBarStep / 2.5);
	else
		m_nBarWidth = (int)(m_fOctBarStep / 1.5);

	if (m_nBarWidth == 0)
		m_nBarWidth = 1;

	if (m_nViewMode != VM_SPLIT) {
		pFftWindow->m_nScaleHeight = pFftWindow->m_nFrameHeight;
		DrawScaleOct(pFftWindow, pFftWindow->m_nFrameLeft, pFftWindow->m_nFrameTop, pFftWindow->m_nFrameRight, pFftWindow->m_nFrameBottom, TRUE, tm, m_nChannel == CH_STEREO ? "Lch / Rch" : NULL);
	} else {
		pFftWindow->m_nScaleHeight = pFftWindow->m_nFrameHeight / 2 - pFftWindow->m_nHeight / 50;
		DrawScaleOct(pFftWindow, pFftWindow->m_nFrameLeft, pFftWindow->m_nFrameTop, pFftWindow->m_nFrameRight, pFftWindow->m_nFrameTop + pFftWindow->m_nScaleHeight, FALSE, tm, "Lch");
		DrawScaleOct(pFftWindow, pFftWindow->m_nFrameLeft, pFftWindow->m_nFrameBottom - pFftWindow->m_nScaleHeight, pFftWindow->m_nFrameRight, pFftWindow->m_nFrameBottom, TRUE, tm, "Rch");
	}

	m_dcMem.SelectObject(pFontOrg);

	if (m_bValidFftData) {
		m_aFftData[0].m_bOctData = FALSE;
		m_aFftData[1].m_bOctData = FALSE;
		m_aFftData[0].m_nOctDispBand = 0;
		m_aFftData[1].m_nOctDispBand = 0;
	}

	LoadBimapStretch(m_bitmapLeft, IDB_RTA_OCT_LEFT, m_nBarWidth, 1);
	LoadBimapStretch(m_bitmapRight, IDB_RTA_OCT_RIGHT, m_nBarWidth, 1);
	LoadBimapStretch(m_bitmapPeakLeft, IDB_RTA_OCT_PEAK_LEFT, m_nBarWidth, 1);
	LoadBimapStretch(m_bitmapPeakRight, IDB_RTA_OCT_PEAK_RIGHT, m_nBarWidth, 1);

	m_bPeakReset = TRUE;
}

void CFftWnd::DrawScaleOct(const FFTWINDOW *pFftWindow, int nLeft, int nTop, int nRight, int nBottom, BOOL bAxisX, TEXTMETRIC &tm, LPCSTR pText)
{
	int	nLevel;
	int y;
	int i, j;
	const int nWidth = nRight - nLeft;
	const int nHeight = nBottom - nTop;
	CSize size;
	CString sText;

	CPen *pPenOrg = m_dcMem.SelectObject(&m_penBlack);
	CBrush *pBrushOrg = m_dcMem.SelectObject(&m_brushGraph);
	m_dcMem.Rectangle(nLeft - 1, nTop - 1, nRight + 1, nBottom + 1);

	m_dcMem.SelectObject(&m_penGray);

	m_dcMem.SetTextColor(COLOR_TEXT);
	m_dcMem.SetBkMode(TRANSPARENT);

	m_dcMem.SelectObject(&m_Font);

	double xPos = nLeft + m_fOctBarStep / 2;
	if (bAxisX && g_oSetData.Fft.nOctaveBand == 0) {
		for (i = 0; i < m_nOctBand; i++) {
			size = m_dcMem.GetOutputTextExtent(m_aFreqScale[i]);
			m_dcMem.TextOut((int)(xPos - size.cx / 2), nBottom + 2 + (tm.tmAscent + 2) / 2, m_aFreqScale[i]);
			xPos += m_fOctBarStep;
		}
	} else if (g_oSetData.Fft.nOctaveBand != 0) {
		int nBarWidth = m_nBarWidth;
		if (m_nViewMode == VM_OVERLAY)
			nBarWidth *= 2;

		for (i = 0, j = 0; i < m_nOctBand; i++) {
			if (m_bOctScaleFlag[i]) {
				if (m_bOctScaleFlag[i] & 2)
					m_dcMem.FillSolidRect((int)(xPos - nBarWidth / 2), nTop, nBarWidth, nHeight, RGB(0xe0,0xe0,0xe0));

				if (bAxisX) {
					size = m_dcMem.GetOutputTextExtent(m_aFreqScale[i]);
					m_dcMem.TextOut((int)(xPos - size.cx / 2), nBottom + 2 + (j + 1) % 2 * (tm.tmAscent + 2), m_aFreqScale[i]);
				}
				j++;
			}
			xPos += m_fOctBarStep;
		}
	}

	const int step = GetLinearScaleStep(m_nMaxLevel, m_nMinLevel, nHeight);
	const double fSpanLevel = m_nMaxLevel - m_nMinLevel;
	for (nLevel = GetScaleStartValue(m_nMinLevel, step); nLevel <= m_nMaxLevel; nLevel += step) {
		y = nBottom - (int)((nLevel - m_nMinLevel) * nHeight / fSpanLevel + 0.5);

		if (y > nTop && y < nBottom) {
			m_dcMem.MoveTo(nLeft, y);
			m_dcMem.LineTo(nRight, y);
		}

		sText.Format("%d", nLevel);
		size = m_dcMem.GetOutputTextExtent(sText);
		m_dcMem.TextOut(nLeft - size.cx - 2, y - size.cy / 2, sText);
	}

	if (bAxisX) {
		sText.LoadString(IDS_FREQUENCY);
		sText += " [Hz]";
		size = m_dcMem.GetOutputTextExtent(sText);
		m_dcMem.TextOut(nLeft + (nWidth - size.cx) / 2, pFftWindow->m_nHeight - size.cy, sText);
	}

	m_dcMem.SelectObject(&m_Font2);
	sText.LoadString(IDS_SPL);
	sText += " [dB]";
	size = m_dcMem.GetOutputTextExtent(sText);
	m_dcMem.TextOut(0, nTop + (nHeight + size.cx) / 2, sText);

	DrawNote(pText, nTop, nRight);

	m_dcMem.SelectObject(pBrushOrg);
	m_dcMem.SelectObject(pPenOrg);
}

void CFftWnd::GetWaveDataOct(const FFTWINDOW *pFftWindow)
{
	if (m_bValidFftData) {
		if (m_bLch)
			CalcOct(&m_aFftData[0]);

		if (m_bRch)
			CalcOct(&m_aFftData[1]);
	}

	DispOctInfo(pFftWindow);
}

void CFftWnd::CalcOct(FFTDATA *pFftData) const
{
	int i;
	double *pFftPBuf = pFftData->m_oPowerSpecBuf.GetBuf();
	double *pOctData = pFftData->m_fOctData;
	double const *pOctTbl = m_fOctTbl;
	int nOctData[MAX_OCT];
	int t1, t2, t3;

	if (m_nOctBand > 0) {
		memset(pOctData, 0, m_nOctBand * sizeof(double));
		memset(nOctData, 0, m_nOctBand * sizeof(int));
	}

	pOctData[0] = pFftData->m_fFftAllPower ? dB10(pFftData->m_fFftAllPower) : m_nMinLevel;

	int tblp = 0;
	for (i = 1; i < m_nFftSize / 2; i++) {
		while (i * m_fFreqStep >= pOctTbl[tblp])
			tblp++;

		if (tblp >= 1 && tblp < m_nOctBand) {
			pOctData[tblp] += pFftPBuf[i];
			nOctData[tblp]++;
		}
	}

	for (i = 1; i < m_nOctBand; i++) {
		if (nOctData[i] != 0)
			pOctData[i] = pOctData[i] ? dB10(pOctData[i] / nOctData[i] * ((pOctTbl[i] - pOctTbl[i - 1]) / m_fFreqStep)) : m_nMinLevel;
	}

	for (i = 1; i < m_nOctBand; i++) {
		if (nOctData[i] == 0) {
			for (t1 = i; t1 > 0 && nOctData[t1] == 0; t1--)
				;
			for (t2 = i; t2 < m_nOctBand && nOctData[t2] == 0; t2++)
				;
			if (t1 > 0 && t2 < m_nOctBand) {
				t3 = t2 - t1;
				pOctData[i] = (pOctData[t1] * (t2 - i) + pOctData[t2] * (i - t1)) / t3;
			} else
				pOctData[i] = MIN_VALUE;
		}
	}

	double *pPeakLevel = pFftData->m_fOctPeakLevel;
	for (i = 0; i < m_nOctBand; i++) {
		if (pOctData[i] > *pPeakLevel || m_bPeakReset)
			*pPeakLevel = pOctData[i];

		pPeakLevel++;
	}

	pFftData->m_bOctData = TRUE;
}

void CFftWnd::DispOctInfo(const FFTWINDOW *pFftWindow)
{
	if (m_bLch)
		DispOctInfoSub(pFftWindow, &m_aFftData[0], WM_FREQ_LEVEL_LEFT);

	if (m_bRch)
		DispOctInfoSub(pFftWindow, &m_aFftData[1], WM_FREQ_LEVEL_RIGHT);
}

void CFftWnd::DispOctInfoSub(const FFTWINDOW *pFftWindow, FFTDATA *pFftData, int nMessageId)
{
	CString sFreq, sLevel;

	if (g_oSetData.Fft.bPeakDisp) {
		double *pOctData = pFftData->m_fOctData;
		double maxLevel = MIN_VALUE;
		for (int i = 1; i < m_nOctBand; i++) {
			if (pOctData[i] > maxLevel) {
				maxLevel = pOctData[i];
				pFftData->m_nOctDispBand = i;
			}
		}
	}

	sLevel.Format("%.2lf", pFftData->m_fOctData[pFftData->m_nOctDispBand]);
	sFreq = m_aFreqScale[pFftData->m_nOctDispBand];

	if (pFftWindow->m_bMasterWindow)
		m_pFftDlg->SendMessage(nMessageId, (WPARAM)(LPCSTR)sFreq, (LPARAM)(LPCSTR)sLevel);
	else
		pFftWindow->m_pWnd->SendMessage(nMessageId, (WPARAM)(LPCSTR)sFreq, (LPARAM)(LPCSTR)sLevel);
}

void CFftWnd::PaintOct(const FFTWINDOW *pFftWindow, int nChannel)
{
	CRect rect;
	int i;
	double *pOctData;
	double *pPeakLevel;
	const double xStep = m_fOctBarStep;
	const double yStep = (double)pFftWindow->m_nScaleHeight / (m_nMinLevel - m_nMaxLevel);
	FFTDATA *pFftData;

	double xPos = xStep / 2;

	CObject *pBitmapOrg = NULL;

	if (nChannel == 0) {
		pFftData = &m_aFftData[0];

		if (pFftData->m_bOctData) {
			pBitmapOrg = m_dcMem3.SelectObject(&m_bitmapLeft);

			if (m_nViewMode != VM_OVERLAY)
				xPos -= m_nBarWidth / 2;
			else
				xPos -= m_nBarWidth;
			pOctData = pFftData->m_fOctData;
			pPeakLevel = pFftData->m_fOctPeakLevel;
			for (i = 0; i < m_nOctBand; i++) {
				rect.left = (int)xPos;
				rect.top = (int)((*pOctData++ - m_nMaxLevel) * yStep + 0.5);
				rect.right = rect.left + m_nBarWidth;
				rect.bottom = pFftWindow->m_nScaleHeight;

				if (i == pFftData->m_nOctDispBand) {
					m_dcMem3.SelectObject(&m_bitmapPeakLeft);
					m_dcMem2.SelectObject(&m_penLeftPeak);
				} else {
					m_dcMem3.SelectObject(&m_bitmapLeft);
					m_dcMem2.SelectObject(&m_penLeft);
				}

				if (m_nBarWidth == 1) {
					m_dcMem2.MoveTo(rect.left, rect.top);
					m_dcMem2.LineTo(rect.left, rect.bottom);
				} else if (m_nBarWidth == 2) {
					m_dcMem2.Rectangle(&rect);
				} else {
					m_dcMem2.StretchBlt(rect.left, rect.top, rect.Width(), rect.Height(), &m_dcMem3, 0, 0, rect.Width(), 1, SRCCOPY);
					m_dcMem2.MoveTo(rect.left, rect.top);
					m_dcMem2.LineTo(rect.right, rect.top);
				}

				if (g_oSetData.Fft.bPeakHold) {
					rect.top = (int)((*pPeakLevel++ - m_nMaxLevel) * yStep + 0.5);
					rect.bottom = rect.top + 2;
					m_dcMem2.FillRect(&rect, &m_brushLeftHold);
				}

				xPos += xStep;
			}
		}
	} else {
		pFftData = &m_aFftData[1];

		if (pFftData->m_bOctData) {
			if (m_nViewMode != VM_OVERLAY)
				xPos -= m_nBarWidth / 2;

			pBitmapOrg = m_dcMem3.SelectObject(&m_bitmapRight);

			pOctData = pFftData->m_fOctData;
			pPeakLevel = pFftData->m_fOctPeakLevel;
			for (i = 0; i < m_nOctBand; i++) {
				rect.left = (int)xPos;
				rect.top = (int)((*pOctData++ - m_nMaxLevel) * yStep + 0.5);
				rect.right = rect.left + m_nBarWidth;
				rect.bottom = pFftWindow->m_nScaleHeight;

				if (i == pFftData->m_nOctDispBand) {
					m_dcMem3.SelectObject(&m_bitmapPeakRight);
					m_dcMem2.SelectObject(&m_penRightPeak);
				} else {
					m_dcMem3.SelectObject(&m_bitmapRight);
					m_dcMem2.SelectObject(&m_penRight);
				}

				if (m_nBarWidth == 1) {
					m_dcMem2.MoveTo(rect.left, rect.top);
					m_dcMem2.LineTo(rect.left, rect.bottom);
				} else if (m_nBarWidth == 2) {
					m_dcMem2.Rectangle(&rect);
				} else {
					m_dcMem2.StretchBlt(rect.left, rect.top, rect.Width(), rect.Height(), &m_dcMem3, 0, 0, rect.Width(), 1, SRCCOPY);
					m_dcMem2.MoveTo(rect.left, rect.top);
					m_dcMem2.LineTo(rect.right, rect.top);
				}

				if (g_oSetData.Fft.bPeakHold) {
					rect.top = (int)((*pPeakLevel++ - m_nMaxLevel) * yStep + 0.5);
					rect.bottom = rect.top + 2;
					m_dcMem2.FillRect(&rect, &m_brushRightHold);
				}

				xPos += xStep;
			}
		}
	}

	if (pBitmapOrg != NULL)
		m_dcMem3.SelectObject(pBitmapOrg);
}

void CFftWnd::SetDispFreqOct(const FFTWINDOW *pFftWindow, CPoint point)
{
	if (!g_oSetData.Fft.bPeakDisp) {
		const int nBand = (int)((point.x - pFftWindow->m_nFrameLeft) / m_fOctBarStep);
		if (nBand >= 0 && nBand < m_nOctBand) {
			m_aFftData[0].m_nOctDispBand = nBand;
			m_aFftData[1].m_nOctDispBand = nBand;
			DispOctInfo(pFftWindow);
			pFftWindow->m_pWnd->Invalidate(FALSE);
		}
	}
}

void CFftWnd::ResetDispFreqOct(const FFTWINDOW *pFftWindow)
{
	if (!g_oSetData.Fft.bPeakDisp) {
		m_aFftData[0].m_nOctDispBand = 0;
		m_aFftData[1].m_nOctDispBand = 0;
		DispOctInfo(pFftWindow);
		pFftWindow->m_pWnd->Invalidate(FALSE);
	}
}

void CFftWnd::CsvOutputOct(CCsvFile &oCsvFile)
{
	oCsvFile.SetData("FFT");
	oCsvFile.SetData("Octave band");
	oCsvFile.Output();

	oCsvFile.SetData("Frequency[Hz]");
	switch (g_oSetData.Fft.nChannel) {
	case CH_MONO:
		oCsvFile.SetData("Level-M[dB]");
		break;
	case CH_STEREO:
		oCsvFile.SetData("Level-L[dB]");
		oCsvFile.SetData("Level-R[dB]");
		break;
	case CH_LEFT_ONLY:
		oCsvFile.SetData("Level-L[dB]");
		break;
	case CH_RIGHT_ONLY:
		oCsvFile.SetData("Level-R[dB]");
		break;
	case CH_DIFFERENCE:
		oCsvFile.SetData("Level-D[dB]");
		break;
	}
	if (g_oSetData.Fft.bPeakHold) {
		switch (g_oSetData.Fft.nChannel) {
		case CH_MONO:
			oCsvFile.SetData("Peak-M[dB]");
			break;
		case CH_STEREO:
			oCsvFile.SetData("Peak-L[dB]");
			oCsvFile.SetData("Peak-R[dB]");
			break;
		case CH_LEFT_ONLY:
			oCsvFile.SetData("Peak-L[dB]");
			break;
		case CH_RIGHT_ONLY:
			oCsvFile.SetData("Peak-R[dB]");
			break;
		case CH_DIFFERENCE:
			oCsvFile.SetData("Peak-D[dB]");
			break;
		}
	}
	oCsvFile.Output();

	double *pBuf1 = NULL;
	double *pBuf2 = NULL;
	double *pBuf3 = NULL;
	double *pBuf4 = NULL;

	if (m_bLch)
		pBuf1 = m_aFftData[0].m_fOctData;

	if (m_bRch)
		pBuf2 = m_aFftData[1].m_fOctData;

	if (g_oSetData.Fft.bPeakHold) {
		if (m_bLch)
			pBuf3 = m_aFftData[0].m_fOctPeakLevel;

		if (m_bRch)
			pBuf4 = m_aFftData[1].m_fOctPeakLevel;
	}

	for (int i = 0; i < m_nOctBand; i++) {
		oCsvFile.SetData(m_aFreqScale[i]);

		if (pBuf1 != NULL)
			oCsvFile.SetData(pBuf1[i]);

		if (pBuf2 != NULL)
			oCsvFile.SetData(pBuf2[i]);

		if (pBuf3 != NULL)
			oCsvFile.SetData(pBuf3[i]);

		if (pBuf4 != NULL)
			oCsvFile.SetData(pBuf4[i]);

		oCsvFile.Output();
	}
}

void CFftWnd::CsvInputOct(CCsvFile &oCsvFile)
{
	if (!oCsvFile.Input() || oCsvFile.GetString(0) != "Frequency[Hz]") {
		::AfxMessageBox(IDS_ERR_CSV_FORMAT, MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	CString sItem = oCsvFile.GetString(1);
	if (sItem == "Level-M[dB]") {
		m_nChannel = CH_MONO;
	} else if (sItem == "Level-L[dB]") {
		if (oCsvFile.GetString(2) == "Level-R[dB]" ) {
			m_nChannel = CH_STEREO;
		} else {
			m_nChannel = CH_LEFT_ONLY;
		}
	} else if (sItem == "Level-R[dB]") {
		m_nChannel = CH_RIGHT_ONLY;
	} else if (sItem == "Level-D[dB]") {
		m_nChannel = CH_DIFFERENCE;
	} else {
		::AfxMessageBox(IDS_ERR_CSV_FORMAT, MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	struct SCsvData {
		CString sFreq;
		double fData[3];
	} oCsvData;
	CList<SCsvData, SCsvData&> oCsvDataList;

	while (oCsvFile.Input()) {
		oCsvData.sFreq = oCsvFile.GetString(0);
		oCsvData.fData[0] = oCsvFile.GetDouble(0);
		oCsvData.fData[1] = oCsvFile.GetDouble(1);
		oCsvData.fData[2] = oCsvFile.GetDouble(2);
		oCsvDataList.AddTail(oCsvData);
	}

	const double fFreq1 = oCsvDataList.GetAt(oCsvDataList.FindIndex(1)).fData[0];
	const double fFreq2 = oCsvDataList.GetAt(oCsvDataList.FindIndex(4)).fData[0];
	const double fStep = fFreq2 / fFreq1;
	int nOctBandIndex;
	if (fStep > 4) {
		nOctBandIndex = 0;
	} else if (fStep > 1.7) {
		nOctBandIndex = 1;
	} else if (fStep > 1.3) {
		nOctBandIndex = 2;
	} else if (fStep > 1.15) {
		nOctBandIndex = 3;
	} else {
		nOctBandIndex = 4;
	}

	m_pFftDlg->SetCsvConditions(FFT_MODE_OCT, m_nChannel, -1, -1, nOctBandIndex, -1);

	double *pBuf1 = NULL;
	double *pBuf2 = NULL;

	if (m_bLch) {
		pBuf1 = m_aFftData[0].m_fOctData;
		m_aFftData[0].m_bOctData = TRUE;
	}

	if (m_bRch) {
		pBuf2 = m_aFftData[1].m_fOctData;
		m_aFftData[1].m_bOctData = TRUE;
	}

	int i = 0;
	while (i < MAX_OCT && !oCsvDataList.IsEmpty()) {
		oCsvData = oCsvDataList.RemoveHead();
		int nCol = 1;
		m_aFreqScale[i++] = oCsvData.sFreq;

		if (pBuf1 != NULL) {
			*pBuf1++ = oCsvData.fData[nCol++];
		}

		if (pBuf2 != NULL) {
			*pBuf2++ = oCsvData.fData[nCol++];
		}
	}

	for ( ; i < MAX_OCT; i++) {
		if (pBuf1 != NULL) {
			*pBuf1++ = -200;
		}

		if (pBuf2 != NULL) {
			*pBuf2++ = -200;
		}
	}

	FFTWINDOW *pFftWindow = GetFftWindow(FFT_MODE_OCT);
	GetWaveDataOct(pFftWindow);
}
