// GraphSpec.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Sas.h"
#include "GraphSpec.h"
#include "Calc.h"
#include "Help\ContextHelp.h"

#define COLOR_LEFT			RGB(0, 128, 255)
#define COLOR_RIGHT			RGB(255, 64, 192)

/////////////////////////////////////////////////////////////////////////////
// CGraphSpec ダイアログ

CGraphSpec::CGraphSpec(CWnd* pParent /*=NULL*/)
	: CDialogAF(CGraphSpec::IDD, pParent)
{
	m_pWaveLeft = NULL;
	m_pWaveRight = NULL;
	m_pSpecLeft = NULL;
	m_pSpecRight = NULL;
	m_pWindowFunc = FALSE;
	m_bReDraw = FALSE;
}

void CGraphSpec::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRAPH_RIGHT, m_cGraphRight);
	DDX_Control(pDX, IDC_GRAPH_LEFT, m_cGraphLeft);
}

BEGIN_MESSAGE_MAP(CGraphSpec, CDialogAF)
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphSpec メッセージ ハンドラ

BOOL CGraphSpec::OnInitDialog()
{
	CDialogAF::OnInitDialog();

	PWAVEDATA pWaveData = (PWAVEDATA)::GlobalLock(m_hWaveData);
	m_nChannels = pWaveData->nChannels;
	m_nData = (int)(m_pAcfCondition->fIntegrationTime * m_fRate);
	::GlobalUnlock(m_hWaveData);

	for (m_nFftSize = 1; m_nFftSize < m_nData; m_nFftSize *= 2)
		;

	m_pWaveLeft = new double[m_nFftSize];
	m_pSpecLeft = new double[m_nFftSize];
	m_pWaveRight = new double[m_nFftSize];
	m_pSpecRight = new double[m_nFftSize];
	memset(m_pWaveLeft, 0, m_nFftSize * sizeof(double));
	memset(m_pWaveRight, 0, m_nFftSize * sizeof(double));

	m_pWindowFunc = new double[m_nData];
	MakeWindowFunc(WF_HANNING, m_nData, m_pWindowFunc);

	if (m_nChannels == 2) {
		CString str;
		str.LoadString(IDS_LEFTCHANNEL);
		m_cGraphLeft.Initialize(90, str, 1, COLOR_LEFT, 0);
		m_cGraphLeft.EnableWindow(TRUE);
		str.LoadString(IDS_RIGHTCHANNEL);
		m_cGraphRight.Initialize(90, str, 1, COLOR_RIGHT, 0);
	} else {
		RECT rectLeft, rectRight;
		m_cGraphLeft.GetWindowRect(&rectLeft);
		m_cGraphRight.GetWindowRect(&rectRight);
		m_cGraphLeft.SetWindowPos(NULL, 0, 0, rectLeft.right - rectLeft.left, rectRight.bottom - rectLeft.top, SWP_NOMOVE | SWP_NOZORDER);
		m_cGraphLeft.Initialize(90, NULL, 1, COLOR_LEFT, 0);
		m_cGraphRight.ShowWindow(SW_HIDE);
	}

	DispGraphWindow(TRUE);

	return TRUE;
}

void CGraphSpec::DispGraphWindow(BOOL bCalc)
{
	if (bCalc) {
		ReadWaveData(m_hWaveData, m_pWaveLeft, m_pWaveRight, m_nData, (int)(m_nStep * m_pAcfCondition->fRunningStep * m_fRate));
		for (int i = 0; i < m_nData; i++) {
			m_pWaveLeft[i] *= m_pWindowFunc[i];
			if (m_nChannels == 2)
				m_pWaveRight[i] *= m_pWindowFunc[i];
		}

		CalcPowerSpectrum(m_pWaveLeft, m_pWaveRight, m_pSpecLeft, m_pSpecRight, m_nFftSize, FALSE);
/*
		for (int i = 0; i < m_nFftSize; i++) {
			m_pSpecLeft[i] /= m_nData;
			if (m_nChannels == 2)
				m_pSpecRight[i] /= m_nData;
		}
*/
	}

	m_cGraphLeft.DispGraph(m_pSpecLeft, m_nFftSize, m_fRate, 10, (int)(m_fRate / 2), -160, 0, NULL, TRUE);
	if (m_nChannels == 2)
		m_cGraphRight.DispGraph(m_pSpecRight, m_nFftSize, m_fRate, 10, (int)(m_fRate / 2), -160, 0, NULL, TRUE);
}

void CGraphSpec::OnDestroy()
{
	CDialogAF::OnDestroy();

	if (m_pWaveLeft != NULL)
		delete [] m_pWaveLeft;

	if (m_pWaveRight != NULL)
		delete [] m_pWaveRight;

	if (m_pSpecLeft != NULL)
		delete [] m_pSpecLeft;

	if (m_pSpecRight != NULL)
		delete [] m_pSpecRight;

	if (m_pWindowFunc != NULL)
		delete [] m_pWindowFunc;
}

void CGraphSpec::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogAF::OnShowWindow(bShow, nStatus);

	if (bShow && m_bReDraw) {
		DispGraphWindow(TRUE);
		m_bReDraw = FALSE;
	}
}

void CGraphSpec::ReDraw(int nSel)
{
	if (nSel != -1)
		m_nStep = nSel;

	if (m_hWnd != NULL) {
		if (IsWindowVisible()) {
			DispGraphWindow(TRUE);
		} else
			m_bReDraw = TRUE;
	}
}

BOOL CGraphSpec::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static constexpr UINT aIDs[] = {
		IDC_GRAPH_LEFT, IDH_GRAPH_SPEC_LEFT,
		IDC_GRAPH_RIGHT, IDH_GRAPH_SPEC_RIGHT,
		0
	};

	::DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}
