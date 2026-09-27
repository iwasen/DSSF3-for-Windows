#include "stdafx.h"
#include "Rta.h"
#include "FftWnd.h"
#include "FftDlg.h"

void CFftWnd::SetBitmapCrf(FFTWINDOW *pFftWindow)
{
	CFont *pFontOrg = m_dcMem.SelectObject(&m_Font);

	TEXTMETRIC tm;
	m_dcMem.GetTextMetrics(&tm);

	pFftWindow->m_nFrameLeft = tm.tmAveCharWidth * 5 + tm.tmAscent + 4;
	pFftWindow->m_nFrameTop = 5;
	pFftWindow->m_nFrameRight = pFftWindow->m_nWidth - 7;
	pFftWindow->m_nFrameBottom = pFftWindow->m_nHeight - (tm.tmAscent + 2) * 2 - 3;
	pFftWindow->m_nFrameWidth = pFftWindow->m_nFrameRight - pFftWindow->m_nFrameLeft;
	pFftWindow->m_nFrameHeight = pFftWindow->m_nFrameBottom - pFftWindow->m_nFrameTop;

	m_nCrfRange = g_oSetData.Fft.nCrfZoom * CRF_RANGE;

	if (m_nViewMode != VM_SPLIT) {
		pFftWindow->m_nScaleWidth = pFftWindow->m_nFrameWidth;
		pFftWindow->m_nScaleHeight = pFftWindow->m_nFrameHeight;
		DrawScaleCrf(pFftWindow, pFftWindow->m_nFrameLeft, pFftWindow->m_nFrameTop, pFftWindow->m_nFrameRight, pFftWindow->m_nFrameBottom, TRUE, m_nChannel == CH_STEREO ? "Lch / Rch" : NULL);
	} else {
		pFftWindow->m_nScaleWidth = pFftWindow->m_nFrameWidth;
		pFftWindow->m_nScaleHeight = pFftWindow->m_nFrameHeight / 2 - pFftWindow->m_nHeight / 50;

		DrawScaleCrf(pFftWindow, pFftWindow->m_nFrameLeft, pFftWindow->m_nFrameTop, pFftWindow->m_nFrameRight, pFftWindow->m_nFrameTop + pFftWindow->m_nScaleHeight, FALSE, "Lch");
		DrawScaleCrf(pFftWindow, pFftWindow->m_nFrameLeft, pFftWindow->m_nFrameBottom - pFftWindow->m_nScaleHeight, pFftWindow->m_nFrameRight, pFftWindow->m_nFrameBottom, TRUE, "Rch");
	}

	m_dcMem.SelectObject(pFontOrg);

	for (int i = 0; i < 2; i++) {
		FFTDATA &oFftData = m_aFftData[i];

		if (oFftData.m_pCrfPoint == NULL)
			oFftData.m_pCrfPoint = new POINT[(m_nFftSize / m_nCrfRange + 1) * 2];

		oFftData.m_nCrfPointCount = 0;
	}
}

void CFftWnd::DrawScaleCrf(const FFTWINDOW *pFftWindow, int nLeft, int nTop, int nRight, int nBottom, BOOL bAxisX, LPCTSTR pText)
{
	CString sText;
	CSize size;
	int ns;
	double step, t1, t2;
	int nCenter = nTop + pFftWindow->m_nScaleHeight / 2;

	CPen *pPenOrg = m_dcMem.SelectObject(&m_penBlack);
	CBrush *pBrushOrg = m_dcMem.SelectObject(&m_brushGraph);
	m_dcMem.Rectangle(nLeft - 1, nTop - 1, nRight + 1, nBottom + 1);

	m_dcMem.SelectObject(&m_penBlack);

	m_dcMem.SetBkMode(TRANSPARENT);
	m_dcMem.SetTextColor(COLOR_TEXT);

	m_dcMem.SelectObject(&m_Font);

	for (int i = 100; i >= -100; i -= 20) {
		int y = (int)(nCenter - (i * ((double)pFftWindow->m_nScaleHeight / 2) / 100) + 0.5);
		if (i != 100 && i != -100) {
			if (i == 0)
				m_dcMem.SelectObject(&m_penBlack);
			else
				m_dcMem.SelectObject(&m_penLightGray);

			m_dcMem.MoveTo(nLeft, y);
			m_dcMem.LineTo(nRight, y);
		}

		sText.Format("%g", (double)i / 100);
		size = m_dcMem.GetOutputTextExtent(sText);
		m_dcMem.TextOut(nLeft - size.cx - 3, y - size.cy / 2, sText);
	}

	if (g_oSetData.Fft.nCorrelation == 0)
		t1 = m_fTimeStep / (10 * m_nCrfRange);
	else
		t1 = m_fTimeStep / (5 * m_nCrfRange);
	t1 *= 500.0 / pFftWindow->m_nScaleWidth;

	step = pow(10.0, floor(log10(t1)));
	t2 = t1 / step;
	if (t2 < 3) {
		step *= 0.5;
		ns = 2;
	} else if (t2 < 5)
		ns = 2;
	else
		ns = 5;

	if (g_oSetData.Fft.nCorrelation == 0)
		SetBitmapCrfSub(nLeft, nTop, nRight, nBottom, bAxisX, step, nLeft, pFftWindow->m_nScaleWidth, 1, ns);
	else {
		SetBitmapCrfSub(nLeft, nTop, nRight, nBottom, bAxisX, step, nLeft + pFftWindow->m_nScaleWidth / 2, pFftWindow->m_nScaleWidth / 2, 1, ns);
		SetBitmapCrfSub(nLeft, nTop, nRight, nBottom, bAxisX, step, nLeft + pFftWindow->m_nScaleWidth / 2, pFftWindow->m_nScaleWidth / 2, -1, ns);
	}

	if (bAxisX) {
		sText.LoadString(IDS_TAU);
		sText += " [ms]";
		size = m_dcMem.GetOutputTextExtent(sText);
		m_dcMem.TextOut(nLeft + (pFftWindow->m_nScaleWidth - size.cx) / 2, pFftWindow->m_nHeight - size.cy, sText);
	}

	m_dcMem.SelectObject(&m_Font2);
	sText.LoadString(IDS_CORRELATION);
	size = m_dcMem.GetOutputTextExtent(sText);
	m_dcMem.TextOut(0, nTop + (pFftWindow->m_nScaleHeight + size.cx) / 2, sText);

	if (g_oSetData.Fft.nCorrelation == 0)
		DrawNote(pText, nTop, nRight);

	m_dcMem.SelectObject(pBrushOrg);
	m_dcMem.SelectObject(pPenOrg);
}

void CFftWnd::SetBitmapCrfSub(int nLeft, int nTop, int nRight, int nBottom, BOOL bAxisX, double step, int zero, int width, int dir, int ns)
{
	CString sText;
	CSize size;
	double fTime = m_fTimeStep / m_nCrfRange;

	for (double t = 0; t < fTime; t += step) {
		int x = zero + int(t * width / fTime + 0.5) * dir;
		if (x >= nLeft && x <= nRight) {
			if (int(t / step + 0.5) % ns == 0) {
				if (x > nLeft) {
					if (t == 0)
						m_dcMem.SelectObject(&m_penBlack);
					else
						m_dcMem.SelectObject(&m_penGray);
					m_dcMem.MoveTo(x, nTop);
					m_dcMem.LineTo(x, nBottom);
				}

				if (bAxisX) {
					sText.Format("%g", (t == 0) ? 0 : t * 1000 * dir);
					size = m_dcMem.GetOutputTextExtent(sText);
					m_dcMem.TextOut(x - size.cx / 2, nBottom + 2, sText);
				}
			} else {
				if (x > nLeft) {
					m_dcMem.SelectObject(&m_penLightGray);
					m_dcMem.MoveTo(x, nTop);
					m_dcMem.LineTo(x, nBottom);
				}
			}
		}
	}
}

void CFftWnd::GetWaveDataCrf(const FFTWINDOW *pFftWindow)
{
	if (g_oSetData.Fft.nCorrelation == 0) {
		// Ž©ŒÈ‘ŠŠÖ
		if (m_bLch) {
			if (m_bValidFftData) {
				CalcAutoCorrelation(m_aFftData[0].m_pPowerSpecBuf, m_aFftData[0].m_pCrfBuf);
			}
			CalcCrf(pFftWindow, &m_aFftData[0], 0, m_nFftSize / m_nCrfRange);
		}

		if (m_bRch) {
			if (m_bValidFftData) {
				CalcAutoCorrelation(m_aFftData[1].m_pPowerSpecBuf, m_aFftData[1].m_pCrfBuf);
			}
			CalcCrf(pFftWindow, &m_aFftData[1], 0, m_nFftSize / m_nCrfRange);
		}
	} else {
		// ‘ŠŒÝ‘ŠŠÖ
		if (m_bValidFftData) {
			CalcLateralCorrelation(m_pCrossBufRe, m_pCrossBufIm);
		}
		CalcCrf(pFftWindow, &m_aFftData[0], m_nFftSize - m_nFftSize / m_nCrfRange, m_nFftSize * 2 / m_nCrfRange);
		m_aFftData[1].m_nCrfPointCount = 0;
	}

	DispCrfInfo(pFftWindow);
}

void CFftWnd::CalcAutoCorrelation(const double *pFftBuf, double *pCrfBuf)
{
	int i, j;
	int nFftSize2 = m_nFftSize / 2;

	pCrfBuf[0] = pCrfBuf[1] = 0;

	for (i = 1; i < nFftSize2; i++) {
		j = i * 2;

		pCrfBuf[j] = pFftBuf[i];
		pCrfBuf[j + 1] = 0;
	}

	m_oFFT.ifft(m_nFftSize, pCrfBuf);

	double f0 = 1 / pCrfBuf[0];
	for (i = 0; i < nFftSize2; i++)
		pCrfBuf[i] *= f0;
}

void CFftWnd::CalcLateralCorrelation(double *pFftBufRe, double *pFftBufIm)
{
	int i, j;
	int nFftSize2 = m_nFftSize / 2;
	double *pCrfBuf = m_aFftData[0].m_pCrfBuf;

	pCrfBuf[0] = pCrfBuf[1] = 0;

	for (i = 1; i < nFftSize2; i++) {
		j = i * 2;

		pCrfBuf[j] = pFftBufRe[i];
		pCrfBuf[j + 1] = pFftBufIm[i];
	}

	m_oFFT.ifft(m_nFftSize, pCrfBuf);

	double f0 = 0;
	for (i = 0; i < nFftSize2; i++) {
		f0 += m_aFftData[0].m_pPowerSpecBuf[i];
		f0 += m_aFftData[1].m_pPowerSpecBuf[i];
	}
	f0 = 2 / f0;

	for (i = 0; i < m_nFftSize; i++)
		pCrfBuf[i] *= f0;
}

void CFftWnd::CalcCrf(const FFTWINDOW *pFftWindow, FFTDATA *pFftData, int nOffset, int nDataSize) const
{
	POINT *pPoint = pFftData->m_pCrfPoint;
	int x, y;
	int x2 = -1, y2 = 0;
	int nPointCount = 0;
	int nIndex;
	int nCenter = pFftWindow->m_nScaleHeight / 2;
	double *pCrfBuf = pFftData->m_pCrfBuf;

	for (int i = 0; ; i++) {
		x = (i * pFftWindow->m_nScaleWidth + nDataSize / 2) / nDataSize;

		nIndex = nOffset + i;
		if (nIndex >= m_nFftSize)
			nIndex -= m_nFftSize;
		y = (int)(nCenter - pCrfBuf[nIndex] * pFftWindow->m_nScaleHeight / 2 + 0.5);

		if (x != x2 || y != y2) {
			pPoint[nPointCount].x = x;
			pPoint[nPointCount].y = y;
			nPointCount++;

			if (x >= pFftWindow->m_nScaleWidth)
				break;

			x2 = x;
			y2 = y;
		}
	}

	pFftData->m_nCrfPointCount = nPointCount;
}

void CFftWnd::DispCrfInfo(const FFTWINDOW *pFftWindow)
{
	if (m_bLch)
		DispCrfInfoSub(pFftWindow, &m_aFftData[0], WM_FREQ_LEVEL_LEFT);

	if (m_bRch)
		DispCrfInfoSub(pFftWindow, &m_aFftData[1], WM_FREQ_LEVEL_RIGHT);
}

void CFftWnd::DispCrfInfoSub(const FFTWINDOW *pFftWindow, FFTDATA *pFftData, int nMessageId)
{
	CString sTime, sLevel;
	double *pCrfBuf = pFftData->m_pCrfBuf;
	int nDataSize = (g_oSetData.Fft.nCorrelation == 0) ? m_nFftSize / m_nCrfRange : m_nFftSize * 2 / m_nCrfRange;
	int nCrfDispTime = (g_oSetData.Fft.nCorrelation == 0) ? pFftData->m_nCrfDispTime : pFftData->m_nCrfDispTime - nDataSize / 2;
	int nIndex = (nCrfDispTime + m_nFftSize) % m_nFftSize;

	pFftData->m_nCrfTimePos = (pFftData->m_nCrfDispTime * pFftWindow->m_nScaleWidth) / nDataSize;
	pFftData->m_nCrfLevelPos = (int)((pFftWindow->m_nScaleHeight / 2) - pCrfBuf[nIndex] * pFftWindow->m_nScaleHeight / 2 + 0.5);

	if (pFftData->m_nCrfDispTime == 0) {
		sLevel.Empty();
		sTime.Empty();
	} else {
		sLevel.Format("%.2lf", pCrfBuf[nIndex]);
		sTime.Format("%.1lf",  1000.0 * nCrfDispTime / m_nSamplingRate);
	}

	if (pFftWindow->m_bMasterWindow)
		m_pFftDlg->SendMessage(nMessageId, (WPARAM)(LPCSTR)sTime, (LPARAM)(LPCSTR)sLevel);
	else
		pFftWindow->m_pWnd->SendMessage(nMessageId, (WPARAM)(LPCSTR)sTime, (LPARAM)(LPCSTR)sLevel);
}

void CFftWnd::PaintCrf(const FFTWINDOW *pFftWindow, int nChannel)
{
	if (nChannel == 0) {
		if (m_aFftData[0].m_nCrfPointCount != 0) {
			if (g_oSetData.Fft.nCorrelation == 0)
				PaintCrfSub(pFftWindow, &m_aFftData[0], 0, m_penLeft, m_penLeftLine, m_penLeftPeak);
			else
				PaintCrfSub(pFftWindow, &m_aFftData[0], 0, m_penGreen, m_penLightGreen, m_penGreen);
		}
	} else {
		if (m_aFftData[1].m_nCrfPointCount != 0)
			PaintCrfSub(pFftWindow, &m_aFftData[1], m_nViewMode == VM_OVERLAY ? 1 : 0, m_penRight, m_penRightLine, m_penRightPeak);
	}
	m_dcMem2.SetROP2(R2_COPYPEN);
}

void CFftWnd::PaintCrfSub(const FFTWINDOW *pFftWindow, FFTDATA *pFftData, int nOffest, CPen &penData, CPen &penLine, CPen &penPeak)
{
	int nCenter = pFftWindow->m_nScaleHeight / 2;
	int i, j, y;
	int dx,dy;

	m_dcMem2.SelectObject(penData);
	m_dcMem2.SetROP2(R2_COPYPEN);
	m_dcMem2.Polyline(pFftData->m_pCrfPoint, pFftData->m_nCrfPointCount);
	m_dcMem2.SelectObject(penLine);
	m_dcMem2.SetROP2(R2_MASKPEN);
	for (i = nOffest, j = 0; i < pFftWindow->m_nScaleWidth; i += 2) {
		while (pFftData->m_pCrfPoint[j].x < i)
			j++;
		if (pFftData->m_pCrfPoint[j].x == i)
			y = pFftData->m_pCrfPoint[j].y;
		else {
			dx = pFftData->m_pCrfPoint[j].x - pFftData->m_pCrfPoint[j - 1].x;
			dy = pFftData->m_pCrfPoint[j].y - pFftData->m_pCrfPoint[j - 1].y;
			y = pFftData->m_pCrfPoint[j].y - dy * (pFftData->m_pCrfPoint[j].x - i) / dx;
		}
		m_dcMem2.MoveTo(i, nCenter);
		m_dcMem2.LineTo(i, y);
	}

	if (pFftData->m_nCrfDispTime != 0) {
		m_dcMem2.SelectObject(&penPeak);
		m_dcMem2.MoveTo(pFftData->m_nCrfTimePos, 0);
		m_dcMem2.LineTo(pFftData->m_nCrfTimePos, pFftWindow->m_nScaleHeight);
		m_dcMem2.MoveTo(0, pFftData->m_nCrfLevelPos);
		m_dcMem2.LineTo(pFftWindow->m_nScaleWidth, pFftData->m_nCrfLevelPos);
	}
}

void CFftWnd::SetDispFreqCrf(const FFTWINDOW *pFftWindow, CPoint point)
{
	if (!g_oSetData.Fft.bPeakDisp) {
		int nDataSize = (g_oSetData.Fft.nCorrelation == 0) ? m_nFftSize / m_nCrfRange : m_nFftSize * 2 / m_nCrfRange;

		int n = (point.x - pFftWindow->m_nFrameLeft) * nDataSize / pFftWindow->m_nScaleWidth;
		m_aFftData[0].m_nCrfDispTime = m_aFftData[1].m_nCrfDispTime = (n > 1 && n < nDataSize) ? n : 0;
		DispCrfInfo(pFftWindow);
		pFftWindow->m_pWnd->Invalidate(FALSE);
	}
}

void CFftWnd::ResetDispFreqCrf(const FFTWINDOW *pFftWindow)
{
	if (!g_oSetData.Fft.bPeakDisp) {
		m_aFftData[0].m_nCrfDispTime = m_aFftData[1].m_nCrfDispTime = 0;
		DispCrfInfo(pFftWindow);
		pFftWindow->m_pWnd->Invalidate(FALSE);
	}
}

void CFftWnd::CsvOutputCrf(CCsvFile &oCsvFile) const
{
	oCsvFile.SetData("FFT");
	oCsvFile.SetData("Correlation");
	oCsvFile.Output();

	if (g_oSetData.Fft.nCorrelation == 0) {
		// Ž©ŒÈ‘ŠŠÖ
		oCsvFile.SetData("Time[ms]");
		switch (g_oSetData.Fft.nChannel) {
		case CH_MONO:
			oCsvFile.SetData("ACF-M");
			break;
		case CH_STEREO:
			oCsvFile.SetData("ACF-L");
			oCsvFile.SetData("ACF-R");
			break;
		case CH_LEFT_ONLY:
			oCsvFile.SetData("ACF-L");
			break;
		case CH_RIGHT_ONLY:
			oCsvFile.SetData("ACF-R");
			break;
		case CH_DIFFERENCE:
			oCsvFile.SetData("ACF-D");
			break;
		}

		oCsvFile.Output();

		double *pBuf1 = NULL;
		double *pBuf2 = NULL;

		if (m_bLch)
			pBuf1 = m_aFftData[0].m_pCrfBuf;

		if (m_bRch)
			pBuf2 = m_aFftData[1].m_pCrfBuf;

		int nFftSize2 = m_nFftSize / CRF_RANGE;
		for (int i = 0; i < nFftSize2; i++) {
			oCsvFile.SetData(1000.0 * i / m_nSamplingRate);

			if (pBuf1 != NULL)
				oCsvFile.SetData(pBuf1[i]);

			if (pBuf2 != NULL)
				oCsvFile.SetData(pBuf2[i]);

			oCsvFile.Output();
		}
	} else {
		// ‘ŠŒÝ‘ŠŠÖ
		oCsvFile.SetData("Time[ms]");
		oCsvFile.SetData("CCF");
		oCsvFile.Output();

		double *pBuf1 = m_aFftData[0].m_pCrfBuf;

		int nFftSize2 = m_nFftSize / CRF_RANGE;
		for (int i = 0; i < nFftSize2 * 2; i++) {
			oCsvFile.SetData(1000.0 * (i - nFftSize2) / m_nSamplingRate);

			oCsvFile.SetData(pBuf1[(i - nFftSize2 + m_nFftSize) % m_nFftSize]);

			oCsvFile.Output();
		}
	}
}

void CFftWnd::CsvInputCrf(CCsvFile &oCsvFile)
{
	if (!oCsvFile.Input() || oCsvFile.GetString(0) != "Time[ms]") {
		::AfxMessageBox(IDS_ERR_CSV_FORMAT, MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	CString sItem = oCsvFile.GetString(1);
	int nChannel;
	int nCorrelation = 0;
	if (sItem == "ACF-M") {
		nChannel = CH_MONO;
	} else if (sItem == "ACF-L") {
		if (oCsvFile.GetString(2) == "ACF-R" ) {
			nChannel = CH_STEREO;
		} else {
			nChannel = CH_LEFT_ONLY;
		}
	} else if (sItem == "ACF-R") {
		nChannel = CH_RIGHT_ONLY;
	} else if (sItem == "ACF-D") {
		nChannel = CH_DIFFERENCE;
	} else if (sItem == "CCF") {
		nChannel = CH_STEREO;
		nCorrelation = 1;
	} else {
		::AfxMessageBox(IDS_ERR_CSV_FORMAT, MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	struct SCsvData {
		double fData[3];
	} oCsvData;
	CList<SCsvData, SCsvData&> oCsvDataList;

	while (oCsvFile.Input()) {
		oCsvData.fData[0] = oCsvFile.GetDouble(0) / 1000.0;
		oCsvData.fData[1] = oCsvFile.GetDouble(1);
		oCsvData.fData[2] = oCsvFile.GetDouble(2);
		oCsvDataList.AddTail(oCsvData);
	}

	int nDataNum = (int)oCsvDataList.GetCount();
	int nFftSize = nDataNum * CRF_RANGE / (nCorrelation == 0 ? 1 : 2);
	int nFFtSizeIndex = (int)floor(log((double)nFftSize) / log(2.0) + 0.5) - 10;
	double fSamplingRate = 1 / (oCsvDataList.GetAt(oCsvDataList.FindIndex(1)).fData[0] - oCsvDataList.GetAt(oCsvDataList.FindIndex(0)).fData[0]);
	int nSamplingRateIndex = GetSamplingRateIndex((int)fSamplingRate);
	m_pFftDlg->SetCsvConditions(FFT_MODE_CRF, nChannel, nFFtSizeIndex, nSamplingRateIndex, -1, nCorrelation);

	double *pBuf1 = NULL;
	double *pBuf2 = NULL;
	int nBufSize;

	if (nCorrelation == 0) {
		if (m_bLch)
			pBuf1 = m_aFftData[0].m_pCrfBuf;

		if (m_bRch)
			pBuf2 = m_aFftData[1].m_pCrfBuf;

		nBufSize = m_nFftSize / CRF_RANGE;

		for (int i = 0; i < nBufSize && !oCsvDataList.IsEmpty(); i++) {
			oCsvData = oCsvDataList.RemoveHead();
			int nCol = 1;

			if (pBuf1 != NULL) {
				*pBuf1++ = oCsvData.fData[nCol++];
			}

			if (pBuf2 != NULL) {
				*pBuf2++ = oCsvData.fData[nCol++];
			}
		}
	} else {
		pBuf1 = m_aFftData[0].m_pCrfBuf;
		nBufSize = m_nFftSize / CRF_RANGE * 2;
		int nFftSize2 = m_nFftSize / CRF_RANGE;

		for (int i = 0; i < nBufSize && !oCsvDataList.IsEmpty(); i++) {
			oCsvData = oCsvDataList.RemoveHead();
			pBuf1[(i - nFftSize2 + m_nFftSize) % m_nFftSize] = oCsvData.fData[1];
		}
	}

	FFTWINDOW *pFftWindow = GetFftWindow(FFT_MODE_CRF);
	GetWaveDataCrf(pFftWindow);
}
