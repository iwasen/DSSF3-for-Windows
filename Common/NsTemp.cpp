// NsTemp.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Common.h"
#include "FileIO.h"

void UpdateNsWeightData()
{
	DbNsTmpRec dbNsTmpRec;
	NsTmpData nsTmpData;
	NsWeightData nsWeightData;
	float fSd1Phi0, fSd1Taue, fSd1Tau1, fSd1Phi1, fSd1IACC, fSd1TIACC, fSd1WIACC;
	float fSd2Phi0, fSd2Taue, fSd2Tau1, fSd2Phi1, fSd2IACC, fSd2TIACC, fSd2WIACC;
	float fTmpPhi0, fTmpTaue, fTmpTau1, fTmpPhi1, fTmpIACC, fTmpTIACC, fTmpWIACC;
	float fData;
	float fMax;

	// 騒音源テンプレートファイルオープン
	CDbNsTmp dbNsTmp;
	if (!dbNsTmp.Open())
		return;

	// 変数初期化
	fSd1Phi0 = fSd1Taue = fSd1Tau1 = fSd1Phi1 = fSd1IACC = fSd1TIACC = fSd1WIACC = 0;
	fSd2Phi0 = fSd2Taue = fSd2Tau1 = fSd2Phi1 = fSd2IACC = fSd2TIACC = fSd2WIACC = 0;
	fTmpPhi0 = fTmpTaue = fTmpTau1 = fTmpPhi1 = fTmpIACC = fTmpTIACC = fTmpWIACC = 0;
	int nRec = 0;

	// 分散値を求めるために騒音源テンプレートファイルを読む
	for (;;) {
		if (!dbNsTmp.ReadRecNext(&dbNsTmpRec, &nsTmpData))
			break;

		if (nsTmpData.nSum == 0)
			continue;

		nRec++;

		fData = (nsTmpData.fPhi0Sum2 - nsTmpData.nSum * nsTmpData.fPhi0Standard * nsTmpData.fPhi0Standard) / nsTmpData.nSum;
		fData = (fData > 0) ? (float)sqrt(fData) : 0;
		fSd1Phi0 += fData;

		fData = (nsTmpData.fTaueSum2 - nsTmpData.nSum * nsTmpData.fTaueStandard * nsTmpData.fTaueStandard) / nsTmpData.nSum;
		fData = (fData > 0) ? (float)sqrt(fData) : 0;
		fSd1Taue += fData;

		fData = (nsTmpData.fTau1Sum2 - nsTmpData.nSum * nsTmpData.fTau1Standard * nsTmpData.fTau1Standard) / nsTmpData.nSum;
		fData = (fData > 0) ? (float)sqrt(fData) : 0;
		fSd1Tau1 += fData;

		fData = (nsTmpData.fPhi1Sum2 - nsTmpData.nSum * nsTmpData.fPhi1Standard * nsTmpData.fPhi1Standard) / nsTmpData.nSum;
		fData = (fData > 0) ? (float)sqrt(fData) : 0;
		fSd1Phi1 += fData;

		fData = (nsTmpData.fIACCSum2 - nsTmpData.nSum * nsTmpData.fIACCStandard * nsTmpData.fIACCStandard) / nsTmpData.nSum;
		fData = (fData > 0) ? (float)sqrt(fData) : 0;
		fSd1IACC += fData;

		fData = (nsTmpData.fTIACCSum2 - nsTmpData.nSum * nsTmpData.fTIACCStandard * nsTmpData.fTIACCStandard) / nsTmpData.nSum;
		fData = (fData > 0) ? (float)sqrt(fData) : 0;
		fSd1TIACC += fData;

		fData = (nsTmpData.fWIACCSum2 - nsTmpData.nSum * nsTmpData.fWIACCStandard * nsTmpData.fWIACCStandard) / nsTmpData.nSum;
		fData = (fData > 0) ? (float)sqrt(fData) : 0;
		fSd1WIACC += fData;

		fTmpPhi0 += nsTmpData.fPhi0Standard;
		fTmpTaue += nsTmpData.fTaueStandard;
		fTmpTau1 += nsTmpData.fTau1Standard;
		fTmpPhi1 += nsTmpData.fPhi1Standard;
		fTmpIACC += nsTmpData.fIACCStandard;
		fTmpTIACC += nsTmpData.fTIACCStandard;
		fTmpWIACC += nsTmpData.fWIACCStandard;

		fSd2Phi0 += nsTmpData.fPhi0Standard * nsTmpData.fPhi0Standard;
		fSd2Taue += nsTmpData.fTaueStandard * nsTmpData.fTaueStandard;
		fSd2Tau1 += nsTmpData.fTau1Standard * nsTmpData.fTau1Standard;
		fSd2Phi1 += nsTmpData.fPhi1Standard * nsTmpData.fPhi1Standard;
		fSd2IACC += nsTmpData.fIACCStandard * nsTmpData.fIACCStandard;
		fSd2TIACC += nsTmpData.fTIACCStandard * nsTmpData.fTIACCStandard;
		fSd2WIACC += nsTmpData.fWIACCStandard * nsTmpData.fWIACCStandard;
	}

	// 同一騒音源の分散の平均を求める
	fSd1Phi0 /= nRec;
	fSd1Taue /= nRec;
	fSd1Tau1 /= nRec;
	fSd1Phi1 /= nRec;
	fSd1IACC /= nRec;
	fSd1TIACC /= nRec;
	fSd1WIACC /= nRec;

	// 異なる騒音源間の分散を求める
	fSd2Phi0 = (fSd2Phi0 - fTmpPhi0 * fTmpPhi0 / nRec) / nRec;
	fSd2Phi0 = (fSd2Phi0 > 0) ? (float)sqrt(fSd2Phi0) : 0;
	fSd2Taue = (fSd2Taue - fTmpTaue * fTmpTaue / nRec) / nRec;
	fSd2Taue = (fSd2Taue > 0) ? (float)sqrt(fSd2Taue) : 0;
	fSd2Tau1 = (fSd2Tau1 - fTmpTau1 * fTmpTau1 / nRec) / nRec;
	fSd2Tau1 = (fSd2Tau1 > 0) ? (float)sqrt(fSd2Tau1) : 0;
	fSd2Phi1 = (fSd2Phi1 - fTmpPhi1 * fTmpPhi1 / nRec) / nRec;
	fSd2Phi1 = (fSd2Phi1 > 0) ? (float)sqrt(fSd2Phi1) : 0;
	fSd2IACC = (fSd2IACC - fTmpIACC * fTmpIACC / nRec) / nRec;
	fSd2IACC = (fSd2IACC > 0) ? (float)sqrt(fSd2IACC) : 0;
	fSd2TIACC = (fSd2TIACC - fTmpTIACC * fTmpTIACC / nRec) / nRec;
	fSd2TIACC = (fSd2TIACC > 0) ? (float)sqrt(fSd2TIACC) : 0;
	fSd2WIACC = (fSd2WIACC - fTmpWIACC * fTmpWIACC / nRec) / nRec;
	fSd2WIACC = (fSd2WIACC > 0) ? (float)sqrt(fSd2WIACC) : 0;

	// 重み係数ファイル読み込み
	::ReadNsWeightData(&nsWeightData);

	// sqrt(異なる騒音源間の分散／同一騒音源の分散）を求める
//	nsWeightData.fPhi0 = (float)sqrt((fSd1Phi0 != 0) ? fSd2Phi0 / fSd1Phi0 : 1);
	nsWeightData.fTaue = (float)sqrt((fSd1Taue != 0) ? fSd2Taue / fSd1Taue : 1);
	nsWeightData.fTau1 = (float)sqrt((fSd1Tau1 != 0) ? fSd2Tau1 / fSd1Tau1 : 1);
	nsWeightData.fPhi1 = (float)sqrt((fSd1Phi1 != 0) ? fSd2Phi1 / fSd1Phi1 : 1);
	nsWeightData.fIACC = (float)sqrt((fSd1IACC != 0) ? fSd2IACC / fSd1IACC : 1);
	nsWeightData.fTIACC = (float)sqrt((fSd1TIACC != 0) ? fSd2TIACC / fSd1TIACC : 1);
	nsWeightData.fWIACC = (float)sqrt((fSd1WIACC != 0) ? fSd2WIACC / fSd1WIACC : 1);

	// 最大の係数を求める
//	fMax = nsWeightData.fPhi0;
	fMax = 0;
	if (nsWeightData.fTaue > fMax)
		fMax = nsWeightData.fTaue;
	if (nsWeightData.fTau1 > fMax)
		fMax = nsWeightData.fTau1;
	if (nsWeightData.fPhi1 > fMax)
		fMax = nsWeightData.fPhi1;
	if (nsWeightData.fIACC > fMax)
		fMax = nsWeightData.fIACC;
	if (nsWeightData.fTIACC > fMax)
		fMax = nsWeightData.fTIACC;
	if (nsWeightData.fWIACC > fMax)
		fMax = nsWeightData.fWIACC;

	// 最大の係数を１として正規化
//	nsWeightData.fPhi0 /= fMax;
	nsWeightData.fTaue /= fMax;
	nsWeightData.fTau1 /= fMax;
	nsWeightData.fPhi1 /= fMax;
	nsWeightData.fIACC /= fMax;
	nsWeightData.fTIACC /= fMax;
	nsWeightData.fWIACC /= fMax;

	// 重み係数ファイル書き込み
	::WriteNsWeightData(&nsWeightData);
}
