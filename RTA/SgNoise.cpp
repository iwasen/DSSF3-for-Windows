// SgNoise.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Rta.h"
#include "SgNoise.h"
#include "Help\ContextHelp.h"

static const int tMaxTimeRange[] = {10, 100, 1000};

/////////////////////////////////////////////////////////////////////////////
// CSgNoise ダイアログ

CSgNoise::CSgNoise(CWnd* pParent /*=NULL*/)
	: CDialogAF(CSgNoise::IDD, pParent)
{
	m_nSgNoiseType = -1;
	m_nSgNoiseMode = -1;
}

void CSgNoise::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_WHITE_NOISE, m_nSgNoiseType);
	DDX_Radio(pDX, IDC_MONO, m_nSgNoiseMode);
	DDX_Control(pDX, IDC_TIME_SLIDER, m_cTimeSlider);
	DDX_Control(pDX, IDC_TIME_VALUE, m_cTimeValue);
	DDX_Control(pDX, IDC_TIME0, m_cTime0);
	DDX_Control(pDX, IDC_TIME_RANGE, m_cTimeRange);
	DDX_Control(pDX, IDC_TIME_VALUE_SPIN, m_cTimeValueSpin);
}

BEGIN_MESSAGE_MAP(CSgNoise, CDialogAF)
	ON_BN_CLICKED(IDC_WHITE_NOISE, OnWhiteNoise)
	ON_BN_CLICKED(IDC_PINK_NOISE, OnPinkNoise)
	ON_BN_CLICKED(IDC_MONO, OnMono)
	ON_BN_CLICKED(IDC_STREO, OnStreo)
	ON_BN_CLICKED(IDC_INVERSE, OnInverse)
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_TIME_VALUE, OnEnChangeTimeValue)
	ON_BN_CLICKED(IDC_TIME0, OnBnClickedTime0)
	ON_BN_CLICKED(IDC_BROWN_NOISE, OnBnClickedBrownNoise)
	ON_CBN_SELCHANGE(IDC_TIME_RANGE, OnCbnSelchangeTimeRange)
	ON_WM_VSCROLL()
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSgNoise メッセージ ハンドラ

BOOL CSgNoise::OnInitDialog()
{
	CDialogAF::OnInitDialog();

	m_nSgNoiseType = g_oSetData.Sg.nNoiseType;
	m_nSgNoiseMode = g_oSetData.Sg.nNoiseMode;

	CString str;
	for (int i = 0; i < ARRAY_SIZE(tMaxTimeRange); i++) {
		str.Format("%d ms", tMaxTimeRange[i]);
		m_cTimeRange.AddString(str);
	}
	m_cTimeRange.SetCurSel(g_oSetData.Sg.nTimeRange);

	m_cTimeSlider.SetRange(0, 200);
	m_cTimeSlider.SetPageSize(10);

	m_cTimeValueSpin.SetRange(0, 200);

	SetTimeDiffRange();

	SetTimeDiff(g_oSetData.Sg.fTimeDiff);

	EnableTimeDiff();

	UpdateData(FALSE);

	m_cTimeValue.SetValidChar(VC_NUM | VC_MINUS | VC_POINT);

	return TRUE;
}

void CSgNoise::OnWhiteNoise()
{
	if (g_oSetData.Sg.nNoiseType != NOISE_WHITE) {
		g_oSetData.Sg.nNoiseType = NOISE_WHITE;
		GetOwner()->SendMessage(WM_SG_CHANGE, 1, 0);
	}
}

void CSgNoise::OnPinkNoise()
{
	if (g_oSetData.Sg.nNoiseType != NOISE_PINK) {
		g_oSetData.Sg.nNoiseType = NOISE_PINK;
		GetOwner()->SendMessage(WM_SG_CHANGE, 1, 0);
	}
}

void CSgNoise::OnBnClickedBrownNoise()
{
	if (g_oSetData.Sg.nNoiseType != NOISE_BROWN) {
		g_oSetData.Sg.nNoiseType = NOISE_BROWN;
		GetOwner()->SendMessage(WM_SG_CHANGE, 1, 0);
	}
}

void CSgNoise::OnMono()
{
	if (g_oSetData.Sg.nNoiseMode != NOISE_MODE_MONO) {
		g_oSetData.Sg.nNoiseMode = NOISE_MODE_MONO;
		GetOwner()->SendMessage(WM_SG_CHANGE, 1, 0);
		EnableTimeDiff();
	}
}

void CSgNoise::OnStreo()
{
	if (g_oSetData.Sg.nNoiseMode != NOISE_MODE_STEREO) {
		g_oSetData.Sg.nNoiseMode = NOISE_MODE_STEREO;
		GetOwner()->SendMessage(WM_SG_CHANGE, 1, 0);
		EnableTimeDiff();
	}
}

void CSgNoise::OnInverse()
{
	if (g_oSetData.Sg.nNoiseMode != NOISE_MODE_INVERSE) {
		g_oSetData.Sg.nNoiseMode = NOISE_MODE_INVERSE;
		GetOwner()->SendMessage(WM_SG_CHANGE, 1, 0);
		EnableTimeDiff();
	}
}

void CSgNoise::OnOK()
{
}

void CSgNoise::OnCancel()
{
}

void CSgNoise::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	UINT pos;

	switch (pScrollBar->GetDlgCtrlID()) {
	case IDC_TIME_SLIDER:
		pos = ((CSliderCtrl *)pScrollBar)->GetPos();
		SetTimeDiff(((int)pos - 100) * tMaxTimeRange[g_oSetData.Sg.nTimeRange] / 100.0);
		break;
	}

	CDialogAF::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CSgNoise::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (nSBCode == SB_THUMBPOSITION) {
		switch (pScrollBar->GetDlgCtrlID()) {
		case IDC_TIME_VALUE_SPIN:
			SetTimeDiff(((int)nPos - 100) * tMaxTimeRange[g_oSetData.Sg.nTimeRange] / 100.0);
			break;
		}
	}

	CDialogAF::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CSgNoise::OnEnChangeTimeValue()
{
	SetTimeDiff(m_cTimeValue);
}

void CSgNoise::OnBnClickedTime0()
{
	SetTimeDiff(0);
}

void CSgNoise::SetTimeDiffRange()
{
	int nTimeRange = tMaxTimeRange[g_oSetData.Sg.nTimeRange];

	m_cTimeSlider.ClearTics();

	CString str;
	for (int i = 0; i <= 20; i++) {
		m_cTimeSlider.SetTic(i * 10);

		if (i % 5 == 0) {
			str.Format("%d", (i - 10) * nTimeRange / 10);
			m_cTimeSlider.AddTicString(i, str);
		}
	}
}

void CSgNoise::SetTimeDiff(double fTime)
{
	if (fTime < -tMaxTimeRange[g_oSetData.Sg.nTimeRange])
		fTime = -tMaxTimeRange[g_oSetData.Sg.nTimeRange];
	else if (fTime > tMaxTimeRange[g_oSetData.Sg.nTimeRange])
		fTime = tMaxTimeRange[g_oSetData.Sg.nTimeRange];

	if (m_cTimeValue.IsEmpty() || (double)m_cTimeValue != fTime)
		m_cTimeValue = fTime;
	int nPos = (int)((fTime + tMaxTimeRange[g_oSetData.Sg.nTimeRange]) / tMaxTimeRange[g_oSetData.Sg.nTimeRange] * 100 + 0.5);
	m_cTimeSlider.SetPos(nPos);
	m_cTimeValueSpin.SetPos(nPos);
	g_oSetData.Sg.fTimeDiff = fTime;
}

void CSgNoise::EnableTimeDiff()
{
	BOOL bEnable = (g_oSetData.Sg.nNoiseMode != NOISE_MODE_STEREO);
	m_cTimeRange.EnableWindow(bEnable);
	m_cTimeSlider.EnableWindow(bEnable);
	m_cTimeValue.EnableWindow(bEnable);
	m_cTime0.EnableWindow(bEnable);
}

void CSgNoise::OnCbnSelchangeTimeRange()
{
	int nOldTimeRange = g_oSetData.Sg.nTimeRange;

	g_oSetData.Sg.nTimeRange = m_cTimeRange.GetCurSel();

	SetTimeDiffRange();

	SetTimeDiff(g_oSetData.Sg.fTimeDiff / tMaxTimeRange[nOldTimeRange] * tMaxTimeRange[g_oSetData.Sg.nTimeRange]);
}

BOOL CSgNoise::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static const UINT aIDs[] = {
		IDC_WHITE_NOISE, IDH_SG_NOISE_WHITE_NOISE,
		IDC_PINK_NOISE, IDH_SG_NOISE_PINK_NOISE,
		IDC_BROWN_NOISE, IDH_SG_NOISE_BROWN_NOISE,
		IDC_MONO, IDH_SG_NOISE_MONO,
		IDC_STREO, IDH_SG_NOISE_STREO,
		IDC_INVERSE, IDH_SG_NOISE_INVERSE,
		IDC_TIME_RANGE, IDH_SG_NOISE_TIME_RANGE,
		IDC_TIME_SLIDER, IDH_SG_NOISE_TIME_SLIDER,
		IDC_TIME_VALUE, IDH_SG_NOISE_TIME_VALUE,
		IDC_TIME0, IDH_SG_NOISE_TIME0,
		0
	};

	DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}
