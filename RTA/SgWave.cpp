// SgWave.cpp : 実装ファイル
//

#include "stdafx.h"
#include "RTA.h"
#include "SgWave.h"
#include "WaveEditDlg.h"
#include "WaveEditManual.h"
#include "Help\ContextHelp.h"

#define MIN_POS		0
#define MAX_POS		300

// CSgWave ダイアログ

CSgWave::CSgWave(CWnd* pParent /*=NULL*/)
	: CDialogAF(CSgWave::IDD, pParent)
	, m_iWaveForm(g_oSetData.Sg.nWaveFormNo)
{
	m_pWaveForm = NULL;
}

void CSgWave::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_WAVE_WND, m_cWaveWnd);
	DDX_Control(pDX, IDC_SFREQ, m_cSFreq);
	DDX_Control(pDX, IDC_EFREQ, m_cEFreq);
	DDX_Radio(pDX, IDC_WAVEFORM1, m_iWaveForm);
	DDX_Control(pDX, IDC_EFREQ_SPIN, m_cEFreqSpin);
}

BEGIN_MESSAGE_MAP(CSgWave, CDialogAF)
	ON_BN_CLICKED(IDC_EDIT, OnBnClickedEdit)
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_EFREQ, OnEnChangeEfreq)
	ON_BN_CLICKED(IDC_WAVEFORM1, OnBnClickedWaveform)
	ON_BN_CLICKED(IDC_WAVEFORM2, OnBnClickedWaveform)
	ON_BN_CLICKED(IDC_WAVEFORM3, OnBnClickedWaveform)
	ON_BN_CLICKED(IDC_WAVEFORM4, OnBnClickedWaveform)
	ON_BN_CLICKED(IDC_WAVEFORM5, OnBnClickedWaveform)
	ON_WM_VSCROLL()
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

// CSgWave メッセージ ハンドラ

BOOL CSgWave::OnInitDialog()
{
	CDialogAF::OnInitDialog();

	m_cSFreq.SetRange(MIN_POS, MAX_POS);
	m_cSFreq.SetPageSize(MAX_POS / 20);

	m_cEFreqSpin.SetRange32(0, 99999);

	SetFreqRange();

	m_cEFreq = g_oSetData.Sg.fWaveFreq;
	m_cSFreq.SetPos(CalcSliderPos(g_oSetData.Sg.fWaveFreq));

	m_cEFreq.SetValidChar(VC_NUM | VC_POINT);

	m_cWaveWnd.Initialize();

	DispWaveView();

	return TRUE;
}

void CSgWave::OnBnClickedEdit()
{
	CWaveEditDlg dlg;

	dlg.m_pSgDlg = (CSgDlg *)GetParent();
	dlg.m_pWaveForm = m_pWaveForm;
	INT_PTR ret = dlg.DoModal();
	if (ret == IDOK) {
		g_oWaveForm.WriteWaveForm(g_oSetData.Sg.nWaveFormNo);
		DispWaveView();
	}
}

void CSgWave::SetFreqRange()
{
	int	nFreq;
	int	pos;
	int nTic;

	m_cSFreq.ClearTics();
	for (nFreq = (int)MIN_FREQ, nTic = 0; nFreq <= (int)MAX_FREQ; nTic++) {
		pos = CalcSliderPos(nFreq);
		m_cSFreq.SetTic(pos);

		if (g_oSetData.Sg.nFreqRange == 0) {
			if (nTic % 9 == 0)
				m_cSFreq.AddTicString(nTic, GetNumberString(nFreq));
		} else {
			if (nTic == 0 || nTic == 1 || nTic == 4 || nTic == 9)
				m_cSFreq.AddTicString(nTic, GetNumberString(nFreq));
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

int CSgWave::CalcSliderPos(double fFreq)
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

void CSgWave::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	UINT pos;

	switch (pScrollBar->GetDlgCtrlID()) {
	case IDC_SFREQ:
		pos = ((CSliderCtrl *)pScrollBar)->GetPos();
		SetFreq(floor(CalcFreq(pos) * 10 + 0.5) / 10);
		break;
	}

	CDialogAF::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CSgWave::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (nSBCode == SB_THUMBPOSITION) {
		switch (pScrollBar->GetDlgCtrlID()) {
		case IDC_EFREQ_SPIN:
			SetFreq(nPos);
		}
	}

	CDialogAF::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CSgWave::SetFreq(double fFreq)
{
	if (fFreq < 0)
		fFreq = 0;
	else if (fFreq > MAX_FREQ)
		fFreq = MAX_FREQ;

	if ((double)m_cEFreq != fFreq)
		m_cEFreq = fFreq;

	m_cSFreq.SetPos(CalcSliderPos(fFreq));
	m_cEFreqSpin.SetPos32((int)fFreq);
	g_oSetData.Sg.fWaveFreq = fFreq;
}

double CSgWave::CalcFreq(int pos)
{
	if (pos < MIN_POS)
		pos = MIN_POS;
	else if (pos > MAX_POS)
		pos = MAX_POS;

	return exp(log(MIN_FREQ) + pos * (log(MAX_FREQ) - log(MIN_FREQ)) / MAX_POS);
}

void CSgWave::OnEnChangeEfreq()
{
	SetFreq(m_cEFreq);
}

void CSgWave::DispWaveView()
{
	m_pWaveForm = g_oWaveForm.ReadWaveForm(g_oSetData.Sg.nWaveFormNo);
	m_cWaveWnd.DispGraph(m_pWaveForm);
}

void CSgWave::OnBnClickedWaveform()
{
	UpdateData(TRUE);
	g_oSetData.Sg.nWaveFormNo = m_iWaveForm;
	DispWaveView();
}

BOOL CSgWave::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static const UINT aIDs[] = {
		IDC_WAVE_WND, IDH_SG_WAVE_WND,
		IDC_WAVEFORM1, IDH_SG_WAVE_WAVEFORM1_5,
		IDC_WAVEFORM2, IDH_SG_WAVE_WAVEFORM1_5,
		IDC_WAVEFORM3, IDH_SG_WAVE_WAVEFORM1_5,
		IDC_WAVEFORM4, IDH_SG_WAVE_WAVEFORM1_5,
		IDC_WAVEFORM5, IDH_SG_WAVE_WAVEFORM1_5,
		IDC_EDIT, IDH_SG_WAVE_EDIT,
		IDC_SFREQ, IDH_SG_WAVE_SFREQ,
		IDC_EFREQ, IDH_SG_WAVE_EFREQ,
		0
	};

	DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}
