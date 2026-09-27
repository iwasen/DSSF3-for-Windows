// WaveIn.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "WaveIn.h"
#include "Common.h"

/////////////////////////////////////////////////////////////////////////////
// CWaveIn

CWaveIn::CWaveIn()
{
	CreateEx(0, ::AfxRegisterWndClass(0, 0, 0, 0), "WaveIn", WS_POPUP, 0, 0, 0, 0, HWND_MESSAGE, 0);

	m_hWave = NULL;
	m_pWaveHdr = NULL;
	m_pWnd = NULL;
	m_pSamplesBuffer = NULL;
}

CWaveIn::~CWaveIn()
{
	DestroyWindow();
}

BEGIN_MESSAGE_MAP(CWaveIn, CWnd)
	ON_WM_DESTROY()
	ON_MESSAGE(MM_WIM_DATA, OnWaveInData)
END_MESSAGE_MAP()

BOOL CWaveIn::m_b16BitOnly;

/////////////////////////////////////////////////////////////////////////////
// CWaveIn メッセージ ハンドラ

BOOL CWaveIn::Open(INT_PTR nWaveDevice,
				IWaveNotify *pWnd,
				int nChannels,
				int nSamplesPerSec,
				int nSamplesPerBuffer,
				int nBufferNum,
				BOOL bErrMsg)
{
	LPWAVEHDR pWaveHdr;
	UINT err;
	char errMsg[MAXERRORLENGTH];
	int nBitsPerSample = 24;
	int nBufferSize = nSamplesPerBuffer * nChannels * (nBitsPerSample / 8);

	CWaveIn::Close();

	m_nWaveDevice = nWaveDevice;
	m_pWnd = pWnd;
	m_bErrMsg = bErrMsg;

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

	if (m_b16BitOnly || waveInOpen(&m_hWave, (UINT)m_nWaveDevice, (LPWAVEFORMATEX)&m_oWaveFormat, (DWORD_PTR)m_hWnd, 0, CALLBACK_WINDOW) != 0) {
		nBitsPerSample = 16;
		nBufferSize = nSamplesPerBuffer * nChannels * (nBitsPerSample / 8);

		m_oWaveFormat.Format.wFormatTag = WAVE_FORMAT_PCM;
		m_oWaveFormat.Format.nChannels = (WORD)nChannels;
		m_oWaveFormat.Format.nSamplesPerSec = nSamplesPerSec;
		m_oWaveFormat.Format.nAvgBytesPerSec = nSamplesPerSec * nChannels * (nBitsPerSample / 8);
		m_oWaveFormat.Format.nBlockAlign = (WORD)(nChannels * (nBitsPerSample / 8));
		m_oWaveFormat.Format.wBitsPerSample = (WORD)nBitsPerSample;
		m_oWaveFormat.Format.cbSize = sizeof(WAVEFORMATEX);

		if ((err = waveInOpen(&m_hWave, (UINT)m_nWaveDevice, &m_oWaveFormat.Format, (DWORD_PTR)m_hWnd, 0, CALLBACK_WINDOW)) != 0) {
			m_pWnd = NULL;
			if (m_bErrMsg) {
				waveInGetErrorText(err, errMsg, sizeof(errMsg));
				::AfxMessageBox(errMsg, NULL, MB_ICONEXCLAMATION);
			}
			return FALSE;
		}
	}

//	MixerSetInput();

	if (!AllocWaveBuffer(nBufferNum, nBufferSize)) {
		Close();
		return FALSE;
	}

	m_pSamplesBuffer = new double[nSamplesPerBuffer * nChannels];

	pWaveHdr = m_pWaveHdr;
	while (pWaveHdr != NULL) {
		waveInAddBuffer(m_hWave, pWaveHdr, sizeof(WAVEHDR));

		pWaveHdr = (LPWAVEHDR)pWaveHdr->dwUser;
	}

	NotifyMessage(WAVEIN_OPEN, NULL);

	return TRUE;
}

int CWaveIn::NotifyMessage(int nCode, LPWAVEHDR pWaveHdr)
{
	LONG nRc = 0;

	if (m_pWnd != NULL) {
		WAVENOTIFY waveNotify{};

		if (pWaveHdr != NULL) {
			int nBytesPerSample = m_oWaveFormat.Format.wBitsPerSample / 8 * m_oWaveFormat.Format.nChannels;
			waveNotify.pSamplesData = m_pSamplesBuffer;
			waveNotify.nSamplesNum = pWaveHdr->dwBufferLength / nBytesPerSample;
			waveNotify.nSamplesRecorded = pWaveHdr->dwBytesRecorded / nBytesPerSample;
			waveNotify.nFlags = pWaveHdr->dwFlags;
			waveNotify.nChannels = m_oWaveFormat.Format.nChannels;
			waveNotify.nSamplesPerSec = m_oWaveFormat.Format.nSamplesPerSec;

			ConvertWaveToDouble(pWaveHdr);

			nRc = m_pWnd->OnWaveNotify(nCode, &waveNotify) * nBytesPerSample;

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

void CWaveIn::ConvertWaveToDouble(LPWAVEHDR pWaveHdr)
{
	int nDataNum = pWaveHdr->dwBytesRecorded / (m_oWaveFormat.Format.wBitsPerSample / 8);
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
			*pSamplesData++ = *sp++ / PowDouble(16);
		break;
	case 24:
		bp = (BYTE *)pWaveHdr->lpData;
		ib.b[0] = 0;
		for (i = 0; i < nDataNum; i++) {
			ib.b[1] = *bp++;
			ib.b[2] = *bp++;
			ib.b[3] = *bp++;
			*pSamplesData++ = ib.i / PowDouble(32);
		}
		break;
	case 32:
		lp = (long *)pWaveHdr->lpData;
		for (i = 0; i < nDataNum; i++)
			*pSamplesData++ = *lp++ / PowDouble(32);
		break;
	}
}

void CWaveIn::Start() const
{
	if (m_hWave != NULL)
		waveInStart(m_hWave);
}

void CWaveIn::Close()
{
	if (m_hWave != NULL) {
		waveInStop(m_hWave);
		waveInReset(m_hWave);
		FreeWaveBuffer();
		waveInClose(m_hWave);
		m_hWave = NULL;

		if (m_hWnd != NULL) {
			MSG msg;
			for (;;) {
				if (!::PeekMessage(&msg, m_hWnd, MM_WIM_DATA, MM_WIM_DATA, PM_REMOVE))
					break;
			}
		}

//		MixerRestoreInput();
	}

	if (m_pWnd != NULL) {
		NotifyMessage(WAVEIN_CLOSE, NULL);
		m_pWnd = NULL;
	}

	if (m_pSamplesBuffer != NULL) {
		delete [] m_pSamplesBuffer;
		m_pSamplesBuffer = NULL;
	}
}

void CWaveIn::Reset() const
{
	if (m_hWave != NULL)
		waveInReset(m_hWave);
}

void CWaveIn::Stop() const
{
	if (m_hWave != NULL)
		waveInStop(m_hWave);
}

BOOL CWaveIn::AllocWaveBuffer(int nBufferNum, int nBufferSize)
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

		if (waveInPrepareHeader(m_hWave, pWaveHdr, sizeof(WAVEHDR)) != 0) {
			FreeWaveBuffer();
			return FALSE;
		}
	}

	return TRUE;
}

void CWaveIn::FreeWaveBuffer()
{
	HGLOBAL hMem;

	LPWAVEHDR pWaveHdr = m_pWaveHdr;

	while (pWaveHdr != NULL) {
		LPWAVEHDR pWaveHdr2 = (LPWAVEHDR)pWaveHdr->dwUser;

		waveInUnprepareHeader(m_hWave, pWaveHdr, sizeof(WAVEHDR));

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

void CWaveIn::OnDestroy()
{
	CWnd::OnDestroy();

	Close();
}

LRESULT CWaveIn::OnWaveInData(WPARAM /*wParam*/, LPARAM lParam)
{
	LPWAVEHDR lpWaveHdr = (LPWAVEHDR)lParam;
	BOOL bValid = FALSE;

	if (m_hWave != NULL && m_pWnd != NULL)
		bValid = NotifyMessage(WAVEIN_DATA, lpWaveHdr);

	if (bValid) {
		waveInAddBuffer(m_hWave, lpWaveHdr, sizeof(WAVEHDR));
	} else
		Close();

	return 0L;
}

int CWaveIn::GetBitsPerSample() const
{
	return m_oWaveFormat.Format.wBitsPerSample;
}
