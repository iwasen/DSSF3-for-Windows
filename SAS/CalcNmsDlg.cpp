// CalcNmsDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Sas.h"
#include "CalcNmsDlg.h"
#include "Help\ContextHelp.h"

/////////////////////////////////////////////////////////////////////////////
// CCalcNmsDlg ダイアログ

CCalcNmsDlg::CCalcNmsDlg(CWnd* pParent /*=NULL*/)
	: CDialogAF(CCalcNmsDlg::IDD, pParent)
{
	m_dIntegrationTime = 0.0;
	m_dMaxDelayTime = 0.0;
	m_iCalcData = 0;
	m_dTaueInterval = 0.0;
	m_dTaueEndLevel = 0.0;
	m_dTaueEndTime = 0.0;
	m_dRunningStep = 0.0;
	m_bTaueOrigin = FALSE;
	m_iIdentAfterStep = 0;
	m_iIdentBeforeStep = 0;
	m_iIdentMode = -1;
}

void CCalcNmsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IDENT_BEFORE_STEP, m_cIdentBeforeStep);
	DDX_Control(pDX, IDC_IDENT_AFTER_STEP, m_cIdentAfterStep);
	DDX_Control(pDX, IDC_FREQ_FILTER, m_cFreqFilter);
	DDX_Text(pDX, IDC_INTEGRATION_TIME, m_dIntegrationTime);
	DDX_Text(pDX, IDC_MAX_DELAY_TIME, m_dMaxDelayTime);
	DDX_Radio(pDX, IDC_CALC_NO, m_iCalcData);
	DDX_Text(pDX, IDC_TAUE_INTERVAL, m_dTaueInterval);
	DDX_Text(pDX, IDC_TAUE_ENDLEVEL, m_dTaueEndLevel);
	DDX_Text(pDX, IDC_TAUE_ENDTIME, m_dTaueEndTime);
	DDX_Text(pDX, IDC_RUNNING_STEP, m_dRunningStep);
	DDX_Check(pDX, IDC_TAUE_ORIGIN, m_bTaueOrigin);
	DDX_Text(pDX, IDC_IDENT_AFTER_STEP, m_iIdentAfterStep);
	DDX_Text(pDX, IDC_IDENT_BEFORE_STEP, m_iIdentBeforeStep);
	DDX_Radio(pDX, IDC_IDENT_SPL, m_iIdentMode);
}

BEGIN_MESSAGE_MAP(CCalcNmsDlg, CDialogAF)
	ON_BN_CLICKED(IDC_IDENT_SPL, OnIdentSpl)
	ON_BN_CLICKED(IDC_IDENT_TAUE, OnIdentTaue)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCalcNmsDlg メッセージ ハンドラ

BOOL CCalcNmsDlg::OnInitDialog()
{
	m_dIntegrationTime = m_AcfCondition.fIntegrationTime;
	m_dRunningStep = m_AcfCondition.fRunningStep;
	m_dMaxDelayTime = m_AcfCondition.fMaxDelayTime;
	m_dTaueInterval = m_AcfCondition.fTaueInterval;
	m_dTaueEndLevel = m_AcfCondition.fTaueEndLevel;
	m_dTaueEndTime = m_AcfCondition.fTaueEndTime;
	m_bTaueOrigin = m_AcfCondition.bTaueOrigin;
	m_iIdentMode = m_IdentCondition.nIdentMode;
	m_iIdentBeforeStep = m_IdentCondition.nBeforeStep;
	m_iIdentAfterStep = m_IdentCondition.nAfterStep;

	CDialogAF::OnInitDialog();

	SetFilterNameList(m_cFreqFilter);
	m_cFreqFilter.SetCurSel(m_AcfCondition.nFreqWeighting);

	EnableControl();

	return TRUE;
}

void CCalcNmsDlg::OnOK()
{
	UpdateData(TRUE);

	m_AcfCondition.fIntegrationTime = m_dIntegrationTime;
	m_AcfCondition.fRunningStep = m_dRunningStep;
	m_AcfCondition.fMaxDelayTime = m_dMaxDelayTime;
	m_AcfCondition.nPhi0RefData = 0;
	m_AcfCondition.fPhi0RefLevel = 0;
	m_AcfCondition.fTaueInterval = m_dTaueInterval;
	m_AcfCondition.fTaueEndLevel = m_dTaueEndLevel;
	m_AcfCondition.fTaueEndTime = m_dTaueEndTime;
	m_AcfCondition.bTaueOrigin = (short)m_bTaueOrigin;
	m_IdentCondition.nIdentMode = m_iIdentMode;
	m_IdentCondition.nBeforeStep = m_iIdentBeforeStep;
	m_IdentCondition.nAfterStep = m_iIdentAfterStep;
	m_AcfCondition.nFreqWeighting = m_cFreqFilter.GetCurSel();

	CDialogAF::OnOK();
}

void CCalcNmsDlg::OnIdentSpl()
{
	UpdateData(TRUE);
	EnableControl();
}

void CCalcNmsDlg::OnIdentTaue()
{
	UpdateData(TRUE);
	EnableControl();
}

void CCalcNmsDlg::EnableControl()
{
	if (m_iIdentMode == IDENT_MODE_MAXSPL) {
		m_cIdentBeforeStep.EnableWindow(FALSE);
		m_cIdentAfterStep.EnableWindow(FALSE);
	} else if (m_iIdentMode == IDENT_MODE_MINTAUE) {
		m_cIdentBeforeStep.EnableWindow(TRUE);
		m_cIdentAfterStep.EnableWindow(TRUE);
	}
}

BOOL CCalcNmsDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static const UINT aIDs[] = {
		IDOK, IDH_CALC_NMS_OK,
		IDCANCEL, IDH_CALC_NMS_CANCEL,
		IDC_CALC_NO, IDH_CALC_NMS_CALC_NO,
		IDC_CALC_SELECT, IDH_CALC_NMS_CALC_SELECT,
		IDC_CALC_ALL, IDH_CALC_NMS_CALC_ALL,
		IDC_INTEGRATION_TIME, IDH_CALC_NMS_INTEGRATION_TIME,
		IDC_RUNNING_STEP, IDH_CALC_NMS_RUNNING_STEP,
		IDC_FREQ_FILTER, IDH_CALC_NMS_FREQ_FILTER,
		IDC_MAX_DELAY_TIME, IDH_CALC_NMS_MAX_DELAY_TIME,
		IDC_TAUE_INTERVAL, IDH_CALC_NMS_TAUE_INTERVAL,
		IDC_TAUE_ENDLEVEL, IDH_CALC_NMS_TAUE_ENDLEVEL,
		IDC_TAUE_ENDTIME, IDH_CALC_NMS_TAUE_ENDTIME,
		IDC_TAUE_ORIGIN, IDH_CALC_NMS_TAUE_ORIGIN,
		IDC_IDENT_SPL, IDH_CALC_NMS_IDENT_SPL,
		IDC_IDENT_TAUE, IDH_CALC_NMS_IDENT_TAUE,
		IDC_IDENT_BEFORE_STEP, IDH_CALC_NMS_IDENT_BEFORE_STEP,
		IDC_IDENT_AFTER_STEP, IDH_CALC_NMS_IDENT_AFTER_STEP,
		0
	};

	DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}
