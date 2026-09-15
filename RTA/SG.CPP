// SG.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Rta.h"
#include "SG.h"

int SG_SPB;

/////////////////////////////////////////////////////////////////////////////
// CSG

CSG::CSG()
{
	m_pRingBuf = NULL;
	m_nBitsPerSample = 0;
}

CSG::~CSG()
{
	if (m_pRingBuf != NULL)
		delete m_pRingBuf;
}

/////////////////////////////////////////////////////////////////////////////
// CSG メッセージ ハンドラ

int CSG::SetWaveData(LPWAVENOTIFY pWaveNotify)
{
	int size = 0;

	m_bInitSpline = FALSE;

	switch (g_oSetData.Sg.nMode) {
	case SG_MODE_TONE:
		size = WaveOutTone(pWaveNotify);
		break;
	case SG_MODE_NOISE:
		size = WaveOutNoise(pWaveNotify);
		break;
	case SG_MODE_SWEEP:
		size = WaveOutSweep(pWaveNotify);
		break;
	case SG_MODE_PULSE:
		size = WaveOutPulse(pWaveNotify);
		break;
	case SG_MODE_SYNTH:
		size = WaveOutSynth(pWaveNotify);
		break;
	case SG_MODE_SCALE:
		size = WaveOutScale(pWaveNotify);
		break;
	case SG_MODE_WAVE:
		size = WaveOutWave(pWaveNotify);
		break;
	}

	return size;
}

int CSG::WaveOutTone(LPWAVENOTIFY pWaveNotify)
{
	double *pWaveBuf = pWaveNotify->pSamplesData;
	SWorkData &oWorkDataLeft = m_aWorkData[0];
	SWorkData &oWorkDataRight = m_aWorkData[1];

	if (g_oSetData.Sg.bRendouF && oWorkDataLeft.fFreq == oWorkDataRight.fFreq) {
		oWorkDataRight.fAngle = oWorkDataLeft.fAngle + (360 - g_oSetData.Sg.nPhase) / 360.0;
		while (oWorkDataRight.fAngle >= 1.0)
			oWorkDataRight.fAngle -= 1.0;
	}

	GenerateTone(pWaveBuf, 2, g_oSetData.Sg.nToneWave, g_oSetData.Sg.fFreqL, oWorkDataLeft);
	GenerateTone(pWaveBuf + 1, 2, g_oSetData.Sg.nToneWave, g_oSetData.Sg.fFreqR, oWorkDataRight);

	return SG_SPB;
}

void CSG::GenerateTone(double *pWaveBuf, int nChannel, int nWaveForm, double fFreq, SWorkData &oWorkData)
{
	double fFreqStep;

	if (oWorkData.fFreq == 0)
		oWorkData.fFreq = fFreq;

	if (oWorkData.fFreq == fFreq)
		fFreqStep = 1;
	else
		fFreqStep = pow(fFreq / oWorkData.fFreq, 1 / (double)SG_SPB);

	double fLevel = (double)g_oSetData.Sg.nDLevel / 100;

	for (int i = 0; i < SG_SPB; i++) {
		switch (nWaveForm) {
		case WAVE_SIN:
			*pWaveBuf = sin(2 * M_PI * oWorkData.fAngle) * fLevel + GetDither(m_nBitsPerSample);
			break;
		case WAVE_TRIANGLE:
			if (oWorkData.fAngle < 0.5)
				*pWaveBuf = (oWorkData.fAngle - 0.25) * 4 * fLevel;
			else
				*pWaveBuf = (0.75 - oWorkData.fAngle) * 4 * fLevel;
			break;
		case WAVE_SQUARE:
			if (oWorkData.fAngle < 0.5)
				*pWaveBuf = fLevel;
			else
				*pWaveBuf = -fLevel;
			break;
		case WAVE_SAW:
			*pWaveBuf = (oWorkData.fAngle - 0.5) * 2 * fLevel;
			break;
		default:
			*pWaveBuf = GetUserWaveForm(nWaveForm - WAVE_USER, oWorkData) * fLevel;
			break;
		}

		pWaveBuf += nChannel;

		oWorkData.fAngle += oWorkData.fFreq / g_oSetData.Sg.nSamplingRate;
		while (oWorkData.fAngle >= 1.0)
			oWorkData.fAngle -= 1.0;

		oWorkData.fFreq *= fFreqStep;
	}

	oWorkData.fFreq = fFreq;
}

void CSG::ResetToneFreq()
{
	m_aWorkData[0].fFreq = 0;
	m_aWorkData[1].fFreq = 0;
}

double CSG::GetUserWaveForm(int nUserWaveForm, SWorkData &oWorkData)
{
	WaveForm *pWaveForm = g_oWaveForm.ReadWaveForm(nUserWaveForm);

	switch (pWaveForm->nDataType) {
	case WAVEFORM_FM:
		return GetUserWaveFormFM((WaveFormFM *)pWaveForm, oWorkData);
	default:
		return GetUserWaveFormManual((WaveFormManual *)pWaveForm, oWorkData);
	}
}

double CSG::GetUserWaveFormManual(const WaveFormManual *pWaveForm, SWorkData &oWorkData)
{
	int i;

	if (pWaveForm->bSmoothing) {
		if (!m_bInitSpline) {
			m_bInitSpline = TRUE;

			int nPoint = pWaveForm->nDataNum;
			double *xp = new double[nPoint];
			double *yp = new double[nPoint];

			for (i = 0; i < nPoint; i++) {
				xp[i] = pWaveForm->vertex[i].x;
				yp[i] = pWaveForm->vertex[i].y;
			}

			m_oSpline.MakeTable(xp, yp, nPoint);

			delete [] yp;
			delete [] xp;
		}

		return m_oSpline.Spline(oWorkData.fAngle);
	} else {
		for (i = pWaveForm->nDataNum - 1; i >= 0; i--) {
			if (oWorkData.fAngle >= pWaveForm->vertex[i].x)
				break;
		}

		double x = pWaveForm->vertex[i].x;
		double y = pWaveForm->vertex[i].y;
		double dx = pWaveForm->vertex[i + 1].x - x;
		double dy = pWaveForm->vertex[i + 1].y - y;

		return y + (oWorkData.fAngle - x) / dx * dy;
	}
}

double CSG::GetUserWaveFormFM(const WaveFormFM *pWaveForm, SWorkData &oWorkData)
{
	double fDiff = oWorkData.fAngle - oWorkData.fPrevAngle;
	oWorkData.fPrevAngle = oWorkData.fAngle;
	if (fDiff < 0)
		fDiff += 1.0;
	if (oWorkData.fModFreq == pWaveForm->fModFreq)
		oWorkData.fModAngle += fDiff * pWaveForm->fModFreq;
	else {
		oWorkData.fModAngle = oWorkData.fAngle * pWaveForm->fModFreq;
		oWorkData.fModFreq = pWaveForm->fModFreq;
	}
	if (oWorkData.fModAngle >= 1.0)
		oWorkData.fModAngle -= 1.0;

	return sin(2 * M_PI * oWorkData.fAngle + pWaveForm->fModAmp * sin(2 * M_PI * oWorkData.fModAngle));
}

int CSG::WaveOutNoise(LPWAVENOTIFY pWaveNotify)
{
	double *pWaveBuf = pWaveNotify->pSamplesData;
	int nRingBufPtr;
	int nPolarity = 1;
	double *buf = new double[SG_SPB];
	double fLevel = (double)g_oSetData.Sg.nDLevel / 100;

	switch (g_oSetData.Sg.nNoiseMode) {
	case NOISE_MODE_INVERSE:
		nPolarity = -1;
		// no break
	case NOISE_MODE_MONO:
		switch (g_oSetData.Sg.nNoiseType) {
		case NOISE_WHITE:
			m_oNoiseLeft.GenerateWhiteNoise(buf, 1, SG_SPB, fLevel);
			break;
		case NOISE_PINK:
			m_oNoiseLeft.GeneratePinkNoise(buf, 1, SG_SPB, fLevel);
			break;
		case NOISE_BROWN:
			m_oNoiseLeft.GenerateBrownNoise(buf, 1, SG_SPB, fLevel);
			break;
		}

		nRingBufPtr = m_nRingBufPtr;
		SetRingBuf(buf);

		if (g_oSetData.Sg.fTimeDiff >= 0) {
			GetRingBuf(pWaveBuf + 1, nRingBufPtr, 1);
			GetRingBuf(pWaveBuf, (nRingBufPtr + m_nRingBufSize - (int)(g_oSetData.Sg.nSamplingRate * (g_oSetData.Sg.fTimeDiff / 1000))) % m_nRingBufSize, nPolarity);
		} else {
			GetRingBuf(pWaveBuf, nRingBufPtr, 1);
			GetRingBuf(pWaveBuf + 1, (nRingBufPtr + m_nRingBufSize - (int)(g_oSetData.Sg.nSamplingRate * (-g_oSetData.Sg.fTimeDiff / 1000))) % m_nRingBufSize, nPolarity);
		}
		break;
	case NOISE_MODE_STEREO:
		switch (g_oSetData.Sg.nNoiseType) {
		case NOISE_WHITE:
			m_oNoiseLeft.GenerateWhiteNoise(pWaveBuf, 2, SG_SPB, fLevel);
			m_oNoiseRight.GenerateWhiteNoise(pWaveBuf + 1, 2, SG_SPB, fLevel);
			break;
		case NOISE_PINK:
			m_oNoiseLeft.GeneratePinkNoise(pWaveBuf, 2, SG_SPB, fLevel);
			m_oNoiseRight.GeneratePinkNoise(pWaveBuf + 1, 2, SG_SPB, fLevel);
			break;
		case NOISE_BROWN:
			m_oNoiseLeft.GenerateBrownNoise(pWaveBuf, 2, SG_SPB, fLevel);
			m_oNoiseRight.GenerateBrownNoise(pWaveBuf + 1, 2, SG_SPB, fLevel);
			break;
		}
		break;
	}

	delete [] buf;

	return SG_SPB;
}

void CSG::SetRingBuf(const double *pBuf)
{
	for (int i = 0; i < SG_SPB; i++) {
		m_pRingBuf[m_nRingBufPtr] = *pBuf++;

		if (++m_nRingBufPtr >= m_nRingBufSize)
			m_nRingBufPtr = 0;
	}
}

void CSG::GetRingBuf(double *pBuf, int nOffset, int nPolarity)
{
	for (int i = 0; i < SG_SPB; i++) {
		*pBuf = m_pRingBuf[nOffset] * nPolarity;
		pBuf += 2;

		if (++nOffset >= m_nRingBufSize)
			nOffset = 0;
	}
}

int CSG::WaveOutSweep(LPWAVENOTIFY pWaveNotify)
{
	int i;
	double *pWaveBuf = pWaveNotify->pSamplesData;
	SWorkData &oWorkData = m_aWorkData[0];

	int nSweepSamples = (int)(g_oSetData.Sg.nSamplingRate * g_oSetData.Sg.fSweepTime);

	double fSample = 1 / (g_oSetData.Sg.nSamplingRate * g_oSetData.Sg.fSweepTime);
	double fFreqStep = pow(g_oSetData.Sg.fEndFreq / g_oSetData.Sg.fStartFreq, fSample);
	double fLevelStep = pow(pow(10.0, (g_oSetData.Sg.fStartLevel - g_oSetData.Sg.fEndLevel) / 20), fSample);

	for (i = 0; i < SG_SPB; i++) {
		if (m_nSweepCount >= nSweepSamples) {
			if (g_oSetData.Sg.bSweepLoop)
				Init();
			else
				break;
		}

		double fLevel = (double)g_oSetData.Sg.nDLevel / 100 * m_fSweepLevel;

		switch (g_oSetData.Sg.nSweepWave) {
		case WAVE_SIN:
			*pWaveBuf++ = sin(2 * M_PI * oWorkData.fAngle) * fLevel + GetDither(m_nBitsPerSample);
			break;
		case WAVE_TRIANGLE:
			if (oWorkData.fAngle < 0.5)
				*pWaveBuf++ = (oWorkData.fAngle - 0.25) * 4 * fLevel;
			else
				*pWaveBuf++ = (0.75 - oWorkData.fAngle) * 4 * fLevel;
			break;
		case WAVE_SQUARE:
			if (oWorkData.fAngle < 0.5)
				*pWaveBuf++ = fLevel;
			else
				*pWaveBuf++ = -fLevel;
			break;
		case WAVE_SAW:
			*pWaveBuf++ = (oWorkData.fAngle - 0.5) * 2 * fLevel;
			break;
		default:
			*pWaveBuf++ = GetUserWaveForm(g_oSetData.Sg.nSweepWave - WAVE_USER, oWorkData) * fLevel;
			break;
		}

		oWorkData.fAngle += m_fSweepFreq / g_oSetData.Sg.nSamplingRate;
		while (oWorkData.fAngle >= 1)
			oWorkData.fAngle -= 1;

		m_fSweepFreq *= fFreqStep;
		m_fSweepLevel *= fLevelStep;
		m_nSweepCount++;
	}

	return i;
}

int CSG::WaveOutPulse(LPWAVENOTIFY pWaveNotify)
{
	int i;
	double *pWaveBuf = pWaveNotify->pSamplesData;

	for (i = 0; i < SG_SPB; i++) {
		m_fPulseCycleTime += m_fPulseSamplingTime;
		if (m_fPulseCycleTime >= g_oSetData.Sg.fPulseCycle) {
			if (!g_oSetData.Sg.bPulseContinue && m_nPulseCycleCount >= g_oSetData.Sg.nPulseNum)
				break;
			m_nPulseWidthCount = 1;
			m_nPulseCycleCount++;
			m_fPulseCycleTime -= g_oSetData.Sg.fPulseCycle;
		}

		if (m_nPulseWidthCount != 0) {
			switch (g_oSetData.Sg.nPulsePolarity) {
			case 0:
				*pWaveBuf++ = (double)g_oSetData.Sg.nDLevel / 100;
				break;
			case 1:
				*pWaveBuf++ = (double)-g_oSetData.Sg.nDLevel / 100;
				break;
			case 2:
				*pWaveBuf++ = (double)((m_nPulseCycleCount % 2) ? g_oSetData.Sg.nDLevel : -g_oSetData.Sg.nDLevel) / 100;
				break;
			}

			if (++m_nPulseWidthCount > g_oSetData.Sg.nPulseWidth)
				m_nPulseWidthCount = 0;
		} else
			*pWaveBuf++ = 0;
	}

	return i;
}

int CSG::WaveOutSynth(LPWAVENOTIFY pWaveNotify)
{
	double *pWaveBuf = pWaveNotify->pSamplesData;
	int i, j;
	double *buf = new double[SG_SPB];
	double *waveBuf = new double[SG_SPB];

	memset(waveBuf, 0, sizeof(double) * SG_SPB);

	for (i = 0; i < N_SG_COMPONENT; i++) {
		if (g_oSetData.Sg.fCompFreq[i] != 0 && g_oSetData.Sg.nCompLevel[i] != 0) {
			double fFreq = g_oSetData.Sg.fSynthFreq * g_oSetData.Sg.fCompFreq[i];
			if (fFreq < g_oSetData.Sg.nSamplingRate / 2) {
				GenerateTone(buf, 1, g_oSetData.Sg.nSynthWave, g_oSetData.Sg.fSynthFreq * g_oSetData.Sg.fCompFreq[i], m_aWorkData[i]);
				for (j = 0; j < SG_SPB; j++)
					waveBuf[j] = waveBuf[j] + buf[j] * g_oSetData.Sg.nCompLevel[i] / 100;
			}
		}
	}

	double fMax = GetMaxData(waveBuf, SG_SPB);
	for (i = 0; i < SG_SPB; i++)
		pWaveBuf[i] = (fMax > 1.0) ? waveBuf[i] / fMax : waveBuf[i];

	delete [] buf;
	delete [] waveBuf;

	return SG_SPB;
}

int CSG::WaveOutScale(LPWAVENOTIFY pWaveNotify)
{
	double *pWaveBuf = pWaveNotify->pSamplesData;
	SWorkData &oWorkData = m_aWorkData[0];

	int nPitch = (g_oSetData.Sg.nOctave - 4) * 12 + (g_oSetData.Sg.nScale - 9);
	double fFreq = g_oSetData.Sg.fReferencePitch * pow(2.0, (double)nPitch / 12);

	oWorkData.fFreq = 0;

	GenerateTone(pWaveBuf, 1, g_oSetData.Sg.nScaleWave, fFreq, oWorkData);

	return SG_SPB;
}

void CSG::Init()
{
	memset(m_aWorkData, 0, sizeof(m_aWorkData));

	switch (g_oSetData.Sg.nMode) {
	case SG_MODE_TONE:
		break;
	case SG_MODE_NOISE:
		if (m_pRingBuf != NULL)
			delete [] m_pRingBuf;
		m_nRingBufSize = SG_SPB + g_oSetData.Sg.nSamplingRate + 1;
		m_pRingBuf = new double[m_nRingBufSize];
		MEMCLEAR(m_pRingBuf, m_nRingBufSize);
		m_nRingBufPtr = 0;
		break;
	case SG_MODE_SWEEP:
		m_fSweepFreq = g_oSetData.Sg.fStartFreq;
		m_fSweepLevel = pow(10.0, -g_oSetData.Sg.fStartLevel / 20);
		m_nSweepCount = 0;
		break;
	case SG_MODE_PULSE:
		m_nPulseCycleCount = 0;
		m_nPulseWidthCount = 0;
		m_fPulseCycleTime = 0;
		m_fPulseSamplingTime = 1000.0 / (double)g_oSetData.Sg.nSamplingRate;
		break;
	case SG_MODE_SYNTH:
		break;
	}
}

int CSG::WaveOutWave(LPWAVENOTIFY pWaveNotify)
{
	double *pWaveBuf = pWaveNotify->pSamplesData;
	SWorkData &oWorkData = m_aWorkData[0];

	oWorkData.fFreq = 0;

	GenerateTone(pWaveBuf, 1, WAVE_USER + g_oSetData.Sg.nWaveFormNo, g_oSetData.Sg.fWaveFreq, oWorkData);

	return SG_SPB;
}
