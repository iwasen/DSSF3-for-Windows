// NmsItem.cpp: CNmsItem クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Nms.h"
#include "NmsItem.h"
#include "CalcAcf.h"
#include "NsTemp.h"
#include "SetData.h"

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

CNmsItem::CNmsItem()
{
	m_nIndex = 0;
	m_bSaved = FALSE;
	m_nStatus = STATUS_INIT;
	m_nDecision = 0;
	m_fDataTime = 0;
	m_nNmsFactorData = 0;
	m_pNmsFactorData = NULL;
	m_nNoiseTmpID = -1;
	m_bLearning = FALSE;
}

CNmsItem::~CNmsItem()
{
	if (m_pNmsFactorData != NULL)
		delete [] m_pNmsFactorData;
}

void CNmsItem::LearningNoiseSrc()
{
	if (m_bLearning)
		return;

	CDbNsTmp dbNsTmp;
	if (!dbNsTmp.Open())
		return;

	DbNsTmpRec dbNsTmpRec;
	NsTmpData nsTmpData;
	if (!dbNsTmp.ReadRecID(m_nNoiseTmpID, &dbNsTmpRec, &nsTmpData))
		return;

	if (nsTmpData.nSum == 0) {
		nsTmpData.fPhi0Sum2 = 0;
		nsTmpData.fTaueSum2 = 0;
		nsTmpData.fTau1Sum2 = 0;
		nsTmpData.fPhi1Sum2 = 0;
		nsTmpData.fIACCSum2 = 0;
		nsTmpData.fTIACCSum2 = 0;
		nsTmpData.fWIACCSum2 = 0;
	}

	const int IdentPos = GetIdentPos(m_pNmsFactorData, m_nNmsFactorData, &g_oSetData.Measurement.IdentCondition);
	AcfFactor acfFactor = m_pNmsFactorData[IdentPos].Acf;
	IAcfFactor iacfFactor = m_pNmsFactorData[IdentPos].IAcf;

	float fData = acfFactor.fPhi0;
	nsTmpData.fPhi0Standard = (nsTmpData.fPhi0Standard * nsTmpData.nSum + fData) / (nsTmpData.nSum + 1);
	nsTmpData.fPhi0Sum2 += fData * fData;

	fData = acfFactor.fTauE;
	nsTmpData.fTaueStandard = (nsTmpData.fTaueStandard * nsTmpData.nSum + fData) / (nsTmpData.nSum + 1);
	nsTmpData.fTaueSum2 += fData * fData;

	fData = acfFactor.fTau1;
	nsTmpData.fTau1Standard = (nsTmpData.fTau1Standard * nsTmpData.nSum + fData) / (nsTmpData.nSum + 1);
	nsTmpData.fTau1Sum2 += fData * fData;

	fData = acfFactor.fPhi1;
	nsTmpData.fPhi1Standard = (nsTmpData.fPhi1Standard * nsTmpData.nSum + fData) / (nsTmpData.nSum + 1);
	nsTmpData.fPhi1Sum2 += fData * fData;

	fData = iacfFactor.fIACC;
	nsTmpData.fIACCStandard = (nsTmpData.fIACCStandard * nsTmpData.nSum + fData) / (nsTmpData.nSum + 1);
	nsTmpData.fIACCSum2 += fData * fData;

	fData = iacfFactor.fTauIACC;
	nsTmpData.fTIACCStandard = (nsTmpData.fTIACCStandard * nsTmpData.nSum + fData) / (nsTmpData.nSum + 1);
	nsTmpData.fTIACCSum2 += fData * fData;

	fData = iacfFactor.fWIACC;
	nsTmpData.fWIACCStandard = (nsTmpData.fWIACCStandard * nsTmpData.nSum + fData) / (nsTmpData.nSum + 1);
	nsTmpData.fWIACCSum2 += fData * fData;

	nsTmpData.nSum++;

	if (!dbNsTmp.UpdateRec(&dbNsTmpRec, &nsTmpData))
		return;

	UpdateNsWeightData();

	m_bLearning = TRUE;
}

void CNmsItem::UnLearningNoiseSrc()
{
	if (!m_bLearning)
		return;

	CDbNsTmp dbNsTmp;
	if (!dbNsTmp.Open())
		return;

	DbNsTmpRec dbNsTmpRec;
	NsTmpData nsTmpData;
	if (!dbNsTmp.ReadRecID(m_nNoiseTmpID, &dbNsTmpRec, &nsTmpData))
		return;

	if (nsTmpData.nSum == 0)
		return;

	if (nsTmpData.nSum == 1) {
		nsTmpData.fPhi0Sum2 = 0;
		nsTmpData.fTaueSum2 = 0;
		nsTmpData.fTau1Sum2 = 0;
		nsTmpData.fPhi1Sum2 = 0;
		nsTmpData.fIACCSum2 = 0;
		nsTmpData.fTIACCSum2 = 0;
		nsTmpData.fWIACCSum2 = 0;
	} else {
		const int IdentPos = GetIdentPos(m_pNmsFactorData, m_nNmsFactorData, &g_oSetData.Measurement.IdentCondition);
		AcfFactor acfFactor = m_pNmsFactorData[IdentPos].Acf;
		IAcfFactor iacfFactor = m_pNmsFactorData[IdentPos].IAcf;

		float fData = acfFactor.fPhi0;
		nsTmpData.fPhi0Standard = (nsTmpData.fPhi0Standard * nsTmpData.nSum - fData) / (nsTmpData.nSum - 1);
		nsTmpData.fPhi0Sum2 -= fData * fData;

		fData = acfFactor.fTauE;
		nsTmpData.fTaueStandard = (nsTmpData.fTaueStandard * nsTmpData.nSum - fData) / (nsTmpData.nSum - 1);
		nsTmpData.fTaueSum2 -= fData * fData;

		fData = acfFactor.fTau1;
		nsTmpData.fTau1Standard = (nsTmpData.fTau1Standard * nsTmpData.nSum - fData) / (nsTmpData.nSum - 1);
		nsTmpData.fTau1Sum2 -= fData * fData;

		fData = acfFactor.fPhi1;
		nsTmpData.fPhi1Standard = (nsTmpData.fPhi1Standard * nsTmpData.nSum - fData) / (nsTmpData.nSum - 1);
		nsTmpData.fPhi1Sum2 -= fData * fData;

		fData = iacfFactor.fIACC;
		nsTmpData.fIACCStandard = (nsTmpData.fIACCStandard * nsTmpData.nSum - fData) / (nsTmpData.nSum - 1);
		nsTmpData.fIACCSum2 -= fData * fData;

		fData = iacfFactor.fTauIACC;
		nsTmpData.fTIACCStandard = (nsTmpData.fTIACCStandard * nsTmpData.nSum - fData) / (nsTmpData.nSum - 1);
		nsTmpData.fTIACCSum2 -= fData * fData;

		fData = iacfFactor.fWIACC;
		nsTmpData.fWIACCStandard = (nsTmpData.fWIACCStandard * nsTmpData.nSum - fData) / (nsTmpData.nSum - 1);
		nsTmpData.fWIACCSum2 -= fData * fData;
	}

	nsTmpData.nSum--;

	if (!dbNsTmp.UpdateRec(&dbNsTmpRec, &nsTmpData))
		return;

	UpdateNsWeightData();

	m_bLearning = FALSE;
}
