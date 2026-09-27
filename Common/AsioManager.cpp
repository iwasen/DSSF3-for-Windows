// ..\Common\AsioManager.cpp : 実装ファイル
//

#include "StdAfx.h"
#include "comres.h"
#include "AsioManager.h"
#include "Common.h"

// CAsioManager
CAsioManager g_cAsioManager;

CAsioManager::CAsioManager()
{
	m_nCurrentDevice = -1;
}

BEGIN_MESSAGE_MAP(CAsioManager, CWnd)
	ON_MESSAGE(WM_ASIO_NOTIFY_IN, OnAsioNotifyIn)
	ON_MESSAGE(WM_ASIO_NOTIFY_OUT, OnAsioNotifyOut)
	ON_MESSAGE(WM_ASIO_CLOSE_WAVEIN, OnAsioCloseWaveIn)
	ON_MESSAGE(WM_ASIO_CLOSE_WAVEOUT, OnAsioCloseWaveOut)
END_MESSAGE_MAP()

// CAsioManager メッセージ ハンドラ

void CAsioManager::Init(void)
{
	CAsioDriver::Initialize();

	CreateEx(0, ::AfxRegisterWndClass(0, 0, 0, 0), "AsioManager", WS_POPUP, 0, 0, 0, 0, HWND_MESSAGE, 0);
}

void CAsioManager::Exit(void)
{
	UnLoadAsioDriver();

	CAsioDriver::UnInitialize();

	FreeBufferIn();
	FreeBufferOut();

	DestroyWindow();
}

BOOL CAsioManager::OpenIn(INT_PTR nWaveDevice, IWaveNotify *pWaveNotify, int nChannels, int nSamplesPerSec, int nSamplesPerBuffer, BOOL bErrMsg)
{
	CloseIn();

	if (m_bOutOpen) {
		if (nSamplesPerSec != m_nSamplesPerSec)
			CloseOut();
	}

	m_nInChannels = nChannels;
	m_nSamplesPerSec = nSamplesPerSec;
	m_nWaveInBufSize = nSamplesPerBuffer;
	m_pWaveInNotify = pWaveNotify;
	m_nInBufCount = 0;
	m_nInBufIndex = 0;
	m_bInOpen = TRUE;
	m_bInStart = FALSE;
	m_bInNotify = FALSE;

	if (!m_bOutOpen) {
		if (!OpenDriver(nWaveDevice, bErrMsg)) {
			m_bInOpen = FALSE;
			return FALSE;
		}
	} else
		ChangeBufSize();

	FreeBufferIn();

	for (int i = 0; i < 2; i++)
		m_pWaveInBuf[i] = new double[m_nWaveInBufSize * 2];

	NotifyMessageIn(WAVEIN_OPEN);

	return TRUE;
}

void CAsioManager::StartIn()
{
	m_bInStart = TRUE;
	m_nInBufWait = 0;

	if (!m_bAsioStart) {
		m_bAsioStart = TRUE;
		m_cAsioDriver.ASIOStart();
	}
}

void CAsioManager::CloseIn()
{
	if (m_bInOpen) {
		m_bInOpen = FALSE;

		if (!m_bOutOpen)
			CloseDriver();

		NotifyMessageIn(WAVEIN_CLOSE);
	}
}

void CAsioManager::ResetIn()
{
}

void CAsioManager::StopIn()
{
	m_bInStart = FALSE;
}

BOOL CAsioManager::OpenOut(INT_PTR nWaveDevice, IWaveNotify *pWaveNotify, int nChannels, int nSamplesPerSec, int nSamplesPerBuffer, BOOL bErrMsg)
{
	CloseOut();

	if (m_bInOpen) {
		if (nSamplesPerSec != m_nSamplesPerSec)
			CloseIn();
	}

	m_nOutChannels = nChannels;
	m_nSamplesPerSec = nSamplesPerSec;
	m_nWaveOutBufSize = nSamplesPerBuffer;
	m_pWaveOutNotify = pWaveNotify;
	m_nOutBufCount = 0;
	m_nWaveOutCount = 0;
	m_nOutBufIndex = 0;
	m_nCloseOutWait = 0;
	m_bOutOpen = TRUE;
	m_bOutStart = FALSE;
	m_bOutNotify = FALSE;

	if (!m_bInOpen) {
		if (!OpenDriver(nWaveDevice, bErrMsg)) {
			m_bOutOpen = FALSE;
			return FALSE;
		}
	} else
		ChangeBufSize();

	FreeBufferOut();

	for (int i = 0; i < 2; i++)
		m_pWaveOutBuf[i] = new double[m_nWaveOutBufSize * 2];

	NotifyMessageOut(WAVEOUT_OPEN);

	return TRUE;
}

void CAsioManager::StartOut()
{
	m_nOutBufSize = NotifyMessageOut(WAVEOUT_DATA, m_pWaveOutBuf[0], m_nWaveOutBufSize, 0, 0);
	if (m_nOutBufSize > 0)
		m_nOutBufSize = NotifyMessageOut(WAVEOUT_DATA, m_pWaveOutBuf[1], m_nWaveOutBufSize, 0, 0);

	m_bOutStart = TRUE;

	if (!m_bAsioStart) {
		m_bAsioStart = TRUE;
		m_cAsioDriver.ASIOStart();
	}
}

void CAsioManager::CloseOut()
{
	if (m_bOutOpen) {
		m_bOutOpen = FALSE;

		if (!m_bInOpen)
			CloseDriver();

		NotifyMessageOut(WAVEOUT_CLOSE);
	}
}

void CAsioManager::ResetOut()
{
}

void CAsioManager::PauseOut()
{
}

void CAsioManager::RestartOut()
{
}

BOOL CAsioManager::LoadAsioDriver(INT_PTR nWaveDevice, BOOL bErrMsg)
{
	if (nWaveDevice != m_nCurrentDevice)
		UnLoadAsioDriver();

	if (m_nCurrentDevice == -1) {
		if (m_cAsioDriver.asioOpenDriver(nWaveDevice) != ASE_OK) {
			if (bErrMsg)
				::AfxMessageBox(IDS_ASIO_OPEN_ERROR, MB_ICONEXCLAMATION);
			return FALSE;
		}

		if (m_cAsioDriver.ASIOInit() != ASE_OK) {
			if (bErrMsg)
				m_cAsioDriver.DispErrorMessage();
			return FALSE;
		}

		m_nCurrentDevice = nWaveDevice;
	}

	return TRUE;
}

void CAsioManager::UnLoadAsioDriver()
{
	if (m_nCurrentDevice != -1) {
		m_cAsioDriver.ASIOExit();
		m_nCurrentDevice = -1;
	}
}

BOOL CAsioManager::OpenDriver(INT_PTR nWaveDevice, BOOL bErrMsg)
{
	int i;

	if (!LoadAsioDriver(nWaveDevice, bErrMsg))
		return FALSE;

	if (m_cAsioDriver.ASIOSetSampleRate(m_nSamplesPerSec) != ASE_OK) {
		if (bErrMsg)
			m_cAsioDriver.DispErrorMessage();
		return FALSE;
	}

	m_nMinSize = m_nMaxSize = m_nPreferredSize = m_nGranularity = 0;
	if (m_cAsioDriver.ASIOGetBufferSize(&m_nMinSize, &m_nMaxSize, &m_nPreferredSize, &m_nGranularity) != ASE_OK) {
		if (bErrMsg)
			m_cAsioDriver.DispErrorMessage();
		return FALSE;
	}

	int nCount = 0;
	ASIOBufferInfo *pBufferInfoTop = m_asioBufferInfo;
	ASIOBufferInfo *pBufferInfo = m_asioBufferInfo;
	ASIOChannelInfo *pChannelInfo = m_asioChannelInfo;
	for (i = 0; i < 2; i++, pBufferInfo++, pChannelInfo++) {
		pBufferInfo->isInput = ASIOTrue;
		pBufferInfo->channelNum = i;
		pBufferInfo->buffers[0] = pBufferInfo->buffers[1] = 0;

		pChannelInfo->channel = i;
		pChannelInfo->isInput = ASIOTrue;
		if (m_cAsioDriver.ASIOGetChannelInfo(pChannelInfo) == ASE_OK) {
			nCount++;
		} else {
			pBufferInfo = m_asioBufferInfo + 2;
			pChannelInfo = m_asioChannelInfo + 2;
			pBufferInfoTop = pBufferInfo;
			nCount = 0;
			break;
		}
	}

	for (i = 0; i < 2; i++, pBufferInfo++, pChannelInfo++) {
		pBufferInfo->isInput = ASIOFalse;
		pBufferInfo->channelNum = i;
		pBufferInfo->buffers[0] = pBufferInfo->buffers[1] = 0;

		pChannelInfo->channel = i;
		pChannelInfo->isInput = ASIOFalse;
		if (m_cAsioDriver.ASIOGetChannelInfo(pChannelInfo) == ASE_OK) {
			nCount++;
		}
	}

	m_nBufSize = GetBufSize();
	if (m_nBufSize == 0) {
		if (bErrMsg)
			::AfxMessageBox(IDS_ASIO_OPEN_ERROR, MB_ICONEXCLAMATION);
		return FALSE;
	}

	// create and activate buffers
	m_asioCallbacks.asioMessage = asioMessages;
	m_asioCallbacks.bufferSwitch = bufferSwitch;
	m_asioCallbacks.bufferSwitchTimeInfo = bufferSwitchTimeInfo;
	m_asioCallbacks.sampleRateDidChange = sampleRateChanged;

	if (m_cAsioDriver.ASIOCreateBuffers(pBufferInfoTop, nCount, m_nBufSize, &m_asioCallbacks) != ASE_OK) {
		if (bErrMsg)
			m_cAsioDriver.DispErrorMessage();
		return FALSE;
	}

	return TRUE;
}

void CAsioManager::CloseDriver()
{
	m_cAsioDriver.ASIOStop();
	m_cAsioDriver.ASIODisposeBuffers();

	m_bAsioStart = FALSE;
}

int CAsioManager::GetBufSize() const
{
	int nBufSize = m_nPreferredSize;

	int nWaveBufSize;
	if (m_bInOpen) {
		nWaveBufSize = m_nWaveInBufSize;
		if (m_bOutOpen && m_nWaveOutBufSize < nWaveBufSize)
			nWaveBufSize = m_nWaveOutBufSize;
	} else if (m_bOutOpen)
		nWaveBufSize = m_nWaveOutBufSize;
	else
		return 0;

	if (nBufSize > nWaveBufSize) {
		if (m_nGranularity > 0) {
			do {
				nBufSize -= m_nGranularity;
				if (nBufSize < m_nMinSize)
					return 0;
			} while (nBufSize > nWaveBufSize);
		} else if (m_nGranularity < 0) {
			do {
				nBufSize /= 2;
				if (nBufSize < m_nMinSize)
					return 0;
			} while (nBufSize > nWaveBufSize);
		} else
			return 0;
	}

	return nBufSize;
}

BOOL CAsioManager::ChangeBufSize()
{
	int nOldBufSize = m_nBufSize;
	int nNewBufSize = GetBufSize();
	if (nOldBufSize != nNewBufSize) {
		m_cAsioDriver.ASIOStop();
		m_cAsioDriver.ASIODisposeBuffers();
		m_bAsioStart = FALSE;

		m_nBufSize = nNewBufSize;
		m_cAsioDriver.ASIOCreateBuffers(m_asioBufferInfo, 4, m_nBufSize, &m_asioCallbacks);
	}

	return 0;
}

void CAsioManager::FreeBufferIn()
{
	for (int i = 0; i < 2; i++) {
		if (m_pWaveInBuf[i] != NULL) {
			delete [] m_pWaveInBuf[i];
			m_pWaveInBuf[i] = NULL;
		}
	}
}

void CAsioManager::FreeBufferOut()
{
	for (int i = 0; i < 2; i++) {
		if (m_pWaveOutBuf[i] != NULL) {
			delete [] m_pWaveOutBuf[i];
			m_pWaveOutBuf[i] = NULL;
		}
	}
}

int CAsioManager::NotifyMessageIn(int nCode, double *pData, int nSamplesNum, int nSamplesRecorded, int nFlags, BOOL bPost)
{
	LONG nRc = 0;

	if (m_pWaveInNotify != NULL) {
		m_cWaveInNotify.pSamplesData = pData;
		m_cWaveInNotify.nSamplesNum = nSamplesNum;
		m_cWaveInNotify.nSamplesRecorded = nSamplesRecorded;
		m_cWaveInNotify.nFlags = nFlags;
		m_cWaveInNotify.nChannels = m_nInChannels;
		m_cWaveInNotify.nSamplesPerSec = m_nSamplesPerSec;

		m_bInNotify = TRUE;

		if (bPost)
			SendMessage(WM_ASIO_NOTIFY_IN, nCode, (LPARAM)&m_cWaveInNotify);
		else
			nRc = m_pWaveInNotify->OnWaveNotify(nCode, &m_cWaveInNotify);

		m_bInNotify = FALSE;
	}

	return nRc;
}

int CAsioManager::NotifyMessageOut(int nCode, double *pData, int nSamplesNum, int nSamplesRecorded, int nFlags, BOOL bPost)
{
	LONG nRc = 0;

	if (m_pWaveOutNotify != NULL) {
		m_cWaveOutNotify.pSamplesData = pData;
		m_cWaveOutNotify.nSamplesNum = nSamplesNum;
		m_cWaveOutNotify.nSamplesRecorded = nSamplesRecorded;
		m_cWaveOutNotify.nFlags = nFlags;
		m_cWaveOutNotify.nChannels = m_nOutChannels;
		m_cWaveOutNotify.nSamplesPerSec = m_nSamplesPerSec;

		m_bOutNotify = TRUE;

		if (bPost)
			SendMessage(WM_ASIO_NOTIFY_OUT, nCode, (LPARAM)&m_cWaveOutNotify);
		else
			nRc = m_pWaveOutNotify->OnWaveNotify(nCode, &m_cWaveOutNotify);

		m_bOutNotify = FALSE;
	}

	return nRc;
}

LRESULT CAsioManager::OnAsioNotifyIn(WPARAM wParam, LPARAM lParam)
{
	if (m_bInOpen) {
		if (m_pWaveInNotify->OnWaveNotify((int)wParam, (LPWAVENOTIFY)lParam) == 0) {
			if (m_bInNotify) {
				PostMessage(WM_ASIO_CLOSE_WAVEIN);
			} else {
				CloseIn();
			}
		}
	}

	return 0;
}

LRESULT CAsioManager::OnAsioCloseWaveIn(WPARAM, LPARAM)
{
	CloseIn();

	return 0;
}

LRESULT CAsioManager::OnAsioNotifyOut(WPARAM wParam, LPARAM lParam)
{
	if (m_bOutOpen) {
		LPWAVENOTIFY pWaveNotify = (LPWAVENOTIFY)lParam;
		m_nOutBufSize = m_pWaveOutNotify->OnWaveNotify((int)wParam, pWaveNotify);
		if (m_nOutBufSize < m_nWaveOutBufSize)
			memset(pWaveNotify->pSamplesData + m_nOutBufSize * m_nOutChannels, 0, (m_nWaveOutBufSize - m_nOutBufSize) * sizeof(double) * m_nOutChannels);

		if (m_nCloseOutWait == 0 && m_nOutBufSize == 0)
			m_nCloseOutWait = 1;

		if (m_nCloseOutWait != 0) {
			if (m_nCloseOutWait++ >= 3) {
				if (m_bOutNotify) {
					PostMessage(WM_ASIO_CLOSE_WAVEOUT);
				} else {
					CloseOut();
				}
			}
		}
	}

	return 0;
}

LRESULT CAsioManager::OnAsioCloseWaveOut(WPARAM, LPARAM)
{
	CloseOut();

	return 0;
}

BOOL CAsioManager::CanSamplingRate(INT_PTR nWaveDevice, int nSamplingRate)
{
	if (!LoadAsioDriver(nWaveDevice, FALSE))
		return FALSE;

	BOOL bRet = m_cAsioDriver.ASIOCanSampleRate(nSamplingRate) == ASE_OK;

	return bRet;
}

long CAsioManager::asioMessages(long selector, long value, void* /*message*/, double* /*opt*/)
{
	// currently the parameters "value", "message" and "opt" are not used.
	long ret = 0;
	switch(selector)
	{
		case kAsioSelectorSupported:
			if(value == kAsioResetRequest
			|| value == kAsioEngineVersion
			|| value == kAsioResyncRequest
			|| value == kAsioLatenciesChanged
			|| value == kAsioSupportsTimeInfo
			|| value == kAsioSupportsTimeCode
			|| value == kAsioSupportsInputMonitor)
				ret = 1L;
			break;
		case kAsioResetRequest:
			ret = 1L;
			break;
		case kAsioResyncRequest:
			ret = 1L;
			break;
		case kAsioLatenciesChanged:
			ret = 1L;
			break;
		case kAsioEngineVersion:
			ret = 2L;
			break;
		case kAsioSupportsTimeInfo:
			ret = 1;
			break;
		case kAsioSupportsTimeCode:
			ret = 0;
			break;
	}
	return ret;
}

void CAsioManager::Buffering(long index)
{
	void *p1, *p2;

	if (m_bInOpen && m_bInStart) {
		if (m_nInBufWait >= 2) {
			int nCopySize = m_nWaveInBufSize - m_nInBufCount;
			if (nCopySize > m_nBufSize)
				nCopySize = m_nBufSize;
			double *pWaveInBuf = m_pWaveInBuf[m_nInBufIndex] + m_nInBufCount * m_nInChannels;
			p1 = ConverToDouble(m_asioChannelInfo[0].type, nCopySize, m_asioBufferInfo[0].buffers[index], pWaveInBuf, m_nInChannels);
			p2 = ConverToDouble(m_asioChannelInfo[1].type, nCopySize, m_asioBufferInfo[1].buffers[index], pWaveInBuf + (m_nInChannels - 1), m_nInChannels);
			m_nInBufCount += nCopySize;

			if (m_nInBufCount >= m_nWaveInBufSize) {
				NotifyMessageIn(WAVEIN_DATA, m_pWaveInBuf[m_nInBufIndex], m_nWaveInBufSize, m_nWaveInBufSize, WHDR_DONE, TRUE);
				if (!m_bInOpen)
					return;
				m_nInBufIndex = ++m_nInBufIndex % 2;
				m_nInBufCount = 0;
			}

			int nCopySize2 = m_nBufSize - nCopySize;
			if (nCopySize2 > 0) {
				ConverToDouble(m_asioChannelInfo[0].type, nCopySize2, p1, m_pWaveInBuf[m_nInBufIndex], m_nInChannels);
				ConverToDouble(m_asioChannelInfo[1].type, nCopySize2, p2, m_pWaveInBuf[m_nInBufIndex] + (m_nInChannels - 1), m_nInChannels);
				m_nInBufCount += nCopySize2;
			}
		} else
			m_nInBufWait++;
	}

	if (m_bOutOpen && m_bOutStart) {
		int nCopySize = m_nWaveOutBufSize - m_nOutBufCount;
		if (nCopySize > m_nBufSize)
			nCopySize = m_nBufSize;
		double *pWaveOutBuf = m_pWaveOutBuf[m_nOutBufIndex] + m_nOutBufCount * m_nOutChannels;
		p1 = ConverFromDouble(m_asioChannelInfo[2].type, nCopySize, m_asioBufferInfo[2].buffers[index], pWaveOutBuf, m_nOutChannels);
		p2 = ConverFromDouble(m_asioChannelInfo[3].type, nCopySize, m_asioBufferInfo[3].buffers[index], pWaveOutBuf + (m_nOutChannels - 1), m_nOutChannels);
		m_nOutBufCount += nCopySize;

		if (m_nOutBufCount >= m_nWaveOutBufSize) {
			NotifyMessageOut(m_nCloseOutWait == 0 ? WAVEOUT_DATA : WAVEOUT_CLOSE_WAIT, m_pWaveOutBuf[m_nOutBufIndex], m_nWaveOutBufSize, 0, ++m_nWaveOutCount > 2 ? WHDR_DONE : 0, TRUE);
			if (!m_bOutOpen)
				return;
//			if (m_nOutBufSize < m_nWaveOutBufSize)
//				memset(m_pWaveOutBuf[m_nOutBufIndex] + m_nOutBufSize * m_nOutChannels, 0, (m_nWaveOutBufSize - m_nOutBufSize) * sizeof(double) * m_nOutChannels);
			m_nOutBufIndex = ++m_nOutBufIndex % 2;
			m_nOutBufCount = 0;
		}

		int nCopySize2 = m_nBufSize - nCopySize;
		if (nCopySize2 > 0) {
			ConverFromDouble(m_asioChannelInfo[2].type, nCopySize2, p1, m_pWaveOutBuf[m_nOutBufIndex], m_nOutChannels);
			ConverFromDouble(m_asioChannelInfo[3].type, nCopySize2, p2, m_pWaveOutBuf[m_nOutBufIndex] + (m_nOutChannels - 1), m_nOutChannels);
			m_nOutBufCount += nCopySize2;
		}

		if (m_bOutOpen)
			m_cAsioDriver.ASIOOutputReady();
	} else {
		ClearBuffer(m_asioChannelInfo[2].type, m_asioBufferInfo[2].buffers[index], m_nBufSize);
		ClearBuffer(m_asioChannelInfo[3].type, m_asioBufferInfo[3].buffers[index], m_nBufSize);
	}
}

void CAsioManager::bufferSwitch(long index, ASIOBool /*processNow*/)
{
	g_cAsioManager.Buffering(index);
}

ASIOTime *CAsioManager::bufferSwitchTimeInfo(ASIOTime * /*timeInfo*/, long index, ASIOBool processNow)
{
	bufferSwitch(index, processNow);

	return NULL;
}

void CAsioManager::sampleRateChanged(ASIOSampleRate /*sRate*/)
{
}

void CAsioManager::ClearBuffer(int nType, void *pBuf, int nBufSize)
{
	int nBytes = 0;

	switch (nType) {
	case ASIOSTInt16LSB:
	case ASIOSTInt16MSB:
		nBytes = 2;
		break;
	case ASIOSTInt24LSB:
	case ASIOSTInt24MSB:
		nBytes = 3;
		break;
	case ASIOSTInt32LSB:
	case ASIOSTFloat32LSB:
	case ASIOSTInt32LSB16:
	case ASIOSTInt32LSB18:
	case ASIOSTInt32LSB20:
	case ASIOSTInt32LSB24:
	case ASIOSTInt32MSB:
	case ASIOSTFloat32MSB:
	case ASIOSTInt32MSB16:
	case ASIOSTInt32MSB18:
	case ASIOSTInt32MSB20:
	case ASIOSTInt32MSB24:
		nBytes = 4;
		break;
	case ASIOSTFloat64LSB:
	case ASIOSTFloat64MSB:
		nBytes = 8;
		break;
	}

	memset(pBuf, 0, nBufSize * nBytes);
}

int CAsioManager::GetBitsPerSample(int nType)
{
	int nBits = 0;

	switch (nType) {
	case ASIOSTInt16LSB:
	case ASIOSTInt16MSB:
	case ASIOSTInt32LSB16:
	case ASIOSTInt32MSB16:
		nBits = 16;
		break;
	case ASIOSTInt32LSB18:
	case ASIOSTInt32MSB18:
		nBits = 18;
		break;
	case ASIOSTInt32LSB20:
	case ASIOSTInt32MSB20:
		nBits = 20;
		break;
	case ASIOSTInt24LSB:
	case ASIOSTInt24MSB:
	case ASIOSTInt32LSB24:
	case ASIOSTInt32MSB24:
		nBits = 24;
		break;
	case ASIOSTInt32LSB:
	case ASIOSTFloat32LSB:
	case ASIOSTInt32MSB:
	case ASIOSTFloat32MSB:
		nBits = 24;//32; 表示上最大ビット数は24としておく
		break;
	case ASIOSTFloat64LSB:
	case ASIOSTFloat64MSB:
		nBits = 24;//64;
		break;
	}

	return nBits;
}

int CAsioManager::GetBitsPerSampleIn() const
{
	return GetBitsPerSample(m_asioChannelInfo[0].type);
}

int CAsioManager::GetBitsPerSampleOut() const
{
	return GetBitsPerSample(m_asioChannelInfo[2].type);
}

#define SwapLong(v) ((((v)>>24)&0xFF)|(((v)>>8)&0xFF00)|(((v)&0xFF00)<<8)|(((v)&0xFF)<<24))
#define SwapShort(v) ((((v)>>8)&0xFF)|(((v)&0xFF)<<8))

void *CAsioManager::ConverFromDouble(int nType, int nSize, void *pOutData, const double *pInData, int nAlign)
{
	int i;
	union {
		void *v;
		char *c;
		short *s;
		long *l;
		double *f;
		double *d;
	} out;

	out.v = pOutData;

	switch (nType) {
	case ASIOSTInt16LSB:
		for(i = 0; i < nSize; i++, pInData += nAlign) {
			*out.s++ = (short)ConvertToInt(*pInData, 16);
		}
		break;
	case ASIOSTInt24LSB:		// used for 20 bits as well
		{
			int t;
			char* pt = (char*)&t;
			for (i = 0; i < nSize; i++, pInData += nAlign) {
				t = ConvertToInt(*pInData, 24);
				*out.c++ = pt[0];
				*out.c++ = pt[1];
				*out.c++ = pt[2];
			}
		}
		break;
	case ASIOSTInt32LSB:
		for (i = 0; i < nSize; i++, pInData += nAlign) {
			*out.l++ = ConvertToInt(*pInData, 32);
		}
		break;
	case ASIOSTFloat32LSB:		// IEEE 754 32 bit float, as found on Intel x86 architecture
		for (i = 0; i < nSize; i++, pInData += nAlign) {
			*out.f++ = (float)*pInData;
		}
		break;
	case ASIOSTFloat64LSB: 		// IEEE 754 64 bit double float, as found on Intel x86 architecture
		for (i = 0; i < nSize; i++, pInData += nAlign) {
			*out.d++ = *pInData;
		}
		break;
	case ASIOSTInt32LSB16:		// 32 bit data with 16 bit alignment
		for (i = 0; i < nSize; i++, pInData += nAlign) {
			*out.l++ = ConvertToInt(*pInData, 16);
		}
		break;
	case ASIOSTInt32LSB18:		// 32 bit data with 18 bit alignment
		for (i = 0; i < nSize; i++, pInData += nAlign) {
			*out.l++ = ConvertToInt(*pInData, 18);
		}
		break;
	case ASIOSTInt32LSB20:		// 32 bit data with 20 bit alignment
		for (i = 0; i < nSize; i++, pInData += nAlign) {
			*out.l++ = ConvertToInt(*pInData, 20);
		}
		break;
	case ASIOSTInt32LSB24:		// 32 bit data with 24 bit alignment
		for (i = 0; i < nSize; i++, pInData += nAlign) {
			*out.l++ = ConvertToInt(*pInData, 24);
		}
		break;
	case ASIOSTInt16MSB:
		for (i = 0; i < nSize; i++, pInData += nAlign) {
			*out.s++ = SwapShort((short)ConvertToInt(*pInData, 16));
		}
		break;
	case ASIOSTInt24MSB:		// used for 24 bits as well
		{
			int t;
			char* pt = (char*)&t;
			for(i = 0; i < nSize; i++, pInData += nAlign) {
				t = ConvertToInt(*pInData, 24);
				*out.c++ = pt[2];
				*out.c++ = pt[1];
				*out.c++ = pt[0];
			}
		}
		break;
	case ASIOSTInt32MSB:
		for(i = 0; i < nSize; i++, pInData += nAlign) {
			long t = ConvertToInt(*pInData, 32);
			*out.l++ = SwapLong(t);
		}
		break;
	case ASIOSTInt32MSB16:		// 32 bit data with 16 bit alignment
		for (i = 0; i < nSize; i++, pInData += nAlign) {
			DWORD t = (DWORD)ConvertToInt(*pInData, 16);
			*out.l++ = SwapLong(t);
		}
		break;
	case ASIOSTInt32MSB18:		// 32 bit data with 18 bit alignment
		for(i = 0; i < nSize; i++, pInData += nAlign) {
			DWORD t = (DWORD)ConvertToInt(*pInData, 18);
			*out.l++ = SwapLong(t);
		}
		break;
	case ASIOSTInt32MSB20:		// 32 bit data with 20 bit alignment
		for (i = 0; i < nSize; i++, pInData += nAlign) {
			DWORD t = (DWORD)ConvertToInt(*pInData, 20);
			*out.l++ = SwapLong(t);
		}
		break;
	case ASIOSTInt32MSB24:		// 32 bit data with 24 bit alignment
		for(i = 0; i < nSize; i++, pInData += nAlign) {
			DWORD t = (DWORD)ConvertToInt(*pInData, 24);
			*out.l++ = SwapLong(t);
		}
		break;
	case ASIOSTFloat32MSB:		// IEEE 754 32 bit float, as found on Intel x86 architecture
		memset (pOutData, 0, nSize * 4);
		out.c += nSize * 4;
		break;
	case ASIOSTFloat64MSB: 		// IEEE 754 64 bit double float, as found on Intel x86 architecture
		memset (pOutData, 0, nSize * 8);
		out.c += nSize * 8;
		break;
	}

	return out.v;
}

void *CAsioManager::ConverToDouble(int nType, int nSize, void *pInData, double *pOutData, int nAlign)
{
	int i;
	union {
		void *v;
		char *c;
		short *s;
		long *l;
		float *f;
		double *d;
	} in;

	in.v = pInData;

	switch (nType) {
	case ASIOSTInt16LSB:
		for(i = 0; i < nSize; i++, pOutData += nAlign) {
			*pOutData = (double)*in.s++ / PowDouble(16);
		}
		break;
	case ASIOSTInt24LSB:		// used for 20 bits as well
		{
			DWORD t;
			char *pt = (char *)&t;
			for (i = 0; i < nSize; i++, pOutData += nAlign) {
				pt[0] = 0;
				pt[1] = *in.c++;
				pt[2] = *in.c++;
				pt[3] = *in.c++;
				*pOutData = (double)t / PowDouble(32);
			}
		}
		break;
	case ASIOSTInt32LSB:
		for (i = 0; i < nSize; i++, pOutData += nAlign) {
			*pOutData = (double)*in.l++ / PowDouble(32);
		}
		break;
	case ASIOSTFloat32LSB:		// IEEE 754 32 bit float, as found on Intel x86 architecture
		for (i = 0; i < nSize; i++, pOutData += nAlign) {
			*pOutData = *in.f++;
		}
		break;
	case ASIOSTFloat64LSB: 		// IEEE 754 64 bit double float, as found on Intel x86 architecture
		for (i = 0; i < nSize; i++, pOutData += nAlign) {
			*pOutData = *in.d++;
		}
		break;
	case ASIOSTInt32LSB16:		// 32 bit data with 18 bit alignment
		for (i = 0; i < nSize; i++, pOutData += nAlign) {
			*pOutData = (double)*in.l++ / PowDouble(16);
		}
		break;
	case ASIOSTInt32LSB18:		// 32 bit data with 18 bit alignment
		for (i = 0; i < nSize; i++, pOutData += nAlign) {
			*pOutData = (double)*in.l++ / PowDouble(18);
		}
		break;
	case ASIOSTInt32LSB20:		// 32 bit data with 20 bit alignment
		for (i = 0; i < nSize; i++, pOutData += nAlign) {
			*pOutData = (double)*in.l++ / PowDouble(20);
		}
		break;
	case ASIOSTInt32LSB24:		// 32 bit data with 24 bit alignment
		for (i = 0; i < nSize; i++, pOutData += nAlign) {
			*pOutData = (double)*in.l++ / PowDouble(24);
		}
		break;
	case ASIOSTInt16MSB:
		for (i = 0; i < nSize; i++, pOutData += nAlign) {
			WORD t = *in.s++;
			*pOutData = (double)SwapShort(t) / PowDouble(16);
		}
		break;
	case ASIOSTInt24MSB:		// used for 20 bits as well
		{
			DWORD t;
			char* pt = (char*)&t;
			for(i = 0; i < nSize; i++, pOutData += nAlign) {
				pt[3] = *in.c++;
				pt[2] = *in.c++;
				pt[1] = *in.c++;
				pt[0] = 0;
				*pOutData = (double)t / PowDouble(32);
			}
		}
		break;
	case ASIOSTInt32MSB:
		for(i = 0; i < nSize; i++, pOutData += nAlign) {
			DWORD t = *in.l++;
			*pOutData = (double)SwapLong(t) / PowDouble(32);
		}
		break;
	case ASIOSTInt32MSB16:		// 32 bit data with 18 bit alignment
		for (i = 0; i < nSize; i++, pOutData += nAlign) {
			DWORD t = *in.l++;
			*pOutData = (double)SwapLong(t) / PowDouble(16);
		}
		break;
	case ASIOSTInt32MSB18:		// 32 bit data with 18 bit alignment
		for(i = 0; i < nSize; i++, pOutData += nAlign) {
			DWORD t = *in.l++;
			*pOutData = (double)SwapLong(t) / PowDouble(18);
		}
		break;
	case ASIOSTInt32MSB20:		// 32 bit data with 20 bit alignment
		for (i = 0; i < nSize; i++, pOutData += nAlign) {
			DWORD t = *in.l++;
			*pOutData = (double)SwapLong(t) / PowDouble(20);
		}
		break;
	case ASIOSTInt32MSB24:		// 32 bit data with 24 bit alignment
		for(i = 0; i < nSize; i++, pOutData += nAlign) {
			DWORD t = *in.l++;
			*pOutData = (double)SwapLong(t) / PowDouble(24);
		}
		break;
	case ASIOSTFloat32MSB:		// IEEE 754 32 bit float, as found on Intel x86 architecture
		memset(pOutData, 0, nSize * 4);
		in.c += nSize * 4;
		break;
	case ASIOSTFloat64MSB: 		// IEEE 754 64 bit double float, as found on Intel x86 architecture
		memset(pOutData, 0, nSize * 8);
		in.c += nSize * 8;
		break;
	}

	return in.v;
}
