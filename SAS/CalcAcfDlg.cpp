// CalcAcfDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Sas.h"
#include "CalcAcfDlg.h"
#include "Help\ContextHelp.h"

/////////////////////////////////////////////////////////////////////////////
// CCalcAcfDlg ダイアログ

CCalcAcfDlg::CCalcAcfDlg(CWnd* pParent /*=NULL*/)
	: CDialogAF(CCalcAcfDlg::IDD, pParent)
{
	m_dIntegrationTime = 0.0;
	m_dMaxDelayTime = 0.0;
	m_iCalcData = 0;
	m_dTaueInterval = 0.0;
	m_dPhi0RefLevel = 0.0;
	m_dTaueEndLevel = 0.0;
	m_dTaueEndTime = 0.0;
	m_dRunningStep = 0.0;
	m_bTaueOrigin = FALSE;
	m_iTau1Peak = -1;
}

void CCalcAcfDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PHI0_REFLEVEL, m_cPhi0RefLevel);
	DDX_Control(pDX, IDC_PHI0_REFDATA, m_cPhi0RefData);
	DDX_Control(pDX, IDC_FREQ_FILTER, m_cFreqFilter);
	DDX_Control(pDX, IDC_CALC_NO, m_cCalcNo);
	DDX_Control(pDX, IDC_CALC_SELECT, m_cCalcSelect);
	DDX_Control(pDX, IDC_CALC_ALL, m_cCalcAll);
	DDX_Control(pDX, IDC_CALC_MEASUREMENT, m_cCalcMeasurement);
	DDX_Text(pDX, IDC_INTEGRATION_TIME, m_dIntegrationTime);
	DDX_Text(pDX, IDC_MAX_DELAY_TIME, m_dMaxDelayTime);
	DDX_Radio(pDX, IDC_CALC_NO, m_iCalcData);
	DDX_Text(pDX, IDC_TAUE_INTERVAL, m_dTaueInterval);
	DDX_Text(pDX, IDC_PHI0_REFLEVEL, m_dPhi0RefLevel);
	DDX_Text(pDX, IDC_TAUE_ENDLEVEL, m_dTaueEndLevel);
	DDX_Text(pDX, IDC_TAUE_ENDTIME, m_dTaueEndTime);
	DDX_Text(pDX, IDC_RUNNING_STEP, m_dRunningStep);
	DDX_Check(pDX, IDC_TAUE_ORIGIN, m_bTaueOrigin);
	DDX_Radio(pDX, IDC_TAU1_A, m_iTau1Peak);
}

BEGIN_MESSAGE_MAP(CCalcAcfDlg, CDialogAF)
	ON_CBN_SELCHANGE(IDC_PHI0_REFDATA, OnSelchangePhi0Refdata)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCalcAcfDlg メッセージ ハンドラ

BOOL CCalcAcfDlg::OnInitDialog()
{
	m_dIntegrationTime = m_AcfCondition.fIntegrationTime;
	m_dRunningStep = m_AcfCondition.fRunningStep;
	m_dMaxDelayTime = m_AcfCondition.fMaxDelayTime;
	m_dPhi0RefLevel = m_AcfCondition.fPhi0RefLevel;
	m_dTaueInterval = m_AcfCondition.fTaueInterval;
	m_dTaueEndLevel = m_AcfCondition.fTaueEndLevel;
	m_dTaueEndTime = m_AcfCondition.fTaueEndTime;
	m_bTaueOrigin = m_AcfCondition.bTaueOrigin;
	m_iTau1Peak = m_AcfCondition.nTau1Peak;

	CDialogAF::OnInitDialog();

	SetPhi0RefData();

	SetFilterNameList(m_cFreqFilter);
	m_cFreqFilter.SetCurSel(m_AcfCondition.nFreqWeighting);

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

void CCalcAcfDlg::OnOK()
{
	UpdateData(TRUE);

	m_AcfCondition.fIntegrationTime = m_dIntegrationTime;
	m_AcfCondition.fRunningStep = m_dRunningStep;
	m_AcfCondition.fMaxDelayTime = m_dMaxDelayTime;
	m_AcfCondition.nPhi0RefData = GetPhi0RefData();
	m_AcfCondition.fPhi0RefLevel = m_dPhi0RefLevel;
	m_AcfCondition.fTaueInterval = m_dTaueInterval;
	m_AcfCondition.fTaueEndLevel = m_dTaueEndLevel;
	m_AcfCondition.fTaueEndTime = m_dTaueEndTime;
	m_AcfCondition.bTaueOrigin = (short)m_bTaueOrigin;
	m_AcfCondition.nFreqWeighting = m_cFreqFilter.GetCurSel();
	m_AcfCondition.nTau1Peak = (short)m_iTau1Peak;

	CDialogAF::OnOK();
}

void CCalcAcfDlg::SetPhi0RefData()
{
	CString str;
	LONG nRec;
	int index = 0;

	str.LoadString(IDS_ABSVALUE);
	m_cPhi0RefData.InsertString(index, str);
	m_cPhi0RefData.SetItemData(index, (DWORD_PTR)SPL_ABSOLUTE);
	if (m_AcfCondition.nPhi0RefData == SPL_ABSOLUTE)
		m_cPhi0RefData.SetCurSel(index);
	index++;

	str.LoadString(IDS_MAXVALUE);
	m_cPhi0RefData.InsertString(index, str);
	m_cPhi0RefData.SetItemData(index, (DWORD_PTR)SPL_MAXREF);
	if (m_AcfCondition.nPhi0RefData == SPL_MAXREF)
		m_cPhi0RefData.SetCurSel(index);
	index++;

	CDbAcf dbAcf;
	if (!dbAcf.Open())
		return;

	dbAcf.DBChgIdx(dbAcf.m_nIdxAcfID);
	str.Format("#%d=%ld", dbAcf.m_nIdxFolderID, m_nFolderID);
	dbAcf.DBSelect(str, &nRec);

	DbAcfRec dbAcfRec;
	for (;;) {
		if (!dbAcf.ReadRecNext(&dbAcfRec))
			break;

		m_cPhi0RefData.InsertString(index, dbAcfRec.sTitle);
		m_cPhi0RefData.SetItemData(index, dbAcfRec.nAcfID);
		if (m_AcfCondition.nPhi0RefData == dbAcfRec.nAcfID)
			m_cPhi0RefData.SetCurSel(index);
		index++;
	}

	OnSelchangePhi0Refdata();
}

long CCalcAcfDlg::GetPhi0RefData()
{
	return (long)m_cPhi0RefData.GetItemData(m_cPhi0RefData.GetCurSel());
}

void CCalcAcfDlg::OnSelchangePhi0Refdata()
{
	const int index = m_cPhi0RefData.GetCurSel();
	m_cPhi0RefLevel.EnableWindow(m_cPhi0RefData.GetItemData(index) != SPL_ABSOLUTE);
}

BOOL CCalcAcfDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static constexpr UINT aIDs[] = {
		IDOK, IDH_CALC_ACF_IDOK,
		IDCANCEL, IDH_CALC_ACF_IDCANCEL,
		IDC_CALC_NO, IDH_CALC_ACF_CALC_NO,
		IDC_CALC_SELECT, IDH_CALC_ACF_CALC_SELECT,
		IDC_CALC_ALL, IDH_CALC_ACF_CALC_ALL,
		IDC_INTEGRATION_TIME, IDH_CALC_ACF_INTEGRATION_TIME,
		IDC_RUNNING_STEP, IDH_CALC_ACF_RUNNING_STEP,
		IDC_FREQ_FILTER, IDH_CALC_ACF_FREQ_FILTER,
		IDC_MAX_DELAY_TIME, IDH_CALC_ACF_MAX_DELAY_TIME,
		IDC_PHI0_REFDATA, IDH_CALC_ACF_PHI0_REFDATA,
		IDC_PHI0_REFLEVEL, IDH_CALC_ACF_PHI0_REFLEVEL,
		IDC_TAUE_INTERVAL, IDH_CALC_ACF_TAUE_INTERVAL,
		IDC_TAUE_ENDLEVEL, IDH_CALC_ACF_TAUE_ENDLEVEL,
		IDC_TAUE_ENDTIME, IDH_CALC_ACF_TAUE_ENDTIME,
		IDC_TAUE_ORIGIN, IDH_CALC_ACF_TAUE_ORIGIN,
		IDC_TAU1_A, IDH_CALC_ACF_TAU1_A,
		IDC_TAU1_B, IDH_CALC_ACF_TAU1_B,
		0
	};

	::DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}
