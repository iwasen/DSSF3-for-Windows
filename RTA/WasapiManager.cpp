#include "StdAfx.h"
#include "WasapiManager.h"
#include "Common.h"
#include "Global.h"

#define AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED AUDCLNT_ERR(0x019)

#define SAFE_RELEASE(x)			{ if (x) { x->Release(); x = NULL; } }
#define SAFE_CLOSE_HANDLE(x)	{ if (x) { ::CloseHandle(x); x = NULL; } }
#define SAFE_DELETE(x)			{ if (x) { delete x; x = NULL; } }
#define SAFE_DELETE_ARRAY(x)	{ if (x) { delete[] x; x = NULL; } }

// インターフェースのGUIDの実体
const CLSID CLSID_MMDeviceEnumerator	= __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator		= __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient				= __uuidof(IAudioClient);
const IID IID_IAudioCaptureClient		= __uuidof(IAudioCaptureClient);
const IID IID_IAudioRenderClient		= __uuidof(IAudioRenderClient);

// CAsioManager
CWasapiManager g_cWasapiManager;

BEGIN_MESSAGE_MAP(CWasapiManager, CWnd)
	ON_MESSAGE(WM_WASAPI_NOTIFY_IN, OnWasapiNotifyIn)
	ON_MESSAGE(WM_WASAPI_NOTIFY_OUT, OnWasapiNotifyOut)
END_MESSAGE_MAP()

CWasapiManager::CWasapiManager(void)
{
	m_pDeviceEnumerator = NULL;
	m_pDeviceCollectionIn = NULL;
	m_pDeviceCollectionOut = NULL;
	m_pDeviceIn = NULL;
	m_pDeviceOut = NULL;
	m_pAudioClientIn = NULL;
	m_pAudioClientOut = NULL;
	m_pCaptureClient = NULL;
	m_pRenderClient = NULL;

	m_hEventIn = NULL;
	m_hEventOut = NULL;
	m_hEventExit = NULL;
	m_hThread = NULL;

	m_pWaveInNotify = NULL;
	m_pWaveOutNotify = NULL;
	m_pSamplesBufferIn = NULL;
	m_pSamplesBufferOut = NULL;

	m_hNotifyDeviceChange = NULL;
}

void CWasapiManager::Init(void)
{
	HRESULT ret = ::CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&m_pDeviceEnumerator);
	if (FAILED(ret))
		return;

	HRESULT retIn = m_pDeviceEnumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &m_pDeviceCollectionIn);
	HRESULT retOut = m_pDeviceEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &m_pDeviceCollectionOut);
	if (FAILED(retIn) && FAILED(retOut))
		return;

	CreateEx(0, ::AfxRegisterWndClass(0, 0, 0, 0), "WasapiManager", WS_POPUP, 0, 0, 0, 0, HWND_MESSAGE, 0);

	m_hEventIn = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hEventOut = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hEventExit = ::CreateEvent(NULL, FALSE, FALSE, NULL);

	DWORD dwThread;
	m_hThread = ::CreateThread(NULL, 0, AudioThreadEntry, this, 0, &dwThread);

	m_pDeviceEnumerator->RegisterEndpointNotificationCallback(this);
}

void CWasapiManager::Exit(void)
{
	// スレッド終了処理
	if (m_hThread) {
		if (m_hEventExit) {
			::SetEvent(m_hEventExit);

			// スレッドが完全に終了するまで待機
			::WaitForSingleObject(m_hThread, INFINITE);
			SAFE_CLOSE_HANDLE(m_hThread);
		}
	}

	if (m_pDeviceEnumerator != NULL)
		m_pDeviceEnumerator->UnregisterEndpointNotificationCallback(this);

	// イベントを開放処理
	SAFE_CLOSE_HANDLE(m_hEventIn);
	SAFE_CLOSE_HANDLE(m_hEventOut);
	SAFE_CLOSE_HANDLE(m_hEventExit);

	// インターフェース開放
	SAFE_RELEASE(m_pCaptureClient);
	SAFE_RELEASE(m_pAudioClientIn);
	SAFE_RELEASE(m_pDeviceIn);
	SAFE_RELEASE(m_pDeviceCollectionIn);
	SAFE_RELEASE(m_pRenderClient);
	SAFE_RELEASE(m_pAudioClientOut);
	SAFE_RELEASE(m_pDeviceOut);
	SAFE_RELEASE(m_pDeviceCollectionOut);
	SAFE_RELEASE(m_pDeviceEnumerator);

	DestroyWindow();
}

void CWasapiManager::SetNorifyDeviceChange(HWND hWnd)
{
	m_hNotifyDeviceChange = hWnd;
}

void CWasapiManager::ResetInterface()
{
	if (m_pDeviceEnumerator != NULL)
		m_pDeviceEnumerator->UnregisterEndpointNotificationCallback(this);

	SAFE_RELEASE(m_pDeviceCollectionIn);
	SAFE_RELEASE(m_pDeviceCollectionOut);
	SAFE_RELEASE(m_pDeviceEnumerator);

	HRESULT ret = ::CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&m_pDeviceEnumerator);
	if (FAILED(ret))
		return;

	HRESULT retIn = m_pDeviceEnumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &m_pDeviceCollectionIn);
	HRESULT retOut = m_pDeviceEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &m_pDeviceCollectionOut);
	if (FAILED(retIn) && FAILED(retOut))
		return;

	m_pDeviceEnumerator->RegisterEndpointNotificationCallback(this);
}

BOOL CWasapiManager::GetDevicesIn(CStringArray &aDeviceNames)
{
	return GetDevices(m_pDeviceCollectionIn, aDeviceNames);
}

BOOL CWasapiManager::GetDevices(PIMMDeviceCollection pDeviceCollection, CStringArray &aDeviceNames)
{
	HRESULT ret;

	aDeviceNames.RemoveAll();

	if (pDeviceCollection == NULL)
		return FALSE;

	UINT nDeviceCount;
	ret = pDeviceCollection->GetCount(&nDeviceCount);
	if (FAILED(ret)) {
		return FALSE;
	}

	for (UINT i = 0; i < nDeviceCount; i++) {
		IMMDevice *pDevice;
		ret = pDeviceCollection->Item(i, &pDevice);
		if (FAILED(ret)) {
			return FALSE;
		}

		IPropertyStore *pPropertyStore;
		ret = pDevice->OpenPropertyStore(STGM_READ, &pPropertyStore);
		if (FAILED(ret)) {
			pDevice->Release();
			return FALSE;
		}

		PROPVARIANT friendlyName;
		::PropVariantInit(&friendlyName);
		ret = pPropertyStore->GetValue(PKEY_Device_FriendlyName, &friendlyName);
		if (FAILED(ret)) {
			::PropVariantClear(&friendlyName);
			pPropertyStore->Release();
			pDevice->Release();
			return FALSE;
		}

		aDeviceNames.Add(friendlyName.bstrVal);

		::PropVariantClear(&friendlyName);
		pPropertyStore->Release();
		pDevice->Release();
	}

	return TRUE;
}

BOOL CWasapiManager::CanSamplingRateIn(INT_PTR nWaveDevice, int nSamplesPerSec)
{
	return CanSamplingRate(nWaveDevice, nSamplesPerSec, m_pDeviceCollectionIn);
}

BOOL CWasapiManager::CanSamplingRate(INT_PTR nWaveDevice, int nSamplesPerSec, PIMMDeviceCollection &pDeviceCollection)
{
	HRESULT ret;
	PIMMDevice pDevice = NULL;
	PIAudioClient pAudioClient = NULL;
	WAVEFORMATEXTENSIBLE oWaveFormat;

	ret = pDeviceCollection->Item((UINT)nWaveDevice, &pDevice);
	if (FAILED(ret))
		return FALSE;

	// フォーマットの構築
	ZeroMemory(&oWaveFormat, sizeof(oWaveFormat));
	oWaveFormat.Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE);
	oWaveFormat.Format.wFormatTag			= WAVE_FORMAT_EXTENSIBLE;
	oWaveFormat.Format.nChannels			= 2;
	oWaveFormat.Format.nSamplesPerSec		= nSamplesPerSec;
	oWaveFormat.Format.wBitsPerSample		= 16;
	oWaveFormat.Format.nBlockAlign		= oWaveFormat.Format.nChannels * oWaveFormat.Format.wBitsPerSample / 8;
	oWaveFormat.Format.nAvgBytesPerSec	= oWaveFormat.Format.nSamplesPerSec * oWaveFormat.Format.nBlockAlign;
	oWaveFormat.Samples.wValidBitsPerSample	= oWaveFormat.Format.wBitsPerSample;
	oWaveFormat.dwChannelMask				= SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT;
	oWaveFormat.SubFormat					= KSDATAFORMAT_SUBTYPE_PCM;

	// オーディオクライアント
	ret = pDevice->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (void **)&pAudioClient);
	if (FAILED(ret)) {
		SAFE_RELEASE(pDevice);
		return FALSE;
	}

	// フォーマットのサポートチェック
	ret = pAudioClient->IsFormatSupported(AUDCLNT_SHAREMODE_EXCLUSIVE, (WAVEFORMATEX*)&oWaveFormat, NULL);

	SAFE_RELEASE(pDevice);
	SAFE_RELEASE(pAudioClient);

	return !FAILED(ret);
}

BOOL CWasapiManager::OpenIn(INT_PTR nWaveDevice, IWaveNotify *pWaveNotify, int nChannels, int nSamplesPerSec, int nSamplesPerBuffer, BOOL bErrMsg)
{
	CloseIn();

	m_pWaveInNotify = pWaveNotify;
	m_nChannelsIn = nChannels;

	int nBitsPerSample = g_oSetData2.bInput16BitOnly ? 16 : 24;
	HRESULT ret = OpenSub(m_pDeviceCollectionIn, m_pDeviceIn, m_pAudioClientIn, (LPVOID *)&m_pCaptureClient, IID_IAudioCaptureClient, m_hEventIn, nWaveDevice, nBitsPerSample, nSamplesPerSec, nSamplesPerBuffer, m_oWaveFormatIn);
	if (FAILED(ret)) {
		SAFE_RELEASE(m_pCaptureClient);
		SAFE_RELEASE(m_pAudioClientIn);
		SAFE_RELEASE(m_pDeviceIn);

		if (bErrMsg) {
			CString sMessage = HrToStr(ret);
			::AfxMessageBox(sMessage, NULL, MB_ICONEXCLAMATION);
		}

		return FALSE;
	}

	m_pSamplesBufferIn = new double[nSamplesPerBuffer * nChannels];

	NotifyMessageIn(WAVEIN_OPEN);

	return TRUE;
}

void CWasapiManager::StartIn()
{
	if (m_pAudioClientIn) {
		m_pAudioClientIn->Start();
	}
}

void CWasapiManager::CloseIn()
{
	if (m_pAudioClientIn) {
		m_pAudioClientIn->Stop();

		SAFE_RELEASE(m_pCaptureClient);
		SAFE_RELEASE(m_pAudioClientIn);
		SAFE_RELEASE(m_pDeviceIn);

		NotifyMessageIn(WAVEIN_CLOSE);
	}

	SAFE_DELETE_ARRAY(m_pSamplesBufferIn);
}

void CWasapiManager::ResetIn()
{
}

void CWasapiManager::StopIn()
{
}

int CWasapiManager::GetBitsPerSampleIn() const
{
	return m_oWaveFormatIn.Samples.wValidBitsPerSample;
}

BOOL CWasapiManager::IsOpenIn() const
{
	return m_pAudioClientIn != NULL;
}

int CWasapiManager::NotifyMessageIn(int nCode)
{
	LONG nRc = 0;

	if (m_pWaveInNotify != NULL) {
		WAVENOTIFY waveNotify{};

		if (nCode == WAVEIN_DATA) {
			UINT frame;
			m_pAudioClientIn->GetBufferSize(&frame);
			waveNotify.pSamplesData = m_pSamplesBufferIn;
			waveNotify.nSamplesNum = frame;
			waveNotify.nSamplesRecorded = frame;
			waveNotify.nFlags = WHDR_DONE;
		}
		waveNotify.nChannels = m_nChannelsIn;
		waveNotify.nSamplesPerSec = m_oWaveFormatIn.Format.nSamplesPerSec;

		nRc = m_pWaveInNotify->OnWaveNotify(nCode, &waveNotify);
	}

	return nRc;
}

BOOL CWasapiManager::GetDevicesOut(CStringArray &aDeviceNames)
{
	return GetDevices(m_pDeviceCollectionOut, aDeviceNames);
}

BOOL CWasapiManager::CanSamplingRateOut(INT_PTR nWaveDevice, int nSamplesPerSec)
{
	return CanSamplingRate(nWaveDevice, nSamplesPerSec, m_pDeviceCollectionOut);
}

BOOL CWasapiManager::OpenOut(INT_PTR nWaveDevice, IWaveNotify *pWaveNotify, int nChannels, int nSamplesPerSec, int nSamplesPerBuffer, BOOL bErrMsg)
{
	CloseOut();

	m_pWaveOutNotify = pWaveNotify;
	m_nChannelsOut = nChannels;

	int nBitsPerSample = g_oSetData2.bOutput16BitOnly ? 16 : 24;
	HRESULT ret = OpenSub(m_pDeviceCollectionOut, m_pDeviceOut, m_pAudioClientOut, (LPVOID *)&m_pRenderClient, IID_IAudioRenderClient, m_hEventOut, nWaveDevice, nBitsPerSample, nSamplesPerSec, nSamplesPerBuffer, m_oWaveFormatOut);
	if (FAILED(ret)) {
		SAFE_RELEASE(m_pRenderClient);
		SAFE_RELEASE(m_pAudioClientOut);
		SAFE_RELEASE(m_pDeviceOut);

		if (bErrMsg) {
			CString sMessage = HrToStr(ret);
			::AfxMessageBox(sMessage, NULL, MB_ICONEXCLAMATION);
		}

		return FALSE;
	}

	m_pSamplesBufferOut = new double[nSamplesPerBuffer * nChannels];
	m_nSamplesNumOut = 0;

	NotifyMessageOut(WAVEOUT_OPEN);

	return TRUE;
}

HRESULT CWasapiManager::OpenSub(PIMMDeviceCollection &pDeviceCollection, PIMMDevice &pDevice, PIAudioClient &pAudioClient, LPVOID *pClient, IID iid, HANDLE &hEvent, INT_PTR nWaveDevice, int nBitsPerSample, int nSamplesPerSec, int nSamplesPerBuffer, WAVEFORMATEXTENSIBLE &oWaveFormat)
{
	HRESULT ret;

	ret = pDeviceCollection->Item((UINT)nWaveDevice, &pDevice);
	if (FAILED(ret))
		return ret;

	// フォーマットの構築
	ZeroMemory(&oWaveFormat, sizeof(oWaveFormat));
	oWaveFormat.Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE);
	oWaveFormat.Format.wFormatTag			= WAVE_FORMAT_EXTENSIBLE;
	oWaveFormat.Format.nChannels			= 2;
	oWaveFormat.Format.nSamplesPerSec		= nSamplesPerSec;
	oWaveFormat.Format.wBitsPerSample		= (WORD)nBitsPerSample;
	oWaveFormat.Format.nBlockAlign			= oWaveFormat.Format.nChannels * oWaveFormat.Format.wBitsPerSample / 8;
	oWaveFormat.Format.nAvgBytesPerSec		= oWaveFormat.Format.nSamplesPerSec * oWaveFormat.Format.nBlockAlign;
	oWaveFormat.Samples.wValidBitsPerSample	= oWaveFormat.Format.wBitsPerSample;
	oWaveFormat.dwChannelMask				= SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT;
	oWaveFormat.SubFormat					= KSDATAFORMAT_SUBTYPE_PCM;

	int nRetry;
	REFERENCE_TIME nDevicePeriod = nSamplesPerBuffer * 10000000LL / nSamplesPerSec;
	for (nRetry = 0; nRetry < 5; nRetry++) {
		// オーディオクライアント
		ret = pDevice->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (void **)&pAudioClient);
		if (FAILED(ret))
			return ret;

		// フォーマットのサポートチェック
		ret = pAudioClient->IsFormatSupported(AUDCLNT_SHAREMODE_EXCLUSIVE, (WAVEFORMATEX*)&oWaveFormat, NULL);
		if (FAILED(ret)) {
			if (nBitsPerSample == 24) {
				if (ret == AUDCLNT_E_UNSUPPORTED_FORMAT) {
					oWaveFormat.Format.wBitsPerSample	= 32;
					oWaveFormat.Format.nBlockAlign		= oWaveFormat.Format.nChannels * oWaveFormat.Format.wBitsPerSample / 8;
					oWaveFormat.Format.nAvgBytesPerSec	= oWaveFormat.Format.nSamplesPerSec * oWaveFormat.Format.nBlockAlign;
					ret = pAudioClient->IsFormatSupported(AUDCLNT_SHAREMODE_EXCLUSIVE, (WAVEFORMATEX*)&oWaveFormat, NULL);
					if (FAILED(ret)) {
						if (ret == AUDCLNT_E_UNSUPPORTED_FORMAT) {
							oWaveFormat.Format.wBitsPerSample		= 16;
							oWaveFormat.Format.nBlockAlign			= oWaveFormat.Format.nChannels * oWaveFormat.Format.wBitsPerSample / 8;
							oWaveFormat.Format.nAvgBytesPerSec		= oWaveFormat.Format.nSamplesPerSec * oWaveFormat.Format.nBlockAlign;
							oWaveFormat.Samples.wValidBitsPerSample	= oWaveFormat.Format.wBitsPerSample;
							ret = pAudioClient->IsFormatSupported(AUDCLNT_SHAREMODE_EXCLUSIVE, (WAVEFORMATEX*)&oWaveFormat, NULL);
							if (FAILED(ret))
								return ret;
						} else
							return ret;
					}
				} else
					return ret;
			} else
				return ret;
		}

		ret = pAudioClient->Initialize(AUDCLNT_SHAREMODE_EXCLUSIVE,
										AUDCLNT_STREAMFLAGS_NOPERSIST | AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
										nDevicePeriod,
										nDevicePeriod,
										(WAVEFORMATEX*)&oWaveFormat,
										NULL);
		if (FAILED(ret))
			return ret;

		UINT32 nBufferFrames = 0;
		ret = pAudioClient->GetBufferSize(&nBufferFrames);
		if (FAILED(ret))
			return ret;

		if (nBufferFrames < (UINT)nSamplesPerBuffer)
			nDevicePeriod += 10000000LL / nSamplesPerSec;
		else if (nBufferFrames > (UINT)nSamplesPerBuffer)
			nDevicePeriod -= 10000000LL / nSamplesPerSec;
		else
			break;

		SAFE_RELEASE(pAudioClient);
	}
	if (nRetry == 5)
		return ret;

	ret = pAudioClient->SetEventHandle(hEvent);
	if (FAILED(ret))
		return ret;

	ret = pAudioClient->GetService(iid, pClient);
	if (FAILED(ret))
		return ret;

	return S_OK;
}

void CWasapiManager::StartOut()
{
	if (m_pAudioClientOut) {
		OnWasapiNotifyOut(0, 0);

		m_pAudioClientOut->Start();
	}
}

void CWasapiManager::CloseOut()
{
	if (m_pAudioClientOut) {
		m_pAudioClientOut->Stop();

		SAFE_RELEASE(m_pRenderClient);
		SAFE_RELEASE(m_pAudioClientOut);
		SAFE_RELEASE(m_pDeviceOut);

		NotifyMessageOut(WAVEOUT_CLOSE);
	}

	SAFE_DELETE_ARRAY(m_pSamplesBufferOut);
}

void CWasapiManager::ResetOut()
{
}

void CWasapiManager::PauseOut()
{
}

void CWasapiManager::RestartOut()
{
}

int CWasapiManager::GetBitsPerSampleOut() const
{
	return m_oWaveFormatOut.Samples.wValidBitsPerSample;
}

BOOL CWasapiManager::IsOpenOut() const
{
	return m_pAudioClientOut != NULL;
}

int CWasapiManager::NotifyMessageOut(int nCode)
{
	LONG nRc = 0;

	if (m_pWaveOutNotify != NULL) {
		WAVENOTIFY waveNotify{};

		if (nCode == WAVEOUT_DATA) {
			UINT frame;
			m_pAudioClientOut->GetBufferSize(&frame);
			waveNotify.pSamplesData = m_pSamplesBufferOut;
			waveNotify.nSamplesNum = frame;
			waveNotify.nSamplesRecorded = frame;
			waveNotify.nFlags = WHDR_DONE;
		}
		waveNotify.nChannels = m_nChannelsOut;
		waveNotify.nSamplesPerSec = m_oWaveFormatOut.Format.nSamplesPerSec;
		nRc = m_pWaveOutNotify->OnWaveNotify(nCode, &waveNotify);
	}

	return nRc;
}

DWORD CALLBACK CWasapiManager::AudioThreadEntry(LPVOID param)
{
	CWasapiManager *pWasapiManager = dynamic_cast<CWasapiManager*>(reinterpret_cast<CWnd*>(param));
	if (pWasapiManager != NULL)
		pWasapiManager->AudioThread();

	return 0;
}

void CWasapiManager::AudioThread()
{
	HANDLE aWaitEvents[3] = {m_hEventIn, m_hEventOut, m_hEventExit};

	BOOL bLoop = TRUE;
	while (bLoop) {
		DWORD dwResult = WaitForMultipleObjects(3, aWaitEvents, FALSE, INFINITE);
		switch (dwResult) {
		case WAIT_OBJECT_0 + 0:	// In
			PostMessage(WM_WASAPI_NOTIFY_IN);
			break;
		case WAIT_OBJECT_0 + 1:	// Out
			PostMessage(WM_WASAPI_NOTIFY_OUT);
			break;
		case WAIT_OBJECT_0 + 2:	// Exit
			bLoop = FALSE;
			break;
		}
	}
}

LRESULT CWasapiManager::OnWasapiNotifyIn(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	if (m_pAudioClientIn != NULL && m_pWaveInNotify != NULL) {
		ReadWaveData();

		BOOL bValid = NotifyMessageIn(WAVEIN_DATA);
		if (!bValid)
			CloseIn();
	}

	return 0;
}

LRESULT CWasapiManager::OnWasapiNotifyOut(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	if (m_pAudioClientOut != NULL) {
		m_nSamplesNumOut = NotifyMessageOut(WAVEOUT_DATA);

		if (m_nSamplesNumOut != 0)
			WriteWaveData();
		else
			CloseOut();
	}

	return 0;
}

void CWasapiManager::ReadWaveData()
{
	if (m_pCaptureClient != NULL) {
		HRESULT ret;
		LPBYTE pBuffer;
		UINT32 framesAvailable;
		DWORD  flags;
		ret = m_pCaptureClient->GetBuffer(&pBuffer, &framesAvailable, &flags, NULL, NULL);
		if (FAILED(ret))
			return;

		ConvertWaveToDouble(pBuffer, framesAvailable);

		ret = m_pCaptureClient->ReleaseBuffer(framesAvailable);
		if (FAILED(ret))
			return;
	}
}

void CWasapiManager::WriteWaveData()
{
	if (m_pRenderClient != NULL) {
		HRESULT ret;

		UINT32 nFrameCount;
		ret = m_pAudioClientOut->GetBufferSize(&nFrameCount);
		if (FAILED(ret))
			return;

		LPBYTE pBuffer;
		ret = m_pRenderClient->GetBuffer(nFrameCount, &pBuffer);
		if (FAILED(ret))
			return;

		if ((UINT32)m_nSamplesNumOut < nFrameCount)
			memset(pBuffer, 0, nFrameCount * m_oWaveFormatOut.Format.nBlockAlign);

		ConvertDoubleToWave(pBuffer, m_nSamplesNumOut);

		ret = m_pRenderClient->ReleaseBuffer(nFrameCount, 0);
		if (FAILED(ret))
			return;

		m_nSamplesNumOut = 0;
	}
}

CString CWasapiManager::HrToStr(HRESULT hr)
{
    LPVOID string;
	CString sMessage;

    ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                  NULL,
                  hr,
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  (LPTSTR)&string,
                  0,
                  NULL);
    if (string != NULL) {
        sMessage = (LPCSTR)string;
	    ::LocalFree(string);
	} else
		sMessage.Format("Error Code: 0x%08x", hr);

	return sMessage;
}

void CWasapiManager::ConvertDoubleToWave(LPVOID pWaveData, int nSize)
{
	double *pSamplesData = m_pSamplesBufferOut;
	int nDataNum = nSize * m_nChannelsOut;
	BYTE *bp;
	short *sp;
	long *lp;
	int i;
	union {
		int i;
		BYTE b[4];
	} ib;

	switch (m_oWaveFormatOut.Format.wBitsPerSample) {
	case 16:
		sp = (short *)pWaveData;
		for (i = 0; i < nDataNum; i++) {
			short s = (short)ConvertToInt(*pSamplesData++, 16);
			*sp++ = s;
			if (m_nChannelsOut == 1)
				*sp++ = s;
		}
		break;
	case 24:
		bp = (BYTE *)pWaveData;
		for (i = 0; i < nDataNum; i++) {
			ib.i = ConvertToInt(*pSamplesData++, 24);
			*bp++ = ib.b[0];
			*bp++ = ib.b[1];
			*bp++ = ib.b[2];
			if (m_nChannelsOut == 1) {
				*bp++ = ib.b[0];
				*bp++ = ib.b[1];
				*bp++ = ib.b[2];
			}
		}
		break;
	case 32:
		lp = (long *)pWaveData;
		long mask = 0xffffffff << (32 - m_oWaveFormatOut.Samples.wValidBitsPerSample);
		for (i = 0; i < nDataNum; i++) {
			long l = ConvertToInt(*pSamplesData++, 32) & mask;
			*lp++ = l;
			if (m_nChannelsOut == 1)
				*lp++ = l;
		}
		break;
	}
}

void CWasapiManager::ConvertWaveToDouble(LPVOID pWaveData, int nSize)
{
	double *pSamplesData = m_pSamplesBufferIn;
	int nDataNum = nSize * m_nChannelsIn;
	BYTE *bp;
	short *sp;
	long *lp;
	int i;
	union {
		int i;
		BYTE b[4];
	} ib;
	double div;

	switch (m_oWaveFormatIn.Format.wBitsPerSample) {
	case 16:
		div = PowDouble(16);
		sp = (short *)pWaveData;
		for (i = 0; i < nDataNum; i++) {
			*pSamplesData++ = *sp++ / div;
			if (m_nChannelsIn == 1)
				sp++;
		}
		break;
	case 24:
		div = PowDouble(32);
		bp = (BYTE *)pWaveData;
		ib.b[0] = 0;
		for (i = 0; i < nDataNum; i++) {
			ib.b[1] = *bp++;
			ib.b[2] = *bp++;
			ib.b[3] = *bp++;
			*pSamplesData++ = ib.i / div;
			if (m_nChannelsIn == 1)
				bp += 3;
		}
		break;
	case 32:
		div = PowDouble(32);
		lp = (long *)pWaveData;
		long mask = 0xffffffff << (32 - m_oWaveFormatIn.Samples.wValidBitsPerSample);
		for (i = 0; i < nDataNum; i++) {
			*pSamplesData++ = (*lp++ & mask) / div;
			if (m_nChannelsIn == 1)
				lp++;
		}
		break;
	}
}
ULONG STDMETHODCALLTYPE CWasapiManager::AddRef()
{
	return 1;
}

ULONG STDMETHODCALLTYPE CWasapiManager::Release()
{
	return 1;
}

HRESULT STDMETHODCALLTYPE CWasapiManager::QueryInterface(REFIID /*iid*/, LPVOID* /*object*/)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CWasapiManager::OnDefaultDeviceChanged(
  EDataFlow /*flow*/,
  ERole     /*role*/,
  LPCWSTR   /*pwstrDefaultDeviceId*/
)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CWasapiManager::OnDeviceAdded(
  LPCWSTR /*pwstrDeviceId*/
)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CWasapiManager::OnDeviceRemoved(
  LPCWSTR /*pwstrDeviceId*/
)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CWasapiManager::OnDeviceStateChanged(
  LPCWSTR /*pwstrDeviceId*/,
  DWORD   /*dwNewState*/
)
{
	if (m_hNotifyDeviceChange != NULL)
		::PostMessage(m_hNotifyDeviceChange, WM_NOTIFY_DEVICE_CHANGE, 0, 0);

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CWasapiManager::OnPropertyValueChanged(
  LPCWSTR /*pwstrDeviceId*/,
  const PROPERTYKEY /*key*/
)
{
	return S_OK;
}
