// SgTone.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Rta.h"
#include "SgTone.h"
#include "Help\ContextHelp.h"

#define MIN_POS 0
#define MAX_POS 300

struct FreqRange {
	int nMinFreq;
	int nMaxFreq;
} tFreqRange[] = {
	{10, 20000},
	{10, 100},
	{100, 1000},
	{1000, 10000},
	{10000, 100000}
};

/////////////////////////////////////////////////////////////////////////////
// CSgTone ダイアログ

CSgTone::CSgTone(CWnd* pParent /*=NULL*/)
	: CDialogAF(CSgTone::IDD, pParent)
{
}

void CSgTone::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RENDOU_F, m_cRendouF);
	DDX_Control(pDX, IDC_SFREQ_R, m_cSFreqR);
	DDX_Control(pDX, IDC_SFREQ_L, m_cSFreqL);
	DDX_Control(pDX, IDC_EFREQ_R, m_cEFreqR);
	DDX_Control(pDX, IDC_EFREQ_L, m_cEFreqL);
	DDX_Control(pDX, IDC_FREQ_RANGE, m_cFreqRange);
	DDX_Control(pDX, IDC_WAVE_FORM, m_cWaveForm);
	DDX_Control(pDX, IDC_PRESET1, m_cPreset1);
	DDX_Control(pDX, IDC_PRESET2, m_cPreset2);
	DDX_Control(pDX, IDC_PRESET3, m_cPreset3);
	DDX_Control(pDX, IDC_PRESET4, m_cPreset4);
	DDX_Control(pDX, IDC_PRESET, m_cPreset);
	DDX_Control(pDX, IDC_PHASE_SLIDER, m_cPhaseSlider);
	DDX_Control(pDX, IDC_PHASE_VALUE, m_cPhaseValue);
	DDX_Control(pDX, IDC_PHASE0, m_cPhase0);
	DDX_Control(pDX, IDC_FREQ_L_SPIN, m_cFreqLSpin);
	DDX_Control(pDX, IDC_FREQ_R_SPIN, m_cFreqRSpin);
}

BEGIN_MESSAGE_MAP(CSgTone, CDialogAF)
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_EFREQ_L, OnChangeEfreqL)
	ON_EN_CHANGE(IDC_EFREQ_R, OnChangeEfreqR)
	ON_BN_CLICKED(IDC_RENDOU_F, OnRendouF)
	ON_CBN_SELCHANGE(IDC_FREQ_RANGE, OnCbnSelchangeFreqRange)
	ON_CBN_SELCHANGE(IDC_WAVE_FORM, OnCbnSelchangeWaveForm)
	ON_BN_CLICKED(IDC_PRESET1, OnBnClickedPreset1)
	ON_BN_CLICKED(IDC_PRESET2, OnBnClickedPreset2)
	ON_BN_CLICKED(IDC_PRESET3, OnBnClickedPreset3)
	ON_BN_CLICKED(IDC_PRESET4, OnBnClickedPreset4)
	ON_EN_CHANGE(IDC_PHASE_VALUE, OnEnChangePhaseValue)
	ON_BN_CLICKED(IDC_PHASE0, OnBnClickedPhase0)
	ON_WM_VSCROLL()
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSgTone メッセージ ハンドラ

BOOL CSgTone::OnInitDialog()
{
	CDialogAF::OnInitDialog();

	m_cSFreqL.SetRange(MIN_POS, MAX_POS);
	m_cSFreqL.SetPageSize(MAX_POS / 20);
	m_cSFreqR.SetRange(MIN_POS, MAX_POS);
	m_cSFreqR.SetPageSize(MAX_POS / 20);

	m_cFreqLSpin.SetRange32(1, 99999);
	m_cFreqRSpin.SetRange32(1, 99999);

	SetFreqRange(FALSE);

	m_cEFreqL = g_oSetData.Sg.fFreqL;
//	m_SFreqL.SetPos(CalcSliderPos(g_oSetData.Sg.fFreqL));

	m_cEFreqR = g_oSetData.Sg.fFreqR;
//	m_SFreqR.SetPos(CalcSliderPos(g_oSetData.Sg.fFreqR));

	m_cRendouF = g_oSetData.Sg.bRendouF;

	m_cPhaseSlider.SetRange(0, 360 / 5);
	m_cPhaseSlider.SetPageSize(9);
	SetPhaseRange();

	m_cPhaseValue = g_oSetData.Sg.nPhase;
	m_cPhaseSlider.SetPos((g_oSetData.Sg.nPhase + 180) / 5);

	EnablePhase();

	SetWaveFormList(m_cWaveForm, g_oSetData.Sg.nToneWave);

	DispAllPresetFreq();

	UpdateData(FALSE);

	m_cEFreqL.SetValidChar(VC_NUM | VC_POINT);
	m_cEFreqR.SetValidChar(VC_NUM | VC_POINT);
	m_cPhaseValue.SetValidChar(VC_NUM | VC_MINUS);

	return TRUE;
}

void CSgTone::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	UINT pos;

	switch (pScrollBar->GetDlgCtrlID()) {
	case IDC_SFREQ_L:
		pos = ((CSliderCtrl *)pScrollBar)->GetPos();
		SetLFreq(floor(CalcFreq(pos) * 10 + 0.5) / 10);
		break;
	case IDC_SFREQ_R:
		pos = ((CSliderCtrl *)pScrollBar)->GetPos();
		SetRFreq(floor(CalcFreq(pos) * 10 + 0.5) / 10);
		break;
	case IDC_PHASE_SLIDER:
		pos = ((CSliderCtrl *)pScrollBar)->GetPos();
		SetPhase((int)pos * 5 - 180);
		break;
	}

	CDialogAF::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CSgTone::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (nSBCode == SB_THUMBPOSITION) {
		switch (pScrollBar->GetDlgCtrlID()) {
		case IDC_FREQ_L_SPIN:
			SetLFreq(nPos);
			break;
		case IDC_FREQ_R_SPIN:
			SetRFreq(nPos);
			break;
		}
	}

	CDialogAF::OnVScroll(nSBCode, nPos, pScrollBar);
}

int CSgTone::CalcSliderPos(double fFreq)
{
	if (fFreq < MIN_FREQ)
		fFreq = MIN_FREQ;
	else if (fFreq > m_nMaxFreq)
		fFreq = m_nMaxFreq;

	int nPos = (int)((log(fFreq) - log((double)m_nMinFreq)) * MAX_POS / (log((double)m_nMaxFreq) - log((double)m_nMinFreq)) + 0.5);

	if (nPos < 0)
		nPos = 0;
	else if (nPos > MAX_POS)
		nPos = MAX_POS;

	return nPos;
}

double CSgTone::CalcFreq(int pos)
{
	if (pos < MIN_POS)
		pos = MIN_POS;
	else if (pos > MAX_POS)
		pos = MAX_POS;

	return exp(log((double)m_nMinFreq) + pos * (log((double)m_nMaxFreq) - log((double)m_nMinFreq)) / MAX_POS);
}

void CSgTone::OnOK()
{
}

void CSgTone::OnCancel()
{
}

void CSgTone::OnChangeEfreqL()
{
	if (m_cEFreqL.m_hWnd)
		SetLFreq(m_cEFreqL);
}

void CSgTone::OnChangeEfreqR()
{
	if (m_cEFreqR.m_hWnd)
		SetRFreq(m_cEFreqR);
}

void CSgTone::OnRendouF()
{
	g_oSetData.Sg.bRendouF = m_cRendouF;
}

void CSgTone::SetLFreq(double fFreq)
{
	SetLFreq2(fFreq);
	if (g_oSetData.Sg.bRendouF)
		SetRFreq2(fFreq);

	EnablePhase();
}

void CSgTone::SetLFreq2(double fFreq)
{
	if (fFreq < 0)
		fFreq = 0;
	else if (fFreq > m_nMaxFreq)
		fFreq = m_nMaxFreq;

	if ((double)m_cEFreqL != fFreq)
		m_cEFreqL = fFreq;
	m_cSFreqL.SetPos(CalcSliderPos(fFreq));
	m_cFreqLSpin.SetPos32((int)fFreq);
	g_oSetData.Sg.fFreqL = fFreq;
}

void CSgTone::SetRFreq(double fFreq)
{
	SetRFreq2(fFreq);
	if (g_oSetData.Sg.bRendouF)
		SetLFreq2(fFreq);

	EnablePhase();
}

void CSgTone::SetRFreq2(double fFreq)
{
	if (fFreq < 0)
		fFreq = 0;
	else if (fFreq > m_nMaxFreq)
		fFreq = m_nMaxFreq;

	if ((double)m_cEFreqR != fFreq)
		m_cEFreqR = fFreq;
	m_cSFreqR.SetPos(CalcSliderPos(fFreq));
	m_cFreqRSpin.SetPos32((int)fFreq);
	g_oSetData.Sg.fFreqR = fFreq;
}

void CSgTone::OnCbnSelchangeFreqRange()
{
	g_oSetData.Sg.nFreqRange = m_cFreqRange.GetCurSel();
	SetFreqScale();

	m_cSFreqL.SetPos(CalcSliderPos(g_oSetData.Sg.fFreqL));
	m_cSFreqR.SetPos(CalcSliderPos(g_oSetData.Sg.fFreqR));
}

void CSgTone::OnCbnSelchangeWaveForm()
{
	g_oSetData.Sg.nToneWave = m_cWaveForm.GetCurSel();
}

void CSgTone::SetFreqScale()
{
	int	nFreq;
	int pos;
	int nTic;

	m_nMinFreq = tFreqRange[g_oSetData.Sg.nFreqRange].nMinFreq;
	m_nMaxFreq = tFreqRange[g_oSetData.Sg.nFreqRange].nMaxFreq;

	m_cSFreqR.ClearTics();
	for (nFreq = m_nMinFreq, nTic = 0; nFreq <= m_nMaxFreq; nTic++) {
		pos = CalcSliderPos(nFreq);
		m_cSFreqR.SetTic(pos);

		if (g_oSetData.Sg.nFreqRange == 0) {
			if (nTic % 9 == 0)
				m_cSFreqR.AddTicString(nTic, GetNumberString(nFreq));
		} else {
			if (nTic == 0 || nTic == 1 || nTic == 4 || nTic == 9)
				m_cSFreqR.AddTicString(nTic, GetNumberString(nFreq));
		}

		if (nFreq < 100)
			nFreq += 10;
		else if (nFreq < 1000)
			nFreq += 100;
		else if (nFreq < 10000)
			nFreq += 1000;
		else
			nFreq += 10000;
	}
}

void CSgTone::SetPresetFreq(int nPresetNo)
{
	if (m_cPreset.GetCheck()) {
		g_oSetData.Sg.fPresetFreq[nPresetNo] = g_oSetData.Sg.fFreqL;
		DispPresetFreq(nPresetNo);
		m_cPreset.SetCheck(0);
	} else {
		GetOwner()->SendMessage(WM_SG_CHANGE, 0, 1);

		m_cEFreqL = g_oSetData.Sg.fPresetFreq[nPresetNo];
		m_cSFreqL.SetPos(CalcSliderPos(g_oSetData.Sg.fFreqL));

		m_cEFreqR = g_oSetData.Sg.fPresetFreq[nPresetNo];
		m_cSFreqR.SetPos(CalcSliderPos(g_oSetData.Sg.fFreqR));
	}
}

void CSgTone::DispPresetFreq(int nPresetNo)
{
	CString str;

	str = GetNumberString(g_oSetData.Sg.fPresetFreq[nPresetNo]);

	switch (nPresetNo) {
	case 0:
		m_cPreset1.SetWindowText(str);
		break;
	case 1:
		m_cPreset2.SetWindowText(str);
		break;
	case 2:
		m_cPreset3.SetWindowText(str);
		break;
	case 3:
		m_cPreset4.SetWindowText(str);
		break;
	}
}

void CSgTone::DispAllPresetFreq()
{
	for (int i = 0; i < N_SG_PRESET; i++)
		DispPresetFreq(i);
}

void CSgTone::OnBnClickedPreset1()
{
	SetPresetFreq(0);
}

void CSgTone::OnBnClickedPreset2()
{
	SetPresetFreq(1);
}

void CSgTone::OnBnClickedPreset3()
{
	SetPresetFreq(2);
}

void CSgTone::OnBnClickedPreset4()
{
	SetPresetFreq(3);
}

void CSgTone::SetPhaseRange()
{
	CString str;

	for (int i = 0, nTic = 0; i <= 360; i += 45, nTic++) {
		m_cPhaseSlider.SetTic(i / 5);

		if (i % 90 == 0) {
			str.Format("%d", i - 180);
			m_cPhaseSlider.AddTicString(nTic, str);
		}
	}
}

void CSgTone::SetPhase(int nPhase)
{
	if (nPhase < MIN_PHASE)
		nPhase = MIN_PHASE;
	else if (nPhase > MAX_PHASE)
		nPhase = MAX_PHASE;

	if ((double)m_cPhaseValue != nPhase)
		m_cPhaseValue = nPhase;
	m_cPhaseSlider.SetPos((nPhase + 180) / 5);
	g_oSetData.Sg.nPhase = nPhase;
}

void CSgTone::OnEnChangePhaseValue()
{
	SetPhase(m_cPhaseValue);
}

void CSgTone::EnablePhase()
{
	BOOL bEnable = (g_oSetData.Sg.fFreqL == g_oSetData.Sg.fFreqR);

	m_cPhaseSlider.EnableWindow(bEnable);
	m_cPhaseValue.EnableWindow(bEnable);
	m_cPhase0.EnableWindow(bEnable);
}

void CSgTone::OnBnClickedPhase0()
{
	SetPhase(0);
}

void CSgTone::SetFreqRange(BOOL bSetSlider)
{
	CString str;

	int nMaxFreq = (int)(g_oSetData.Sg.nSamplingRate / 2);

	tFreqRange[0].nMaxFreq = nMaxFreq;

	if (nMaxFreq > 10000) {
		m_nFreqRange = 5;
//		tFreqRange[4].nMaxFreq = nMaxFreq;
	} else {
		m_nFreqRange = 4;
//		tFreqRange[3].nMaxFreq = nMaxFreq;
	}

	m_cFreqRange.ResetContent();
	for (int i = 0; i < m_nFreqRange; i++) {
		str.Format("%sHz-%sHz", (LPCSTR)GetNumberString(tFreqRange[i].nMinFreq), (LPCSTR)GetNumberString(tFreqRange[i].nMaxFreq));
		m_cFreqRange.AddString(str);
	}

	if (g_oSetData.Sg.nFreqRange >= m_nFreqRange)
		g_oSetData.Sg.nFreqRange = 0;
	m_cFreqRange.SetCurSel(g_oSetData.Sg.nFreqRange);

	SetFreqScale();

	if (bSetSlider) {
		SetLFreq(m_cEFreqL);
		SetRFreq(m_cEFreqR);
	}
}

BOOL CSgTone::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static const UINT aIDs[] = {
		IDC_SFREQ_L, IDH_SG_TONE_SFREQ_L,
		IDC_EFREQ_L, IDH_SG_TONE_EFREQ_L,
		IDC_SFREQ_R, IDH_SG_TONE_SFREQ_R,
		IDC_EFREQ_R, IDH_SG_TONE_EFREQ_R,
		IDC_PRESET1, IDH_SG_TONE_PRESET1_4,
		IDC_PRESET2, IDH_SG_TONE_PRESET1_4,
		IDC_PRESET3, IDH_SG_TONE_PRESET1_4,
		IDC_PRESET4, IDH_SG_TONE_PRESET1_4,
		IDC_PRESET, IDH_SG_TONE_PRESET,
		IDC_FREQ_RANGE, IDH_SG_TONE_FREQ_RANGE,
		IDC_RENDOU_F, IDH_SG_TONE_RENDOU_F,
		IDC_PHASE_SLIDER, IDH_SG_TONE_PHASE_SLIDER,
		IDC_PHASE_VALUE, IDH_SG_TONE_PHASE_VALUE,
		IDC_PHASE0, IDH_SG_TONE_PHASE0,
		IDC_WAVE_FORM, IDH_SG_TONE_WAVE_FORM,
		0
	};

	DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}
