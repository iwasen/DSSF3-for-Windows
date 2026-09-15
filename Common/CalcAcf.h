#pragma once

#include "FileIO.h"

typedef BOOL (*AbortCheckFunc)(int nPercent);

extern BOOL g_bAbortCalc;

extern AcfFactorData *CalcAcfFactor(HWAVEDATA hWaveData, const AcfConditionData *pAcfConditionData, double fSamplingRate, int *nFactor, AbortCheckFunc AbortCheck, TaunPhinData **pTaunPhinData);
extern NmsFactorData *CalcNmsFactor(HWAVEDATA hWaveData, const NmsConditionData *pNmsConditionData, double fSamplingRate, int *nFactor, AbortCheckFunc AbortCheck);
extern double *CalcAcf1(const double *pWaveBuf, int nDataNum1, int nDataNum2, int nFftSize, const double *pFilterTbl);
extern double CalcSPL(const double *pWaveBuf1, const double *pWaveBuf2, int nDataNum, int nFftSize, const double *pFilterTbl);
extern double *CalcIAcf(const double *pWaveBuf1, const double *pWaveBuf2, int nDataNum, int nFftSize, const double *pFilterTbl);
extern double GetAdjustPhi0(long folderID, long nPhi0RefData, double fPhi0RefLevel);
extern long CalcNoiseSource(const NmsFactorData *pNmsFactorData, int nNmsFactorData, NoiseSrcData *pNoiseSrcData, const IdentCondition *pIdentCondition);
extern int GetIdentPos(const NmsFactorData *pNmsFactorData, int nNmsFactorData, const IdentCondition *pIdentCondition);
extern double GetIdentFactor(double fData, double fStandard, double fWeight);
extern void GetAcfFactor(const AcfCondition *pAcfCondition, AcfFactor *pAcfFactor, TaunPhinData *pTaunPhinData, double fSamplingRate, double *pData, int nData);
extern void GetIAcfFactor(const IAcfCondition *pIAcfCondition, IAcfFactor *pIAcfFactor, double fSamplingRate, const double *pData, int nData, double fSPL);
