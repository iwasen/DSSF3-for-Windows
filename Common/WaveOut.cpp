// WaveOut.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include <mmsystem.h>
#include "WaveOut.h"
#include "Mixer.h"
#include "Common.h"

/////////////////////////////////////////////////////////////////////////////
// CWaveOut

CWaveOut::CWaveOut()
{
	CreateEx(0, ::AfxRegisterWndClass(0, 0, 0, 0), "WaveOut", WS_POPUP, 0, 0, 0, 0, HWND_MESSAGE, 0);

	m_hWave = NULL;
	m_pWaveHdr = NULL;
	m_pWnd = NULL;
	m_pSamplesBuffer = NULL;
}

CWaveOut::~CWaveOut()
{
	DestroyWindow();
}

BEGIN_MESSAGE_MAP(CWaveOut, CWnd)
	ON_WM_DESTROY()
	ON_MESSAGE(MM_WOM_DONE, OnWaveOutDone)
END_MESSAGE_MAP()

BOOL CWaveOut::m_b16BitOnly;

/////////////////////////////////////////////////////////////////////////////
// CWaveOut メッセージ ハンドラ

BOOL CWaveOut::Open(INT_PTR nWaveDevice,
				IWaveNotify *pWnd,
				int nChannels,
				int nSamplesPerSec,
				int nSamplesPerBuffer,
				int nBufferNum,
				BOOL bErrMsg,
				BOOL bVolumeSet)
{
	UINT err;
	char errMsg[MAXERRORLENGTH];
	int nBitsPerSample = 24;
	int nBufferSize = nSamplesPerBuffer * nChannels * (nBitsPerSample / 8);

	Close();

	m_pWnd = pWnd;
	m_bVolumeSet = bVolumeSet;

	m_oWaveFormat.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
	m_oWaveFormat.Format.nChannels = (WORD)nChannels;
	m_oWaveFormat.Format.nSamplesPerSec = nSamplesPerSec;
	m_oWaveFormat.Format.nAvgBytesPerSec = nSamplesPerSec * nChannels * (nBitsPerSample / 8);
	m_oWaveFormat.Format.nBlockAlign = (WORD)(nChannels * (nBitsPerSample / 8));
	m_oWaveFormat.Format.wBitsPerSample = (WORD)nBitsPerSample;
	m_oWaveFormat.Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE);
	m_oWaveFormat.Samples.wValidBitsPerSample = (WORD)nBitsPerSample;
	m_oWaveFormat.dwChannelMask = 0;
	m_oWaveFormat.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;

	if (m_b16BitOnly || waveOutOpen(&m_hWave, (UINT)nWaveDevice, (LPWAVEFORMATEX)&m_oWaveFormat, (DWORD_PTR)m_hWnd, 0, CALLBACK_WINDOW) != 0) {
		nBitsPerSample = 16;
		nBufferSize = nSamplesPerBuffer * nChannels * (nBitsPerSample / 8);

		m_oWaveFormat.Format.wFormatTag = WAVE_FORMAT_PCM;
		m_oWaveFormat.Format.nChannels = (WORD)nChannels;
		m_oWaveFormat.Format.nSamplesPerSec = nSamplesPerSec;
		m_oWaveFormat.Format.nAvgBytesPerSec = nSamplesPerSec * nChannels * (nBitsPerSample / 8);
		m_oWaveFormat.Format.nBlockAlign = (WORD)(nChannels * (nBitsPerSample / 8));
		m_oWaveFormat.Format.wBitsPerSample = (WORD)nBitsPerSample;
		m_oWaveFormat.Format.cbSize = sizeof(WAVEFORMATEX);

		if ((err = waveOutOpen(&m_hWave, (UINT)nWaveDevice, &m_oWaveFormat.Format, (DWORD_PTR)m_hWnd, 0, CALLBACK_WINDOW)) != 0) {
			m_pWnd = NULL;
			if (bErrMsg) {
				waveOutGetErrorText(err, errMsg, sizeof(errMsg));
				::AfxMessageBox(errMsg, NULL, MB_ICONEXCLAMATION);
			}
			return FALSE;
		}
	}

	if (m_bVolumeSet) {
		waveOutGetVolume(m_hWave, &m_dwOrgVolume);
		MixerSetOutput();
	}

	if (!AllocWaveBuffer(nBufferNum, nBufferSize)) {
		Close();
		return FALSE;
	}

	m_pSamplesBuffer = new double[nSamplesPerBuffer * nChannels];

	m_bCloseWait = FALSE;

	NotifyMessage(WAVEOUT_OPEN, NULL);

	return TRUE;
}

int CWaveOut::NotifyMessage(int nCode, LPWAVEHDR pWaveHdr)
{
	LONG nRc = 0;

	if (m_pWnd != NULL) {
		WAVENOTIFY waveNotify;
		memset(&waveNotify, 0, sizeof(waveNotify));

		if (pWaveHdr != NULL) {
			int nBytesPerSample = m_oWaveFormat.Format.wBitsPerSample / 8 * m_oWaveFormat.Format.nChannels;
			waveNotify.pSamplesData = m_pSamplesBuffer;
			waveNotify.nSamplesNum = pWaveHdr->dwBufferLength / nBytesPerSample;
			waveNotify.nSamplesRecorded = pWaveHdr->dwBytesRecorded / nBytesPerSample;
			waveNotify.nFlags = pWaveHdr->dwFlags;
			waveNotify.nChannels = m_oWaveFormat.Format.nChannels;
			waveNotify.nSamplesPerSec = m_oWaveFormat.Format.nSamplesPerSec;

			nRc = m_pWnd->OnWaveNotify(nCode, &waveNotify) * nBytesPerSample;

			ConvertDoubleToWave(pWaveHdr, nRc);

//			pWaveHdr->lpData = waveNotify.pSamplesData;
			pWaveHdr->dwBufferLength = waveNotify.nSamplesNum * nBytesPerSample;
			pWaveHdr->dwBytesRecorded = waveNotify.nSamplesRecorded * nBytesPerSample;
			pWaveHdr->dwFlags = waveNotify.nFlags;
		} else {
			waveNotify.nChannels = m_oWaveFormat.Format.nChannels;
			waveNotify.nSamplesPerSec = m_oWaveFormat.Format.nSamplesPerSec;
			nRc = m_pWnd->OnWaveNotify(nCode, &waveNotify);
		}
	}

	return nRc;
}

void CWaveOut::ConvertDoubleToWave(LPWAVEHDR pWaveHdr, int rc)
{
	int nDataNum = min((int)pWaveHdr->dwBufferLength, rc) / (m_oWaveFormat.Format.wBitsPerSample / 8);
	double *pSamplesData = m_pSamplesBuffer;
	BYTE *bp;
	short *sp;
	long *lp;
	int i;
	union {
		int i;
		BYTE b[4];
	} ib;

	switch (m_oWaveFormat.Format.wBitsPerSample) {
	case 16:
		sp = (short *)pWaveHdr->lpData;
		for (i = 0; i < nDataNum; i++)
			*sp++ = (short)ConvertToInt(*pSamplesData++, 16);
		break;
	case 24:
		bp = (BYTE *)pWaveHdr->lpData;
		for (i = 0; i < nDataNum; i++) {
			ib.i = ConvertToInt(*pSamplesData++, 24);
			*bp++ = ib.b[0];
			*bp++ = ib.b[1];
			*bp++ = ib.b[2];
		}
		break;
	case 32:
		lp = (long *)pWaveHdr->lpData;
		for (i = 0; i < nDataNum; i++)
			*lp++ = ConvertToInt(*pSamplesData++, 32);
		break;
	}
}

void CWaveOut::Start()
{
	if (m_hWave != NULL) {
		m_nBufCounter = 0;
		if (m_pWnd != NULL) {
			LPWAVEHDR pWaveHdr = m_pWaveHdr;
			while (pWaveHdr != NULL) {
				if ((pWaveHdr->dwBufferLength = NotifyMessage(WAVEOUT_DATA, pWaveHdr)) != 0) {
					waveOutWrite(m_hWave, pWaveHdr, sizeof(WAVEHDR));
					m_nBufCounter++;
				}

				pWaveHdr = (LPWAVEHDR)pWaveHdr->dwUser;
			}
		}

		if (m_nBufCounter == 0)
			Close();
	}
}

void CWaveOut::Close()
{
	if (m_hWave != NULL) {
		waveOutReset(m_hWave);
		if (m_bVolumeSet)
			waveOutSetVolume(m_hWave, m_dwOrgVolume);
		FreeWaveBuffer();
		waveOutClose(m_hWave);
		m_hWave = NULL;

		if (m_hWnd != NULL) {
			MSG msg;
			for (;;) {
				if (!::PeekMessage(&msg, m_hWnd, MM_WOM_DONE, MM_WOM_DONE, PM_REMOVE))
					break;
			}
		}

		NotifyMessage(WAVEOUT_CLOSE, NULL);
		m_pWnd = NULL;

		if (m_bVolumeSet)
			MixerRestoreOutput();
	}

	if (m_pSamplesBuffer != NULL) {
		delete [] m_pSamplesBuffer;
		m_pSamplesBuffer = NULL;
	}
}

void CWaveOut::Reset()
{
	if (m_hWave != NULL)
		waveOutReset(m_hWave);
}

void CWaveOut::Pause()
{
	if (m_hWave != NULL)
		waveOutPause(m_hWave);
}

void CWaveOut::Restart()
{
	if (m_hWave != NULL)
		waveOutRestart(m_hWave);
}

void CWaveOut::SetVolume(UINT left, UINT right)
{
	if (m_hWave != NULL)
		waveOutSetVolume(m_hWave, (right << 16) | left);
}

BOOL CWaveOut::AllocWaveBuffer(int nBufferNum, int nBufferSize)
{
	if (m_hWave == NULL)
		return FALSE;

	HGLOBAL hData, hWaveHdr;
	LPSTR	pData;
	LPWAVEHDR pWaveHdr;
	LPWAVEHDR pWaveHdr2 = NULL;

	for (int i = 0; i < nBufferNum; i++) {
		if ((hData = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, nBufferSize)) == NULL) {
			FreeWaveBuffer();
			return FALSE;
		}

		if ((hWaveHdr = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE | GMEM_ZEROINIT, sizeof(WAVEHDR))) == NULL) {
			::GlobalFree(hData);
			FreeWaveBuffer();
			return FALSE;
		}

		pData = (LPSTR)::GlobalLock(hData);
		pWaveHdr = (LPWAVEHDR)::GlobalLock(hWaveHdr);

		m_pWaveHdr = pWaveHdr;

		pWaveHdr->lpData = pData;
		pWaveHdr->dwBufferLength = nBufferSize;
		pWaveHdr->dwUser = (DWORD_PTR)pWaveHdr2;
		pWaveHdr2 = pWaveHdr;

		if (waveOutPrepareHeader(m_hWave, pWaveHdr, sizeof(WAVEHDR)) != 0) {
			FreeWaveBuffer();
			return FALSE;
		}
	}

	return TRUE;
}

void CWaveOut::FreeWaveBuffer()
{
	HGLOBAL hMem;

	LPWAVEHDR pWaveHdr = m_pWaveHdr;

	while (pWaveHdr != NULL) {
		LPWAVEHDR pWaveHdr2 = (LPWAVEHDR)pWaveHdr->dwUser;

		waveOutUnprepareHeader(m_hWave, pWaveHdr, sizeof(WAVEHDR));

		hMem = ::GlobalHandle(pWaveHdr->lpData);
		::GlobalUnlock(hMem);
		::GlobalFree(hMem);

		hMem = ::GlobalHandle(pWaveHdr);
		::GlobalUnlock(hMem);
		::GlobalFree(hMem);

		pWaveHdr = pWaveHdr2;
	}

	m_pWaveHdr = NULL;
}

void CWaveOut::OnDestroy()
{
	CWnd::OnDestroy();

	Close();
}

LRESULT CWaveOut::OnWaveOutDone(WPARAM /*wParam*/, LPARAM lParam)
{
	LPWAVEHDR lpWaveHdr = (LPWAVEHDR)lParam;

	m_nBufCounter--;

	if (m_hWave != NULL && m_pWnd != NULL) {
		if (!m_bCloseWait) {
			DWORD dwBufferLength = NotifyMessage(WAVEOUT_DATA, lpWaveHdr);
			if (m_hWave == NULL)
				return 0L;

			if (dwBufferLength != 0) {
				lpWaveHdr->dwBufferLength = dwBufferLength;
				waveOutWrite(m_hWave, lpWaveHdr, sizeof(WAVEHDR));
				m_nBufCounter++;
			} else
				m_bCloseWait = TRUE;
		} else
			NotifyMessage(WAVEOUT_CLOSE_WAIT, lpWaveHdr);
	}

	if (m_nBufCounter == 0)
		Close();

	return 0L;
}

int CWaveOut::GetBitsPerSample()
{
	return m_oWaveFormat.Format.wBitsPerSample;
}
