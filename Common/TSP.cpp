// TSP.cpp: CTSP クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TSP.h"

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

CTSP::CTSP()
{
	m_nStage = 0;
	m_pSignal = NULL;
}

CTSP::~CTSP()
{
	FreeBuffers();
}

void CTSP::InitMethod(int nStage)
{
	static constexpr int tMParam[] = {10, 12, 13, 14, 15, 15, 15, 15, 15, 15};

	if (nStage == m_nStage)
		return;

	m_nStage = nStage;
	m_nN = 1 << m_nStage;
	m_nM = tMParam[m_nStage - 10] * m_nN / 32;

	FreeBuffers();
}

void CTSP::GenerateSequence(double *pData, int nAtt)
{
	int i, j;
	double e;
	const int nLevel = 1 / nAtt;

	if (m_pSignal == NULL) {
		double *f1 = new double[m_nN];

		f1[0] = 1.0;
		f1[1] = 1.0;
		for (i = 1; i < m_nN / 2; i++) {
			e = 4 * M_PI * m_nM * i * i / ((double)m_nN * m_nN);
			j = i * 2;
			f1[j] = cos(e);
			f1[j + 1] = -sin(e);
		}

		m_oRFFT.ifft(m_nN, f1);

		double fMax = 0;
		double fAbs;
		for (i = 0; i < m_nN; i++) {
			fAbs = fabs(f1[i]);
			if (fAbs > fMax)
				fMax = fAbs;
		}

		for (i = 0; i < m_nN; i++)
			f1[i] /= fMax;

		m_pSignal = new double[m_nN];

		const int nShift = m_nN / 2 - m_nM;
		for (i = 0; i < m_nN - nShift; i++)
			m_pSignal[i] = f1[i + nShift] * nLevel;
		for ( ; i < m_nN; i++)
			m_pSignal[i] = f1[i - (m_nN - nShift)] * nLevel;

		delete [] f1;
	}

	memcpy(pData, m_pSignal, sizeof(double) * m_nN);
}

void CTSP::CalcImpulse(double *pData)
{
	if (m_nStage == 0)
		return;

	int i, j;
	double e;
	double *f1 = new double[m_nN];
	double t1, t2;

	f1[0] = 1.0;
	f1[1] = 1.0;
	for (i = 1; i < m_nN / 2; i++) {
		e = 4 * M_PI * m_nM * i * i / ((double)m_nN * m_nN);
		j = i * 2;
		f1[j] = cos(e);
		f1[j + 1] = sin(e);
	}

	m_oRFFT.fft(m_nN, pData);

	double mk = 0.005794; // 0.005794はM系列と整合
	f1[0] = 0;
	f1[1] = 0;
	for (i = 1; i < m_nN / 2; i++) {
		j = i * 2;
		t1 = (f1[j] * pData[j] - f1[j + 1] * pData[j + 1]) * mk;
		t2 = (f1[j] * pData[j + 1] + f1[j + 1] * pData[j]) * mk;
		f1[j] = t1;
		f1[j + 1] = t2;
	}

	m_oRFFT.ifft(m_nN, f1);

	const int nShift = m_nN / 2 - m_nM;
	memcpy(pData, f1 + (m_nN - nShift), sizeof(double) * nShift);
	memcpy(pData + nShift, f1, sizeof(double) * (m_nN - nShift));

	delete [] f1;

	PolarityCheck(m_nN, pData);
}

void CTSP::FreeBuffers()
{
	if (m_pSignal != NULL) {
		delete [] m_pSignal;
		m_pSignal = NULL;
	}
}
