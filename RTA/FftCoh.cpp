#include "stdafx.h"
#include "Rta.h"
#include "FftWnd.h"
#include "FftDlg.h"

void CFftWnd::SetBitmapCoh(FFTWINDOW *pFftWindow)
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
	DrawScaleCoh(pFftWindow, pFftWindow->m_nFrameLeft, pFftWindow->m_nFrameTop, pFftWindow->m_nFrameRight, pFftWindow->m_nFrameBottom, TRUE);

	m_dcMem.SelectObject(pFontOrg);

	if (m_pCohPoint == NULL)
		m_pCohPoint = new POINT[pFftWindow->m_nScaleWidth * 2 + 4];
	m_nCohPointCount = 0;
}

void CFftWnd::DrawScaleCoh(const FFTWINDOW *pFftWindow, int nLeft, int nTop, int nRight, int nBottom, BOOL bAxisX)
{
	int	nFreq;
	int x, y;
	const int nWidth = nRight - nLeft;
	const int nHeight = nBottom - nTop;
	CString sText;
	CSize size;
	int i;

	CPen *pPenOrg = m_dcMem.SelectObject(&m_penBlack);
	CBrush *pBrushOrg = m_dcMem.SelectObject(&m_brushGraph);

	m_dcMem.Rectangle(nLeft - 1, nTop - 1, nRight + 1, nBottom + 1);

	m_dcMem.SetBkMode(TRANSPARENT);
	m_dcMem.SetTextColor(COLOR_TEXT);

	m_dcMem.SelectObject(&m_Font);

	if (g_oSetData.Fft.nFftScale == 0) {
		const int step = GetLogScaleStep(m_nMinFreq);
		for (nFreq = GetScaleStartValue(m_nMinFreq, step); nFreq <= m_nMaxFreq; nFreq += GetLogScaleStep(nFreq)) {
			x =  (int)((log((double)nFreq) - m_fLogMinFreq) * nWidth / (m_fLogMaxFreq - m_fLogMinFreq) + nLeft + 0.5);

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
			x =  (int)((nFreq - m_nMinFreq) * nWidth / (m_nMaxFreq - m_nMinFreq) + nLeft + 0.5);

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

	for (i = 0; i <= 100; i += 10) {
		y = (int)(nBottom - (i * ((double)pFftWindow->m_nScaleHeight) / 100) + 0.5);
		if (i != 100 && i != 0) {
			m_dcMem.MoveTo(nLeft, y);
			m_dcMem.LineTo(nRight, y);
		}

		sText.Format("%g", (double)i / 100);
		size = m_dcMem.GetOutputTextExtent(sText);
		m_dcMem.TextOut(nLeft - size.cx - 3, y - size.cy / 2, sText);
	}

	if (bAxisX) {
		sText.LoadString(IDS_FREQUENCY);
		sText += " [Hz]";
		size = m_dcMem.GetOutputTextExtent(sText);
		m_dcMem.TextOut(nLeft + (nWidth - size.cx) / 2, pFftWindow->m_nHeight - size.cy, sText);
	}

	m_dcMem.SelectObject(&m_Font2);
	sText.LoadString(IDS_COHERENCE2);
	size = m_dcMem.GetOutputTextExtent(sText);
	m_dcMem.TextOut(0, nTop + (nHeight + size.cx) / 2, sText);

	m_dcMem.SelectObject(pBrushOrg);
	m_dcMem.SelectObject(pPenOrg);
}

void CFftWnd::GetWaveDataCoh(const FFTWINDOW *pFftWindow)
{
	if (m_bValidFftData) {
		double w, cr, ci;
		double fCoherence;
		const int nFftSize2 = m_nFftSize / 2;

		double *pPowerSpecBufL = m_aFftData[0].m_oPowerSpecBuf.GetBuf();
		double *pPowerSpecBufR = m_aFftData[1].m_oPowerSpecBuf.GetBuf();
		double *pCrossBufRe = m_oCrossBufRe.GetBuf();
		double *pCrossBufIm = m_oCrossBufIm.GetBuf();
		m_pCoherenceBuf[0] = 0;
		for (int i = 1; i < nFftSize2; i++) {
			cr = pCrossBufRe[i];
			ci = pCrossBufIm[i];

			w = pPowerSpecBufL[i] * pPowerSpecBufR[i];
			fCoherence = (w == 0) ? 0 : (cr * cr + ci * ci) / w;

			m_pCoherenceBuf[i] = fCoherence * m_fSmoothing1 + m_pCoherenceBuf[i] * m_fSmoothing2;
		}
	}

	m_nCohPointCount = CalcCoh(pFftWindow, m_pCoherenceBuf, m_pCohPoint);

	DispCohInfo(pFftWindow);
}

int CFftWnd::CalcCoh(const FFTWINDOW *pFftWindow, const double *pBuf, POINT *pPoint)
{
	int x;
	int x2 = 0;
	const double fTmp1 = (double)pFftWindow->m_nScaleWidth / (m_fLogMaxFreq - m_fLogMinFreq);
	const double fTmp2 = (double)pFftWindow->m_nScaleWidth / (m_nMaxFreq - m_nMinFreq);
	double fCoherence;
	const int nFftSize2 = m_nFftSize / 2;
	int nPointCount = 0;
	double ymin = pBuf[0];
	double ymax = pBuf[0];

	for (int i = 0; i < nFftSize2; i++) {
		if (g_oSetData.Fft.nFftScale == 0)
			x = (int)((m_pLogFreqTbl[i] - m_fLogMinFreq) * fTmp1);
		else
			x = (int)((m_fFreqStep * i - m_nMinFreq) * fTmp2);

		if (x != x2) {
			if (x >= 0) {
				pPoint[nPointCount].x = x2;
				pPoint[nPointCount].y = pFftWindow->m_nScaleHeight - (int)(ymin * pFftWindow->m_nScaleHeight);
				nPointCount++;
				if (ymax != ymin) {
					pPoint[nPointCount].x = x2;
					pPoint[nPointCount].y = pFftWindow->m_nScaleHeight - (int)(ymax * pFftWindow->m_nScaleHeight);
					nPointCount++;
				}
				if (x2 >= pFftWindow->m_nScaleWidth)
					break;
			}

			x2 = x;
			ymin = 1;
			ymax = 0;
		}

		fCoherence = pBuf[i];

		if (fCoherence < ymin)
			ymin = fCoherence;
		if (fCoherence > ymax)
			ymax = fCoherence;
	}

	return nPointCount;
}

void CFftWnd::DispCohInfo(const FFTWINDOW *pFftWindow)
{
	CString sFreq, sLevel;
	double *pBuf = m_pCoherenceBuf;

	if (g_oSetData.Fft.nFftScale == 0)
		m_nCohFreqPos = (int)((m_pLogFreqTbl[m_nCohDispFreq] - m_fLogMinFreq) * ((double)pFftWindow->m_nFrameWidth / (m_fLogMaxFreq - m_fLogMinFreq)));
	else
		m_nCohFreqPos = (int)((m_fFreqStep * m_nCohDispFreq - m_nMinFreq) * ((double)pFftWindow->m_nFrameWidth / (m_nMaxFreq - m_nMinFreq)));
	m_nCohLevelPos = pFftWindow->m_nScaleHeight - (int)(pBuf[m_nCohDispFreq] * pFftWindow->m_nScaleHeight);

	if (m_nCohDispFreq == 0) {
		sLevel.Empty();
		sFreq.Empty();
	} else {
		sLevel.Format("%.2lf", pBuf[m_nCohDispFreq]);
		sFreq.Format("%.1lf",  m_fFreqStep * m_nCohDispFreq);
	}

	if (pFftWindow->m_bMasterWindow)
		m_pFftDlg->SendMessage(WM_FREQ_LEVEL_LEFT, (WPARAM)(LPCSTR)sFreq, (LPARAM)(LPCSTR)sLevel);
	else
		pFftWindow->m_pWnd->SendMessage(WM_FREQ_LEVEL_LEFT, (WPARAM)(LPCSTR)sFreq, (LPARAM)(LPCSTR)sLevel);
}

void CFftWnd::PaintCoh(const FFTWINDOW *pFftWindow, int nChannel)
{
	if (nChannel == 0) {
		if (m_nCohPointCount != 0) {
			m_dcMem2.SelectObject(&m_penCoherence);
			m_dcMem2.Polyline(m_pCohPoint, m_nCohPointCount);
		}

		if (m_nCohDispFreq != 0) {
			m_dcMem2.SelectObject(&m_penGreen);
			m_dcMem2.MoveTo(m_nCohFreqPos, 0);
			m_dcMem2.LineTo(m_nCohFreqPos, pFftWindow->m_nScaleHeight);
			m_dcMem2.MoveTo(0, m_nCohLevelPos);
			m_dcMem2.LineTo(pFftWindow->m_nScaleWidth, m_nCohLevelPos);
		}
	}
}

void CFftWnd::SetDispFreqCoh(const FFTWINDOW *pFftWindow, CPoint point)
{
	int n;

	if (g_oSetData.Fft.nFftScale == 0)
		n = (int)(exp((point.x - pFftWindow->m_nFrameLeft) * (m_fLogMaxFreq - m_fLogMinFreq) / pFftWindow->m_nScaleWidth + m_fLogMinFreq) / m_fFreqStep);
	else
		n = (int)(((point.x - pFftWindow->m_nFrameLeft) * (m_nMaxFreq - m_nMinFreq) / pFftWindow->m_nScaleWidth + m_nMinFreq) / m_fFreqStep);

	m_nCohDispFreq = (n > 1 && n < m_nFftSize / 2) ? n : 0;
	DispCohInfo(pFftWindow);
	pFftWindow->m_pWnd->Invalidate(FALSE);
}

void CFftWnd::ResetDispFreqCoh(const FFTWINDOW *pFftWindow)
{
	m_nCohDispFreq = 0;
	DispCohInfo(pFftWindow);
	pFftWindow->m_pWnd->Invalidate(FALSE);
}

void CFftWnd::CsvOutputCoh(CCsvFile &oCsvFile)
{
	oCsvFile.SetData("FFT");
	oCsvFile.SetData("Coherence");
	oCsvFile.Output();

	oCsvFile.SetData("Frequency[Hz]");
	oCsvFile.SetData("Coherence");
	oCsvFile.Output();

	const int nFftSize2 = m_nFftSize / 2;
	for (int i = 1; i < nFftSize2; i++) {
		oCsvFile.SetData(m_fFreqStep * i);

		oCsvFile.SetData(m_pCoherenceBuf[i]);

		oCsvFile.Output();
	}
}

void CFftWnd::CsvInputCoh(CCsvFile &oCsvFile)
{
	if (!oCsvFile.Input() || oCsvFile.GetString(0) != "Frequency[Hz]") {
		::AfxMessageBox(IDS_ERR_CSV_FORMAT, MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	CString sItem = oCsvFile.GetString(1);
	if (sItem != "Coherence") {
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

	m_pFftDlg->SetCsvConditions(FFT_MODE_COH, -1, nFFtSizeIndex, nSamplingRateIndex, -1, -1);

	double *pBuf1 = m_pCoherenceBuf;
	const int nBufSize = m_nFftSize / 2;

	for (int i = 0; i < nBufSize && !oCsvDataList.IsEmpty(); i++) {
		oCsvData = oCsvDataList.RemoveHead();

		*pBuf1++ = oCsvData.fData[1];
	}

	FFTWINDOW *pFftWindow = GetFftWindow(FFT_MODE_COH);
	GetWaveDataCoh(pFftWindow);
}
