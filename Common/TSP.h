#pragma once

#include "fft.h"
#include "IRMethod.h"

class CTSP : public CIRMethod
{
public:
	CTSP();
	virtual ~CTSP();

	void InitMethod(int nStage);
	void GenerateSequence(double *pData, int nAtt);
	void CalcImpulse(double *pData);

protected:
	int m_nStage;
	int m_nN;
	int m_nM;
	double *m_pSignal;
	CRFFT m_oRFFT;

	void FreeBuffers();
};
