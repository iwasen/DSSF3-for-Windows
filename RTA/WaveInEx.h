#pragma once

#include "WaveIn.h"
#include "WaveEx.h"
#include "AutoRec.h"

class CWaveInEx : public CWaveEx, public IWaveNotify
{
public:
	CWaveInEx();
	~CWaveInEx();

	static void Init();
	static void Exit();
	static CWaveInEx *GetInstance();
	void ReleaseInstance();
	BOOL Open(INT_PTR nWaveDevice, IWaveNotify *pWaveNotify, int nChannels, int nSamplesPerSec, int nSamplesPerBuffer, int nBufferNum, BOOL bErrMsg = TRUE);
	void Close();
	static void SetMonitor(IWaveNotify *pMonitorWnd);
	static BOOL SetRecMode(IWaveNotify *pRecWnd, int nRecMode);
	static void CloseAll();
	static CWaveInEx *GetRecWave();
	static void CloseRecWave();
	static int GetFixedSamplingRate();
	void Start();
	void Stop();
	void Reset();
	int GetSamplesPerSec();
	int GetChannels();
	int GetBitsPerSample();
	void CreateWasapiSaveFile(int nChannels, int nSamplesPerSec, int nBitsPerSample);
	void WriteWasapiSaveFile(LPWAVENOTIFY pWaveNotify);
	void CloseWasapiSaveFile();

protected:
	CWaveIn m_oWaveIn;
	static CObList s_WaveInList;
	static IWaveNotify *s_pMonitorWnd;
	static IWaveNotify *s_pRecWnd;
	static int s_nRecMode;
	int m_nRecMode;
	IWaveNotify *m_pWaveNotify;
	IWaveNotify *m_pMonitorWnd;
	IWaveNotify *m_pRecWnd;
	CWaveOut m_cWaveOut;
	INT_PTR m_nWaveDevice;
	int m_nChannels;
	int m_nSamplesPerSec;
	BOOL m_bErrMsg;
	CAutoRec m_oAutoRec;
	CWaveFile *m_pWaveFile16;
	CWaveFile *m_pWaveFile24;
	int m_nWaveFileCounter;
	BOOL m_bWaveRecordEnd;

	BOOL OpenDevice(INT_PTR nWaveDevice, IWaveNotify *pWaveNotify, int nChannels, int nSamplesPerSec, int nSamplesPerBuffer, int nBufferNum, BOOL bErrMsg);
	static CWaveInEx *FindOpen();
	static CWaveInEx *FindMonitor();
	static CWaveInEx *FindRec();
	BOOL SetRecMode2(IWaveNotify *pRecWnd, int nRecMode);
	void CloseRecMonitor();
	int OnWaveNotify(int nCode, LPWAVENOTIFY pWaveNotify);
	int WaveNotify(IWaveNotify *pWaveNotifyWnd, int nCode, LPWAVENOTIFY pWaveNotify);
};
