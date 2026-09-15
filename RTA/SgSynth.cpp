// SgSynth.cpp : 実装ファイル
//

#include "stdafx.h"
#include "Rta.h"
#include "SgSynth.h"
#include "Help\ContextHelp.h"

#define MIN_POS 0
#define MAX_POS 300

// CSgSynth ダイアログ

CSgSynth::CSgSynth(CWnd* pParent /*=NULL*/)
	: CDialogAF(CSgSynth::IDD, pParent)
{
}

void CSgSynth::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMP_FREQ_1, m_cCompFreq[0]);
	DDX_Control(pDX, IDC_COMP_FREQ_2, m_cCompFreq[1]);
	DDX_Control(pDX, IDC_COMP_FREQ_3, m_cCompFreq[2]);
	DDX_Control(pDX, IDC_COMP_FREQ_4, m_cCompFreq[3]);
	DDX_Control(pDX, IDC_COMP_FREQ_5, m_cCompFreq[4]);
	DDX_Control(pDX, IDC_COMP_FREQ_6, m_cCompFreq[5]);
	DDX_Control(pDX, IDC_COMP_FREQ_7, m_cCompFreq[6]);
	DDX_Control(pDX, IDC_COMP_FREQ_8, m_cCompFreq[7]);
	DDX_Control(pDX, IDC_COMP_LEVEL_1, m_cCompLevel[0]);
	DDX_Control(pDX, IDC_COMP_LEVEL_2, m_cCompLevel[1]);
	DDX_Control(pDX, IDC_COMP_LEVEL_3, m_cCompLevel[2]);
	DDX_Control(pDX, IDC_COMP_LEVEL_4, m_cCompLevel[3]);
	DDX_Control(pDX, IDC_COMP_LEVEL_5, m_cCompLevel[4]);
	DDX_Control(pDX, IDC_COMP_LEVEL_6, m_cCompLevel[5]);
	DDX_Control(pDX, IDC_COMP_LEVEL_7, m_cCompLevel[6]);
	DDX_Control(pDX, IDC_COMP_LEVEL_8, m_cCompLevel[7]);
	DDX_Control(pDX, IDC_LEVEL_SLIDER_1, m_cLevelSlider[0]);
	DDX_Control(pDX, IDC_LEVEL_SLIDER_2, m_cLevelSlider[1]);
	DDX_Control(pDX, IDC_LEVEL_SLIDER_3, m_cLevelSlider[2]);
	DDX_Control(pDX, IDC_LEVEL_SLIDER_4, m_cLevelSlider[3]);
	DDX_Control(pDX, IDC_LEVEL_SLIDER_5, m_cLevelSlider[4]);
	DDX_Control(pDX, IDC_LEVEL_SLIDER_6, m_cLevelSlider[5]);
	DDX_Control(pDX, IDC_LEVEL_SLIDER_7, m_cLevelSlider[6]);
	DDX_Control(pDX, IDC_LEVEL_SLIDER_8, m_cLevelSlider[7]);
	DDX_Control(pDX, IDC_SYNTH_FREQ, m_cSynthFreq);
	DDX_Control(pDX, IDC_FREQ_SLIDER, m_cFreqSlider);
	DDX_Control(pDX, IDC_WAVE_FORM, m_cWaveForm);
	DDX_Control(pDX, IDC_SYNTH_FREQ_SPIN, m_cSynthFreqSpin);
}

BEGIN_MESSAGE_MAP(CSgSynth, CDialogAF)
	ON_EN_CHANGE(IDC_COMP_FREQ_1, OnEnChangeCompFreq1)
	ON_EN_CHANGE(IDC_COMP_FREQ_2, OnEnChangeCompFreq2)
	ON_EN_CHANGE(IDC_COMP_FREQ_3, OnEnChangeCompFreq3)
	ON_EN_CHANGE(IDC_COMP_FREQ_4, OnEnChangeCompFreq4)
	ON_EN_CHANGE(IDC_COMP_FREQ_5, OnEnChangeCompFreq5)
	ON_EN_CHANGE(IDC_COMP_FREQ_6, OnEnChangeCompFreq6)
	ON_EN_CHANGE(IDC_COMP_FREQ_7, OnEnChangeCompFreq7)
	ON_EN_CHANGE(IDC_COMP_FREQ_8, OnEnChangeCompFreq8)
	ON_EN_CHANGE(IDC_COMP_LEVEL_1, OnEnChangeCompLevel1)
	ON_EN_CHANGE(IDC_COMP_LEVEL_2, OnEnChangeCompLevel2)
	ON_EN_CHANGE(IDC_COMP_LEVEL_3, OnEnChangeCompLevel3)
	ON_EN_CHANGE(IDC_COMP_LEVEL_4, OnEnChangeCompLevel4)
	ON_EN_CHANGE(IDC_COMP_LEVEL_5, OnEnChangeCompLevel5)
	ON_EN_CHANGE(IDC_COMP_LEVEL_6, OnEnChangeCompLevel6)
	ON_EN_CHANGE(IDC_COMP_LEVEL_7, OnEnChangeCompLevel7)
	ON_EN_CHANGE(IDC_COMP_LEVEL_8, OnEnChangeCompLevel8)
	ON_EN_CHANGE(IDC_SYNTH_FREQ, OnEnChangeSynthFreq)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_CBN_SELCHANGE(IDC_WAVE_FORM, OnCbnSelchangeWaveForm)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

// CSgSynth メッセージ ハンドラ

BOOL CSgSynth::OnInitDialog()
{
	CDialogAF::OnInitDialog();

	m_cFreqSlider.SetRange(MIN_POS, MAX_POS);
	m_cFreqSlider.SetPageSize(MAX_POS / 20);

	m_cSynthFreqSpin.SetRange32(1, 99999);

	SetFreqRange();

	for (int i = 0; i < N_SG_COMPONENT; i++) {
		m_cLevelSlider[i].SetRange(0, 100);
		m_cLevelSlider[i].SetPageSize(10);
		m_cLevelSlider[i].SetPos(100 - g_oSetData.Sg.nCompLevel[i]);

		m_cCompFreq[i] = g_oSetData.Sg.fCompFreq[i];
		m_cCompLevel[i] = g_oSetData.Sg.nCompLevel[i];

		m_cCompFreq[i].SetValidChar(VC_NUM | VC_POINT);
		m_cCompLevel[i].SetValidChar(VC_NUM);
	}

	m_cFreqSlider.SetPos(CalcSliderPos(g_oSetData.Sg.fSynthFreq));

	m_cSynthFreq = g_oSetData.Sg.fSynthFreq;
	m_cSynthFreq.SetValidChar(VC_NUM | VC_POINT);

	SetWaveFormList(m_cWaveForm, g_oSetData.Sg.nSynthWave);

	return TRUE;
}

void CSgSynth::SetFreqRange()
{
	int	nFreq;
	int pos;
	int nTic;

	m_cFreqSlider.ClearTics();
	for (nFreq = (int)MIN_FREQ, nTic = 0; nFreq <= (int)MAX_FREQ; nTic++) {
		pos = CalcSliderPos(nFreq);
		m_cFreqSlider.SetTic(pos);

		if (nTic % 9 == 0)
			m_cFreqSlider.AddTicString(nTic, GetNumberString(nFreq));

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

int CSgSynth::CalcSliderPos(double fFreq)
{
	if (fFreq < MIN_FREQ)
		fFreq = MIN_FREQ;
	else if (fFreq > MAX_FREQ)
		fFreq = MAX_FREQ;

	int nPos = (int)((log(fFreq) - log(MIN_FREQ)) * MAX_POS / (log(MAX_FREQ) - log(MIN_FREQ)) + 0.5);

	if (nPos < 0)
		nPos = 0;
	else if (nPos > MAX_POS)
		nPos = MAX_POS;

	return nPos;
}

void CSgSynth::SetSynthFreq(double fFreq)
{
	if (fFreq < 0)
		fFreq = 0;
	else if (fFreq > MAX_FREQ)
		fFreq = MAX_FREQ;

	if ((double)m_cSynthFreq != fFreq)
		m_cSynthFreq = fFreq;
	m_cFreqSlider.SetPos(CalcSliderPos(fFreq));
	m_cSynthFreqSpin.SetPos32((int)fFreq);
	g_oSetData.Sg.fSynthFreq = fFreq;
}

double CSgSynth::CalcFreq(int pos)
{
	if (pos < MIN_POS)
		pos = MIN_POS;
	else if (pos > MAX_POS)
		pos = MAX_POS;

	return exp(log(MIN_FREQ) + pos * (log(MAX_FREQ) - log(MIN_FREQ)) / MAX_POS);
}

void CSgSynth::SetCompFreq(int nIndex)
{
	g_oSetData.Sg.fCompFreq[nIndex] = m_cCompFreq[nIndex];
}

void CSgSynth::SetCompLevel(int nIndex)
{
	int nLevel = m_cCompLevel[nIndex];
	if (nLevel > 100) {
		nLevel = 100;
		m_cCompLevel[nIndex] = nLevel;
	}
	g_oSetData.Sg.nCompLevel[nIndex] = nLevel;
	m_cLevelSlider[nIndex].SetPos(100 - nLevel);
}

void CSgSynth::OnEnChangeCompFreq1()
{
	SetCompFreq(0);
}

void CSgSynth::OnEnChangeCompFreq2()
{
	SetCompFreq(1);
}

void CSgSynth::OnEnChangeCompFreq3()
{
	SetCompFreq(2);
}

void CSgSynth::OnEnChangeCompFreq4()
{
	SetCompFreq(3);
}

void CSgSynth::OnEnChangeCompFreq5()
{
	SetCompFreq(4);
}

void CSgSynth::OnEnChangeCompFreq6()
{
	SetCompFreq(5);
}

void CSgSynth::OnEnChangeCompFreq7()
{
	SetCompFreq(6);
}

void CSgSynth::OnEnChangeCompFreq8()
{
	SetCompFreq(7);
}

void CSgSynth::OnEnChangeCompLevel1()
{
	SetCompLevel(0);
}

void CSgSynth::OnEnChangeCompLevel2()
{
	SetCompLevel(1);
}

void CSgSynth::OnEnChangeCompLevel3()
{
	SetCompLevel(2);
}

void CSgSynth::OnEnChangeCompLevel4()
{
	SetCompLevel(3);
}

void CSgSynth::OnEnChangeCompLevel5()
{
	SetCompLevel(4);
}

void CSgSynth::OnEnChangeCompLevel6()
{
	SetCompLevel(5);
}

void CSgSynth::OnEnChangeCompLevel7()
{
	SetCompLevel(6);
}

void CSgSynth::OnEnChangeCompLevel8()
{
	SetCompLevel(7);
}

void CSgSynth::OnEnChangeSynthFreq()
{
	SetSynthFreq(m_cSynthFreq);
}

void CSgSynth::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	UINT pos;

	switch (pScrollBar->GetDlgCtrlID()) {
	case IDC_FREQ_SLIDER:
		pos = ((CSliderCtrl *)pScrollBar)->GetPos();
		SetSynthFreq(floor(CalcFreq(pos) + 0.5));
		break;
	}

	CDialogAF::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CSgSynth::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int nIndex = -1;

	switch (pScrollBar->GetDlgCtrlID()) {
	case IDC_LEVEL_SLIDER_1:
		nIndex = 0;
		break;
	case IDC_LEVEL_SLIDER_2:
		nIndex = 1;
		break;
	case IDC_LEVEL_SLIDER_3:
		nIndex = 2;
		break;
	case IDC_LEVEL_SLIDER_4:
		nIndex = 3;
		break;
	case IDC_LEVEL_SLIDER_5:
		nIndex = 4;
		break;
	case IDC_LEVEL_SLIDER_6:
		nIndex = 5;
		break;
	case IDC_LEVEL_SLIDER_7:
		nIndex = 6;
		break;
	case IDC_LEVEL_SLIDER_8:
		nIndex = 7;
		break;
	case IDC_SYNTH_FREQ_SPIN:
		if (nSBCode == SB_THUMBPOSITION)
			SetSynthFreq(nPos);
		break;
	}

	if (nIndex >= 0) {
		g_oSetData.Sg.nCompLevel[nIndex] = 100 - ((CSliderCtrl *)pScrollBar)->GetPos();
		m_cCompLevel[nIndex] = g_oSetData.Sg.nCompLevel[nIndex];
	}

	CDialogAF::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CSgSynth::OnCbnSelchangeWaveForm()
{
	g_oSetData.Sg.nSynthWave = m_cWaveForm.GetCurSel();
}

BOOL CSgSynth::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static const UINT aIDs[] = {
		IDC_COMP_FREQ_1, IDH_SG_SYNTH_COMP_FREQ_1_8,
		IDC_LEVEL_SLIDER_1, IDH_SG_SYNTH_LEVEL_SLIDER_1_8,
		IDC_COMP_LEVEL_1, IDH_SG_SYNTH_COMP_LEVEL_1_8,
		IDC_COMP_FREQ_2, IDH_SG_SYNTH_COMP_FREQ_1_8,
		IDC_LEVEL_SLIDER_2, IDH_SG_SYNTH_LEVEL_SLIDER_1_8,
		IDC_COMP_LEVEL_2, IDH_SG_SYNTH_COMP_LEVEL_1_8,
		IDC_COMP_FREQ_3, IDH_SG_SYNTH_COMP_FREQ_1_8,
		IDC_LEVEL_SLIDER_3, IDH_SG_SYNTH_LEVEL_SLIDER_1_8,
		IDC_COMP_LEVEL_3, IDH_SG_SYNTH_COMP_LEVEL_1_8,
		IDC_COMP_FREQ_4, IDH_SG_SYNTH_COMP_FREQ_1_8,
		IDC_LEVEL_SLIDER_4, IDH_SG_SYNTH_LEVEL_SLIDER_1_8,
		IDC_COMP_LEVEL_4, IDH_SG_SYNTH_COMP_LEVEL_1_8,
		IDC_COMP_FREQ_5, IDH_SG_SYNTH_COMP_FREQ_1_8,
		IDC_LEVEL_SLIDER_5, IDH_SG_SYNTH_LEVEL_SLIDER_1_8,
		IDC_COMP_LEVEL_5, IDH_SG_SYNTH_COMP_LEVEL_1_8,
		IDC_COMP_FREQ_6, IDH_SG_SYNTH_COMP_FREQ_1_8,
		IDC_LEVEL_SLIDER_6, IDH_SG_SYNTH_LEVEL_SLIDER_1_8,
		IDC_COMP_LEVEL_6, IDH_SG_SYNTH_COMP_LEVEL_1_8,
		IDC_COMP_FREQ_7, IDH_SG_SYNTH_COMP_FREQ_1_8,
		IDC_LEVEL_SLIDER_7, IDH_SG_SYNTH_LEVEL_SLIDER_1_8,
		IDC_COMP_LEVEL_7, IDH_SG_SYNTH_COMP_LEVEL_1_8,
		IDC_COMP_FREQ_8, IDH_SG_SYNTH_COMP_FREQ_1_8,
		IDC_LEVEL_SLIDER_8, IDH_SG_SYNTH_LEVEL_SLIDER_1_8,
		IDC_COMP_LEVEL_8, IDH_SG_SYNTH_COMP_LEVEL_1_8,
		IDC_FREQ_SLIDER, IDH_SG_SYNTH_FREQ_SLIDER,
		IDC_SYNTH_FREQ, IDH_SG_SYNTH_SYNTH_FREQ,
		IDC_WAVE_FORM, IDH_SG_SYNTH_WAVE_FORM,
		0
	};

	DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}
