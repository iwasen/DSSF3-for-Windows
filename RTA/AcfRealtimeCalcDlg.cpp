// AcfRealtimeCalcDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "RTA.h"
#include "AcfRealtimeCalcDlg.h"

// CAcfRealtimeCalcDlg ダイアログ

CAcfRealtimeCalcDlg::CAcfRealtimeCalcDlg(CWnd* pParent /*=NULL*/)
	: CDialogAF(CAcfRealtimeCalcDlg::IDD, pParent)
{
}

void CAcfRealtimeCalcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FREQ_FILTER, m_cFreqFilter);
	DDX_Text(pDX, IDC_INTEGRATION_TIME, m_dIntegrationTime);
	DDX_Text(pDX, IDC_MAX_DELAY_TIME, m_dMaxDelayTime);
	DDX_Text(pDX, IDC_TAUE_INTERVAL, m_dTaueInterval);
	DDX_Text(pDX, IDC_TAUE_ENDLEVEL, m_dTaueEndLevel);
	DDX_Text(pDX, IDC_TAUE_ENDTIME, m_dTaueEndTime);
	DDX_Text(pDX, IDC_RUNNING_STEP, m_dRunningStep);
	DDX_Check(pDX, IDC_TAUE_ORIGIN, m_bTaueOrigin);
	DDX_Radio(pDX, IDC_TAU1_A, m_iTau1Peak);
	DDX_Check(pDX, IDC_TAUE_FIXED_SCALE, m_bTaueFixedScale);
	DDX_Text(pDX, IDC_TAUE_MAX_TIME, m_dTaueMaxTime);
	DDX_Text(pDX, IDC_TAUE_MIN_TIME, m_dTaueMinTime);
	DDX_Radio(pDX, IDC_TAUE_LOG_SCALE, m_iTaueScale);
	DDX_Text(pDX, IDC_SPG_MAX_LEVEL, m_iSpgMaxLevel);
	DDX_Text(pDX, IDC_SPG_MIN_LEVEL, m_iSpgMinLevel);
	DDX_Radio(pDX, IDC_SPG_LOG_SCALE, m_iSpgScale);
	DDX_Control(pDX, IDC_TAUE_MAX_TIME, m_cTaueMaxTime);
	DDX_Control(pDX, IDC_TAUE_MIN_TIME, m_cTaueMinTime);
}

BEGIN_MESSAGE_MAP(CAcfRealtimeCalcDlg, CDialogAF)
	ON_BN_CLICKED(IDC_TAUE_FIXED_SCALE, &CAcfRealtimeCalcDlg::OnBnClickedTaueFixedScale)
END_MESSAGE_MAP()

// CAcfRealtimeCalcDlg メッセージ ハンドラ

BOOL CAcfRealtimeCalcDlg::OnInitDialog()
{
	m_dIntegrationTime = g_oSetData.Acf.fRealtimeIntegrationTime;
	m_dRunningStep = g_oSetData.Acf.fRealtimeRunningStep;
	m_dMaxDelayTime = g_oSetData.Acf.fRealtimeMaxDelayTime;
	m_dTaueInterval = g_oSetData.Acf.fRealtimeTaueInterval;
	m_dTaueEndLevel = g_oSetData.Acf.fRealtimeTaueEndLevel;
	m_dTaueEndTime = g_oSetData.Acf.fRealtimeTaueEndTime;
	m_bTaueOrigin = g_oSetData.Acf.bRealtimeTaueOrigin;
	m_bTaueFixedScale = g_oSetData.Acf.bRealtimeTaueFixedScale;
	m_dTaueMaxTime = g_oSetData.Acf.fRealtimeTaueMaxTime;
	m_dTaueMinTime = g_oSetData.Acf.fRealtimeTaueMinTime;
	m_iTaueScale = g_oSetData.Acf.nRealtimeTaueScale;
	m_iTau1Peak = g_oSetData.Acf.nRealtimeTau1Peak;
	m_iSpgMaxLevel = g_oSetData.Acf.nRealtimeMaxLevel;
	m_iSpgMinLevel = g_oSetData.Acf.nRealtimeMinLevel;
	m_iSpgScale = g_oSetData.Acf.nRealtimeScale;

	CDialogAF::OnInitDialog();

	SetFilterNameList(m_cFreqFilter);
	m_cFreqFilter.SetCurSel(g_oSetData.Acf.nRealtimeFreqWeighting);

	OnBnClickedTaueFixedScale();

	return TRUE;
}

void CAcfRealtimeCalcDlg::OnOK()
{
	UpdateData(TRUE);

	g_oSetData.Acf.fRealtimeIntegrationTime = m_dIntegrationTime;
	g_oSetData.Acf.fRealtimeRunningStep = m_dRunningStep;
	g_oSetData.Acf.fRealtimeMaxDelayTime = m_dMaxDelayTime;
	g_oSetData.Acf.fRealtimeTaueInterval = m_dTaueInterval;
	g_oSetData.Acf.fRealtimeTaueEndLevel = m_dTaueEndLevel;
	g_oSetData.Acf.fRealtimeTaueEndTime = m_dTaueEndTime;
	g_oSetData.Acf.bRealtimeTaueOrigin = m_bTaueOrigin;
	g_oSetData.Acf.bRealtimeTaueFixedScale = m_bTaueFixedScale;
	g_oSetData.Acf.fRealtimeTaueMaxTime = m_dTaueMaxTime;
	g_oSetData.Acf.fRealtimeTaueMinTime = m_dTaueMinTime;
	g_oSetData.Acf.nRealtimeTaueScale = m_iTaueScale;
	g_oSetData.Acf.nRealtimeFreqWeighting = m_cFreqFilter.GetCurSel();
	g_oSetData.Acf.nRealtimeTau1Peak = m_iTau1Peak;
	g_oSetData.Acf.nRealtimeMaxLevel = m_iSpgMaxLevel;
	g_oSetData.Acf.nRealtimeMinLevel = m_iSpgMinLevel;
	g_oSetData.Acf.nRealtimeScale = m_iSpgScale;

	CDialogAF::OnOK();
}

void CAcfRealtimeCalcDlg::OnBnClickedTaueFixedScale()
{
	UpdateData(TRUE);

	m_cTaueMaxTime.EnableWindow(m_bTaueFixedScale);
	m_cTaueMinTime.EnableWindow(m_bTaueFixedScale);
}
