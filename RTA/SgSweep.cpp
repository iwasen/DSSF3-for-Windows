// SgSweep.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Rta.h"
#include "SgSweep.h"
#include "Help\ContextHelp.h"

/////////////////////////////////////////////////////////////////////////////
// CSgSweep ダイアログ

CSgSweep::CSgSweep(CWnd* pParent /*=NULL*/)
	: CDialogAF(CSgSweep::IDD, pParent)
{
}

void CSgSweep::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOOP, m_cLoop);
	DDX_Control(pDX, IDC_SWEEP_SPEED, m_cSweepTime);
	DDX_Control(pDX, IDC_START_FREQ, m_cStartFreq);
	DDX_Control(pDX, IDC_END_FREQ, m_cEndFreq);
	DDX_Control(pDX, IDC_WAVE_FORM, m_cWaveForm);
	DDX_Control(pDX, IDC_START_LEVEL, m_cStartLevel);
	DDX_Control(pDX, IDC_END_LEVEL, m_cEndLevel);
}

BEGIN_MESSAGE_MAP(CSgSweep, CDialogAF)
	ON_EN_CHANGE(IDC_START_FREQ, OnChangeStartFreq)
	ON_EN_CHANGE(IDC_END_FREQ, OnChangeEndFreq)
	ON_EN_CHANGE(IDC_SWEEP_SPEED, OnChangeSweepSpeed)
	ON_BN_CLICKED(IDC_LOOP, OnLoop)
	ON_CBN_SELCHANGE(IDC_WAVE_FORM, OnCbnSelchangeWaveForm)
	ON_EN_CHANGE(IDC_START_LEVEL, OnEnChangeStartLevel)
	ON_EN_CHANGE(IDC_END_LEVEL, OnEnChangeEndLevel)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSgSweep メッセージ ハンドラ

BOOL CSgSweep::OnInitDialog()
{
	CDialogAF::OnInitDialog();

	m_cStartFreq = g_oSetData.Sg.fStartFreq;
	m_cEndFreq = g_oSetData.Sg.fEndFreq;
	m_cStartLevel = g_oSetData.Sg.fStartLevel;
	m_cEndLevel = g_oSetData.Sg.fEndLevel;
	m_cSweepTime = g_oSetData.Sg.fSweepTime;
	m_cLoop = g_oSetData.Sg.bSweepLoop;

	SetWaveFormList(m_cWaveForm, g_oSetData.Sg.nSweepWave);

	UpdateData(FALSE);

	m_cStartFreq.SetValidChar(VC_NUM | VC_POINT);
	m_cEndFreq.SetValidChar(VC_NUM | VC_POINT);
	m_cStartLevel.SetValidChar(VC_NUM | VC_POINT);
	m_cEndLevel.SetValidChar(VC_NUM | VC_POINT);
	m_cSweepTime.SetValidChar(VC_NUM | VC_POINT);

	return TRUE;
}

void CSgSweep::OnChangeStartFreq()
{
	g_oSetData.Sg.fStartFreq = m_cStartFreq;
}

void CSgSweep::OnChangeEndFreq()
{
	g_oSetData.Sg.fEndFreq = m_cEndFreq;
}

void CSgSweep::OnChangeSweepSpeed()
{
	g_oSetData.Sg.fSweepTime = m_cSweepTime;
}

void CSgSweep::OnLoop()
{
	g_oSetData.Sg.bSweepLoop = m_cLoop;
}

void CSgSweep::OnOK()
{
}

void CSgSweep::OnCancel()
{
}

void CSgSweep::OnCbnSelchangeWaveForm()
{
	g_oSetData.Sg.nSweepWave = m_cWaveForm.GetCurSel();
}

void CSgSweep::OnEnChangeStartLevel()
{
	g_oSetData.Sg.fStartLevel = m_cStartLevel;
}

void CSgSweep::OnEnChangeEndLevel()
{
	g_oSetData.Sg.fEndLevel = m_cEndLevel;
}

BOOL CSgSweep::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static const UINT aIDs[] = {
		IDC_START_FREQ, IDH_SG_SWEEP_START_FREQ,
		IDC_END_FREQ, IDH_SG_SWEEP_END_FREQ,
		IDC_START_LEVEL, IDH_SG_SWEEP_START_LEVEL,
		IDC_END_LEVEL, IDH_SG_SWEEP_END_LEVEL,
		IDC_SWEEP_SPEED, IDH_SG_SWEEP_SWEEP_SPEED,
		IDC_LOOP, IDH_SG_SWEEP_LOOP,
		IDC_WAVE_FORM, IDH_SG_SWEEP_WAVE_FORM,
		0
	};

	DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}
