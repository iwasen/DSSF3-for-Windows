#pragma once

#include "Spline.h"
#include "Noise.h"

extern int	SG_SPB;

class CSG
{
public:
	CSG();
	virtual ~CSG();

	int m_nBitsPerSample;

	void Init();
	int SetWaveData(LPWAVENOTIFY pWaveNotify);
	void ResetToneFreq();

protected:
	struct SWorkData {
		double fAngle;
		double fFreq;
		double fPrevAngle;
		double fModAngle;
		double fModFreq;
	};

	SWorkData m_aWorkData[N_SG_COMPONENT];
	double m_fSweepFreq;
	double m_fSweepLevel;
	int m_nSweepCount;
	int m_nPulseCycleCount;
	int m_nPulseWidthCount;
	double m_fPulseCycleTime;
	double m_fPulseSamplingTime;
	double *m_pRingBuf;
	int m_nRingBufPtr;
	int m_nRingBufSize;
	CSpline m_oSpline;
	BOOL m_bInitSpline;
	CNoise m_oNoiseLeft;
	CNoise m_oNoiseRight;

	int WaveOutTone(LPWAVENOTIFY pWaveNotify);
	void GenerateTone(double *pWaveBuf, int nChannel, int nWaveForm, double fFreq, SWorkData &oWorkData);
	double GetUserWaveForm(int nUserWaveForm, SWorkData &oWorkData);
	double GetUserWaveFormManual(const WaveFormManual *pWaveForm, SWorkData &oWorkData);
	double GetUserWaveFormFM(const WaveFormFM *pWaveForm, SWorkData &oWorkData);
	int WaveOutNoise(LPWAVENOTIFY pWaveNotify);
	void SetRingBuf(const double *pBuf);
	void GetRingBuf(double *pBuf, int nOffset, int nPolarity);
	int WaveOutSweep(LPWAVENOTIFY pWaveNotify);
	int WaveOutPulse(LPWAVENOTIFY pWaveNotify);
	int WaveOutSynth(LPWAVENOTIFY pWaveNotify);
	int WaveOutScale(LPWAVENOTIFY pWaveNotify);
	void GenerateWhiteNoise(double *pData, int nStep);
	void GeneratePinkNoise(double *pData, int nStep, double b[]);
	double GeneratePinkNoiseSub(double b[]);
	void GenerateBrownNoise(double *pData, int nStep, double &c);
	int WaveOutWave(LPWAVENOTIFY pWaveNotify);
};
