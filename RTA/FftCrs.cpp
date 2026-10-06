#include "stdafx.h"
#include "Rta.h"
#include "FftWnd.h"
#include "FftDlg.h"

void CFftWnd::SetBitmapCrs(FFTWINDOW *pFftWindow)
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

	pFftWindow->m_nScaleWidth = pFftWindow->m_nFrameWidth;
	pFftWindow->m_nScaleHeight = pFftWindow->m_nFrameHeight;
	DrawScaleCrs(pFftWindow, pFftWindow->m_nFrameLeft, pFftWindow->m_nFrameTop, pFftWindow->m_nFrameRight, pFftWindow->m_nFrameBottom, TRUE);

	m_dcMem.SelectObject(pFontOrg);

	if (m_pCrsPoint == NULL)
		m_pCrsPoint = new POINT[pFftWindow->m_nScaleWidth * 2 + 4];
	m_nCrsPointCount = 0;

	if (m_pCrsPeakPoint == NULL)
		m_pCrsPeakPoint = new POINT[pFftWindow->m_nScaleWidth * 2 + 4];
	m_nCrsPeakCount = 0;
}

void CFftWnd::DrawScaleCrs(const FFTWINDOW *pFftWindow, int nLeft, int nTop, int nRight, int nBottom, BOOL bAxisX)
{
	int	nFreq, nLevel;
	int x, y;
	const int nWidth = nRight - nLeft;
	const int nHeight = nBottom - nTop;
	CString sText;
	CSize size;

	CPen *pPenOrg = m_dcMem.SelectObject(&m_penBlack);
	CBrush *pBrushOrg = m_dcMem.SelectObject(&m_brushGraph);

	m_dcMem.Rectangle(nLeft - 1, nTop - 1, nRight + 1, nBottom + 1);

	m_dcMem.SetBkMode(TRANSPARENT);
	m_dcMem.SetTextColor(COLOR_TEXT);

	m_dcMem.SelectObject(&m_Font);

	if (g_oSetData.Fft.nFftScale == 0) {
		const int step = GetLogScaleStep(m_nMinFreq);
		for (nFreq = GetScaleStartValue(m_nMinFreq, step); nFreq <= m_nMaxFreq; nFreq += GetLogScaleStep(nFreq)) {
			x =  (int)((log((double)nFreq) - m_fLogMinFreq) * nWidth / (m_fLogMaxFreq - m_fLogMinFreq) + 0.5) + nLeft;

			if (nFreq < 1000)
				sText.Format("%d", nFreq);
			else
				sText.Format("%gk", (double)nFreq / 1000);

			if (bAxisX && (sText[0] == '1' || sText[0] == '2' || sText[0] == '5')) {
				size = m_dcMem.GetOutputTextExtent(sText);
				m_dcMem.TextOut(x - size.cx / 2, nBottom + 2, sText);
			}

			if (x > nLeft && x < nRight) {
				m_dcMem.SelectObject(sText[0] == '1' ? m_penGray : m_penLightGray);
				m_dcMem.MoveTo(x, nTop);
				m_dcMem.LineTo(x, nBottom);
			}
		}
	} else {
		m_dcMem.SelectObject(m_penGray);

		const int step = GetLinearScaleStep(m_nMaxFreq, m_nMinFreq, (int)(nWidth * 0.75));
		for (nFreq = GetScaleStartValue(m_nMinFreq, step); nFreq <= m_nMaxFreq; nFreq += step) {
			x =  (int)((nFreq - m_nMinFreq) * nWidth / (double)(m_nMaxFreq - m_nMinFreq) + 0.5) + nLeft;

			if (x > nLeft && x < nRight) {
				m_dcMem.MoveTo(x, nTop);
				m_dcMem.LineTo(x, nBottom);
			}

			if (bAxisX) {
				if (nFreq < 1000)
					sText.Format("%d", nFreq);
				else
					sText.Format("%gk", (double)nFreq / 1000);
				size = m_dcMem.GetOutputTextExtent(sText);
				m_dcMem.TextOut(x - size.cx / 2, nBottom + 2, sText);
			}
		}
	}

	m_dcMem.SelectObject(m_penGray);

	const int step = GetLinearScaleStep(m_nMaxLevel, m_nMinLevel, nHeight);
	for (nLevel = GetScaleStartValue(m_nMinLevel, step); nLevel <= m_nMaxLevel; nLevel += step) {
		y = (int)(nTop - (nLevel - m_nMaxLevel) * nHeight / (double)(m_nMaxLevel - m_nMinLevel) + 0.5);

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
	sText.LoadString(IDS_LEVEL);
	sText += " [dB]";
	size = m_dcMem.GetOutputTextExtent(sText);
	m_dcMem.TextOut(0, nTop + (nHeight + size.cx) / 2, sText);

	m_dcMem.SelectObject(pBrushOrg);
	m_dcMem.SelectObject(pPenOrg);
}

void CFftWnd::GetWaveDataCrs(const FFTWINDOW *pFftWindow)
{
	CalcCrs(pFftWindow);

	DispCrsInfo(pFftWindow);
}

void CFftWnd::CalcCrs(const FFTWINDOW *pFftWindow)
{
	int i;
	double fLevel;
	double *pCrossBufRe = m_oCrossBufRe.GetBuf();
	double *pCrossBufIm = m_oCrossBufIm.GetBuf();
	const int nFftSize2 = m_nFftSize / 2;

	m_fCrossAllPower = 0;
	for (i = 1; i < nFftSize2; i++) {
		if (m_bValidFftData) {
			m_pCrsBuf[i] = sqrt(pCrossBufRe[i] * pCrossBufRe[i] + pCrossBufIm[i] * pCrossBufIm[i]);
		}
		m_fCrossAllPower += m_pCrsBuf[i];
	}

	m_nCrsPointCount = CalcCrsSub(pFftWindow, m_pCrsBuf, m_pCrsPoint);

	if (g_oSetData.Fft.bPeakHold) {
		double *pPeakLevel = m_pCrsPeakLevel;
		for (i = 0; i < nFftSize2; i++) {
			fLevel = m_pCrsBuf[i];
			if (fLevel > *pPeakLevel || m_bPeakReset)
				*pPeakLevel = fLevel;

			pPeakLevel++;
		}

		m_nCrsPeakCount = CalcCrsSub(pFftWindow, m_pCrsPeakLevel, m_pCrsPeakPoint);
	}
}

int CFftWnd::CalcCrsSub(const FFTWINDOW *pFftWindow, const double *pFftPBuf, POINT *pPoint)
{
	int x;
	int x2 = 0;
	const double fStepY = (double)pFftWindow->m_nScaleHeight / (m_nMinLevel - m_nMaxLevel);
	const double fTmp1 = (double)pFftWindow->m_nScaleWidth / (m_fLogMaxFreq - m_fLogMinFreq);
	const double fTmp2 = (double)pFftWindow->m_nScaleWidth / (m_nMaxFreq - m_nMinFreq);
	double fLevel;
	const int nFftSize2 = m_nFftSize / 2;
	int nPointCount = 0;
	double ymin = pFftPBuf[0];
	double ymax = pFftPBuf[0];

	for (int i = 0; i < nFftSize2; i++) {
		if (g_oSetData.Fft.nFftScale == 0)
			x = (int)((m_pLogFreqTbl[i] - m_fLogMinFreq) * fTmp1 + 0.5);
		else
			x = (int)((m_fFreqStep * i - m_nMinFreq) * fTmp2 + 0.5);

		if (x != x2) {
			if (x >= 0 && ymin > 0) {
				pPoint[nPointCount].x = x2;
				pPoint[nPointCount].y = (int)((dB10(ymin) - m_nMaxLevel) * fStepY + 0.5);
				nPointCount++;
				if (ymax != ymin) {
					pPoint[nPointCount].x = x2;
					pPoint[nPointCount].y = (int)((dB10(ymax) - m_nMaxLevel) * fStepY + 0.5);
					nPointCount++;
				}
				if (x2 >= pFftWindow->m_nScaleWidth)
					break;
			}

			x2 = x;
			ymin = DBL_MAX;
			ymax = 0;
		}

		fLevel = pFftPBuf[i];
		if (fLevel < ymin)
			ymin = fLevel;
		if (fLevel > ymax)
			ymax = fLevel;
	}

	return nPointCount;
}

void CFftWnd::DispCrsInfo(const FFTWINDOW *pFftWindow)
{
	CString sFreq, sLevel;
	double *pBuf = m_pCrsBuf;

	if (g_oSetData.Fft.bPeakDisp) {
		const int nFftSize2 = m_nFftSize / 2;
		double fPeakLevel = 0;
		for (int i = 1; i < nFftSize2; i++) {
			const double fFreq = m_fFreqStep * i;
			if (fFreq >= m_nMinFreq && fFreq <= m_nMaxFreq && pBuf[i] > fPeakLevel) {
				fPeakLevel = pBuf[i];
				m_nCrsDispFreq = i;
			}
		}
	}

	if (g_oSetData.Fft.nFftScale == 0)
		m_nCrsFreqPos = (int)((m_pLogFreqTbl[m_nCrsDispFreq] - m_fLogMinFreq) * ((double)pFftWindow->m_nFrameWidth / (m_fLogMaxFreq - m_fLogMinFreq)));
	else
		m_nCrsFreqPos = (int)((m_fFreqStep * m_nCrsDispFreq - m_nMinFreq) * ((double)pFftWindow->m_nFrameWidth / (m_nMaxFreq - m_nMinFreq)));
	m_nCrsLevelPos = (int)((dB10(pBuf[m_nCrsDispFreq]) - m_nMaxLevel) * ((double)pFftWindow->m_nScaleHeight / (m_nMinLevel - m_nMaxLevel)));

	if (m_nCrsDispFreq == 0) {
		if (m_fCrossAllPower != 0)
			sLevel.Format("%.2lf", dB10(m_fCrossAllPower));
		sFreq = "ALL";
	} else {
		if (pBuf[m_nCrsDispFreq] != 0)
			sLevel.Format("%.2lf", dB10(pBuf[m_nCrsDispFreq]));
		sFreq.Format("%.1lf",  m_fFreqStep * m_nCrsDispFreq);
	}

	if (pFftWindow->m_bMasterWindow)
		m_pFftDlg->SendMessage(WM_FREQ_LEVEL_LEFT, (WPARAM)(LPCSTR)sFreq, (LPARAM)(LPCSTR)sLevel);
	else
		pFftWindow->m_pWnd->SendMessage(WM_FREQ_LEVEL_LEFT, (WPARAM)(LPCSTR)sFreq, (LPARAM)(LPCSTR)sLevel);
}

void CFftWnd::PaintCrs(const FFTWINDOW *pFftWindow, int nChannel)
{
	if (nChannel == 0) {
		if (m_nCrsPointCount != 0) {
			m_dcMem2.SelectObject(&m_penCross);
			m_dcMem2.Polyline(m_pCrsPoint, m_nCrsPointCount);

			if (g_oSetData.Fft.bPeakHold) {
				m_dcMem2.SelectObject(&m_penCrossPeak);
				m_dcMem2.Polyline(m_pCrsPeakPoint, m_nCrsPeakCount);
			}

			if (m_nCrsDispFreq != 0) {
				m_dcMem2.SelectObject(&m_penGreen);
				m_dcMem2.MoveTo(m_nCrsFreqPos, 0);
				m_dcMem2.LineTo(m_nCrsFreqPos, pFftWindow->m_nScaleHeight);
				m_dcMem2.MoveTo(0, m_nCrsLevelPos);
				m_dcMem2.LineTo(pFftWindow->m_nScaleWidth, m_nCrsLevelPos);
			}
		}
	}
}

void CFftWnd::SetDispFreqCrs(const FFTWINDOW *pFftWindow, CPoint point)
{
	if (!g_oSetData.Fft.bPeakDisp) {
		int n;

		if (g_oSetData.Fft.nFftScale == 0)
			n = (int)(exp((point.x - pFftWindow->m_nFrameLeft) * (m_fLogMaxFreq - m_fLogMinFreq) / pFftWindow->m_nScaleWidth + m_fLogMinFreq) / m_fFreqStep);
		else
			n = (int)(((point.x - pFftWindow->m_nFrameLeft) * (m_nMaxFreq - m_nMinFreq) / pFftWindow->m_nScaleWidth + m_nMinFreq) / m_fFreqStep);

		m_nCrsDispFreq = (n > 1 && n < m_nFftSize / 2) ? n : 0;
		DispCrsInfo(pFftWindow);
		pFftWindow->m_pWnd->Invalidate(FALSE);
	}
}

void CFftWnd::ResetDispFreqCrs(const FFTWINDOW *pFftWindow)
{
	if (!g_oSetData.Fft.bPeakDisp) {
		m_nCrsDispFreq = 0;
		DispCrsInfo(pFftWindow);
		pFftWindow->m_pWnd->Invalidate(FALSE);
	}
}

void CFftWnd::CsvOutputCrs(CCsvFile &oCsvFile)
{
	oCsvFile.SetData("FFT");
	oCsvFile.SetData("Cross Spectrum");
	oCsvFile.Output();

	oCsvFile.SetData("Frequency[Hz]");
	oCsvFile.SetData("Level[dB]");
	if (g_oSetData.Fft.bPeakHold)
		oCsvFile.SetData("Peak[dB]");
	oCsvFile.Output();

	double *pBuf1 = NULL;
	double *pBuf2 = NULL;

	pBuf1 = m_pCrsBuf;

	if (g_oSetData.Fft.bPeakHold)
		pBuf2 = m_pCrsPeakLevel;

	const int nFftSize2 = m_nFftSize / 2;
	for (int i = 1; i < nFftSize2; i++) {
		oCsvFile.SetData(m_fFreqStep * i);

		if (pBuf1 != NULL)
			oCsvFile.SetData(dB10(pBuf1[i]));

		if (pBuf2 != NULL)
			oCsvFile.SetData(dB10(pBuf2[i]));

		oCsvFile.Output();
	}
}

void CFftWnd::CsvInputCrs(CCsvFile &oCsvFile)
{
	if (!oCsvFile.Input() || oCsvFile.GetString(0) != "Frequency[Hz]") {
		::AfxMessageBox(IDS_ERR_CSV_FORMAT, MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	CString sItem = oCsvFile.GetString(1);
	if (sItem != "Level[dB]") {
		::AfxMessageBox(IDS_ERR_CSV_FORMAT, MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	struct SCsvData {
		double fData[2];
	} oCsvData;
	CList<SCsvData, SCsvData&> oCsvDataList;

	while (oCsvFile.Input()) {
		oCsvData.fData[0] = oCsvFile.GetDouble(0);
		oCsvData.fData[1] = oCsvFile.GetDouble(1);
		oCsvDataList.AddTail(oCsvData);
	}

	const int nDataNum = (int)oCsvDataList.GetCount();
	const double fLastFreq = oCsvDataList.GetTail().fData[0];
	const int nFFtSizeIndex = (int)floor(log((double)nDataNum) / log(2.0) + 0.5) - 9;
	const int nSamplingRateIndex = GetSamplingRateIndex((int)fLastFreq * 2);

	m_pFftDlg->SetCsvConditions(FFT_MODE_CRS, -1, nFFtSizeIndex, nSamplingRateIndex, -1, -1);

	double *pBuf1 = m_pCrsBuf;
	const int nBufSize = m_nFftSize / 2;

	for (int i = 0; i < nBufSize && !oCsvDataList.IsEmpty(); i++) {
		oCsvData = oCsvDataList.RemoveHead();

		*pBuf1++ = pow(10, oCsvData.fData[1] / 10);
	}

	FFTWINDOW *pFftWindow = GetFftWindow(FFT_MODE_CRS);
	GetWaveDataCrs(pFftWindow);
}
