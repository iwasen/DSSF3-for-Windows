// GraphACF.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Sas.h"
#include "GraphACF.h"
#include "CalcAcf.h"
#include "MakeFilter.h"
#include "Help\ContextHelp.h"

#define SCROLL_RANGE 16384
#define MAX_ZOOM_TIME	512

/////////////////////////////////////////////////////////////////////////////
// CGraphACF ダイアログ

CGraphACF::CGraphACF(CWnd* pParent /*=NULL*/)
	: CDialogExt(CGraphACF::IDD, pParent)
{
	m_bLog = FALSE;
	m_pAcfData = NULL;
	m_pAcfDataLog = NULL;
	m_nScrollSize = -1;
	m_nScrollPos = -1;
	m_bReDraw = FALSE;
	m_fStartTime = 0;
	m_fDispTime = 0;
}

void CGraphACF::DoDataExchange(CDataExchange* pDX)
{
	CDialogExt::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ZOOM_H, m_cZoomH);
	DDX_Control(pDX, IDC_SCROLL_H, m_cScrollH);
	DDX_Control(pDX, IDC_GRAPH_ACF, m_cGraphAcf);
	DDX_Check(pDX, IDC_LOG, m_bLog);
}

BEGIN_MESSAGE_MAP(CGraphACF, CDialogExt)
	ON_COMMAND(IDM_ZOOM_FULL_H, OnZoomFullH)
	ON_COMMAND(IDM_ZOOM_IN_H, OnZoomInH)
	ON_COMMAND(IDM_ZOOM_OUT_H, OnZoomOutH)
	ON_WM_HSCROLL()
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_LOG, OnLog)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphACF メッセージ ハンドラ

BOOL CGraphACF::OnInitDialog()
{
	CDialogExt::OnInitDialog();

	m_cGraphAcf.Initialize(90, "ACF", GraphCallBack, (LPARAM)this);

	if (m_tbZoomH.Create(this, WS_CHILD | WS_VISIBLE | CBRS_SIZE_FIXED | CBRS_ALIGN_LEFT | CBRS_TOOLTIPS, ID_ZOOM_H) &&
		m_tbZoomH.LoadToolBar(IDR_ZOOM_H)) {
		CRect rcClient;
		GetClientRect(rcClient);

		CSize sizeToolBar;
		sizeToolBar = m_tbZoomH.CalcFixedLayout(FALSE, FALSE);
		m_tbZoomH.MoveWindow(0, rcClient.bottom - sizeToolBar.cy + 6, sizeToolBar.cx, sizeToolBar.cy);
	}

	m_nData = (int)(m_pAcfCondition->fMaxDelayTime * m_fRate);

	m_pAcfData = new double[m_nData];
	m_pAcfDataLog = new double[m_nData];

	m_fTotalTime = (double)m_nData / m_fRate;

	CalcGraphWindow();
	DispGraphWindow();

	return TRUE;
}

void CGraphACF::OnZoomFullH()
{
	m_fDispTime = m_fTotalTime;
	DispGraphWindow();
}

void CGraphACF::OnZoomInH()
{
	m_fDispTime /= 2;
	DispGraphWindow();
}

void CGraphACF::OnZoomOutH()
{
	m_fDispTime *= 2;
	DispGraphWindow();
}

void CGraphACF::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* /*pScrollBar*/)
{
	switch (nSBCode) {
	case SB_LEFT:
		m_fStartTime = 0;
		break;
	case SB_LINELEFT:
		m_fStartTime -= m_fDispTime / 32;
		break;
	case SB_LINERIGHT:
		m_fStartTime += m_fDispTime / 32;
		break;
	case SB_PAGELEFT:
		m_fStartTime -= m_fDispTime;
		break;
	case SB_PAGERIGHT:
		m_fStartTime += m_fDispTime;
		break;
	case SB_RIGHT:
		m_fStartTime = m_fTotalTime - m_fDispTime;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		m_fStartTime = m_fTotalTime * nPos / SCROLL_RANGE;
		break;
	}

	DispGraphWindow();
}

void CGraphACF::OnDestroy()
{
	CDialogExt::OnDestroy();

	if (m_pAcfData != NULL) {
		delete [] m_pAcfData;
		m_pAcfData = NULL;
	}

	if (m_pAcfDataLog != NULL) {
		delete [] m_pAcfDataLog;
		m_pAcfDataLog = NULL;
	}
}

void CGraphACF::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogExt::OnShowWindow(bShow, nStatus);

	if (bShow && m_bReDraw) {
		CalcGraphWindow();
		DispGraphWindow();
		m_bReDraw = FALSE;
	}
}

void CGraphACF::CalcGraphWindow()
{
	CWaitCursor waitCursor;
	int i;

	// 積分データ数取得
	const int nIntegrationSize1 = (int)(m_pAcfCondition->fIntegrationTime * m_fRate);
	const int nMaxDelaySize = (int)(m_pAcfCondition->fMaxDelayTime * m_fRate);
	const int nIntegrationSize2 = nIntegrationSize1 + nMaxDelaySize;

	// Wave データ取得
	double *pWaveBuf = new double[nIntegrationSize2];
	double *pWaveBufTemp = new double[nIntegrationSize2];
	ReadWaveData(m_hWaveData, pWaveBuf, pWaveBufTemp, nIntegrationSize2, (int)(m_nStep * m_pAcfCondition->fRunningStep * m_fRate));
	delete [] pWaveBufTemp;

	// FFT バッファサイズ取得
	int nFftSize;
	for (nFftSize = 1; nFftSize < nIntegrationSize2; nFftSize *= 2)
		;

	// フィルタ作成
	double *pFilterTbl = new double[nFftSize];
	if (m_pNmsMicCal != NULL) {
		double *pCalibration = new double[nFftSize];
		MakeFilterTbl3(pCalibration, nFftSize, m_fRate, (FilterData *)m_pNmsMicCal->freq, m_pNmsMicCal->nFreqData, 20);
		pCalibration[0] = 1;
		const double sens = pow(10.0, -m_pNmsMicCal->fInputSens / 20);
		MakeFilterTbl2(pFilterTbl, nFftSize, m_fRate, m_pAcfCondition->nFreqWeighting, 20);
		for (i = 0; i < nFftSize; i++)
			pFilterTbl[i] *= sens / pCalibration[i];
		delete [] pCalibration;
	} else
		MakeFilterTbl2(pFilterTbl, nFftSize, m_fRate, m_pAcfCondition->nFreqWeighting, 20);

	// ACF計算
	double *pAcfData = CalcAcf1(pWaveBuf, nIntegrationSize1, nIntegrationSize2, nFftSize, pFilterTbl);
	memcpy(m_pAcfData, pAcfData, m_nData * sizeof(double));

	delete [] pAcfData;
	delete [] pFilterTbl;
	delete [] pWaveBuf;

	const double fPhi0 = m_pAcfData[0];
	for (i = 0; i < m_nData; i++)
		m_pAcfData[i] /= fPhi0;

	for (i = 0; i < m_nData; i++)
		m_pAcfDataLog[i] = dB10(fabs(m_pAcfData[i]));
}

void CGraphACF::DispGraphWindow()
{
	if (m_fDispTime == 0)
		m_fDispTime = m_fTotalTime;
	else if (m_fDispTime < m_fTotalTime / MAX_ZOOM_TIME)
		m_fDispTime = m_fTotalTime / MAX_ZOOM_TIME;
	else if (m_fDispTime > m_fTotalTime)
		m_fDispTime = m_fTotalTime;

	if (m_fStartTime < 0)
		m_fStartTime = 0;
	else if (m_fStartTime > m_fTotalTime - m_fDispTime)
		m_fStartTime = m_fTotalTime - m_fDispTime;

	const int nScrollSize = (int)(m_fDispTime / m_fTotalTime * SCROLL_RANGE);
	const int nScrollPos = (int)(m_fStartTime / m_fTotalTime * SCROLL_RANGE);

	if (nScrollSize != m_nScrollSize || nScrollPos != m_nScrollPos) {
		m_nScrollSize = nScrollSize;
		m_nScrollPos = nScrollPos;

		SCROLLINFO si{
			.cbSize = sizeof(SCROLLINFO),
			.fMask = SIF_ALL,
			.nMin = 0,
			.nMax = SCROLL_RANGE,
			.nPage = (UINT)nScrollSize,
			.nPos = nScrollPos
		};
		m_cScrollH.SetScrollInfo(&si, TRUE);
	}

	CToolBarCtrl &tc = m_tbZoomH.GetToolBarCtrl();
	if (m_fDispTime == m_fTotalTime / MAX_ZOOM_TIME) {
		tc.EnableButton(IDM_ZOOM_FULL_H, TRUE);
		tc.EnableButton(IDM_ZOOM_IN_H, FALSE);
		tc.EnableButton(IDM_ZOOM_OUT_H, TRUE);
	} else if (m_fDispTime == m_fTotalTime) {
		tc.EnableButton(IDM_ZOOM_FULL_H, FALSE);
		tc.EnableButton(IDM_ZOOM_IN_H, TRUE);
		tc.EnableButton(IDM_ZOOM_OUT_H, FALSE);
	} else {
		tc.EnableButton(IDM_ZOOM_FULL_H, TRUE);
		tc.EnableButton(IDM_ZOOM_IN_H, TRUE);
		tc.EnableButton(IDM_ZOOM_OUT_H, TRUE);
	}

	BOOL bScroll = (m_fDispTime < m_fTotalTime);
	if (m_bLog) {
		m_cGraphAcf.DispGraph2(m_fTotalTime, m_fStartTime, m_fDispTime, m_pAcfDataLog, m_nData,
				m_pAcfFactor[m_nStep].fTauE, m_pAcfFactor[m_nStep].fTauE0, 0, -15, bScroll);
	} else {
		m_cGraphAcf.DispGraph1(m_fTotalTime, m_fStartTime, m_fDispTime, m_pAcfData, m_nData,
				m_pAcfFactor[m_nStep].fTau1, m_pAcfFactor[m_nStep].fPhi1, bScroll);
	}

	m_cZoomH = int(m_fTotalTime / m_fDispTime);
}

void CGraphACF::ReDraw(int nStep)
{
	if (nStep != -1)
		m_nStep = nStep;

	if (m_hWnd != NULL) {
		if (IsWindowVisible()) {
			CalcGraphWindow();
			DispGraphWindow();
		} else
			m_bReDraw = TRUE;
	}
}

void CGraphACF::OnLog()
{
	UpdateData(TRUE);

	DispGraphWindow();
}

void CALLBACK CGraphACF::GraphCallBack(WPARAM code, LPARAM data, double param1, double /*param2*/)
{
	CGraphACF *pWnd = (CGraphACF *)data;

	if (code == CC_SCROLL) {
		pWnd->m_fStartTime -= param1 * pWnd->m_fDispTime;
		pWnd->DispGraphWindow();
	}
}

BOOL CGraphACF::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static constexpr UINT aIDs[] = {
		IDC_GRAPH_ACF, IDH_GRAPH_ACF,
		IDC_ZOOM_H, IDH_GRAPH_ACF_ZOOM_H,
		IDC_SCROLL_H, IDH_GRAPH_ACF_SCROLL_H,
		IDC_LOG, IDH_GRAPH_ACF_LOG,
		ID_ZOOM_H_FULL, IDH_GRAPH_ACF_H_FULL,
		ID_ZOOM_H_IN, IDH_GRAPH_ACF_H_IN,
		ID_ZOOM_H_OUT, IDH_GRAPH_ACF_H_OUT,
		0
	};

	::DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}
