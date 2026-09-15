// IRMethod.cpp: CIRMethod クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IRMethod.h"

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

CIRMethod::~CIRMethod()
{
	// virtualを継承させるため必要
}

void CIRMethod::PolarityCheck(int nData, double *pData)
{
	int i;
	double fMin = 0, fMax = 0;

	for (i = 0; i < nData; i++) {
		if (pData[i] < fMin)
			fMin = pData[i];
		if (pData[i] > fMax)
			fMax = pData[i];
	}

	if (-fMin > fMax) {
		for (i = 0; i < nData; i++)
			pData[i] = -pData[i];
	}
}
