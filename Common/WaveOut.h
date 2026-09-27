#pragma once

#include <mmsystem.h>
#pragma warning(disable : 4819)
#include <Mmreg.h>
#include <ks.h>
#include <ksmedia.h>
#include "WaveNotify.h"

class CWaveOut : public CWnd
{
public:
	CWaveOut();
	virtual ~CWaveOut();

	HWAVEOUT m_hWave;
	IWaveNotify *m_pWnd;
	static BOOL m_b16BitOnly;

	BOOL Open(INT_PTR nWaveDevice, IWaveNotify *pWnd, int nChannels, int nSamplesPerSec, int nSamplesPerBuffer, int nBufferNum, BOOL bErrMsg = TRUE, BOOL bVolumeSet = TRUE);
	void Start();
	void Close();
	void Reset() const;
	void Pause() const;
	void Restart() const;
	void SetVolume(UINT left, UINT right) const;
	int GetBitsPerSample() const;

protected:
	WAVEFORMATEXTENSIBLE m_oWaveFormat;
	LPWAVEHDR m_pWaveHdr;
	int m_nBufCounter;
	DWORD m_dwOrgVolume;
	BOOL m_bCloseWait;
	BOOL m_bVolumeSet;
	double *m_pSamplesBuffer;

	int NotifyMessage(int nCode, LPWAVEHDR pWaveHdr);
	BOOL AllocWaveBuffer(int nBufferNum, int nBufferSize);
	void FreeWaveBuffer();
	void ConvertDoubleToWave(LPWAVEHDR pWaveHdr, int rc);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnDestroy();
	afx_msg LRESULT OnWaveOutDone(WPARAM wParam, LPARAM lParam);
};
