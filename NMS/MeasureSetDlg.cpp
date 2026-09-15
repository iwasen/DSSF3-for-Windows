// MeasureSetDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Nms.h"
#include "MeasureSetDlg.h"
#include "SetData.h"
#include "Help\ContextHelp.h"

/////////////////////////////////////////////////////////////////////////////
// CMeasureSetDlg ダイアログ

CMeasureSetDlg::CMeasureSetDlg(CWnd* pParent /*=NULL*/)
	: CDialogAF(CMeasureSetDlg::IDD, pParent)
{
	m_dIntegrationTime = 0.0;
	m_dMaxDelayTime = 0.0;
	m_dRunningStep = 0.0;
	m_dSamplingLevel = 0.0;
	m_dTaueEndLevel = 0.0;
	m_dTaueEndTime = 0.0;
	m_dTaueInterval = 0.0;
	m_dTimeConstant = 0.0;
	m_bTaueOrigin = FALSE;
	m_iIdentMode = -1;
	m_iIdentAfterStep = 0;
	m_iIdentBeforeStep = 0;
	m_tEndDate = 0;
	m_tEndTime = 0;
	m_tStartDate = 0;
	m_tStartTime = 0;
	m_bEndDateTime = FALSE;
	m_bStartDateTime = FALSE;
	m_iSamplingTiming = -1;
	m_dSamplingTime1 = 0.0;
	m_dSamplingTime2 = 0.0;
	m_bRelativeLevel = FALSE;
}

void CMeasureSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_START_CURRENT_TIME, m_cStartCurrentTime);
	DDX_Control(pDX, IDC_END_CURRENT_TIME, m_cEndCurrentTime);
	DDX_Control(pDX, IDC_START_TIME, m_cStartTime);
	DDX_Control(pDX, IDC_START_DATE, m_cStartDate);
	DDX_Control(pDX, IDC_END_TIME, m_cEndTime);
	DDX_Control(pDX, IDC_END_DATE, m_cEndDate);
	DDX_Control(pDX, IDC_IDENT_BEFORE_STEP, m_cIdentBeforeStep);
	DDX_Control(pDX, IDC_IDENT_AFTER_STEP, m_cIdentAfterStep);
	DDX_Control(pDX, IDC_SAMPLING_RATE, m_cSamplingRate);
	DDX_Control(pDX, IDC_FREQ_WEIGHTING, m_cFreqWeighting);
	DDX_Text(pDX, IDC_INTEGRATION_TIME, m_dIntegrationTime);
	DDX_Text(pDX, IDC_MAX_DELAY_TIME, m_dMaxDelayTime);
	DDX_Text(pDX, IDC_RUNNING_STEP, m_dRunningStep);
	DDX_Text(pDX, IDC_SAMPLING_LEVEL, m_dSamplingLevel);
	DDX_Text(pDX, IDC_TAUE_ENDLEVEL, m_dTaueEndLevel);
	DDX_Text(pDX, IDC_TAUE_ENDTIME, m_dTaueEndTime);
	DDX_Text(pDX, IDC_TAUE_INTERVAL, m_dTaueInterval);
	DDX_Text(pDX, IDC_TIMECONSTANT, m_dTimeConstant);
	DDX_Check(pDX, IDC_TAUE_ORIGIN, m_bTaueOrigin);
	DDX_Radio(pDX, IDC_IDENT_SPL, m_iIdentMode);
	DDX_Text(pDX, IDC_IDENT_AFTER_STEP, m_iIdentAfterStep);
	DDX_Text(pDX, IDC_IDENT_BEFORE_STEP, m_iIdentBeforeStep);
	DDX_DateTimeCtrl(pDX, IDC_END_DATE, m_tEndDate);
	DDX_DateTimeCtrl(pDX, IDC_END_TIME, m_tEndTime);
	DDX_DateTimeCtrl(pDX, IDC_START_DATE, m_tStartDate);
	DDX_DateTimeCtrl(pDX, IDC_START_TIME, m_tStartTime);
	DDX_Check(pDX, IDC_END_DATE_TIME, m_bEndDateTime);
	DDX_Check(pDX, IDC_START_DATE_TIME, m_bStartDateTime);
	DDX_Radio(pDX, IDC_TIMING_PEAK, m_iSamplingTiming);
	DDX_Text(pDX, IDC_SAMPLING_TIME1, m_dSamplingTime1);
	DDX_Text(pDX, IDC_SAMPLING_TIME2, m_dSamplingTime2);
	DDX_Check(pDX, IDC_RELATIVE_LEVEL, m_bRelativeLevel);
}

BEGIN_MESSAGE_MAP(CMeasureSetDlg, CDialogAF)
	ON_BN_CLICKED(IDC_IDENT_SPL, OnIdentSpl)
	ON_BN_CLICKED(IDC_IDENT_TAUE, OnIdentTaue)
	ON_BN_CLICKED(IDC_START_DATE_TIME, OnStartDateTime)
	ON_BN_CLICKED(IDC_END_DATE_TIME, OnEndDateTime)
	ON_BN_CLICKED(IDC_START_CURRENT_TIME, OnStartCurrentTime)
	ON_BN_CLICKED(IDC_END_CURRENT_TIME, OnEndCurrentTime)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMeasureSetDlg メッセージ ハンドラ

BOOL CMeasureSetDlg::OnInitDialog()
{
	m_dSamplingLevel = g_oSetData.Measurement.fSamplingLevel;
	m_bRelativeLevel = g_oSetData.Measurement.bRelativeLevel;
	m_dTimeConstant = g_oSetData.Measurement.fTimeConstant;
	m_iSamplingTiming = g_oSetData.Measurement.nSamplingTiming;
	m_dSamplingTime1 = g_oSetData.Measurement.fSamplingTime1;
	m_dSamplingTime2 = g_oSetData.Measurement.fSamplingTime2;
	m_dIntegrationTime = g_oSetData.Measurement.AcfCondition.fIntegrationTime;
	m_dRunningStep = g_oSetData.Measurement.AcfCondition.fRunningStep;
	m_dMaxDelayTime = g_oSetData.Measurement.AcfCondition.fMaxDelayTime;
	m_dTaueInterval = g_oSetData.Measurement.AcfCondition.fTaueInterval;
	m_dTaueEndLevel = g_oSetData.Measurement.AcfCondition.fTaueEndLevel;
	m_dTaueEndTime = g_oSetData.Measurement.AcfCondition.fTaueEndTime;
	m_bTaueOrigin = g_oSetData.Measurement.AcfCondition.bTaueOrigin;
	m_iIdentMode = g_oSetData.Measurement.IdentCondition.nIdentMode;
	m_iIdentBeforeStep = g_oSetData.Measurement.IdentCondition.nBeforeStep;
	m_iIdentAfterStep = g_oSetData.Measurement.IdentCondition.nAfterStep;
	m_bStartDateTime = g_oSetData.Measurement.bStartDateTime;
	m_tStartDate = g_oSetData.Measurement.tStartDateTime;
	m_tStartTime = g_oSetData.Measurement.tStartDateTime;
	m_bEndDateTime = g_oSetData.Measurement.bEndDateTime;
	m_tEndDate = g_oSetData.Measurement.tEndDateTime;
	m_tEndTime = g_oSetData.Measurement.tEndDateTime;

	CDialogAF::OnInitDialog();

	SetSamplingRateList(m_cSamplingRate, WAVE_IN, g_oSetData.InputDevice.nInputDevice, 0, g_oSetData.Measurement.nSamplingRate);

	SetFilterNameList(m_cFreqWeighting);
	m_cFreqWeighting.SetCurSel(g_oSetData.Measurement.AcfCondition.nFreqWeighting);

	EnableControl();

	return TRUE;
}

void CMeasureSetDlg::OnOK()
{
	if (!UpdateData(TRUE))
		return;

	g_oSetData.Measurement.nSamplingRate = (int)m_cSamplingRate.GetItemData(m_cSamplingRate.GetCurSel());
	g_oSetData.Measurement.fSamplingLevel = m_dSamplingLevel;
	g_oSetData.Measurement.bRelativeLevel = m_bRelativeLevel;
	g_oSetData.Measurement.fTimeConstant = m_dTimeConstant;
	g_oSetData.Measurement.nSamplingTiming = m_iSamplingTiming;
	g_oSetData.Measurement.fSamplingTime1 = m_dSamplingTime1;
	g_oSetData.Measurement.fSamplingTime2 = m_dSamplingTime2;
	g_oSetData.Measurement.AcfCondition.fIntegrationTime = m_dIntegrationTime;
	g_oSetData.Measurement.AcfCondition.fRunningStep = m_dRunningStep;
	g_oSetData.Measurement.AcfCondition.fMaxDelayTime = m_dMaxDelayTime;
	g_oSetData.Measurement.AcfCondition.fTaueInterval = m_dTaueInterval;
	g_oSetData.Measurement.AcfCondition.fTaueEndLevel = m_dTaueEndLevel;
	g_oSetData.Measurement.AcfCondition.fTaueEndTime = m_dTaueEndTime;
	g_oSetData.Measurement.AcfCondition.bTaueOrigin = (short)m_bTaueOrigin;
	g_oSetData.Measurement.IdentCondition.nIdentMode = m_iIdentMode;
	g_oSetData.Measurement.IdentCondition.nBeforeStep = m_iIdentBeforeStep;
	g_oSetData.Measurement.IdentCondition.nAfterStep = m_iIdentAfterStep;
	g_oSetData.Measurement.AcfCondition.nFreqWeighting = m_cFreqWeighting.GetCurSel();
	g_oSetData.Measurement.bStartDateTime = m_bStartDateTime;
	g_oSetData.Measurement.tStartDateTime = CTime(
			m_tStartDate.GetYear(),
			m_tStartDate.GetMonth(),
			m_tStartDate.GetDay(),
			m_tStartTime.GetHour(),
			m_tStartTime.GetMinute(),
			m_tStartTime.GetSecond());
	g_oSetData.Measurement.bEndDateTime = m_bEndDateTime;
	g_oSetData.Measurement.tEndDateTime = CTime(
			m_tEndDate.GetYear(),
			m_tEndDate.GetMonth(),
			m_tEndDate.GetDay(),
			m_tEndTime.GetHour(),
			m_tEndTime.GetMinute(),
			m_tEndTime.GetSecond());

	EndDialog(IDOK);
}

void CMeasureSetDlg::OnIdentSpl()
{
	UpdateData(TRUE);
	EnableControl();
}

void CMeasureSetDlg::OnIdentTaue()
{
	UpdateData(TRUE);
	EnableControl();
}

void CMeasureSetDlg::EnableControl()
{
	if (m_iIdentMode == IDENT_MODE_MAXSPL) {
		m_cIdentBeforeStep.EnableWindow(FALSE);
		m_cIdentAfterStep.EnableWindow(FALSE);
	} else if (m_iIdentMode == IDENT_MODE_MINTAUE) {
		m_cIdentBeforeStep.EnableWindow(TRUE);
		m_cIdentAfterStep.EnableWindow(TRUE);
	}

	if (m_bStartDateTime) {
		m_cStartDate.EnableWindow(TRUE);
		m_cStartTime.EnableWindow(TRUE);
		m_cStartCurrentTime.EnableWindow(TRUE);
	} else {
		m_cStartDate.EnableWindow(FALSE);
		m_cStartTime.EnableWindow(FALSE);
		m_cStartCurrentTime.EnableWindow(FALSE);
	}

	if (m_bEndDateTime) {
		m_cEndDate.EnableWindow(TRUE);
		m_cEndTime.EnableWindow(TRUE);
		m_cEndCurrentTime.EnableWindow(TRUE);
	} else {
		m_cEndDate.EnableWindow(FALSE);
		m_cEndTime.EnableWindow(FALSE);
		m_cEndCurrentTime.EnableWindow(FALSE);
	}
}

void CMeasureSetDlg::OnStartDateTime()
{
	UpdateData(TRUE);
	EnableControl();
}

void CMeasureSetDlg::OnEndDateTime()
{
	UpdateData(TRUE);
	EnableControl();
}

void CMeasureSetDlg::OnStartCurrentTime()
{
	UpdateData(TRUE);
	m_tStartDate = CTime::GetCurrentTime();
	m_tStartTime = CTime::GetCurrentTime();
	UpdateData(FALSE);
}

void CMeasureSetDlg::OnEndCurrentTime()
{
	UpdateData(TRUE);
	m_tEndDate = CTime::GetCurrentTime();
	m_tEndTime = CTime::GetCurrentTime();
	UpdateData(FALSE);
}

BOOL CMeasureSetDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static const UINT aIDs[] = {
		IDOK, IDH_MEASURE_SET_OK,
		IDCANCEL, IDH_MEASURE_SET_CANCEL,
		IDC_SAMPLING_RATE, IDH_MEASURE_SET_SAMPLING_RATE,
		IDC_SAMPLING_LEVEL, IDH_MEASURE_SET_SAMPLING_LEVEL,
		IDC_RELATIVE_LEVEL, IDH_MEASURE_SET_RELATIVE_LEVEL,
		IDC_TIMECONSTANT, IDH_MEASURE_SET_TIMECONSTANT,
		IDC_TIMING_PEAK, IDH_MEASURE_SET_TIMING_PEAK,
		IDC_TIMING_LEVEL, IDH_MEASURE_SET_TIMING_LEVEL,
		IDC_SAMPLING_TIME1, IDH_MEASURE_SET_SAMPLING_TIME1,
		IDC_SAMPLING_TIME2, IDH_MEASURE_SET_SAMPLING_TIME2,
		IDC_INTEGRATION_TIME, IDH_MEASURE_SET_INTEGRATION_TIME,
		IDC_RUNNING_STEP, IDH_MEASURE_SET_RUNNING_STEP,
		IDC_FREQ_WEIGHTING, IDH_MEASURE_SET_FREQ_WEIGHTING,
		IDC_MAX_DELAY_TIME, IDH_MEASURE_SET_MAX_DELAY_TIME,
		IDC_TAUE_INTERVAL, IDH_MEASURE_SET_TAUE_INTERVAL,
		IDC_TAUE_ENDLEVEL, IDH_MEASURE_SET_TAUE_ENDLEVEL,
		IDC_TAUE_ENDTIME, IDH_MEASURE_SET_TAUE_ENDTIME,
		IDC_TAUE_ORIGIN, IDH_MEASURE_SET_TAUE_ORIGIN,
		IDC_IDENT_SPL, IDH_MEASURE_SET_IDENT_SPL,
		IDC_IDENT_TAUE, IDH_MEASURE_SET_IDENT_TAUE,
		IDC_IDENT_BEFORE_STEP, IDH_MEASURE_SET_IDENT_BEFORE_STEP,
		IDC_IDENT_AFTER_STEP, IDH_MEASURE_SET_IDENT_AFTER_STEP,
		IDC_START_DATE_TIME, IDH_MEASURE_SET_START_DATE_TIME,
		IDC_START_DATE, IDH_MEASURE_SET_START_DATE,
		IDC_START_TIME, IDH_MEASURE_SET_START_TIME,
		IDC_START_CURRENT_TIME, IDH_MEASURE_SET_START_CURRENT_TIME,
		IDC_END_DATE_TIME, IDH_MEASURE_SET_END_DATE_TIME,
		IDC_END_DATE, IDH_MEASURE_SET_END_DATE,
		IDC_END_TIME, IDH_MEASURE_SET_END_TIME,
		IDC_END_CURRENT_TIME, IDH_MEASURE_SET_END_CURRENT_TIME,
		0
	};

	DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}
