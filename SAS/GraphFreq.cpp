// GraphFreq.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Sas.h"
#include "GraphFreq.h"
#include "Calc.h"
#include "Help\ContextHelp.h"

#define COLOR_LEFT			RGB(0, 128, 255)
#define COLOR_RIGHT			RGB(255, 64, 192)

/////////////////////////////////////////////////////////////////////////////
// CGraphFreq ダイアログ

CGraphFreq::CGraphFreq(CWnd* pParent /*=NULL*/)
	: CDialogAF(CGraphFreq::IDD, pParent)
{
	m_bMean = TRUE;

	m_pFreqLeft = NULL;
	m_pFreqRight = NULL;
	m_bReDraw = FALSE;
}

void CGraphFreq::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FREQ_RIGHT, m_cFreqRight);
	DDX_Control(pDX, IDC_FREQ_LEFT, m_cFreqLeft);
	DDX_Check(pDX, IDC_MEAN, m_bMean);
}

BEGIN_MESSAGE_MAP(CGraphFreq, CDialogAF)
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_MEAN, OnMean)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphFreq メッセージ ハンドラ

BOOL CGraphFreq::OnInitDialog()
{
	CDialogAF::OnInitDialog();

	CString str;
	if (m_pDbImpulseRec->nChannel == 1) {
		str.LoadString(IDS_SPECTRUM);
		m_cFreqLeft.Initialize(90, str, 1, COLOR_LEFT, 0);
	} else {
		str.LoadString(IDS_LEFTCHANNEL);
		m_cFreqLeft.Initialize(90, str, 1, COLOR_LEFT, 0);
		str.LoadString(IDS_RIGHTCHANNEL);
		m_cFreqRight.Initialize(90, str, 1, COLOR_RIGHT, 0);
	}

	m_nData = 1 << m_pDbImpulseRec->nMeasTime;

	m_pFreqLeft = new double[m_nData];
	m_pFreqRight = new double[m_nData];

	m_cFreqLeft.EnableWindow(TRUE);
	if (m_pDbImpulseRec->nChannel == 2)
		m_cFreqRight.EnableWindow(TRUE);

	DispFreqWindow(TRUE);

	return TRUE;
}

void CGraphFreq::DispFreqWindow(BOOL bCalc)
{
	if (bCalc) {
		CalcPowerSpectrum(m_pWaveLeft, m_pWaveRight, m_pFreqLeft, m_pFreqRight, m_nData);
		for (int i = 0; i < m_nData; i++) {
			m_pFreqLeft[i] /= m_nData;
			if (m_pDbImpulseRec->nChannel == 2)
				m_pFreqRight[i] /= m_nData;
		}
	}

	m_cFreqLeft.DispGraph(m_pFreqLeft, m_nData, m_fRate, 10, (int)(m_fRate / 2), -140, 0, NULL, m_bMean);
	if (m_pDbImpulseRec->nChannel == 2)
		m_cFreqRight.DispGraph(m_pFreqRight, m_nData, m_fRate, 10, (int)(m_fRate / 2), -140, 0, NULL, m_bMean);
}

void CGraphFreq::OnDestroy()
{
	CDialogAF::OnDestroy();

	if (m_pFreqLeft != NULL)
		delete [] m_pFreqLeft;

	if (m_pFreqRight != NULL)
		delete [] m_pFreqRight;
}

void CGraphFreq::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogAF::OnShowWindow(bShow, nStatus);

	if (bShow && m_bReDraw) {
		DispFreqWindow(TRUE);
		m_bReDraw = FALSE;
	}
}

void CGraphFreq::ReDraw()
{
	if (m_hWnd != NULL) {
		if (IsWindowVisible())
			DispFreqWindow(TRUE);
		else
			m_bReDraw = TRUE;
	}
}

void CGraphFreq::OnMean()
{
	UpdateData(TRUE);
	DispFreqWindow(FALSE);
}

BOOL CGraphFreq::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static const UINT aIDs[] = {
		IDC_FREQ_LEFT, IDH_GRAPH_FREQ_LEFT,
		IDC_FREQ_RIGHT, IDH_GRAPH_FREQ_RIGHT,
		IDC_MEAN, IDH_GRAPH_FREQ_MEAN,
		0
	};

	DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}
