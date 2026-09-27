// FftFreqDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "RTA.h"
#include "FftFreqDlg.h"

#define MIN_POS 0
#define MAX_POS 300

// CFfrFreqDlg ダイアログ

CFftFreqDlg::CFftFreqDlg(CWnd* pParent /*=NULL*/)
	: CDialogAF(CFftFreqDlg::IDD, pParent)
{
}

void CFftFreqDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FFT_MAX_FREQ, m_cFftMaxFreq);
	DDX_Control(pDX, IDC_FFT_MIN_FREQ, m_cFftMinFreq);
}

BEGIN_MESSAGE_MAP(CFftFreqDlg, CDialogAF)
	ON_WM_DESTROY()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

// CFfrFreqDlg メッセージ ハンドラ

BOOL CFftFreqDlg::OnInitDialog()
{
	CDialogAF::OnInitDialog();

	m_cFftMaxFreq.SetRange(MIN_POS, MAX_POS);
	m_cFftMaxFreq.SetPageSize(MAX_POS / 20);
	m_cFftMaxFreq.SetPos(CalcSliderPos(m_nMaxFreq));
	SetFreqScale(m_cFftMaxFreq);

	m_cFftMinFreq.SetRange(MIN_POS, MAX_POS);
	m_cFftMinFreq.SetPageSize(MAX_POS / 20);
	m_cFftMinFreq.SetPos(CalcSliderPos(m_nMinFreq));
	SetFreqScale(m_cFftMinFreq);

	return TRUE;
}

void CFftFreqDlg::OnDestroy()
{
	CDialogAF::OnDestroy();

	m_pFftDlg->CloseFreqDlg();
}

void CFftFreqDlg::OnOK()
{
	DestroyWindow();
}

void CFftFreqDlg::OnCancel()
{
	DestroyWindow();
}

void CFftFreqDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	switch (pScrollBar->GetDlgCtrlID()) {
	case IDC_FFT_MAX_FREQ:
		if (m_cFftMaxFreq.GetPos() <= m_cFftMinFreq.GetPos())
			m_cFftMaxFreq.SetPos(m_cFftMinFreq.GetPos() + 1);
		m_pFftDlg->SetFftMaxFreq(CalcFreq(m_cFftMaxFreq.GetPos()));
		break;
	case IDC_FFT_MIN_FREQ:
		if (m_cFftMinFreq.GetPos() >= m_cFftMaxFreq.GetPos())
			m_cFftMinFreq.SetPos(m_cFftMaxFreq.GetPos() - 1);
		m_pFftDlg->SetFftMinFreq(CalcFreq(m_cFftMinFreq.GetPos()));
		break;
	}

	CDialogAF::OnHScroll(nSBCode, nPos, pScrollBar);
}

int CFftFreqDlg::CalcSliderPos(double fFreq) const
{
	if (fFreq < MIN_FREQ)
		fFreq = MIN_FREQ;
	else if (fFreq > m_nMaxRange)
		fFreq = m_nMaxRange;

	int nPos = (int)((log(fFreq) - log((double)m_nMinRange)) * MAX_POS / (log((double)m_nMaxRange) - log((double)m_nMinRange)) + 0.5);

	if (nPos < 0)
		nPos = 0;
	else if (nPos > MAX_POS)
		nPos = MAX_POS;

	return nPos;
}

int CFftFreqDlg::CalcFreq(int pos) const
{
	return (int)(exp(log((double)m_nMinRange) + pos * (log((double)m_nMaxRange) - log((double)m_nMinRange)) / MAX_POS) + 0.5);
}

void CFftFreqDlg::SetFreqScale(CSliderCtrl &oSliderCtrl)
{
	oSliderCtrl.ClearTics();
	for (int nFreq = m_nMinRange, nTic = 0; nFreq <= m_nMaxRange; nTic++) {
		int pos = CalcSliderPos(nFreq);
		oSliderCtrl.SetTic(pos);

//		if (nTic % 9 == 0)
//			oSliderCtrl.AddTicString(nTic, GetNumberString(nFreq));

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

void CFftFreqDlg::PostNcDestroy()
{
	CDialogAF::PostNcDestroy();

	delete this;
}
