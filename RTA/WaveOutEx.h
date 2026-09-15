#pragma once

#include "WaveOut.h"
#include "WaveEx.h"
#include "AutoRec.h"

class CWaveOutEx : public CWaveEx, public IWaveNotify
{
public:
	CWaveOutEx();

	static void Init();
	static void Exit();
	static CWaveOutEx *GetInstance();
	void ReleaseInstance();
	static void SetMonitor(IWaveNotify *pMonitorWnd);
	static CWaveOutEx *SetRecMode(IWaveNotify *pRecWnd);
	static void CloseAll();
	static CWaveOutEx *GetRecWave();
	BOOL Open(INT_PTR nWaveDevice, IWaveNotify *pWaveNotify, int nChannels, int nSamplesPerSec, int nSamplesPerBuffer, int nBufferNum, BOOL bErrMsg = TRUE, BOOL bVolumeSet = TRUE);
	void Start();
	void Close();
	void Reset();
	void Pause();
	void Restart();
	void SetVolume(UINT left, UINT right);
	int GetSamplesPerSec();
	int GetChannels();
	int GetBitsPerSample();

protected:
	CWaveOut m_oWaveOut;
	static CWaveOutEx *s_pWaveOutEx;
	static IWaveNotify *s_pRecWnd;
	IWaveNotify *m_pWaveNotify;
	IWaveNotify *m_pMonitorWnd;
	IWaveNotify *m_pRecWnd;
	INT_PTR m_nWaveDevice;
	int m_nChannels;
	int m_nSamplesPerSec;
	int m_nBitsPerSample;
	CAutoRec m_oAutoRec;

	BOOL SetRecMode2(IWaveNotify *pRecWnd);
	int OnWaveNotify(int nCode, LPWAVENOTIFY pWaveNotify);
};
