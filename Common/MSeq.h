#pragma once

#include "IRMethod.h"

#define TOPMSEQPARAM	10
#define N_MSEQDATA		(sizeof(tMSeqParam) / sizeof(MSeqParam))

struct MSeqParam {
	int nBit;
	int ntap;
	int taps[4];
	int tob[19];
};

class CMSeq : public CIRMethod
{
public:
	CMSeq();
	virtual ~CMSeq();

	void InitMethod(int nStage);
	void GenerateSequence(double *pData, int nAtt);
	void CalcImpulse(double *pData);

protected:
	int m_nStage;
	int *m_pPermut;
	char *m_pMSequence;

	void FreeBuffers();
};
