#pragma once

#include "AsioDriver.h"
#include "WaveNotify.h"

// CAsioManager

class CAsioManager : public CWnd
{
public:
	CAsioManager();

	BOOL m_bInOpen;
	BOOL m_bOutOpen;

	void Init(void);
	void Exit(void);

	BOOL CanSamplingRate(INT_PTR nWaveDevice, int nSamplingRate);

	BOOL OpenIn(INT_PTR nWaveDevice, IWaveNotify *pWaveNotify, int nChannels, int nSamplesPerSec, int nSamplesPerBuffer, BOOL bErrMsg);
	void StartIn();
	void CloseIn();
	void ResetIn();
	void StopIn();
	int GetBitsPerSampleIn();

	BOOL OpenOut(INT_PTR nWaveDevice, IWaveNotify *pWaveNotify, int nChannels, int nSamplesPerSec, int nSamplesPerBuffer, BOOL bErrMsg);
	void StartOut();
	void CloseOut();
	void ResetOut();
	void PauseOut();
	void RestartOut();
	int GetBitsPerSampleOut();

protected:
	CAsioDriver m_cAsioDriver;
	ASIOBufferInfo m_asioBufferInfo[4];
	ASIOChannelInfo m_asioChannelInfo[4];
	IWaveNotify *m_pWaveInNotify;
	IWaveNotify *m_pWaveOutNotify;
	INT_PTR m_nCurrentDevice;
	int m_nInChannels;
	int m_nOutChannels;
	int m_nSamplesPerSec;
	int m_nWaveInBufSize;
	int m_nWaveOutBufSize;
	double *m_pWaveInBuf[2];
	double *m_pWaveOutBuf[2];
	long m_nMinSize;
	long m_nMaxSize;
	long m_nPreferredSize;
	long m_nGranularity;
	long m_nBufSize;
	int m_nInBufIndex;
	int m_nInBufCount;
	int m_nInBufWait;
	int m_nOutBufIndex;
	int m_nOutBufCount;
	int m_nWaveOutCount;
	int m_nOutBufSize;
	int m_nCloseOutWait;
	ASIOCallbacks m_asioCallbacks;
	WAVENOTIFY m_cWaveInNotify;
	WAVENOTIFY m_cWaveOutNotify;
	BOOL m_bAsioStart;
	BOOL m_bInStart;
	BOOL m_bOutStart;
	BOOL m_bInNotify;
	BOOL m_bOutNotify;

	BOOL LoadAsioDriver(INT_PTR nWaveDevice, BOOL bErrMsg);
	void UnLoadAsioDriver();
	BOOL OpenDriver(INT_PTR nWaveDevice, BOOL bErrMsg);
	void CloseDriver();
	int GetBufSize();
	BOOL ChangeBufSize();
	void FreeBufferIn();
	void FreeBufferOut();
	int NotifyMessageIn(int nCode, double *pData = NULL, int nSamplesNum = 0, int nSamplesRecorded = 0, int nFlags = 0, BOOL bPost = FALSE);
	int NotifyMessageOut(int nCode, double *pData = NULL, int nSamplesNum = 0, int nSamplesRecorded = 0, int nFlags = 0, BOOL bPost = FALSE);
	void Buffering(long index);
	static long asioMessages(long selector, long value, void* message, double* opt);
	static void bufferSwitch(long index, ASIOBool processNow);
	static ASIOTime *bufferSwitchTimeInfo(ASIOTime *timeInfo, long index, ASIOBool processNow);
	static void sampleRateChanged(ASIOSampleRate sRate);
	static void ClearBuffer(int nType, void *pBuf, int nBufSize);
	static int GetBitsPerSample(int nType);
	static void *ConverFromDouble(int nType, int nSize, void *pOutData, const double *pInData, int nAlign);
	static void *ConverToDouble(int nType, int nSize, void *pInData, double *pOutData, int nAlign);

	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnAsioNotifyIn(WPARAM nCode, LPARAM nIndex);
	afx_msg LRESULT OnAsioNotifyOut(WPARAM nCode, LPARAM nIndex);
	afx_msg LRESULT OnAsioCloseWaveIn(WPARAM nCode, LPARAM nIndex);
	afx_msg LRESULT OnAsioCloseWaveOut(WPARAM nCode, LPARAM nIndex);
};

extern CAsioManager g_cAsioManager;
