#pragma once

#include "WaveIn.h"
#include "FileIO.h"

class CMeasurement : public CWaveIn
{
public:
	CMeasurement();
	virtual ~CMeasurement();

	class CNmsDoc *m_pDoc;
	DbMicCalRec m_oMicCalData;

	BOOL Start(int nMeasurementMode);
	void Stop();
	void StartManualSampling();
	void StopManualSampling();
	BOOL InCalculating() const;
	void ResetCounter();
	void Recalclation();
	void WaveFileMeasuring(LPCTSTR pWaveFileName);

protected:
	struct WaveData {
		double fLeft;
		double fRight;
	};

	int m_nMeasurementMode;
	int m_nSamplingRate;
	int m_nSamplePerBlock;
	double m_fSamplingTime;
	double m_fTimeConstant;
	double m_fSamplingLevel;
	int m_nRingBufSize;
	WaveData *m_pRingBuf;
	int m_nRingBufPtr;
	BOOL m_bRingBufFill;
	double m_fTimeConst1;
	double m_fTimeConst2;
	double m_fPower;
	double m_fSPL;
	double m_nSampleCount;
	double m_fMaxLevel;
	double m_fMinLevel;
	BOOL m_bResetLevel;
	BOOL m_bMeasuring;
	double *m_pFilter;
	CRFFT m_oRFFT;
	double *m_pFftBuf1;
	double *m_pFftBuf2;
	int m_nSaveCounter;
	int m_nCalcCounter;
	CEvent m_cCalcEvent;
	CEvent m_cExitEvent;
	int m_nSamplingFwdCount;
	int m_nSamplingBwdCount;
	int m_nBaseLevelCount;
	double m_fBaseLevel;
	BOOL m_bInCalculating;
	BOOL m_bExitThread;

	void AutoMeasuring(LPWAVENOTIFY pWaveNotify);
	void ManualMeasuring(LPWAVENOTIFY pWaveNotify);
	double CalcPower(double fData) const;
	BOOL SaveSampleData();
	void FreeBuffers();
	static UINT CalcThreadEntry(LPVOID pParam);
	void CalcThread();
	void AddItem(double fSamplingTime, CTime *pTime);

	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnWaveInData(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnChangeStatus(WPARAM wParam, LPARAM lParam);
};
