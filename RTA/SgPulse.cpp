// SgPulse.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Rta.h"
#include "SgPulse.h"
#include "Help\ContextHelp.h"

/////////////////////////////////////////////////////////////////////////////
// CSgPulse ダイアログ

CSgPulse::CSgPulse(CWnd* pParent /*=NULL*/)
	: CDialogAF(CSgPulse::IDD, pParent)
{
	m_nSgPulsePolarity = -1;
}

void CSgPulse::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PULSE_NUM, m_cPulseNum);
	DDX_Control(pDX, IDC_PULSE_CYCLE, m_cPulseCycle);
	DDX_Control(pDX, IDC_PULSE_CONTINUE, m_cPulseContinue);
	DDX_Control(pDX, IDC_PULSE_WIDTH, m_cPulseWidth);
	DDX_Radio(pDX, IDC_PULSE_POSITIVE, m_nSgPulsePolarity);
	DDX_Control(pDX, IDC_PULSE_NUM_SPIN, m_cPulseNumSpin);
	DDX_Control(pDX, IDC_PULSE_CYCLE_SPIN, m_cPulseCycleSpin);
}

BEGIN_MESSAGE_MAP(CSgPulse, CDialogAF)
	ON_CBN_SELCHANGE(IDC_PULSE_WIDTH, OnSelchangePulseWidth)
	ON_BN_CLICKED(IDC_PULSE_POSITIVE, OnPulsePositive)
	ON_BN_CLICKED(IDC_PULSE_NEGATIVE, OnPulseNegative)
	ON_BN_CLICKED(IDC_PULSE_CONTINUE, OnPulseContinue)
	ON_EN_CHANGE(IDC_PULSE_CYCLE, OnChangePulseCycle)
	ON_MESSAGE(WM_TAB_NOTIFY, OnTabNotify)
	ON_EN_CHANGE(IDC_PULSE_NUM, OnChangePulseNum)
	ON_BN_CLICKED(IDC_PULSE_BOTH, OnPulseBoth)
	ON_WM_VSCROLL()
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSgPulse メッセージ ハンドラ

BOOL CSgPulse::OnInitDialog()
{
	CDialogAF::OnInitDialog();

	SetPulseWidth();

	m_cPulseNumSpin.SetRange(0, 9999);
	m_cPulseCycleSpin.SetRange(0, 9999);

	m_nSgPulsePolarity = g_oSetData.Sg.nPulsePolarity;
	m_cPulseNum = g_oSetData.Sg.nPulseNum;
	m_cPulseNumSpin.SetPos(g_oSetData.Sg.nPulseNum);
	m_cPulseContinue = g_oSetData.Sg.bPulseContinue;
	m_cPulseCycle = g_oSetData.Sg.fPulseCycle;
	m_cPulseCycleSpin.SetPos((int)g_oSetData.Sg.fPulseCycle);

	m_cPulseNum.EnableWindow(!g_oSetData.Sg.bPulseContinue);

	UpdateData(FALSE);

	m_cPulseNum.SetValidChar(VC_NUM);
	m_cPulseCycle.SetValidChar(VC_NUM | VC_POINT);

	return TRUE;
}

void CSgPulse::OnSelchangePulseWidth()
{
	g_oSetData.Sg.nPulseWidth = m_cPulseWidth.GetCurSel();
}

void CSgPulse::OnPulsePositive()
{
	g_oSetData.Sg.nPulsePolarity = 0;
}

void CSgPulse::OnPulseNegative()
{
	g_oSetData.Sg.nPulsePolarity = 1;
}

void CSgPulse::OnPulseBoth()
{
	g_oSetData.Sg.nPulsePolarity = 2;
}

void CSgPulse::OnChangePulseNum()
{
	g_oSetData.Sg.nPulseNum = m_cPulseNum;
	m_cPulseNumSpin.SetPos(g_oSetData.Sg.nPulseNum);
}

void CSgPulse::OnPulseContinue()
{
	g_oSetData.Sg.bPulseContinue = m_cPulseContinue;
	m_cPulseNum.EnableWindow(!g_oSetData.Sg.bPulseContinue);
}

void CSgPulse::OnChangePulseCycle()
{
	g_oSetData.Sg.fPulseCycle = m_cPulseCycle;
	m_cPulseCycleSpin.SetPos((int)g_oSetData.Sg.fPulseCycle);
}

void CSgPulse::SetPulseWidth()
{
	CString strBuf;

	const double t = 1000.0 / g_oSetData.Sg.nSamplingRate;

	m_cPulseWidth.ResetContent();
	for (int i = 1; i < 50; i++) {
		strBuf.Format("%.3f", t * i);
		m_cPulseWidth.AddString(strBuf);
	}
	m_cPulseWidth.SetCurSel(g_oSetData.Sg.nPulseWidth);
}

LRESULT CSgPulse::OnTabNotify(WPARAM wParam, LPARAM /*lParam*/)
{
	switch (wParam) {
	case TN_CHANGE_SAMPLING_RATE:
		SetPulseWidth();
		break;
	}

	return 0;
}

void CSgPulse::OnOK()
{
}

void CSgPulse::OnCancel()
{
}

void CSgPulse::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (nSBCode == SB_THUMBPOSITION) {
		switch (pScrollBar->GetDlgCtrlID()) {
		case IDC_PULSE_NUM_SPIN:
			g_oSetData.Sg.nPulseNum = nPos;
			m_cPulseNum = g_oSetData.Sg.nPulseNum;
			break;
		case IDC_PULSE_CYCLE_SPIN:
			g_oSetData.Sg.fPulseCycle = nPos;
			m_cPulseCycle = g_oSetData.Sg.fPulseCycle;
			break;
		}
	}

	CDialogAF::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CSgPulse::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static constexpr UINT aIDs[] = {
		IDC_PULSE_WIDTH, IDH_SG_PULSE_WIDTH,
		IDC_PULSE_NUM, IDH_SG_PULSE_NUM,
		IDC_PULSE_CONTINUE, IDH_SG_PULSE_CONTINUE,
		IDC_PULSE_CYCLE, IDH_SG_PULSE_CYCLE,
		IDC_PULSE_POSITIVE, IDH_SG_PULSE_POSITIVE,
		IDC_PULSE_NEGATIVE, IDH_SG_PULSE_NEGATIVE,
		IDC_PULSE_BOTH, IDH_SG_PULSE_BOTH,
		0
	};

	::DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}
