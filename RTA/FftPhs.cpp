#include "stdafx.h"
#include "Rta.h"
#include "FftWnd.h"
#include "FftDlg.h"

void CFftWnd::SetBitmapPhs(FFTWINDOW *pFftWindow)
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
	DrawScalePhs(pFftWindow, pFftWindow->m_nFrameLeft, pFftWindow->m_nFrameTop, pFftWindow->m_nFrameRight, pFftWindow->m_nFrameBottom, TRUE);

	m_dcMem.SelectObject(pFontOrg);

	if (m_pPhsPoint == NULL)
		m_pPhsPoint = new POINT[pFftWindow->m_nScaleWidth * 2 + 4];
	m_nPhsPointCount = 0;
}

void CFftWnd::DrawScalePhs(const FFTWINDOW *pFftWindow, int nLeft, int nTop, int nRight, int nBottom, BOOL bAxisX)
{
	int	fFreq, fPhase;
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
		const double fSpanFreq = m_fLogMaxFreq - m_fLogMinFreq;
		for (fFreq = GetScaleStartValue(m_nMinFreq, step); fFreq <= m_nMaxFreq; fFreq += GetLogScaleStep(fFreq)) {
			x = nLeft + (int)((log((double)fFreq) - m_fLogMinFreq) * nWidth / fSpanFreq + 0.5);

			if (fFreq < 1000)
				sText.Format("%d", fFreq);
			else
				sText.Format("%gk", (double)fFreq / 1000);

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

		const int step = GetLinearScaleStep(m_nMaxFreq, m_nMinFreq, nWidth);
		const double fSpanFreq = m_nMaxFreq - m_nMinFreq;
		for (fFreq = GetScaleStartValue(m_nMinFreq, step); fFreq <= m_nMaxFreq; fFreq += step) {
			x = nLeft + (int)((fFreq - m_nMinFreq) * nWidth / fSpanFreq + 0.5);

			if (x > nLeft && x < nRight) {
				m_dcMem.MoveTo(x, nTop);
				m_dcMem.LineTo(x, nBottom);
			}

			if (bAxisX) {
				if (fFreq < 1000)
					sText.Format("%d", fFreq);
				else
					sText.Format("%gk", (double)fFreq / 1000);
				size = m_dcMem.GetOutputTextExtent(sText);
				m_dcMem.TextOut(x - size.cx / 2, nBottom + 2, sText);
			}
		}
	}

	m_dcMem.SelectObject(m_penGray);

	const int nCenter = (nTop + nBottom) / 2;
	for (fPhase = 180; fPhase >= -180; fPhase -= 45) {
		y = nCenter - fPhase * nHeight / 2 / 180;
		if (fPhase != 180 && fPhase != -180) {
			if (fPhase == 0)
				m_dcMem.SelectObject(&m_penBlack);
			else
				m_dcMem.SelectObject(&m_penGray);

			m_dcMem.MoveTo(nLeft, y);
			m_dcMem.LineTo(nRight, y);
		}

		sText.Format("%+d", fPhase);
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
	sText.LoadString(IDS_PHASE2);
	sText += " [degree]";
	size = m_dcMem.GetOutputTextExtent(sText);
	m_dcMem.TextOut(0, nTop + (nHeight + size.cx) / 2, sText);

	m_dcMem.SelectObject(pBrushOrg);
	m_dcMem.SelectObject(pPenOrg);
}

void CFftWnd::GetWaveDataPhs(const FFTWINDOW *pFftWindow)
{
	if (m_bValidFftData) {
		int i, j;
		double xt1, yt1, xt2, yt2;
		const int nFftSize2 = m_nFftSize / 2;

		m_pPhaseBuf[0] = 0;
		for (i = 1; i < nFftSize2; i++) {
			j = i * 2;

			xt1 = m_pFftBufL[j];
			yt1 = m_pFftBufL[j + 1];

			xt2 = m_pFftBufR[j];
			yt2 = m_pFftBufR[j + 1];

			double fPhase = atan2(xt2 * yt1 - xt1 * yt2, xt1 * xt2 + yt1 * yt2);

			if (fPhase - m_pPhaseBuf[i] < -M_PI * 1.1)
				fPhase += 2 * M_PI;
			else if (fPhase - m_pPhaseBuf[i] > M_PI * 1.1)
				fPhase -= 2 * M_PI;

			fPhase = fPhase * m_fSmoothing1 + m_pPhaseBuf[i] * m_fSmoothing2;

			if (fPhase < -M_PI)
				fPhase += 2 * M_PI;
			else if (fPhase > M_PI)
				fPhase -= 2 * M_PI;

			m_pPhaseBuf[i] = fPhase;
		}
	}

	CalcPhs(pFftWindow);

	DispPhsInfo(pFftWindow);
}

void CFftWnd::CalcPhs(const FFTWINDOW *pFftWindow)
{
	m_nPhsPointCount = CalcPhsSub(pFftWindow, m_pPhaseBuf, m_pPhsPoint);
}

int CFftWnd::CalcPhsSub(const FFTWINDOW *pFftWindow, double *pPhaseBuf, POINT *pPoint)
{
	int x;
	int x2 = 0;
	const double fTmp1 = (double)pFftWindow->m_nScaleWidth / (m_fLogMaxFreq - m_fLogMinFreq);
	const double fTmp2 = (double)pFftWindow->m_nScaleWidth / (m_nMaxFreq - m_nMinFreq);
	double fPhase;
	const int nFftSize2 = m_nFftSize / 2;
	int nPointCount = 0;
	double ymin = pPhaseBuf[0];
	double ymax = pPhaseBuf[0];
	const int nCenter = pFftWindow->m_nScaleHeight / 2;

	for (int i = 0; i < nFftSize2; i++) {
		if (g_oSetData.Fft.nFftScale == 0)
			x = (int)((m_pLogFreqTbl[i] - m_fLogMinFreq) * fTmp1);
		else
			x = (int)((m_fFreqStep * i - m_nMinFreq) * fTmp2);

		if (x != x2) {
			if (x >= 0) {
				pPoint[nPointCount].x = x2;
				pPoint[nPointCount].y = nCenter - (int)(ymin / M_PI * pFftWindow->m_nScaleHeight / 2);
				nPointCount++;
				if (ymax != ymin) {
					pPoint[nPointCount].x = x2;
					pPoint[nPointCount].y = nCenter - (int)(ymax / M_PI * pFftWindow->m_nScaleHeight / 2);
					nPointCount++;
				}
				if (x2 >= pFftWindow->m_nScaleWidth)
					break;
			}

			x2 = x;
			ymin = M_PI;
			ymax = -M_PI;
		}

		fPhase = pPhaseBuf[i];

		if (fPhase < ymin)
			ymin = fPhase;
		if (fPhase > ymax)
			ymax = fPhase;
	}

	return nPointCount;
}

void CFftWnd::DispPhsInfo(const FFTWINDOW *pFftWindow)
{
	CString sFreq, sLevel;
	double *pBuf = m_pPhaseBuf;

	if (g_oSetData.Fft.nFftScale == 0)
		m_nPhsFreqPos = (int)((m_pLogFreqTbl[m_nPhsDispFreq] - m_fLogMinFreq) * ((double)pFftWindow->m_nFrameWidth / (m_fLogMaxFreq - m_fLogMinFreq)));
	else
		m_nPhsFreqPos = (int)((m_fFreqStep * m_nPhsDispFreq - m_nMinFreq) * ((double)pFftWindow->m_nFrameWidth / (m_nMaxFreq - m_nMinFreq)));
	m_nPhsLevelPos = (pFftWindow->m_nScaleHeight / 2) - (int)(pBuf[m_nPhsDispFreq] / M_PI * pFftWindow->m_nScaleHeight / 2);

	if (m_nPhsDispFreq == 0) {
		sLevel.Empty();
		sFreq.Empty();
	} else {
		sLevel.Format("%.2lf", pBuf[m_nPhsDispFreq] / M_PI * 180);
		sFreq.Format("%.1lf",  m_fFreqStep * m_nPhsDispFreq);
	}

	if (pFftWindow->m_bMasterWindow)
		m_pFftDlg->SendMessage(WM_FREQ_LEVEL_LEFT, (WPARAM)(LPCSTR)sFreq, (LPARAM)(LPCSTR)sLevel);
	else
		pFftWindow->m_pWnd->SendMessage(WM_FREQ_LEVEL_LEFT, (WPARAM)(LPCSTR)sFreq, (LPARAM)(LPCSTR)sLevel);
}

void CFftWnd::PaintPhs(const FFTWINDOW *pFftWindow, int nChannel)
{
	if (nChannel == 0) {
		if (m_nPhsPointCount != 0) {
			m_dcMem2.SelectObject(&m_penPhase);
			m_dcMem2.Polyline(m_pPhsPoint, m_nPhsPointCount);
		}

		if (m_nPhsDispFreq != 0) {
			m_dcMem2.SelectObject(&m_penGreen);
			m_dcMem2.MoveTo(m_nPhsFreqPos, 0);
			m_dcMem2.LineTo(m_nPhsFreqPos, pFftWindow->m_nScaleHeight);
			m_dcMem2.MoveTo(0, m_nPhsLevelPos);
			m_dcMem2.LineTo(pFftWindow->m_nScaleWidth, m_nPhsLevelPos);
		}
	}
}

void CFftWnd::SetDispFreqPhs(const FFTWINDOW *pFftWindow, CPoint point)
{
	int n;

	if (g_oSetData.Fft.nFftScale == 0)
		n = (int)(exp((point.x - pFftWindow->m_nFrameLeft) * (m_fLogMaxFreq - m_fLogMinFreq) / pFftWindow->m_nScaleWidth + m_fLogMinFreq) / m_fFreqStep);
	else
		n = (int)(((point.x - pFftWindow->m_nFrameLeft) * (m_nMaxFreq - m_nMinFreq) / pFftWindow->m_nScaleWidth + m_nMinFreq) / m_fFreqStep);

	if (n > 1 && n < m_nFftSize / 2)
		m_nPhsDispFreq = n;
	else
		m_nPhsDispFreq = 0;

	DispPhsInfo(pFftWindow);
	pFftWindow->m_pWnd->Invalidate(FALSE);
}

void CFftWnd::ResetDispFreqPhs(const FFTWINDOW *pFftWindow)
{
	m_nPhsDispFreq = 0;
	DispPhsInfo(pFftWindow);
	pFftWindow->m_pWnd->Invalidate(FALSE);
}

void CFftWnd::CsvOutputPhs(CCsvFile &oCsvFile)
{
	oCsvFile.SetData("FFT");
	oCsvFile.SetData("Phase");
	oCsvFile.Output();

	oCsvFile.SetData("Frequency[Hz]");
	oCsvFile.SetData("Phase[deg]");
	oCsvFile.Output();

	const int nFftSize2 = m_nFftSize / 2;
	for (int i = 1; i < nFftSize2; i++) {
		oCsvFile.SetData(m_fFreqStep * i);

		oCsvFile.SetData(m_pPhaseBuf[i] / M_PI * 180);

		oCsvFile.Output();
	}
}

void CFftWnd::CsvInputPhs(CCsvFile &oCsvFile)
{
	if (!oCsvFile.Input() || oCsvFile.GetString(0) != "Frequency[Hz]") {
		::AfxMessageBox(IDS_ERR_CSV_FORMAT, MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	CString sItem = oCsvFile.GetString(1);
	if (sItem != "Phase[deg]") {
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

	m_pFftDlg->SetCsvConditions(FFT_MODE_PHS, -1, nFFtSizeIndex, nSamplingRateIndex, -1, -1);

	double *pBuf1 = m_pPhaseBuf;
	const int nBufSize = m_nFftSize / 2;

	for (int i = 0; i < nBufSize && !oCsvDataList.IsEmpty(); i++) {
		oCsvData = oCsvDataList.RemoveHead();

		*pBuf1++ = oCsvData.fData[1] * M_PI / 180;
	}

	FFTWINDOW *pFftWindow = GetFftWindow(FFT_MODE_PHS);
	GetWaveDataPhs(pFftWindow);
}
