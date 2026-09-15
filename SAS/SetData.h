#pragma once

#include "FileIO.h"

struct SetCalcParam {
	int nFreqBand;
	int nStartFreq;
	int nEndFreq;
	BOOL bAFilter;
	long nSplRefData;
	double fSplRefLevel;
	double fDT1MinTime;
	double fTsubEnd;
	BOOL bTsubAuto;
	double fTsubNoise;
	double fWIACCLevel;
	double fPrefSPL;
	double fPrefTauE;
	double fGRefData;
	double fTCustom1;
	double fTCustom2;
	double fCCustom;
};

struct SetNmsCondition {
	AcfCondition AcfCondition;
	IAcfCondition IAcfCondition;
	IdentCondition IdentCondition;
};

struct ImpScreenCopy {
	int nCopyMode;
	BOOL bFrame;
	BOOL bMulti;
	union {
		struct {
			BOOL bImp;
			BOOL bEcho;
			BOOL bIACC;
			BOOL bFreq;
			BOOL bPref;
			BOOL bParam;
			BOOL bMTF;
		};
		BOOL bGraph[7];
	};
	int nColumns;
	int nColSpace;
	int nRowSpace;
};

struct AcfScreenCopy {
	int nCopyMode;
	BOOL bFrame;
	BOOL bMulti;
	union {
		struct {
			BOOL bPhi0;
			BOOL bTaue;
			BOOL bTau1;
			BOOL bPhi1;
			BOOL bIACC;
			BOOL bTauIACC;
			BOOL bWIACC;
			BOOL bACF;
			BOOL bIACF;
		};
		BOOL bGraph[9];
	};
	int nColumns;
	int nColSpace;
	int nRowSpace;
};

struct NmsScreenCopy {
	int nCopyMode;
	BOOL bFrame;
	BOOL bMulti;
	union {
		struct {
			BOOL bPhi0;
			BOOL bTaue;
			BOOL bTau1;
			BOOL bPhi1;
			BOOL bSPL;
			BOOL bIACC;
			BOOL bTauIACC;
			BOOL bWIACC;
			BOOL bACF;
			BOOL bIACF;
		};
		BOOL bGraph[10];
	};
	int nColumns;
	int nColSpace;
	int nRowSpace;
};

class CSetData {
public:
	CSetData();
	~CSetData();

	struct SetMainWindow {
		int left;
		int top;
		int width;
		int height;
		int width2;
	} MainWindow;

	SetCalcParam CalcParam;
	AcfCondition AcfCondition;
	SetNmsCondition NmsCondition;
	ImpScreenCopy ImpSC;
	AcfScreenCopy AcfSC;
	NmsScreenCopy NmsSC;
};

extern CSetData setData;
