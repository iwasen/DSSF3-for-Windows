#pragma once

#include "pcscl.h"

// フォルダファイル名
#define FN_FOLDER_DBF	"FOLDER.DBF"
#define FN_FOLDER1_EDX	"FOLDER1.EDX"

// インパルス応答測定データファイル名
#define FN_IMPULSE_DBF	"IMPULSE.DBF"
#define FN_IMPULSE_BIN	"IMPULSE.BIN"
#define FN_IMPULSE1_EDX	"IMPULSE1.EDX"
#define FN_IMPULSE2_EDX	"IMPULSE2.EDX"

// Running ACF 測定データファイル
#define FN_ACF_DBF		"ACF.DBF"
#define FN_ACF_BIN		"ACF.BIN"
#define FN_ACF1_EDX		"ACF1.EDX"
#define FN_ACF2_EDX		"ACF2.EDX"

// 騒音測定データファイル
#define FN_NMS_DBF		"NMS.DBF"
#define FN_NMS_BIN		"NMS.BIN"
#define FN_NMS1_EDX		"NMS1.EDX"
#define FN_NMS2_EDX		"NMS2.EDX"

// 騒音源テンプレートファイル
#define FN_NSTMP_DBF	"NSTMP.DBF"
#define FN_NSTMP_BIN	"NSTMP.BIN"
#define FN_NSTMP1_EDX	"NSTMP1.EDX"
#define FN_NSTMP2_EDX	"NSTMP2.EDX"

// 騒音源同定重み係数ファイル
#define FN_NSWEIGHT_DAT	"NSWEIGHT.DAT"

// 音響パラメータ計算結果ファイル名
#define FN_ACPARAM_DBF	"ACPARAM.DBF"
#define FN_ACPARAM1_EDX	"ACPARAM1.EDX"

// Running ACF 計算結果ファイル名
#define FN_ACFFACT_DBF	"ACFFACT.DBF"
#define FN_ACFFACT_BIN	"ACFFACT.BIN"
#define FN_ACFFACT1_EDX	"ACFFACT1.EDX"

// Running ACF マーカー
#define FN_ACFMARKER_DBF	"ACFMARK.DBF"
#define FN_ACFMARKER1_EDX	"ACFMARK1.EDX"
#define FN_ACFMARKER2_EDX	"ACFMARK2.EDX"

// 逆フィルタファイル名
#define FN_IFILTER_DBF	"IFILTER.DBF"
#define FN_IFILTER1_EDX	"IFILTER1.EDX"

// マイク較正ファイル名
#define FN_MICCAL_DBF	"MICCAL.DBF"
#define FN_MICCAL1_EDX	"MICCAL1.EDX"

// プリセットファイル名
#define FN_PRESET_DBF	"PRESET5.DBF"
#define FN_PRESET_BIN	"PRESET5.BIN"
#define FN_PRESET1_EDX	"PRESET5.EDX"

// 波形データファイル名
#define FN_WAVEFORM_DBF	"WAVEFORM.DBF"
#define FN_WAVEFORM_BIN	"WAVEFORM.BIN"
#define FN_WAVEFORM1_EDX	"WAVEFORM1.EDX"

// テンポラリファイル名
#define FN_TEMP_DBF		"TEMP.DBF"
#define FN_TEMP_BIN		"TEMP.BIN"

// フォルダタイプ
#define FOLDER_TYPE_IR	'I'		// インパルス応答
#define FOLDER_TYPE_ACF	'A'		// Running ACF
#define FOLDER_TYPE_NMS	'N'		// 騒音計測

#define ACPARAM_NFREQ	34		// 音響パラメータ周波数帯域別データ数
#define MICCAL_NFREQ	20		// マイク較正周波数データ数

// 騒音源同定条件モード
#define IDENT_MODE_MAXSPL	0	// 最大SPL
#define IDENT_MODE_MINTAUE	1	// 最小τe

// データベースフィールド長
#define LEN_ID			8		// ＩＤ
#define LEN_TYPE		1		// フォルダタイプ
#define LEN_TITLE		30		// タイトル
#define LEN_NAME		20		// 測定者氏名
#define LEN_PLACE		20		// 測定場所
#define LEN_DATE		8		// 作成日付
#define LEN_COMMENT		100		// コメント
#define LEN_SAMPLING	6		// サンプリングレート
#define LEN_MESTIME		2		// インパルス応答測定時間
#define LEN_MESNUM		3		// インパルス応答同期加算回数
#define LEN_CHANNEL		1		// サンプリングチャネル数
#define LEN_BIT			2		// サンプリングビット数
#define LEN_TIME		14		// 測定日時
#define LEN_MULTIPLIER	5		// インパルス応答出力レベル倍率
#define LEN_NDATA		2		// 音響パラメータデータ数
#define LEN_FREQBAND	1		// 周波数帯域（1Oct,1/3Oct)
#define LEN_FLOAT		8		// 浮動小数データ
#define LEN_BOOL		1		// BOOL データ
#define LEN_SHORT		6		// short データ
#define LEN_RESERVE		8		// リザーブ
#define LEN_MICINFO		30		// マイク情報（マイク較正）
#define LEN_MICAMPINFO	30		// マイクアンプ情報（マイク較正）
#define LEN_MAXLEVEL	3		// 逆フィルタ最大補償レベル
#define LEN_SIZE		5		// データサイズ
#define LEN_NACFFACT	6		// Running ACF 計算結果データ数
#define LEN_SAMPLENUM	10		// Running ACF サンプリングデータ数
#define LEN_NNMSFACT	6		// 騒音計算結果データ数
#define LEN_NSTMPNAME	32		// 騒音源名称
#define	LEN_WAVEFORMNUM	4		// 波形データ個数

#pragma pack(4)

// フォルダファイルバッファ
struct DbFolderBuf {
	char	aFolderID[LEN_ID];
	char	aType[LEN_TYPE];
	char	aName[LEN_NAME];
	char	aPlace[LEN_PLACE];
	char	aDate[LEN_DATE];
	char	aTitle[LEN_TITLE];
	char	aComment[LEN_COMMENT];
	char	aScale[LEN_FLOAT];
};

// フォルダファイルレコード
struct DbFolderRec {
	long	nFolderID;
	CString	sType;
	CString	sName;
	CString	sPlace;
	CString	sDate;
	CString	sTitle;
	CString	sComment;
	double fScale;
};

// インパルス応答測定データファイルバッファ
struct DbImpulseBuf {
	char	aFolderID[LEN_ID];
	char	aImpulseID[LEN_ID];
	char	aSampling[LEN_SAMPLING];
	char	aMeasTime[LEN_MESTIME];
	char	aMeasNum[LEN_MESNUM];
	char	aChannel[LEN_CHANNEL];
	char	aBit[LEN_BIT];
	char	aTime[LEN_TIME];
	char	aTitle[LEN_TITLE];
	char	aComment[LEN_COMMENT];
	char	aUseFilter[LEN_BOOL];
	char	aIFilterID[LEN_ID];
	char	aTemperature[LEN_FLOAT];
	char	aHumidity[LEN_FLOAT];
	char	aMultiplier[LEN_MULTIPLIER];
};

// インパルス応答測定データファイルレコード
struct DbImpulseRec {
	long	nFolderID;
	long	nImpulseID;
	long	nSampling;
	long	nMeasTime;
	long	nMeasNum;
	long	nChannel;
	long	nBit;
	CString	sTime;
	CString sTitle;
	CString	sComment;
	long	nUseFilter;
	long	nIFilterID;
	double	fTemperature;
	double	fHumidity;
	long	nMultiplier;
	long	bImpulseData;
};

// Running ACF 測定データファイルバッファ
struct DbAcfBuf {
	char	aFolderID[LEN_ID];
	char	aAcfID[LEN_ID];
	char	aSampling[LEN_SAMPLING];
	char	aChannel[LEN_CHANNEL];
	char	aSampleNum[LEN_SAMPLENUM];
	char	aDataTime[LEN_FLOAT];
	char	aTime[LEN_TIME];
	char	aTitle[LEN_TITLE];
	char	aComment[LEN_COMMENT];
};

// Running ACF 測定データファイルレコード
struct DbAcfRec {
	long	nFolderID;
	long	nAcfID;
	long	nSampling;
	long	nChannel;
	long	nSampleNum;
	double	fDataTime;
	CString	sTime;
	CString sTitle;
	CString	sComment;
};

// 騒音測定データファイルバッファ
struct DbNmsBuf {
	char	aFolderID[LEN_ID];
	char	aNmsID[LEN_ID];
	char	aSampling[LEN_SAMPLING];
	char	aChannel[LEN_CHANNEL];
	char	aSampleNum[LEN_SAMPLENUM];
	char	aNumber[LEN_SHORT];
	char	aDataTime[LEN_FLOAT];
	char	aTime[LEN_TIME];
	char	aConditionSize[LEN_SIZE];
	char	aFactorSize[LEN_SIZE];
	char	aNmsFactor[LEN_NNMSFACT];
};

// 騒音測定データファイルレコード
struct DbNmsRec {
	long	nFolderID;
	long	nNmsID;
	long	nSampling;
	long	nChannel;
	long	nSampleNum;
	long	nNumber;
	double	fDataTime;
	CString	sTime;
	long	nConditionSize;
	long	nFactorSize;
	long	nNmsFactor;
};

// 騒音源テンプレートファイルバッファ
struct DbNsTmpBuf {
	char	aNsTmpID[LEN_ID];
	char	aName[LEN_NSTMPNAME];
};

// 騒音源テンプレートファイルレコード
struct DbNsTmpRec {
	long	nNsTmpID;
	CString	sName;
};

// 騒音源テンプレートデータ
struct NsTmpData {
	float fPhi0Standard;
	float fPhi0UpperLimit;
	float fPhi0LowerLimit;
	float fTaueStandard;
	float fTaueUpperLimit;
	float fTaueLowerLimit;
	float fTau1Standard;
	float fTau1UpperLimit;
	float fTau1LowerLimit;
	float fPhi1Standard;
	float fPhi1UpperLimit;
	float fPhi1LowerLimit;
	BOOL bPhi0UpperCheck;
	BOOL bPhi0LowerCheck;
	BOOL bTaueUpperCheck;
	BOOL bTaueLowerCheck;
	BOOL bTau1UpperCheck;
	BOOL bTau1LowerCheck;
	BOOL bPhi1UpperCheck;
	BOOL bPhi1LowerCheck;
	int nSum;
	float fPhi0Sum2;
	float fTaueSum2;
	float fTau1Sum2;
	float fPhi1Sum2;

	// 以下追加分
	float fIACCStandard;
	float fIACCUpperLimit;
	float fIACCLowerLimit;
	float fTIACCStandard;
	float fTIACCUpperLimit;
	float fTIACCLowerLimit;
	float fWIACCStandard;
	float fWIACCUpperLimit;
	float fWIACCLowerLimit;
	BOOL bIACCUpperCheck;
	BOOL bIACCLowerCheck;
	BOOL bTIACCUpperCheck;
	BOOL bTIACCLowerCheck;
	BOOL bWIACCUpperCheck;
	BOOL bWIACCLowerCheck;
	float fIACCSum2;
	float fTIACCSum2;
	float fWIACCSum2;
};

// 騒音源同定重み係数データ
struct NsWeightData {
	float fPhi0;
	float fTaue;
	float fTau1;
	float fPhi1;

	// 以下追加分
	float fIACC;
	float fTIACC;
	float fWIACC;
};

// 音響パラメータ計算条件
struct DbAcParamCond {
	long	nFreqBand;
	long	bAFilter;
	long	nSplRefData;
	double	fSplRefLevel;
	double	fDT1MinTime;
	double	fTsubEnd;
	long	bTsubAuto;
	double	fTsubNoise;
	double	fWIACCLevel;
	double	fPrefSPL;
	double	fPrefTauE;
	double	fGRefData;
	double	fTCustom1;
	double	fTCustom2;
	double	fCCustom;
};

// 音響パラメータ計算結果データ
struct DbAcParamResult {
	double	fDeltaT0L;
	double	fDeltaT0R;
	double	fDeltaT1L;
	double	fDeltaT1R;
	double	fSTI_L;
	double	fSTI_R;
	double	fSTIM_L;
	double	fSTIM_R;
	double	fSTIF_L;
	double	fSTIF_R;
	double	fRASTI_L;
	double	fRASTI_R;
};

// 音響パラメータ帯域別計算結果データ
struct DbAcParamData {
	long	nFreq;
	double	fSplL;
	double	fSplR;
	double	fAL;
	double	fAR;
	double	tSubL;
	double	tSubR;
	double	fIACC;
	double	tIACC;
	double	wIACC;
	double	fG_L;
	double	fG_R;
	double	fT20_L;
	double	fT20_R;
	double	fT30_L;
	double	fT30_R;
	double	fTCustom_L;
	double	fTCustom_R;
	double	fEDT_L;
	double	fEDT_R;
	double	fC50_L;
	double	fC50_R;
	double	fC80_L;
	double	fC80_R;
	double	fCCustom_L;
	double	fCCustom_R;
	double	fD50_L;
	double	fD50_R;
	double	fTs_L;
	double	fTs_R;
	double	fIACCE;
	double	fIACCL;
};

// 音響パラメータ計算結果ファイルバッファ
struct DbAcParamBuf {
	char	aImpulseID[LEN_ID];
	char	aConditionSize[LEN_SIZE];
	char	aDataSize[LEN_SIZE];
	char	aDataNum[LEN_NDATA];
};

// 音響パラメータ計算結果ファイルレコード
struct DbAcParamRec {
	long	nImpulseID;
	long	nConditionSize;
	long	nDataSize;
	long	nDataNum;
	DbAcParamCond dbAcParamCond;
	DbAcParamResult dbAcParamResult;
	DbAcParamData dbAcParamData[ACPARAM_NFREQ];
};

// Running ACF 計算結果ファイルバッファ
struct DbAcfFactorBuf {
	char	aAcfID[LEN_ID];
	char	aConditionSize[LEN_SIZE];
	char	aFactorSize[LEN_SIZE];
	char	aAcfFactor[LEN_NACFFACT];
};

// Running ACF 計算結果ファイルレコード
struct DbAcfFactorRec {
	long	nAcfID;
	long	nConditionSize;
	long	nFactorSize;
	long	nAcfFactor;
};

// Running ACF 計算条件データ（共通部）
struct AcfCondition {
	double fIntegrationTime;
	double fRunningStep;
	int nFreqWeighting;
	double fMaxDelayTime;
	long nPhi0RefData;
	double fPhi0RefLevel;
	double fTaueInterval;
	double fTaueEndLevel;
	double fTaueEndTime;
	short bTaueOrigin;
	short nTau1Peak;
};

// Running IACF 計算条件データ（共通部）
struct IAcfCondition {
	double fIACCWLevel;
};

// Running ACF 計算条件データ
struct AcfConditionData {
	long	nStartPos;
	long	nDataNum;
	AcfCondition AcfCondition;
	IAcfCondition IAcfCondition;
};

// Running ACF 計算結果データ
struct AcfFactor {
	float	fPhi0;
	float	fTauE;
	float	fTau1;
	float	fPhi1;
	float	fTauE0;
};

// Running IACF 計算結果データ
struct IAcfFactor {
	float	fSPL;
	float	fIACC;
	float	fTauIACC;
	float	fWIACC;
	float	fWIACC1;
};

// Running IACF マーカーバッファ
struct DbAcfMarkerBuf {
	char	aMarkerID[LEN_ID];
	char	aAcfID[LEN_ID];
	char	aTime[LEN_FLOAT];
	char	aTitle[LEN_TITLE];
	char	aComment[LEN_COMMENT];
};

// Running IACF マーカーレコード
struct DbAcfMarkerRec {
	long	nMarkerID;
	long	nAcfID;
	double	fTime;
	CString	sTitle;
	CString	sComment;
};

// 騒音源データ
struct NoiseSrcData {
	float	fdPhi0;
	float	fdTauE;
	float	fdTau1;
	float	fdPhi1;
	char	aName[LEN_NSTMPNAME];
	float	fdIACC;
	float	fdTIACC;
	float	fdWIACC;
};

// ACF,IACF計算結果データ
struct AllFactorData {
	AcfFactor	Acf;
	IAcfFactor	IAcf;
};
typedef struct AllFactorData AcfFactorData;
typedef struct AllFactorData NmsFactorData;

// τn, φn 計算結果データ
struct TaunPhinData {
	float	fTaun[10];
	float	fPhin[10];
};

// マイク周波数特性データ
struct MicFreq {
	double	freq;
	double	level;
};

// 騒音計測マイク較正データ
struct NmsMicCal {
	double fInputSens;
	long nFreqData;
	MicFreq freq[MICCAL_NFREQ];
};

// 騒音計算条件データ
struct NmsConditionData {
	long	nStartPos;
	long	nDataNum;
	AcfCondition AcfCondition;
	IAcfCondition IAcfCondition;
	NmsMicCal NmsMicCal;
};

// 騒音計測同定条件
struct IdentCondition {
	int nIdentMode;
	int nBeforeStep;
	int nAfterStep;
};

// 逆フィルタファイルバッファ
struct DbIFilterBuf {
	char	aIFilterID[LEN_ID];
	char	aTitle[LEN_TITLE];
	char	aComment[LEN_COMMENT];
	char	aSampling[LEN_SAMPLING];
	char	aStartPos[LEN_FLOAT];
	char	aEndPos[LEN_FLOAT];
	char	aMaxLevel[LEN_MAXLEVEL];
	char	aPhaseAdj[LEN_BOOL];
};

// 逆フィルタファイルレコード
struct DbIFilterRec {
	long	nIFilterID;
	CString	sTitle;
	CString	sComment;
	long	nSampling;
	double	fStartPos;
	double	fEndPos;
	long	nMaxLevel;
	long	nPhaseAdj;
};

// マイク較正ファイルバッファ
struct DbMicCalBuf {
	char	aMicCalID[LEN_ID];
	char	aMicInfo[LEN_MICINFO];
	char	aMicAmpInfo[LEN_MICAMPINFO];
	char	aComment[LEN_COMMENT];
	char	aInputSens[LEN_FLOAT];
	char	aInputSel[LEN_SHORT];
	char	aInputVol[LEN_SHORT];
	char	aFreqData[LEN_SHORT];
	struct	{
		char	aFreq[LEN_FLOAT];
		char	aFevel[LEN_FLOAT];
	} aFreq[MICCAL_NFREQ];
};

// マイク較正ファイルレコード
struct DbMicCalRec {
	long	nMicCalID;
	CString	sMicInfo;
	CString	sMicAmpInfo;
	CString	sComment;
	double fInputSens;
	long nInputSel;
	long nInputVol;
	long nFreqData;
	MicFreq aFreq[MICCAL_NFREQ];
};

// プリセットファイルバッファ
struct DbPresetBuf {
	char	aPresetID[LEN_ID];
	char	aTitle[LEN_TITLE];
	char	aOpenWindow[LEN_SHORT];
};

// プリセットファイルレコード
struct DbPresetRec {
	long	nPresetID;
	CString sTitle;
	long	nOpenWindow;
};

// 波形データファイルバッファ
struct DbWaveFormBuf {
	char	aWaveFormID[LEN_ID];
	char	aSmoothFlag[LEN_BOOL];
	char	aDataNum[LEN_WAVEFORMNUM];
};

// 波形データファイルレコード
struct DbWaveFormRec {
	long	nWaveFormID;
	long	nSmoothFlag;
	long	nDataNum;
};

// 波形データ
struct WaveFormData {
	float	x;
	float	y;
};

#pragma pack()

extern char g_szDefaultDir[];
extern class CMyPcserve g_oPcserve;

// PCSERVE クラス
class CMyPcserve : public CPcserve
{
public:
	void Initialize();
};

// フォルダファイルクラス
class CDbFolder : public CPSDB
{
public:
	BOOL Open(LPCSTR dir = g_szDefaultDir);
	BOOL GetNewID(long *pID);
	BOOL ReadRecID(long id, DbFolderRec *pDbRec);
	BOOL ReadRecNext(DbFolderRec *pDbRec);
	BOOL StoreRec(DbFolderRec *pDbRec);
	BOOL UpdateRec(DbFolderRec *pDbRec);
};

// インパルス応答測定データファイルクラス
class CDbImpulse : public CPSDB
{
public:
	BOOL Open(LPCSTR dir = g_szDefaultDir);
	BOOL GetNewID(long *pImpulseID);
	BOOL ReadRecID(long id, DbImpulseRec *pDbRec);
	BOOL ReadRecNext(DbImpulseRec *pDbRec);
	BOOL StoreRec(DbImpulseRec *pDbRec);
	BOOL UpdateRec(DbImpulseRec *pDbRec);

	int m_nIdxFolderID;
	int m_nIdxImpulseID;
};

// Runnning ACF 測定データファイルクラス
class CDbAcf : public CPSDB
{
public:
	BOOL Open(LPCSTR dir = g_szDefaultDir);
	BOOL GetNewID(long *pAcfID);
	BOOL ReadRecID(long id, DbAcfRec *pDbRec);
	BOOL ReadRecNext(DbAcfRec *pDbRec);
	BOOL StoreRec(DbAcfRec *pDbRec);
	BOOL UpdateRec(DbAcfRec *pDbRec);

	int m_nIdxFolderID;
	int m_nIdxAcfID;
};

// 騒音測定データファイルクラス
class CDbNms : public CPSDB
{
public:
	BOOL Open(LPCSTR dir = g_szDefaultDir);
	BOOL GetNewID(long *pNmsID);
	BOOL ReadRecID(long id, DbNmsRec *pDbRec, NmsConditionData *pNmsConditionData, NmsFactorData **pNmsFactorData, NoiseSrcData *pNoiseSrcData);
	BOOL ReadRecNext(DbNmsRec *pDbRec);
	BOOL StoreRec(DbNmsRec *pDbRec);
	BOOL UpdateRec(DbNmsRec *pDbRec);

	int m_nIdxFolderID;
	int m_nIdxNmsID;
};

// 騒音源テンプレートファイルクラス
class CDbNsTmp : public CPSDB
{
public:
	BOOL Open(LPCSTR dir = g_szDefaultDir);
	BOOL GetNewID(long *pNsTmpID);
	BOOL ReadRecID(long id, DbNsTmpRec *pDbRec, NsTmpData *pNsTmpData);
	BOOL ReadRecNext(DbNsTmpRec *pDbRec, NsTmpData *pNsTmpData);
	BOOL StoreRec(DbNsTmpRec *pDbRec, NsTmpData *pNsTmpData);
	BOOL UpdateRec(DbNsTmpRec *pDbRec, NsTmpData *pNsTmpData);
	BOOL DeleteRec(long id);

	int m_nIdxNsTmpID;
	int m_nIdxNsTmpName;
};

// 音響パラメータ計算結果ファイルクラス
class CDbAcParam : public CPSDB
{
public:
	BOOL Open(LPCSTR dir = g_szDefaultDir);
	BOOL ReadRecID(long id, DbAcParamRec *pDbRec);
	BOOL SearchID(long id, BOOL bDispErr = TRUE);
	BOOL StoreRec(DbAcParamRec *pDbRec);
	BOOL UpdateRec(DbAcParamRec *pDbRec);
};

// Running ACF 計算結果ファイルクラス
class CDbAcfFactor : public CPSDB
{
public:
	BOOL Open(LPCSTR dir = g_szDefaultDir);
	BOOL ReadRecID(long id, DbAcfFactorRec *pDbRec,
			AcfConditionData *pAcfConditionData = NULL, AcfFactorData **pAcfFactorData = NULL, TaunPhinData **pTaunPhinData = NULL);
	BOOL SearchID(long id);
	BOOL StoreRec(DbAcfFactorRec *pDbRec);
	BOOL UpdateRec(DbAcfFactorRec *pDbRec);
};

// Runnning ACF マーカークラス
class CDbAcfMarker : public CPSDB
{
public:
	BOOL Open(LPCSTR dir = g_szDefaultDir);
	BOOL GetNewID(long *pID);
	BOOL ReadRecID(long id, DbAcfMarkerRec *pDbRec);
	BOOL ReadRecNext(DbAcfMarkerRec *pDbRec);
	BOOL StoreRec(DbAcfMarkerRec *pDbRec);
	BOOL UpdateRec(DbAcfMarkerRec *pDbRec);
	BOOL DeleteRec(long id);
	BOOL ReadRecAcfID(long id, DbAcfMarkerRec **pDbRec, int &nDataNum);

	int m_nIdxMarkerID;
	int m_nIdxAcfID;
};

// 逆フィルタファイルクラス
class CDbIFilter : public CPSDB
{
public:
	BOOL Open(LPCSTR dir = g_szDefaultDir);
	BOOL GetNewID(long *pID);
	BOOL ReadRecID(long id, DbIFilterRec *pDbRec);
	BOOL ReadRecNext(DbIFilterRec *pDbRec);
	BOOL StoreRec(DbIFilterRec *pDbRec);
	BOOL UpdateRec(DbIFilterRec *pDbRec);
	BOOL DeleteID(long id);
};

// マイク較正ファイルクラス
class CDbMicCal : public CPSDB
{
public:
	BOOL Open(LPCSTR dir = g_szDefaultDir);
	BOOL GetNewID(long *pID);
	BOOL ReadRecID(long id, DbMicCalRec *pDbRec);
	BOOL ReadRecNext(DbMicCalRec *pDbRec);
	BOOL StoreRec(DbMicCalRec *pDbRec);
	BOOL UpdateRec(DbMicCalRec *pDbRec);
	BOOL DeleteID(long id);
	BOOL SearchRecID(long id);
};

// プリセットファイルクラス
class CDbPreset : public CPSDB
{
public:
	BOOL Open(LPCSTR dir = g_szDefaultDir);
	BOOL GetNewID(long *pID);
	BOOL ReadRecID(long id, DbPresetRec *pDbRec);
	BOOL ReadRecNext(DbPresetRec *pDbRec);
	BOOL StoreRec(DbPresetRec *pDbRec);
	BOOL UpdateRec(DbPresetRec *pDbRec);
	BOOL DeleteID(long id);
};

// 波形データファイルクラス
class CDbWaveForm : public CPSDB
{
public:
	BOOL Open(LPCSTR dir = g_szDefaultDir);
	BOOL ReadRecID(long id, DbWaveFormRec *pDbRec, WaveFormData *pWaveFormData);
	BOOL WriteRec(DbWaveFormRec *pDbRec, WaveFormData *pWaveFormData);
};

extern void DeleteFolder(long nFolderID);
extern void DeleteImpulse(long nImpulseID);
extern void DeleteAcf(long nAcfID);
extern void DeleteNms(long nNmsID);
extern CString FormatTime(const CString &src);
extern CString UnformatTime(const CString &src);
extern void ReadNsWeightData(NsWeightData *pNsWeightData);
extern void WriteNsWeightData(const NsWeightData *pNsWeightData);
extern void ImportIR(LPCSTR pDir);
extern void ImportACF(LPCSTR pDir);
