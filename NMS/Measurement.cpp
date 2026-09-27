// Measurement.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Nms.h"
#include "NmsDoc.h"
#include "Measurement.h"
#include "MakeFilter.h"
#include "SplWnd.h"
#include "PeakLevelWnd.h"
#include "ManualMode.h"
#include "NmsItem.h"
#include "SetData.h"
#include "WaveFile.h"

#define WAVE_BLOCK_INTERVAL	10
#define BASE_LEVEL_TIMER	2

/////////////////////////////////////////////////////////////////////////////
// CMeasurement

CMeasurement::CMeasurement()
{
	m_pRingBuf = NULL;
	m_pFilter = NULL;
	m_pFftBuf1 = NULL;
	m_pFftBuf2 = NULL;
	m_nSaveCounter = 0;
	m_nCalcCounter = 0;
	m_oMicCalData.fInputSens = 0;
	m_oMicCalData.nFreqData = 0;
	m_nMeasurementMode = MEASUREMENT_STOP;
	m_bExitThread = FALSE;

	AfxBeginThread(CalcThreadEntry, this, THREAD_PRIORITY_LOWEST);
}

CMeasurement::~CMeasurement()
{
	m_bExitThread = TRUE;
	m_cCalcEvent.SetEvent();
	m_cExitEvent.Lock();

	FreeBuffers();
}

BEGIN_MESSAGE_MAP(CMeasurement, CWnd)
	ON_MESSAGE(MM_WIM_DATA, OnWaveInData)
	ON_MESSAGE(WM_CHANGE_STATUS, OnChangeStatus)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMeasurement メッセージ ハンドラ

BOOL CMeasurement::Start(int nMeasurementMode)
{
	int i;

	if (g_oSetData.Measurement.bEndDateTime) {
		if (CTime::GetCurrentTime() > g_oSetData.Measurement.tEndDateTime) {
			g_oSetData.Measurement.bStartDateTime = FALSE;
			g_oSetData.Measurement.bEndDateTime = FALSE;
		}
	}

	m_nMeasurementMode = nMeasurementMode;

	FreeBuffers();

	m_nSamplingRate = g_oSetData.Measurement.nSamplingRate;

	for (m_nSamplePerBlock = 1; m_nSamplePerBlock < m_nSamplingRate / WAVE_BLOCK_INTERVAL; m_nSamplePerBlock *= 2)
		;
	m_fSamplingTime = (m_nMeasurementMode == MEASUREMENT_AUTO) ? (g_oSetData.Measurement.fSamplingTime1 + g_oSetData.Measurement.fSamplingTime2) : g_oSetData.ManualWindow.fSamplingTime;
	m_fTimeConstant = g_oSetData.Measurement.fTimeConstant;
	m_fSamplingLevel = g_oSetData.Measurement.fSamplingLevel;
	m_nBaseLevelCount = g_oSetData.Measurement.bRelativeLevel ? m_nSamplingRate * BASE_LEVEL_TIMER : 0;
	m_fBaseLevel = 0;

	m_nRingBufSize = (int)(m_fSamplingTime * m_nSamplingRate);
	m_pRingBuf = new WaveData[m_nRingBufSize];
	m_nRingBufPtr = 0;
	m_bRingBufFill = FALSE;
	m_fTimeConst1 = 1 / ((double)m_nSamplingRate * m_fTimeConstant);
	m_fTimeConst2 = 1 - m_fTimeConst1;
	m_fPower = 0;
	m_nSampleCount = 0;
	m_fMaxLevel = 0;
	m_bResetLevel = TRUE;
	m_bMeasuring = FALSE;
	m_nSamplingFwdCount = (int)(g_oSetData.Measurement.fSamplingTime1 * m_nSamplingRate);
	m_nSamplingBwdCount = (int)(g_oSetData.Measurement.fSamplingTime2 * m_nSamplingRate);

	m_pFftBuf1 = new double[m_nSamplePerBlock];
	m_pFftBuf2 = new double[m_nSamplePerBlock];

	double *pCalibration = new double[m_nSamplePerBlock];
	MakeFilterTbl3(pCalibration, m_nSamplePerBlock, m_nSamplingRate, (FilterData *)m_oMicCalData.aFreq, m_oMicCalData.nFreqData, 20);
	pCalibration[0] = 1;
	double sens = pow(10.0, -m_oMicCalData.fInputSens / 20);

	m_pFilter = new double[m_nSamplePerBlock];
	::MakeFilterTbl2(m_pFilter, m_nSamplePerBlock, m_nSamplingRate, g_oSetData.Measurement.AcfCondition.nFreqWeighting, 20);
	for (i = 0; i < m_nSamplePerBlock; i++)
		m_pFilter[i] *= sens / pCalibration[i] / m_nSamplePerBlock;

	delete [] pCalibration;

	if (!Open(g_oSetData.InputDevice.nInputDevice, NULL, 2, m_nSamplingRate, m_nSamplePerBlock, 6))
		return FALSE;

	CWaveIn::Start();

	return TRUE;
}

void CMeasurement::Stop()
{
	CWaveIn::Close();
	FreeBuffers();

	if (g_pSplWnd != NULL)
		g_pSplWnd->Clear();

	if (g_pPeakLevelWnd != NULL)
		g_pPeakLevelWnd->m_cPeakLevel.Clear();

	m_nMeasurementMode = MEASUREMENT_STOP;
}

LRESULT CMeasurement::OnWaveInData(WPARAM /*wParam*/, LPARAM lParam)
{
	LPWAVEHDR pWaveHdr = (LPWAVEHDR)lParam;

	int nBytesPerSample = m_oWaveFormat.Format.wBitsPerSample / 8 * m_oWaveFormat.Format.nChannels;
	WAVENOTIFY waveNotify{
		.pSamplesData = m_pSamplesBuffer,
		.nSamplesNum = (int)pWaveHdr->dwBufferLength / nBytesPerSample,
		.nSamplesRecorded = (int)pWaveHdr->dwBytesRecorded / nBytesPerSample,
		.nFlags = (int)pWaveHdr->dwFlags,
		.nChannels = m_oWaveFormat.Format.nChannels,
		.nSamplesPerSec = (int)m_oWaveFormat.Format.nSamplesPerSec
	};

	ConvertWaveToDouble(pWaveHdr);

	switch (m_nMeasurementMode) {
	case MEASUREMENT_AUTO:
		AutoMeasuring(&waveNotify);
		break;
	case MEASUREMENT_MANUAL:
		ManualMeasuring(&waveNotify);
		break;
	}

	if (g_pSplWnd != NULL)
		g_pSplWnd->ShowSpl(m_fSPL);

	if (g_pPeakLevelWnd != NULL)
 		g_pPeakLevelWnd->m_cPeakLevel.WaveData(&waveNotify);

	waveInAddBuffer(m_hWave, pWaveHdr, sizeof(WAVEHDR));

	return 0;
}

void CMeasurement::AutoMeasuring(LPWAVENOTIFY pWaveNotify)
{
	double *pData;
	int i;
	int nCount;
	int nCopy;
	BOOL bMaxLevel;

	pData = pWaveNotify->pSamplesData;
	nCount = pWaveNotify->nSamplesRecorded;
	while (nCount) {
		nCopy = min(nCount, m_nRingBufSize - m_nRingBufPtr);
		memcpy(&m_pRingBuf[m_nRingBufPtr], pData, nCopy * sizeof(WaveData));
		pData += nCopy * 2;
		nCount -= nCopy;
		m_nRingBufPtr += nCopy;
		if (m_nRingBufPtr == m_nRingBufSize) {
			m_nRingBufPtr = 0;
		}

		if (!m_bRingBufFill) {
			if (m_nRingBufPtr > m_nSamplingFwdCount)
				m_bRingBufFill = TRUE;
		}
	}

	pData = m_pSamplesBuffer;
	for (i = 0; i < m_nSamplePerBlock; i++) {
		m_pFftBuf1[i] = *pData++;
		m_pFftBuf2[i] = *pData++;
	}
	m_oRFFT.fft(m_nSamplePerBlock, m_pFftBuf1);
//	m_oRFFT.fft(m_nSamplePerBlock, m_pFftBuf2);
	for (i = 0; i < m_nSamplePerBlock; i++) {
		m_pFftBuf1[i] *= m_pFilter[i];
//		m_pFftBuf2[i] *= m_pFilter[i];
	}
	m_oRFFT.ifft(m_nSamplePerBlock, m_pFftBuf1);
//	m_oRFFT.ifft(m_nSamplePerBlock, m_pFftBuf2);

	for (i = 0; i < m_nSamplePerBlock; i++) {
		m_fPower = CalcPower(m_pFftBuf1[i]);
//		m_fPower = CalcPower(m_pFftBuf2[i]);
		if (m_fPower == 0)
			continue;

		m_fSPL = dB10(m_fPower);

		if (m_nBaseLevelCount != 0) {
			m_fBaseLevel += m_fSPL;
			if (--m_nBaseLevelCount == 0)
				m_fSamplingLevel += m_fBaseLevel / (m_nSamplingRate * BASE_LEVEL_TIMER);
			continue;
		}

		if (!m_bRingBufFill)
			continue;

		if (g_oSetData.Measurement.nSamplingTiming == 0) {
			if (m_bResetLevel) {
				m_fMaxLevel = m_fSPL;
				m_fMinLevel = m_fSPL;
				bMaxLevel = FALSE;
				m_bResetLevel = FALSE;
			} else {
				if (m_fSPL > m_fMaxLevel) {
					m_fMaxLevel = m_fSPL;
					bMaxLevel = TRUE;
				} else
					bMaxLevel = FALSE;

				if (m_fSPL < m_fMinLevel)
					m_fMinLevel = m_fSPL;
			}

			if (!m_bMeasuring) {
				if (bMaxLevel && m_fSPL >= m_fSamplingLevel && m_fSPL >= m_fMinLevel + 5) {
					m_bMeasuring = TRUE;
					m_nSampleCount = 0;
				}
			} else {
				if (bMaxLevel)
					m_nSampleCount = 0;
				else
					m_nSampleCount++;
			}
		} else {
			if (!m_bMeasuring) {
				if (m_fSPL >= m_fSamplingLevel) {
					m_bMeasuring = TRUE;
					m_nSampleCount = 0;
				}
			} else
				m_nSampleCount++;
		}

		if (m_nSampleCount >= m_nSamplingBwdCount) {
			if (SaveSampleData())
				AddItem(m_fSamplingTime, NULL);

			m_nSampleCount = 0;
			m_bResetLevel = TRUE;
			m_bMeasuring = FALSE;
		}
	}
}

// マニュアル測定処理
void CMeasurement::ManualMeasuring(LPWAVENOTIFY pWaveNotify)
{
	double *pData;
	int nCopy;
	int i;

	pData = m_pSamplesBuffer;
	for (i = 0; i < m_nSamplePerBlock; i++) {
		m_pFftBuf1[i] = *pData++;
		m_pFftBuf2[i] = *pData++;
	}
	m_oRFFT.fft(m_nSamplePerBlock, m_pFftBuf1);
//	m_oRFFT.fft(m_nSamplePerBlock, m_pFftBuf2);
	for (i = 0; i < m_nSamplePerBlock; i++) {
		m_pFftBuf1[i] *= m_pFilter[i];
//		m_pFftBuf2[i] *= m_pFilter[i];
	}
	m_oRFFT.ifft(m_nSamplePerBlock, m_pFftBuf1);
//	m_oRFFT.ifft(m_nSamplePerBlock, m_pFftBuf2);

	for (i = 0; i < m_nSamplePerBlock; i++) {
		m_fPower = CalcPower(m_pFftBuf1[i]);
		m_fSPL = dB10(m_fPower);
		if (m_fSPL > m_fMaxLevel)
			m_fMaxLevel = m_fSPL;
	}

	if (m_bMeasuring) {
		pData = pWaveNotify->pSamplesData;
		nCopy = min(pWaveNotify->nSamplesRecorded, m_nRingBufSize - m_nRingBufPtr);
		memcpy(&m_pRingBuf[m_nRingBufPtr], pData, nCopy * sizeof(WaveData));
		m_nRingBufPtr += nCopy;

		if (g_pManualWnd != NULL)
			g_pManualWnd->NotifyElpseTime((double)m_nRingBufPtr / m_nSamplingRate);

		if (m_nRingBufSize == m_nRingBufPtr) {
			if (SaveSampleData())
				AddItem(m_fSamplingTime, NULL);

			m_bMeasuring = FALSE;
			if (g_pManualWnd != NULL)
				g_pManualWnd->NotifyMeasuring(FALSE);
		}
	}
}

void CMeasurement::StartManualSampling()
{
	if (m_nMeasurementMode == MEASUREMENT_MANUAL) {
		m_fMaxLevel = 0;
		m_nRingBufPtr = 0;
		m_bMeasuring = TRUE;
		if (g_pManualWnd != NULL)
			g_pManualWnd->NotifyMeasuring(TRUE);
	}
}

void CMeasurement::StopManualSampling()
{
	if (m_nMeasurementMode == MEASUREMENT_MANUAL) {
		if (m_bMeasuring) {
			if (SaveSampleData())
				AddItem(m_fSamplingTime, NULL);

			m_bMeasuring = FALSE;
			if (g_pManualWnd != NULL)
				g_pManualWnd->NotifyMeasuring(FALSE);
		}
	}
}

double CMeasurement::CalcPower(double fData) const
{
	double fPower;

	fPower = fData * fData * 2;

	return fPower * m_fTimeConst1 + m_fPower * m_fTimeConst2;
}

BOOL CMeasurement::SaveSampleData()
{
	HWAVEDATA hWaveData;
	PWAVEDATA pWaveData;
	CString tempFileName;
	int nDataSize;
	int nCopySize;

	if (g_oSetData.Measurement.bStartDateTime) {
		if (CTime::GetCurrentTime() < g_oSetData.Measurement.tStartDateTime)
			return FALSE;
	}

	if (g_oSetData.Measurement.bEndDateTime) {
		if (CTime::GetCurrentTime() > g_oSetData.Measurement.tEndDateTime)
			return FALSE;
	}

	int nBytesPerSample = m_oWaveFormat.Format.wBitsPerSample / 8 * m_oWaveFormat.Format.nChannels;
	nDataSize = m_bRingBufFill ? m_nRingBufSize : m_nRingBufPtr;
	if ((hWaveData = ::GlobalAlloc(GHND, sizeof(WAVEDATA) + nDataSize * nBytesPerSample)) == NULL)
		return FALSE;

	pWaveData = (PWAVEDATA)::GlobalLock(hWaveData);

	pWaveData->nChannels = m_oWaveFormat.Format.nChannels;
	pWaveData->nBitsPerSample = m_oWaveFormat.Format.wBitsPerSample;
	pWaveData->nSamplesPerSec = m_nSamplingRate;
	pWaveData->dataSize = nDataSize * nBytesPerSample;
	nCopySize = m_bRingBufFill ? (m_nRingBufSize - m_nRingBufPtr) : 0;
	CopyWaveFromDouble(pWaveData->waveData, (double *)(m_pRingBuf + m_nRingBufPtr), NULL, nCopySize * m_oWaveFormat.Format.nChannels, m_oWaveFormat.Format.wBitsPerSample);
	CopyWaveFromDouble((char *)pWaveData->waveData + (nCopySize * nBytesPerSample), (double *)m_pRingBuf, NULL, m_nRingBufPtr * m_oWaveFormat.Format.nChannels, m_oWaveFormat.Format.wBitsPerSample);

	::GlobalUnlock(hWaveData);

	tempFileName.Format("%s\\%d.WAV", (LPCSTR)g_sTempDir, m_nSaveCounter);
	if (MMWaveWriteFile(hWaveData, (LPSTR)(LPCSTR)tempFileName) != 0) {
		::GlobalFree(hWaveData);
		return FALSE;
	}

	::GlobalFree(hWaveData);

	return TRUE;
}

void CMeasurement::FreeBuffers()
{
	if (m_pRingBuf != NULL) {
		delete [] m_pRingBuf;
		m_pRingBuf = NULL;
	}

	if (m_pRingBuf != NULL) {
		delete [] m_pRingBuf;
		m_pRingBuf = NULL;
	}

	if (m_pFilter != NULL) {
		delete [] m_pFilter;
		m_pFilter = NULL;
	}

	if (m_pFftBuf1 != NULL) {
		delete [] m_pFftBuf1;
		m_pFftBuf1 = NULL;
	}

	if (m_pFftBuf2 != NULL) {
		delete [] m_pFftBuf2;
		m_pFftBuf2 = NULL;
	}
}

LRESULT CMeasurement::OnChangeStatus(WPARAM wParam, LPARAM lParam)
{
	m_pDoc->ChangeStatus((int)wParam, (int)lParam);
	return 0;
}

UINT CMeasurement::CalcThreadEntry(LPVOID pParam)
{
	((CMeasurement *)pParam)->CalcThread();
	return 0;
}

void CMeasurement::CalcThread()
{
	for (;;) {
		m_bInCalculating = FALSE;

		while (m_nSaveCounter == m_nCalcCounter) {
			m_cCalcEvent.Lock();

			if (m_bExitThread) {
				break;
			}
		}
		if (m_bExitThread)
			break;

		m_bInCalculating = TRUE;

		SendMessage(WM_CHANGE_STATUS, m_nCalcCounter, STATUS_IN_CALC);

		if (m_pDoc->CalcFactor(m_nCalcCounter)) {
			SendMessage(WM_CHANGE_STATUS, m_nCalcCounter, STATUS_FINISH_CALC);

			m_nCalcCounter++;
		}
	}
	m_cExitEvent.SetEvent();
}

BOOL CMeasurement::InCalculating() const
{
	return m_bInCalculating;
}

void CMeasurement::ResetCounter()
{
	m_nSaveCounter = 0;
	m_nCalcCounter = 0;
}

void CMeasurement::Recalclation()
{
	m_nCalcCounter = 0;
	m_cCalcEvent.SetEvent();
}

void CMeasurement::WaveFileMeasuring(LPCTSTR pWaveFileName)
{
	CFileStatus status;
	CString tempFileName;
	CWaveFile waveFile;
	PCMWAVEFORMAT waveFormat;
	DWORD dwDataSize;

	if (CFile::GetStatus(pWaveFileName, status)) {
		tempFileName.Format("%s\\%d.WAV", (LPCSTR)g_sTempDir, m_nSaveCounter);

		::CopyFile(pWaveFileName, tempFileName, FALSE);

		if (waveFile.Open(tempFileName, &waveFormat, &dwDataSize)) {
			double fSamplingTime = dwDataSize / (waveFormat.wBitsPerSample / 8) / waveFormat.wf.nChannels / (double)waveFormat.wf.nSamplesPerSec;
			AddItem(fSamplingTime, &status.m_mtime);
		}
	}
}

void CMeasurement::AddItem(double fSamplingTime, CTime *pTime)
{
	m_pDoc->AddNmsItem(m_nSaveCounter, fSamplingTime, pTime);
	m_pDoc->SetCondition(m_nSaveCounter);
	m_nSaveCounter++;
	if (g_oSetData.Calc.bAutoCalc)
		m_cCalcEvent.SetEvent();
}
