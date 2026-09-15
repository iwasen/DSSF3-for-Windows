// WaveEditFM.cpp : 実装ファイル
//

#include "stdafx.h"
#include "RTA.h"
#include "WaveEditFM.h"
#include "Help\ContextHelp.h"

// CWaveEditFM ダイアログ

CWaveEditFM::CWaveEditFM(CWnd* pParent /*=NULL*/)
	: CDialogAF(CWaveEditFM::IDD, pParent)
{
}

void CWaveEditFM::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_WAVE_WND, m_cWaveWnd);
	DDX_Control(pDX, IDC_SLIDER_MOD_AMP, m_cSliderModAmp);
	DDX_Control(pDX, IDC_EDIT_MOD_AMP, m_cEditModAmp);
	DDX_Control(pDX, IDC_SLIDER_MOD_FREQ, m_cSliderModFreq);
	DDX_Control(pDX, IDC_EDIT_MOD_FREQ, m_cEditModFreq);
	DDX_Control(pDX, IDC_BTN_TEST, m_cBtnTest);
}

BEGIN_MESSAGE_MAP(CWaveEditFM, CDialogAF)
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_EDIT_MOD_AMP, &CWaveEditFM::OnEnChangeEditModAmp)
	ON_EN_CHANGE(IDC_EDIT_MOD_FREQ, &CWaveEditFM::OnEnChangeEditModFreq)
	ON_BN_CLICKED(IDC_BTN_TEST, &CWaveEditFM::OnBnClickedBtnTest)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

// CWaveEditFM メッセージ ハンドラ

BOOL CWaveEditFM::OnInitDialog()
{
	if (m_oWaveForm.nDataType != WAVEFORM_FM)
		InitData();

	CDialogAF::OnInitDialog();

	m_cWaveWnd.Initialize();

	m_cEditModAmp.Format("%.3f", m_oWaveForm.fModAmp);
	m_cEditModFreq.Format("%.3f", m_oWaveForm.fModFreq);

	m_cSliderModAmp.SetRange(0, 100);
	m_cSliderModAmp.SetPageSize(10);
	m_cSliderModAmp.SetTicFreq(10);
	m_cSliderModAmp.SetPos((int)(m_oWaveForm.fModAmp * 10));

	m_cSliderModFreq.SetRange(0, 100);
	m_cSliderModFreq.SetPageSize(10);
	m_cSliderModFreq.SetTicFreq(10);
	m_cSliderModFreq.SetPos((int)(m_oWaveForm.fModFreq * 10));

	DispGraphWindow();

	m_cEditModAmp.SetValidChar(VC_NUM | VC_POINT);
	m_cEditModFreq.SetValidChar(VC_NUM | VC_POINT);

	return TRUE;
}

void CWaveEditFM::OnOK()
{
}

void CWaveEditFM::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	switch (pScrollBar->GetDlgCtrlID()) {
	case IDC_SLIDER_MOD_AMP:
		m_oWaveForm.fModAmp = m_cSliderModAmp.GetPos() / float(10);
		m_cEditModAmp = m_oWaveForm.fModAmp;
		break;
	case IDC_SLIDER_MOD_FREQ:
		m_oWaveForm.fModFreq = m_cSliderModFreq.GetPos() / float(10);
		m_cEditModFreq = m_oWaveForm.fModFreq;
		break;
	}

	CDialogAF::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CWaveEditFM::OnEnChangeEditModAmp()
{
	m_oWaveForm.fModAmp = m_cEditModAmp;
	m_cSliderModAmp.SetPos((int)(m_oWaveForm.fModAmp * 10));
	DispGraphWindow();
}

void CWaveEditFM::OnEnChangeEditModFreq()
{
	m_oWaveForm.fModFreq = m_cEditModFreq;
	m_cSliderModFreq.SetPos((int)(m_oWaveForm.fModFreq * 10));
	DispGraphWindow();
}

void CWaveEditFM::OnBnClickedBtnTest()
{
	g_oWaveForm.SetTestWaveForm((WaveForm *)&m_oWaveForm);

	m_pSgDlg->StartStop(m_cBtnTest);
}

void CWaveEditFM::InitData()
{
	m_oWaveForm.nDataType = WAVEFORM_FM;
	m_oWaveForm.fModAmp = 0;
	m_oWaveForm.fModFreq = 0;
}

void CWaveEditFM::DispGraphWindow()
{
	m_cWaveWnd.DispGraph((WaveForm *)&m_oWaveForm);
}

BOOL CWaveEditFM::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static const UINT aIDs[] = {
		IDC_BTN_TEST, IDH_SG_EDIT_TEST,
		IDC_SLIDER_MOD_AMP, IDH_SG_EDIT_MOD_AMP,
		IDC_EDIT_MOD_AMP, IDH_SG_EDIT_MOD_AMP,
		IDC_SLIDER_MOD_FREQ, IDH_SG_EDIT_MOD_FREQ,
		IDC_EDIT_MOD_FREQ, IDH_SG_EDIT_MOD_FREQ,
		0
	};

	DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}
