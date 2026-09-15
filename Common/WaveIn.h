#pragma once

#include <mmsystem.h>
#pragma warning(disable : 4819)
#include <Mmreg.h>
#include <ks.h>
#include <ksmedia.h>
#include "WaveOut.h"

class CWaveIn : public CWnd
{
public:
	CWaveIn();
	virtual ~CWaveIn();

	IWaveNotify *m_pWnd;
	HWAVEIN m_hWave;
	static BOOL m_b16BitOnly;

	BOOL Open(INT_PTR nWaveDevice, IWaveNotify *pWnd, int nChannels, int nSamplesPerSec, int nSamplesPerBuffer, int nBufferNum, BOOL bErrMsg = TRUE);
	void Start();
	void Close();
	void Reset();
	void Stop();
	int GetBitsPerSample();

protected:
	WAVEFORMATEXTENSIBLE m_oWaveFormat;
	INT_PTR m_nWaveDevice;
	BOOL m_bErrMsg;
	LPWAVEHDR m_pWaveHdr;
	double *m_pSamplesBuffer;

	int NotifyMessage(int nCode, LPWAVEHDR pWaveHdr);
	BOOL AllocWaveBuffer(int nBufferNum, int nBufferSize);
	void FreeWaveBuffer();
	void ConvertWaveToDouble(LPWAVEHDR pWaveHdr);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnDestroy();
	afx_msg LRESULT OnWaveInData(WPARAM wParam, LPARAM lParam);
};
