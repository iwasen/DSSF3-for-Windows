#pragma once

#include "ProgressDlg.h"
#include "FileIO.h"
#include "fft.h"
#include "MSeq.h"
#include "TSP.h"

#define MeanSPL(left, right)	LogMean(left, right)
#define MeanDT1(left, right)	(((left) + (right)) / 2)
#define MeanTsub(left, right)	(((left) + (right)) / 2)
#define MeanA(left, right)		(((left) + (right)) / 2)
#define MeanG(left, right)		LogMean(left, right)
#define MeanT(left, right)		(((left) + (right)) / 2)
#define MeanC(left, right)		LogMean(left, right)
#define MeanD(left, right)		(((left) + (right)) / 2)
#define MeanTs(left, right)		(((left) + (right)) / 2)

#define INVALID_PREF	-1000

#define N_MTF_FREQ	14
#define N_MTF_BAND	7
#define N_MTF_500	4
#define	N_MTF_2000	5

struct FREQBAND {
	int nData;
	double fBandWidth;
	double fRealFreq[33];
	double fDispFreq[33];
};

extern const struct FREQBAND g_tFilterTbl[2];
extern const double g_tMtfFreq[];

extern void CalcEchoData(const double *pIRLeft, const double *pIRRight, double *pEchoData, int nData, double fRate,  double t1, double noise);
extern void CalcIACCData(const double *pIRLeft, const double *pIRRight, int nData, double *pOutData, int nOutData);
extern BOOL MakeFilterTbl1(double *pFilterTbl, int nData, double fRate, double fFreqLow, double fFreqHigh);
extern void CalcFilter(double *pWaveLeft, double *pWaveRight, int nData, const double *pFilterTbl);
extern BOOL CalcFilterAuto(double *pWaveLeft, double *pWaveRight, int nData, double fRate, int nFreq, int band);
extern void GetDispFreq(int nFreq, int band, CString &str, BOOL bKiro = FALSE);
extern double CalcParamSpl(const double *pData, int nData, int nMultiplier);
extern void CalcParamDeltaT1(const double *pData, int nData, double fRate, double minTime, double *pT0, double *pT1, int *pT0Pos);
extern double CalcParamTSub(const double *pData1, const double *pData2, int nData, double fRate, double t1, double endLevel, BOOL bAuto, double noise, double *pDev, double *pAVal, double *pEndTime = NULL);
extern void CalcParamIACC(const double *pIACCData, int nIACCData, double fRate, double *pIACC, double *pTIACC, double *pWIACC, double wLevel, double *pWIACC1 = NULL, double *pWIACC2 = NULL);
extern void CalcParamTx2(const double *pData1, const double *pData2, int nData, double fRate, int nT0Pos, double *pT20Reg0, double *pT20Reg1, double *pT30Reg0, double *pT30Reg1, double *pEDTReg0, double *pEDTReg1);
extern BOOL CalcAcParam(const DbImpulseRec *pDbImpulseRec, HWAVEDATA hWaveData, DbAcParamRec *pDbAcParamRec, double fRate, CProgressDlg *pProgressDlg = NULL);
extern void CalcTsub(const DbImpulseRec *pDbImpulseRec, HWAVEDATA hWaveData, DbAcParamRec *pDbAcParamRec, double fRate);
extern void CalcIACC(const DbImpulseRec *pDbImpulseRec, HWAVEDATA hWaveData, DbAcParamRec *pDbAcParamRec, double fRate);
extern void CalcPowerSpectrum(const double *pWaveLeft, const double *pWaveRight, double *pFreqLeft, double *pFreqRight, int nData, BOOL bAdjust = TRUE);
extern BOOL CalcPreference(double SPL, double dT1, double Tsub, double IACC, double A, double te, double bestSPL, double *s);
extern BOOL CalcS0(DbAcParamRec *pDbAcParamRec, int nFreq, double fAdjustSPL, double *pPref);
extern double LogMean(double d1, double d2);
extern double GetAdjustSPL(long folderID, long nSplRefData, double fSplRefLevel);
extern void CalcMTF(const double *pIR, int nData, double fRate, const double *pMtfFreq, int nMtfFreq, double *pMTF, BOOL bSN, double fSN);
