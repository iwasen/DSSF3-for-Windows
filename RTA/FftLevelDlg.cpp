// FftLevelDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "RTA.h"
#include "FftLevelDlg.h"

// CFftLevelDlg ダイアログ

CFftLevelDlg::CFftLevelDlg(CWnd* pParent /*=NULL*/)
	: CDialogAF(CFftLevelDlg::IDD, pParent)
{
}

void CFftLevelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FFT_MAX_LEVEL, m_cFftMaxLevel);
	DDX_Control(pDX, IDC_FFT_MIN_LEVEL, m_cFftMinLevel);
}

BEGIN_MESSAGE_MAP(CFftLevelDlg, CDialogAF)
	ON_WM_DESTROY()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

// CFftLevelDlg メッセージ ハンドラ

BOOL CFftLevelDlg::OnInitDialog()
{
	CDialogAF::OnInitDialog();

	m_cFftMaxLevel.SetRange(m_nMinRange + 1000, m_nMaxRange + 1000);
	m_cFftMaxLevel.SetPos(m_nMaxLevel + 1000);
	m_cFftMaxLevel.SetTicFreq(10);

	m_cFftMinLevel.SetRange(m_nMinRange + 1000, m_nMaxRange + 1000);
	m_cFftMinLevel.SetPos(m_nMinLevel + 1000);
	m_cFftMinLevel.SetTicFreq(10);

	return TRUE;
}

void CFftLevelDlg::OnDestroy()
{
	CDialogAF::OnDestroy();

	m_pFftDlg->CloseLevelDlg();
}

void CFftLevelDlg::OnOK()
{
	DestroyWindow();
}

void CFftLevelDlg::OnCancel()
{
	DestroyWindow();
}

void CFftLevelDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	switch (pScrollBar->GetDlgCtrlID()) {
	case IDC_FFT_MAX_LEVEL:
		if (m_cFftMaxLevel.GetPos() <= m_cFftMinLevel.GetPos())
			m_cFftMaxLevel.SetPos(m_cFftMinLevel.GetPos() + 1);
		m_pFftDlg->SetFftMaxLevel(m_cFftMaxLevel.GetPos() - 1000);
		break;
	case IDC_FFT_MIN_LEVEL:
		if (m_cFftMinLevel.GetPos() >= m_cFftMaxLevel.GetPos())
			m_cFftMinLevel.SetPos(m_cFftMaxLevel.GetPos() - 1);
		m_pFftDlg->SetFftMinLevel(m_cFftMinLevel.GetPos() - 1000);
		break;
	}

	CDialogAF::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CFftLevelDlg::PostNcDestroy()
{
	CDialogAF::PostNcDestroy();

	delete this;
}
