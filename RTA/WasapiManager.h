#pragma once

#include <mmdeviceapi.h>
#include <Audioclient.h>
#include <FunctionDiscoveryKeys_devpkey.h>
#include <afxmt.h>
#include "WaveNotify.h"

typedef IMMDeviceEnumerator *PIMMDeviceEnumerator;
typedef IMMDeviceCollection *PIMMDeviceCollection;
typedef IMMDevice *PIMMDevice;
typedef IAudioClient *PIAudioClient;
typedef IAudioCaptureClient *PIAudioCaptureClient;
typedef IAudioRenderClient *PIAudioRenderClient;

class CWasapiManager : public CWnd, IMMNotificationClient
{
public:
	CWasapiManager(void);

	void Init();
	void Exit();
	void SetNorifyDeviceChange(HWND hWnd);
	void ResetInterface();

	BOOL GetDevicesIn(CStringArray &aDeviceNames);
	BOOL CanSamplingRateIn(INT_PTR nWaveDevice, int nSamplesPerSec);
	BOOL OpenIn(INT_PTR nWaveDevice, IWaveNotify *pWaveNotify, int nChannels, int nSamplesPerSec, int nSamplesPerBuffer, BOOL bErrMsg);
	void StartIn();
	void CloseIn();
	void ResetIn();
	void StopIn();
	int GetBitsPerSampleIn();
	BOOL IsOpenIn();

	BOOL GetDevicesOut(CStringArray &aDeviceNames);
	BOOL CanSamplingRateOut(INT_PTR nWaveDevice, int nSamplesPerSec);
	BOOL OpenOut(INT_PTR nWaveDevice, IWaveNotify *pWaveNotify, int nChannels, int nSamplesPerSec, int nSamplesPerBuffer, BOOL bErrMsg);
	void StartOut();
	void CloseOut();
	void ResetOut();
	void PauseOut();
	void RestartOut();
	int GetBitsPerSampleOut();
	BOOL IsOpenOut();

protected:
	PIMMDeviceEnumerator m_pDeviceEnumerator;
	PIMMDeviceCollection m_pDeviceCollectionIn;
	PIMMDeviceCollection m_pDeviceCollectionOut;
	PIMMDevice m_pDeviceIn;
	PIMMDevice m_pDeviceOut;
	PIAudioClient m_pAudioClientIn;
	PIAudioClient m_pAudioClientOut;
	PIAudioCaptureClient m_pCaptureClient;
	PIAudioRenderClient m_pRenderClient;

	HANDLE m_hEventIn;
	HANDLE m_hEventOut;
	HANDLE m_hEventExit;
	HANDLE m_hThread;
	IWaveNotify *m_pWaveInNotify;
	IWaveNotify *m_pWaveOutNotify;
	WAVEFORMATEXTENSIBLE m_oWaveFormatIn;
	WAVEFORMATEXTENSIBLE m_oWaveFormatOut;
	double *m_pSamplesBufferIn;
	double *m_pSamplesBufferOut;
	int m_nSamplesNumOut;
	int m_nChannelsIn;
	int m_nChannelsOut;
	HWND m_hNotifyDeviceChange;

	BOOL GetDevices(PIMMDeviceCollection pDeviceCollection, CStringArray &aDeviceNames);
	BOOL CanSamplingRate(INT_PTR nWaveDevice, int nSamplesPerSec, PIMMDeviceCollection &pDeviceCollection);
	HRESULT OpenSub(PIMMDeviceCollection &pDeviceCollection, PIMMDevice &pDevice, PIAudioClient &pAudioClient, LPVOID *pClient, IID iid, HANDLE &hEvent, INT_PTR nWaveDevice, int nBitsPerSample, int nSamplesPerSec, int nSamplesPerBuffer, WAVEFORMATEXTENSIBLE &oWaveFormat);
	int NotifyMessageIn(int nCode);
	int NotifyMessageOut(int nCode);
	static DWORD CALLBACK AudioThreadEntry(LPVOID param);
	void AudioThread();
	void ReadWaveData();
	void WriteWaveData();
	CString HrToStr(HRESULT hr);
	void ConvertDoubleToWave(LPVOID pWaveData, int nSize);
	void ConvertWaveToDouble(LPVOID pWaveData, int nSize);

	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID* object);
	HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR pwstrDefaultDeviceId);
	HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR pwstrDeviceId);
	HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR pwstrDeviceId);
	HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR pwstrDeviceId, DWORD dwNewState);
	HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(LPCWSTR pwstrDeviceId, const PROPERTYKEY key);

	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnWasapiNotifyIn(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWasapiNotifyOut(WPARAM wParam, LPARAM lParam);
};

extern CWasapiManager g_cWasapiManager;
