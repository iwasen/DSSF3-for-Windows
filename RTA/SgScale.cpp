// SgScale.cpp : 実装ファイル
//

#include "stdafx.h"
#include "Rta.h"
#include "SgScale.h"
#include "Help\ContextHelp.h"

// CSgScale ダイアログ

CSgScale::CSgScale(CWnd* pParent /*=NULL*/)
	: CDialogAF(CSgScale::IDD, pParent)
	, m_iOctave(0)
	, m_iScale(0)
{
}

void CSgScale::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_KEYBOARD, m_cKeyboard);
	DDX_Control(pDX, IDC_WAVE_FORM, m_cWaveForm);
	DDX_Control(pDX, IDC_REFERENCE_PITCH, m_cReferencePitch);
	DDX_Control(pDX, IDC_FREQ, m_cFreq);
	DDX_Radio(pDX, IDC_SCALE_C, m_iScale);
	DDX_Radio(pDX, IDC_OCTAVE0, m_iOctave);
	DDX_Control(pDX, IDC_PLAY_MODE, m_cPlayMode);
	DDX_Control(pDX, IDC_REFERENCE_PITCH_SPIN, m_cReferencePitchSpin);
}

BEGIN_MESSAGE_MAP(CSgScale, CDialogAF)
	ON_MESSAGE(WM_KEYBOARD, OnKeyboard)
	ON_BN_CLICKED(IDC_SCALE_C, OnBnClickedScaleC)
	ON_BN_CLICKED(IDC_SCALE_CS, OnBnClickedScaleCs)
	ON_BN_CLICKED(IDC_SCALE_D, OnBnClickedScaleD)
	ON_BN_CLICKED(IDC_SCALE_DS, OnBnClickedScaleDs)
	ON_BN_CLICKED(IDC_SCALE_E, OnBnClickedScaleE)
	ON_BN_CLICKED(IDC_SCALE_F, OnBnClickedScaleF)
	ON_BN_CLICKED(IDC_SCALE_FS, OnBnClickedScaleFs)
	ON_BN_CLICKED(IDC_SCALE_G, OnBnClickedScaleG)
	ON_BN_CLICKED(IDC_SCALE_GS, OnBnClickedScaleGs)
	ON_BN_CLICKED(IDC_SCALE_A, OnBnClickedScaleA)
	ON_BN_CLICKED(IDC_SCALE_AS, OnBnClickedScaleAs)
	ON_BN_CLICKED(IDC_SCALE_B, OnBnClickedScaleB)
	ON_BN_CLICKED(IDC_OCTAVE0, OnBnClickedOctave0)
	ON_BN_CLICKED(IDC_OCTAVE1, OnBnClickedOctave1)
	ON_BN_CLICKED(IDC_OCTAVE2, OnBnClickedOctave2)
	ON_BN_CLICKED(IDC_OCTAVE3, OnBnClickedOctave3)
	ON_BN_CLICKED(IDC_OCTAVE4, OnBnClickedOctave4)
	ON_BN_CLICKED(IDC_OCTAVE5, OnBnClickedOctave5)
	ON_BN_CLICKED(IDC_OCTAVE6, OnBnClickedOctave6)
	ON_BN_CLICKED(IDC_OCTAVE7, OnBnClickedOctave7)
	ON_BN_CLICKED(IDC_OCTAVE8, OnBnClickedOctave8)
	ON_CBN_SELCHANGE(IDC_WAVE_FORM, OnCbnSelchangeWaveForm)
	ON_EN_CHANGE(IDC_REFERENCE_PITCH, OnEnChangeReferencePitch)
	ON_BN_CLICKED(IDC_PLAY_MODE, OnBnClickedPlayMode)
	ON_WM_VSCROLL()
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

// CSgScale メッセージ ハンドラ

BOOL CSgScale::OnInitDialog()
{
	CDialogAF::OnInitDialog();

	m_cReferencePitch = g_oSetData.Sg.fReferencePitch;

	m_cReferencePitchSpin.SetRange(1, 9999);
	m_cReferencePitchSpin.SetPos((int)g_oSetData.Sg.fReferencePitch);

	SetWaveFormList(m_cWaveForm, g_oSetData.Sg.nScaleWave);

	m_cPlayMode = g_oSetData.Sg.bPlayMode;

	m_iScale = g_oSetData.Sg.nScale;
	m_iOctave = g_oSetData.Sg.nOctave;

	m_cKeyboard.SetMarker(g_oSetData.Sg.nOctave, g_oSetData.Sg.nScale);

	DispFreq();

	UpdateData(FALSE);

	m_cReferencePitch.SetValidChar(VC_NUM | VC_POINT);

	return TRUE;
}

afx_msg LRESULT CSgScale::OnKeyboard(WPARAM wParam, LPARAM lParam)
{
	if (wParam != 0 || lParam != 0) {
		if (m_iOctave != (int)wParam) {
			UpdateData(TRUE);
			m_iOctave = (int)wParam;
			UpdateData(FALSE);

			SetOctave(m_iOctave);
		}

		if (m_iScale != lParam) {
			UpdateData(TRUE);
			m_iScale = (int)lParam;
			UpdateData(FALSE);

			SetScale(m_iScale);
		}
	}

	if (g_oSetData.Sg.bPlayMode) {
		if (wParam == 0 && lParam == 0)
			GetOwner()->SendMessage(WM_SG_CHANGE, 2, 0);
		else
			GetOwner()->SendMessage(WM_SG_CHANGE, 2, 1);
	}

	return 0;
}

void CSgScale::OnBnClickedScaleC()
{
	SetScale(0);
}

void CSgScale::OnBnClickedScaleCs()
{
	SetScale(1);
}

void CSgScale::OnBnClickedScaleD()
{
	SetScale(2);
}

void CSgScale::OnBnClickedScaleDs()
{
	SetScale(3);
}

void CSgScale::OnBnClickedScaleE()
{
	SetScale(4);
}

void CSgScale::OnBnClickedScaleF()
{
	SetScale(5);
}

void CSgScale::OnBnClickedScaleFs()
{
	SetScale(6);
}

void CSgScale::OnBnClickedScaleG()
{
	SetScale(7);
}

void CSgScale::OnBnClickedScaleGs()
{
	SetScale(8);
}

void CSgScale::OnBnClickedScaleA()
{
	SetScale(9);
}

void CSgScale::OnBnClickedScaleAs()
{
	SetScale(10);
}

void CSgScale::OnBnClickedScaleB()
{
	SetScale(11);
}

void CSgScale::OnBnClickedOctave0()
{
	SetOctave(0);
}

void CSgScale::OnBnClickedOctave1()
{
	SetOctave(1);
}

void CSgScale::OnBnClickedOctave2()
{
	SetOctave(2);
}

void CSgScale::OnBnClickedOctave3()
{
	SetOctave(3);
}

void CSgScale::OnBnClickedOctave4()
{
	SetOctave(4);
}

void CSgScale::OnBnClickedOctave5()
{
	SetOctave(5);
}

void CSgScale::OnBnClickedOctave6()
{
	SetOctave(6);
}

void CSgScale::OnBnClickedOctave7()
{
	SetOctave(7);
}

void CSgScale::OnBnClickedOctave8()
{
	SetOctave(8);
}

void CSgScale::SetScale(int nScale)
{
	g_oSetData.Sg.nScale = nScale;
	m_cKeyboard.SetMarker(g_oSetData.Sg.nOctave, g_oSetData.Sg.nScale);
	DispFreq();
}

void CSgScale::SetOctave(int nOctave)
{
	g_oSetData.Sg.nOctave = nOctave;
	m_cKeyboard.SetMarker(g_oSetData.Sg.nOctave, g_oSetData.Sg.nScale);
	DispFreq();
}

void CSgScale::OnCbnSelchangeWaveForm()
{
	g_oSetData.Sg.nScaleWave = m_cWaveForm.GetCurSel();
}

void CSgScale::DispFreq()
{
	const int nPitch = (g_oSetData.Sg.nOctave - 4) * 12 + (g_oSetData.Sg.nScale - 9);
	const double fFreq = g_oSetData.Sg.fReferencePitch * pow(2.0, (double)nPitch / 12);

	m_cFreq.Format("%.1f", fFreq);
}

void CSgScale::OnEnChangeReferencePitch()
{
	g_oSetData.Sg.fReferencePitch = m_cReferencePitch;
	m_cReferencePitchSpin.SetPos((int)g_oSetData.Sg.fReferencePitch);
	DispFreq();
}

void CSgScale::OnBnClickedPlayMode()
{
	g_oSetData.Sg.bPlayMode = m_cPlayMode;
}

void CSgScale::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (nSBCode == SB_THUMBPOSITION) {
		switch (pScrollBar->GetDlgCtrlID()) {
		case IDC_REFERENCE_PITCH_SPIN:
			g_oSetData.Sg.fReferencePitch = nPos;
			m_cReferencePitch = g_oSetData.Sg.fReferencePitch;
			break;
		}
	}

	CDialogAF::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CSgScale::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static constexpr UINT aIDs[] = {
		IDC_KEYBOARD, IDH_SG_SCALE_KEYBOARD,
		IDC_REFERENCE_PITCH, IDH_SG_SCALE_REFERENCE_PITCH,
		IDC_WAVE_FORM, IDH_SG_SCALE_WAVE_FORM,
		IDC_PLAY_MODE, IDH_SG_SCALE_PLAY_MODE,
		IDC_SCALE_C, IDH_SG_SCALE_SCALE_C,
		IDC_SCALE_CS, IDH_SG_SCALE_SCALE_CS,
		IDC_SCALE_D, IDH_SG_SCALE_SCALE_D,
		IDC_SCALE_DS, IDH_SG_SCALE_SCALE_DS,
		IDC_SCALE_E, IDH_SG_SCALE_SCALE_E,
		IDC_SCALE_F, IDH_SG_SCALE_SCALE_F,
		IDC_SCALE_FS, IDH_SG_SCALE_SCALE_FS,
		IDC_SCALE_G, IDH_SG_SCALE_SCALE_G,
		IDC_SCALE_GS, IDH_SG_SCALE_SCALE_GS,
		IDC_SCALE_A, IDH_SG_SCALE_SCALE_A,
		IDC_SCALE_AS, IDH_SG_SCALE_SCALE_AS,
		IDC_SCALE_B, IDH_SG_SCALE_SCALE_B,
		IDC_FREQ, IDH_SG_SCALE_FREQ,
		IDC_OCTAVE0, IDH_SG_SCALE_OCTAVE0_8,
		IDC_OCTAVE1, IDH_SG_SCALE_OCTAVE0_8,
		IDC_OCTAVE2, IDH_SG_SCALE_OCTAVE0_8,
		IDC_OCTAVE3, IDH_SG_SCALE_OCTAVE0_8,
		IDC_OCTAVE4, IDH_SG_SCALE_OCTAVE0_8,
		IDC_OCTAVE5, IDH_SG_SCALE_OCTAVE0_8,
		IDC_OCTAVE6, IDH_SG_SCALE_OCTAVE0_8,
		IDC_OCTAVE7, IDH_SG_SCALE_OCTAVE0_8,
		IDC_OCTAVE8, IDH_SG_SCALE_OCTAVE0_8,
		0
	};

	::DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}
