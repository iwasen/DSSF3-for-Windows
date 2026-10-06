// PlaybackBtn.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "common.h"
#include "comres.h"
#include "PlaybackBtn.h"

#define WAVEBUF_SIZE	1024
#define MODE_HWAVE		0
#define MODE_BUF		1

/////////////////////////////////////////////////////////////////////////////
// CPlaybackBtn

BEGIN_MESSAGE_MAP(CPlaybackBtn, CButton)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlaybackBtn メッセージ ハンドラ

int CPlaybackBtn::OnWaveNotify(int nCode, LPWAVENOTIFY pWaveNotify)
{
	CString str;
	int n = 0;

	switch (nCode) {
	case WAVEOUT_OPEN:
		str.LoadString(IDS_STOP2);
		SetWindowText(str);
		SetCheck(1);
		break;
	case WAVEOUT_DATA:
		switch (m_nMode) {
		case MODE_HWAVE:
			n = ModeHwave(pWaveNotify);
			break;
		case MODE_BUF:
			n = ModeBuf(pWaveNotify);
			break;
		}
		break;
	case WAVEOUT_CLOSE:
		str.LoadString(IDS_REPLAY);
		SetWindowText(str);
		SetCheck(0);
		break;
	}

	return n;
}

int CPlaybackBtn::ModeHwave(LPWAVENOTIFY pWaveNotify)
{
	PWAVEDATA pWaveData = (PWAVEDATA)::GlobalLock(m_hWaveData);

	const int nData = min(WAVEBUF_SIZE, m_nData - m_nOffset);
	const int nBytesPerSample = (pWaveData->nBitsPerSample / 8) * pWaveData->nChannels;
	CopyWaveToDouble((BYTE *)pWaveData->waveData + m_nOffset * nBytesPerSample, pWaveNotify->pSamplesData, NULL, nData * pWaveData->nChannels, pWaveData->nBitsPerSample);
	m_nOffset += nData;

	::GlobalUnlock(m_hWaveData);

	return nData;
}

int CPlaybackBtn::ModeBuf(LPWAVENOTIFY pWaveNotify)
{
	int i;

	double *pData = pWaveNotify->pSamplesData;
	for (i = 0; i < WAVEBUF_SIZE && m_nOffset < m_nData; m_nOffset++) {
		*pData++ = m_pLeftData[m_nOffset] / m_fMaxValue;

		if (m_pRightData != NULL)
			*pData++ = m_pRightData[m_nOffset] / m_fMaxValue;

		i++;
	}

	return i;
}

void CPlaybackBtn::Play(HWAVEDATA hWaveData, DWORD nWaveDevice, double fOffsetTime)
{
	if (GetCheck()) {
		if (hWaveData != NULL) {
			PWAVEDATA pWaveData = (PWAVEDATA)::GlobalLock(hWaveData);
			const WORD nChannels = pWaveData->nChannels;
			const DWORD nSamplesPerSec = pWaveData->nSamplesPerSec;
			const int nSampleSize = (pWaveData->nBitsPerSample / 8) * nChannels;

			m_nMode = MODE_HWAVE;
			m_hWaveData = hWaveData;
			m_nData = pWaveData->dataSize / nSampleSize;
			m_nOffset = (int)(fOffsetTime * pWaveData->nSamplesPerSec);
			::GlobalUnlock(hWaveData);

			if (m_oWaveOut.Open(nWaveDevice, this, nChannels, nSamplesPerSec, WAVEBUF_SIZE, 4, FALSE))
				m_oWaveOut.Start();
		} else
			SetCheck(0);
	} else
		m_oWaveOut.Close();
}

void CPlaybackBtn::Play(const double *pLeftData, const double *pRightData, int nData, int nRate, DWORD nWaveDevice)
{
	if (GetCheck()) {
		const WORD nChannels = (pRightData != NULL) ? 2 : 1;
		const DWORD nSamplesPerSec = nRate;

		m_nMode = MODE_BUF;
		m_pLeftData = pLeftData;
		m_pRightData = pRightData;
		m_nData = nData;
		m_fMaxValue = GetMaxValue();
		m_nOffset = 0;

		if (m_oWaveOut.Open(nWaveDevice, this, nChannels, nSamplesPerSec, WAVEBUF_SIZE, 4, FALSE))
			m_oWaveOut.Start();
	} else
		m_oWaveOut.Close();
}

void CPlaybackBtn::Stop()
{
	m_oWaveOut.Close();
}

double CPlaybackBtn::GetMaxValue()
{
	double fMaxVal = 0;

	for (int i = 0; i < m_nData; i++) {
		if (m_pLeftData != NULL) {
			if (fabs(m_pLeftData[i]) > fMaxVal)
				fMaxVal = fabs(m_pLeftData[i]);
		}

		if (m_pRightData != NULL) {
			if (fabs(m_pRightData[i]) > fMaxVal)
				fMaxVal = fabs(m_pRightData[i]);
		}
	}

	return fMaxVal;
}

void CPlaybackBtn::OnDestroy()
{
	CButton::OnDestroy();

	m_oWaveOut.Close();
}
