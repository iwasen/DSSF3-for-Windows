// CalcParamDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Sas.h"
#include "CalcImpDlg.h"
#include "Calc.h"
#include "Help\ContextHelp.h"

/////////////////////////////////////////////////////////////////////////////
// CCalcImpDlg ダイアログ

CCalcImpDlg::CCalcImpDlg(CWnd* pParent /*=NULL*/)
	: CDialogAF(CCalcImpDlg::IDD, pParent)
	, m_dGRefData(0)
	, m_dTCustom1(0)
	, m_dTCustom2(0)
	, m_dCCustom(0)
{
	m_iCalcData = 0;
	m_dTsubEnd = 0.0;
	m_dTsubNoise = 0.0;
	m_dDT1MinTime = 0.0;
	m_iFreqBand = -1;
	m_bTsubAuto = FALSE;
	m_dWIACCLevel = 0.0;
	m_dPrefSPL = 0.0;
	m_dPrefTauE = 0.0;
	m_dSplRefLevel = 0.0;
	m_bAFilter = FALSE;
}

void CCalcImpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TSUB_END, m_cTsubEnd);
	DDX_Control(pDX, IDC_SPL_REFLEVEL, m_cSplRefLevel);
	DDX_Control(pDX, IDC_SPL_REFDATA, m_cSplRefData);
	DDX_Control(pDX, IDC_END_FREQ, m_cEndFreq);
	DDX_Control(pDX, IDC_START_FREQ, m_cStartFreq);
	DDX_Control(pDX, IDC_CALC_NO, m_cCalcNo);
	DDX_Control(pDX, IDC_CALC_SELECT, m_cCalcSelect);
	DDX_Control(pDX, IDC_CALC_ALL, m_cCalcAll);
	DDX_Control(pDX, IDC_CALC_MEASUREMENT, m_cCalcMeasurement);
	DDX_Radio(pDX, IDC_CALC_NO, m_iCalcData);
	DDX_Text(pDX, IDC_TSUB_END, m_dTsubEnd);
	DDX_Text(pDX, IDC_TSUB_NOISE, m_dTsubNoise);
	DDX_Text(pDX, IDC_DT1_MINTIME, m_dDT1MinTime);
	DDX_Radio(pDX, IDC_1OCT, m_iFreqBand);
	DDX_Check(pDX, IDC_TSUB_AUTO, m_bTsubAuto);
	DDX_Text(pDX, IDC_IACC_WLEVEL, m_dWIACCLevel);
	DDX_Text(pDX, IDC_PREF_SPL, m_dPrefSPL);
	DDX_Text(pDX, IDC_PREF_TAUE, m_dPrefTauE);
	DDX_Text(pDX, IDC_SPL_REFLEVEL, m_dSplRefLevel);
	DDX_Check(pDX, IDC_AFILTER, m_bAFilter);
	DDX_Text(pDX, IDC_G_REFLEVEL, m_dGRefData);
	DDX_Text(pDX, IDC_T_CUSTOM1, m_dTCustom1);
	DDX_Text(pDX, IDC_T_CUSTOM2, m_dTCustom2);
	DDX_Text(pDX, IDC_C_CUSTOM, m_dCCustom);
}

BEGIN_MESSAGE_MAP(CCalcImpDlg, CDialogAF)
	ON_BN_CLICKED(IDC_1OCT, On1oct)
	ON_BN_CLICKED(IDC_3OCT, On3oct)
	ON_CBN_SELCHANGE(IDC_START_FREQ, OnSelchangeStartFreq)
	ON_CBN_SELCHANGE(IDC_END_FREQ, OnSelchangeEndFreq)
	ON_CBN_SELCHANGE(IDC_SPL_REFDATA, OnSelchangeSplRefdata)
	ON_BN_CLICKED(IDC_TSUB_AUTO, OnTsubAuto)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCalcImpDlg メッセージ ハンドラ

BOOL CCalcImpDlg::OnInitDialog()
{
	m_iFreqBand = m_CalcParam.nFreqBand;
	m_bAFilter = m_CalcParam.bAFilter;
	m_dSplRefLevel = m_CalcParam.fSplRefLevel;
	m_dDT1MinTime = m_CalcParam.fDT1MinTime;
	m_dTsubEnd = m_CalcParam.fTsubEnd;
	m_bTsubAuto = m_CalcParam.bTsubAuto;
	m_dTsubNoise = m_CalcParam.fTsubNoise * 100;
	m_dWIACCLevel = m_CalcParam.fWIACCLevel;
	m_dPrefSPL = m_CalcParam.fPrefSPL;
	m_dPrefTauE = m_CalcParam.fPrefTauE;
	m_dGRefData = m_CalcParam.fGRefData;
	m_dTCustom1 = m_CalcParam.fTCustom1;
	m_dTCustom2 = m_CalcParam.fTCustom2;
	m_dCCustom = m_CalcParam.fCCustom;

	CDialogAF::OnInitDialog();

	SetSplRefData();

	SetFreq();

	OnTsubAuto();

	if (m_nFolderID == -2) {
		m_cCalcNo.ShowWindow(SW_HIDE);
		m_cCalcSelect.ShowWindow(SW_HIDE);
		m_cCalcAll.ShowWindow(SW_HIDE);
		m_cCalcMeasurement.SetCheck(1);
	} else {
		m_cCalcMeasurement.ShowWindow(SW_HIDE);
	}

	return TRUE;
}

void CCalcImpDlg::OnOK()
{
	UpdateData(TRUE);
	m_CalcParam.nFreqBand = m_iFreqBand;
	m_CalcParam.bAFilter = m_bAFilter;
	m_CalcParam.nSplRefData = GetSplRefData();
	m_CalcParam.fSplRefLevel = m_dSplRefLevel;
	m_CalcParam.fDT1MinTime = m_dDT1MinTime;
	m_CalcParam.fTsubEnd = m_dTsubEnd;
	m_CalcParam.bTsubAuto = m_bTsubAuto;
	m_CalcParam.fTsubNoise = m_dTsubNoise / 100;
	m_CalcParam.fWIACCLevel = m_dWIACCLevel;
	m_CalcParam.fPrefSPL = m_dPrefSPL;
	m_CalcParam.fPrefTauE = m_dPrefTauE;
	m_CalcParam.fGRefData = m_dGRefData;
	m_CalcParam.fTCustom1 = m_dTCustom1;
	m_CalcParam.fTCustom2 = m_dTCustom2;
	m_CalcParam.fCCustom = m_dCCustom;

	EndDialog(IDOK);
}

void CCalcImpDlg::SetFreq()
{
	m_cStartFreq.ResetContent();
	m_cEndFreq.ResetContent();

	CString str;
	str.LoadString(IDS_NONE);
	m_cStartFreq.AddString(str);

	const FREQBAND *pFilter = &g_tFilterTbl[m_iFreqBand];
	for (int i = 1; i < pFilter->nData; i++) {
		str.Format("%lg", pFilter->fDispFreq[i]);
		m_cStartFreq.AddString(str);
		m_cEndFreq.AddString(str);
	}

	if (m_CalcParam.nStartFreq > pFilter->nData)
		m_CalcParam.nStartFreq = pFilter->nData - 1;

	if (m_CalcParam.nEndFreq > pFilter->nData - 1)
		m_CalcParam.nEndFreq = pFilter->nData - 2;

	m_cStartFreq.SetCurSel(m_CalcParam.nStartFreq);
	m_cEndFreq.SetCurSel(m_CalcParam.nEndFreq);

	m_cEndFreq.EnableWindow(m_CalcParam.nStartFreq != 0);
}

void CCalcImpDlg::On1oct()
{
	UpdateData(TRUE);
	SetFreq();
}

void CCalcImpDlg::On3oct()
{
	UpdateData(TRUE);
	SetFreq();
}

void CCalcImpDlg::OnSelchangeStartFreq()
{
	m_CalcParam.nStartFreq = m_cStartFreq.GetCurSel();
	m_cEndFreq.EnableWindow(m_CalcParam.nStartFreq != 0);
}

void CCalcImpDlg::OnSelchangeEndFreq()
{
	m_CalcParam.nEndFreq = m_cEndFreq.GetCurSel();
}

void CCalcImpDlg::SetSplRefData()
{
	DbImpulseRec dbImpulseRec;
	CString str;
	LONG nRec;
	int index = 0;

	str.LoadString(IDS_ABSVALUE);
	m_cSplRefData.InsertString(index, str);
	m_cSplRefData.SetItemData(index, (DWORD_PTR)SPL_ABSOLUTE);
	if (m_CalcParam.nSplRefData == SPL_ABSOLUTE)
		m_cSplRefData.SetCurSel(index);
	index++;

	str.LoadString(IDS_MAXVALUE);
	m_cSplRefData.InsertString(index, str);
	m_cSplRefData.SetItemData(index, (DWORD_PTR)SPL_MAXREF);
	if (m_CalcParam.nSplRefData == SPL_MAXREF)
		m_cSplRefData.SetCurSel(index);
	index++;

	CDbImpulse dbImpulse;
	if (!dbImpulse.Open())
		return;

	dbImpulse.DBChgIdx(dbImpulse.m_nIdxImpulseID);
	str.Format("#%d=%ld", dbImpulse.m_nIdxFolderID, m_nFolderID);
	dbImpulse.DBSelect(str, &nRec);

	for (;;) {
		if (!dbImpulse.ReadRecNext(&dbImpulseRec))
			break;

		m_cSplRefData.InsertString(index, dbImpulseRec.sTitle);
		m_cSplRefData.SetItemData(index, dbImpulseRec.nImpulseID);
		if (m_CalcParam.nSplRefData == dbImpulseRec.nImpulseID)
			m_cSplRefData.SetCurSel(index);
		index++;
	}

	OnSelchangeSplRefdata();
}

long CCalcImpDlg::GetSplRefData()
{
	return (long)m_cSplRefData.GetItemData(m_cSplRefData.GetCurSel());
}

void CCalcImpDlg::OnSelchangeSplRefdata()
{
	int index = m_cSplRefData.GetCurSel();
	m_cSplRefLevel.EnableWindow(m_cSplRefData.GetItemData(index) != SPL_ABSOLUTE);
}

void CCalcImpDlg::OnTsubAuto()
{
	UpdateData(TRUE);
	m_cTsubEnd.EnableWindow(!m_bTsubAuto);
}

BOOL CCalcImpDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static const UINT aIDs[] = {
		IDOK, IDH_CALC_IMP_OK,
		IDCANCEL, IDH_CALC_IMP_CANCEL,
		IDC_CALC_NO, IDH_CALC_IMP_CALC_NO,
		IDC_CALC_SELECT, IDH_CALC_IMP_CALC_SELECT,
		IDC_CALC_ALL, IDH_CALC_IMP_CALC_ALL,
		IDC_1OCT, IDH_CALC_IMP_1OCT,
		IDC_3OCT, IDH_CALC_IMP_3OCT,
		IDC_START_FREQ, IDH_CALC_IMP_START_FREQ,
		IDC_END_FREQ, IDH_CALC_IMP_END_FREQ,
		IDC_AFILTER, IDH_CALC_IMP_AFILTER,
		IDC_SPL_REFDATA, IDH_CALC_IMP_SPL_REFDATA,
		IDC_SPL_REFLEVEL, IDH_CALC_IMP_SPL_REFLEVEL,
		IDC_TSUB_END, IDH_CALC_IMP_TSUB_END,
		IDC_TSUB_AUTO, IDH_CALC_IMP_TSUB_AUTO,
		IDC_TSUB_NOISE, IDH_CALC_IMP_TSUB_NOISE,
		IDC_G_REFLEVEL, IDH_CALC_IMP_G_REFLEVEL,
		IDC_DT1_MINTIME, IDH_CALC_IMP_DT1_MINTIME,
		IDC_IACC_WLEVEL, IDH_CALC_IMP_IACC_WLEVEL,
		IDC_PREF_SPL, IDH_CALC_IMP_PREF_SPL,
		IDC_PREF_TAUE, IDH_CALC_IMP_PREF_TAUE,
		IDC_T_CUSTOM1, IDH_CALC_IMP_T_CUSTOM1,
		IDC_T_CUSTOM2, IDH_CALC_IMP_T_CUSTOM2,
		IDC_C_CUSTOM, IDH_CALC_IMP_C_CUSTOM,
		0
	};

	DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}
