#pragma once

#define N_SG_PRESET		4
#define N_SG_COMPONENT	8

#pragma pack(4)
struct RtaSetup {
	int nPosLeft;
	int nPosTop;
	int nInputDevice;
	int nOutputDevice;
};

struct SgSetup {
	int nPosLeft;
	int nPosTop;
	int nMode;
	int nSamplingRate;
	int nLLevel;
	int nRLevel;
	int nDLevel;
	BOOL bRendou;
	double fFreqL;
	double fFreqR;
	BOOL bRendouF;
	int nFreqRange;
	int nToneWave;
	double fPresetFreq[N_SG_PRESET];
	int nPhase;
	int nNoiseType;
	int nNoiseMode;
	double fTimeDiff;
	double fStartFreq;
	double fEndFreq;
	double fSweepSpeed;
	BOOL bSweepLoop;
	int nSweepWave;
	int nPulseWidth;
	int nPulsePolarity;
	int nPulseNum;
	BOOL bPulseContinue;
	double fPulseCycle;
	double fReferencePitch;
	int nScale;
	int nOctave;
	int nScaleWave;
	BOOL bPlayMode;
	double fCompFreq[N_SG_COMPONENT];
	int nCompLevel[N_SG_COMPONENT];
	double fSynthFreq;
	int nSynthWave;
	double fWaveFreq;
	int nWaveFormNo;
	int nTimeRange;
	double fSweepTime;
	double fStartLevel;
	double fEndLevel;
};

struct FftSetup {
	int nPosLeft;
	int nPosTop;
	int nMode;
	int nChannel;
	int nSamplingRate;
	BOOL bFftAutoFreq;
	int nFftMinFreq;
	int nFftMaxFreq;
	BOOL bFftAutoLevel;
	int nFftMinLevel;
	int nFftMaxLevel;
	int nFftScale;
	int nSmoothing;
	int nFftWindowFunc;
	int nFftSize;
	BOOL bPeakDisp;
	BOOL bPeakHold;
	int nPeakHoldMode;
	int nPeakHoldTime;
	int nTimeDir;
	int nTimeDataNum;
	int nCorrelation;
	long nMicCalID;
	int nFilter;
	BOOL bRlSplit;
	int nCrfZoom;
	int nTimeRes;
	int nAverage;
	int nOctaveBand;
	int nTimeRange;
	int nColorScale;
	int nAutoStopTime;
	BOOL bSeparateWindow[9];
	RECT rectFftWindow[9];
};

struct OsSetup {
	int nPosLeft;
	int nPosTop;
	int nSamplingRate;
	int nChannel;
	int nLevelL;
	int nLevelR;
	BOOL bLevelAuto;
	int nPosL;
	int nPosR;
	int nSweep;
	int nDelayRange;
	int nTrigLevel;
	int nTrigChannel;
	int nTrigSlope;
	BOOL bTrigAuto;
	BOOL bTrigHighCut;
	BOOL bTrigLowCut;
	BOOL bTrigDisp;
	BOOL bTrigPos;
	BOOL bTrigFree;
	BOOL bReverseL;
	BOOL bReverseR;
	BOOL bXY;
	int nOverlay;
	BOOL bRendou;
	BOOL bZeroLevel;
	double fCalLevel;
	int nCalType;
	double fCalValue;
	char sCalUnit[4];
};

struct FreSetup {
	int nPosLeft;
	int nPosTop;
	int nSamplingRate;
	int nChannel;
	int nMode;
	int nMinLevel;
	int nMaxLevel;
	int nSweepFreqStart;
	int nSweepFreqEnd;
	int nSweepTime;
	int nSweepLevel;
	int nSpotFreqStart;
	int nSpotFreqEnd;
	int nSpotPoint;
	int nSpotLevel;
	int nNoiseFreqStart;
	int nNoiseFreqEnd;
	int nNoiseResolution;
	int nNoiseAveraging;
	int nNoiseLevel;
	long nMicCalID;
};

struct DstSetup {
	int nPosLeft;
	int nPosTop;
	int nSamplingRate;
	int nChannel;
	int nVolume;
	int nMaxHarmonics;
	int nGuardCnt;
	int nScaleMode;
	int nScaleMax;
	int nScaleMin;
	int nMode;
	int nManualFreq;
	int nManualLevel;
	BOOL bManualAverage;
	int nFreqStart;
	int nFreqEnd;
	int nFreqPoint;
	int nFreqLevel;
	BOOL bFreqSpline;
	int nLevelStart;
	int nLevelEnd;
	int nLevelPoint;
	int nLevelFreq;
	BOOL bLevelSpline;
	BOOL bFreqMarker;
	BOOL bLevelMarker;
	int nFreqHD;
	int nLevelHD;
};

struct ImpSetup {
	int nPosLeft;
	int nPosTop;
	int nSamplingRate;
	int nTime;
	int nMeasureNum;
	int nChannel;
	int nBit_unuse;
	int nScrollPos;
	int nScrollSize;
	int nFileType;
	int nFileFormat;
	int nIFilterID;
	BOOL bAutoLevel;
	BOOL bAutoRetry;
	int nMethod;
	double fOffsetTime;
};

struct AcfSetup {
	int nPosLeft;
	int nPosTop;
	int nSamplingRate;
	int nChannel;
	double fIntegrationTime;
	double fCalcCycle;
	double fMaxTau;
	int nTimeDir;
	int nTimeDataNum;
	int nSource;
	int nMeasureTime;
	int nFilter;
	int nScaleType;
	int bScaleAbs;
	int nScaleLevel;
	int nDispChannel;
	int nRealtimePosLeft;
	int nRealtimePosTop;
	double fRealtimeIntegrationTime;
	double fRealtimeRunningStep;
	int nRealtimeFreqWeighting;
	double fRealtimeMaxDelayTime;
	double fRealtimeTaueInterval;
	double fRealtimeTaueEndLevel;
	double fRealtimeTaueEndTime;
	BOOL bRealtimeTaueOrigin;
	int nRealtimeTau1Peak;
	int nRealtimeMinLevel;
	int nRealtimeMaxLevel;
	int nRealtimeScale;
	BOOL bRealtimeTaueFixedScale;
	double fRealtimeTaueMinTime;
	double fRealtimeTaueMaxTime;
	int nRealtimeTaueScale;
};

struct RecSetup {
	int nPosLeft;
	int nPosTop;
	int nPriority;
};

struct FltSetup {
	int nPosLeft;
	int nPosTop;
	int nBand;
	int nOrder;
	int nShape;
	double fRipple1;
	double fRipple2;
	double fCutoff1;
	double fCutoff2;
	int nInputOutput;
};

struct DtrSetup {
	int nPosLeft;
	int nPosTop;
	int nTimingMode;
	int nTimingInterval;
	BOOL bDeleteAtStart;
	BOOL bAlwaysNewData;
};

struct TimSetup {
	int nPosLeft;
	int nPosTop;
	int nFontSize;
	int nFormat;
	int nTransparency;
};
#pragma pack()

class CSetData {
public:
	CSetData();
	~CSetData();

	RtaSetup Pc;
	SgSetup Sg;
	FftSetup Fft;
	OsSetup Os;
	FreSetup Fre;
	DstSetup Dst;
	ImpSetup Imp;
	AcfSetup Acf;
	RecSetup Rec;
	FltSetup Flt;
	DtrSetup Dtr;
	TimSetup Tim;

	void Init();
};

class CSetData2 {
public:
	CSetData2();
	~CSetData2();

	int nFontSizeSg;
	int nFontSizeFft;
	int nFontSizeOs;
	int nFontSizeFre;
	int nFontSizeDst;
	int nFontSizeImp;
	int nFontSizeAcf;

	BOOL bInput16BitOnly;
	BOOL bOutput16BitOnly;

	CString sAutoRecFolder;
	BOOL bAutoRecInput;
	BOOL bAutoRecOutput;

private:
	int nFontSizeSgOrg;
	int nFontSizeFftOrg;
	int nFontSizeOsOrg;
	int nFontSizeFreOrg;
	int nFontSizeDstOrg;
	int nFontSizeImpOrg;
	int nFontSizeAcfOrg;

	BOOL bInput16BitOnlyOrg;
	BOOL bOutput16BitOnlyOrg;

	CString sAutoRecFolderOrg;
	BOOL bAutoRecInputOrg;
	BOOL bAutoRecOutputOrg;

	void WritePrivateProfileInt(LPCTSTR lpAppName, LPCTSTR lpKeyName, int nData, LPCTSTR lpFileName);
};
