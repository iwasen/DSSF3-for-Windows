// GraphFactor.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "nms.h"
#include "GraphFactor.h"
#include "Help\ContextHelp.h"

#define SCROLL_RANGE	16384
#define MAX_ZOOM_TIME	512
#define MAX_ZOOM_VALUE	512
#define ZOOM_LOWER		0
#define ZOOM_CENTER		1
#define ZOOM_UPPER		2

/////////////////////////////////////////////////////////////////////////////
// CGraphFactor ダイアログ

CGraphFactor::CGraphFactor(CWnd* pParent /*=NULL*/)
	: CDialogExt(CGraphFactor::IDD, pParent)
{
	m_pAcfFactor = NULL;
	m_pIAcfFactor = NULL;
	m_nSel = -1;
	m_bReDraw = FALSE;
	m_nHScrollSize = -1;
	m_nHScrollPos = -1;
	m_nVScrollSize = -1;
	m_nVScrollPos = -1;
	m_pfData = NULL;
	m_fStartTime = 0;
	m_fDispTime = 0;
	m_fStartValue = 0;
	m_fDispValue = 0;
}

void CGraphFactor::DoDataExchange(CDataExchange* pDX)
{
	CDialogExt::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRAPH_FACTOR, m_cGraphFactor);
	DDX_Control(pDX, IDC_SCROLL_H, m_cScrollH);
	DDX_Control(pDX, IDC_SCROLL_V, m_cScrollV);
	DDX_Control(pDX, IDC_ZOOM_H, m_cZoomH);
	DDX_Control(pDX, IDC_ZOOM_V, m_cZoomV);
}

BEGIN_MESSAGE_MAP(CGraphFactor, CDialogExt)
	ON_WM_HSCROLL()
	ON_WM_SHOWWINDOW()
	ON_COMMAND(IDM_ZOOM_FULL_H, OnZoomFullH)
	ON_COMMAND(IDM_ZOOM_IN_H, OnZoomInH)
	ON_COMMAND(IDM_ZOOM_OUT_H, OnZoomOutH)
	ON_COMMAND(IDM_ZOOM_FULL_V, OnZoomFullV)
	ON_COMMAND(IDM_ZOOM_IN_V, OnZoomInV)
	ON_COMMAND(IDM_ZOOM_OUT_V, OnZoomOutV)
	ON_WM_VSCROLL()
	ON_WM_DESTROY()
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphFactor メッセージ ハンドラ

BOOL CGraphFactor::OnInitDialog()
{
	CDialogExt::OnInitDialog();

	m_cGraphFactor.Initialize(90, NULL, GraphCallBack, (LPARAM)this);

	if (m_tbZoomH.Create(this, WS_CHILD | WS_VISIBLE | CBRS_SIZE_FIXED | CBRS_ALIGN_LEFT | CBRS_TOOLTIPS, ID_ZOOM_H) &&
		m_tbZoomH.LoadToolBar(IDR_ZOOM_H)) {
		CRect rcClient;
		GetClientRect(rcClient);

		CSize sizeToolBar;
		sizeToolBar = m_tbZoomH.CalcFixedLayout(FALSE, FALSE);
		m_tbZoomH.MoveWindow(0, rcClient.bottom - sizeToolBar.cy + 6, sizeToolBar.cx, sizeToolBar.cy);
	}

	if (m_tbZoomV.Create(this, WS_CHILD | WS_VISIBLE | CBRS_SIZE_DYNAMIC | CBRS_ALIGN_RIGHT | CBRS_TOOLTIPS, ID_ZOOM_V) &&
		m_tbZoomV.LoadToolBar(IDR_ZOOM_V)) {
		CRect rcClient;
		GetClientRect(rcClient);

		CSize sizeToolBar;
		sizeToolBar = m_tbZoomV.CalcFixedLayout(FALSE, FALSE);
		m_tbZoomV.MoveWindow(rcClient.right - sizeToolBar.cx + 3, 0, sizeToolBar.cx, sizeToolBar.cy);
	}

	m_pfData = new double[m_nData];

	CalcData();

	m_fTotalTime = m_fStepTime * m_nData;

	DispGraphWindow();

	return TRUE;
}

void CGraphFactor::OnZoomFullH()
{
	m_fDispTime = m_fTotalTime;
	DispGraphWindow();
}

void CGraphFactor::OnZoomInH()
{
	m_fDispTime /= 2;
	DispGraphWindow();
}

void CGraphFactor::OnZoomOutH()
{
	m_fDispTime *= 2;
	DispGraphWindow();
}

void CGraphFactor::OnZoomFullV()
{
	m_fDispValue = m_fTotalValue;
	DispGraphWindow();
}

void CGraphFactor::OnZoomInV()
{
	m_fDispValue /= 2;
	switch (m_nZoomMode) {
	case ZOOM_CENTER:
		m_fStartValue += m_fDispValue / 2;
		break;
	case ZOOM_UPPER:
		m_fStartValue += m_fDispValue;
		break;
	}

	DispGraphWindow();
}

void CGraphFactor::OnZoomOutV()
{
	switch (m_nZoomMode) {
	case ZOOM_CENTER:
		m_fStartValue -= m_fDispValue / 2;
		break;
	case ZOOM_UPPER:
		m_fStartValue -= m_fDispValue;
		break;
	}
	m_fDispValue *= 2;

	DispGraphWindow();
}

void CGraphFactor::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* /*pScrollBar*/)
{
	switch (nSBCode) {
	case SB_LEFT:
		m_fStartTime = 0;
		break;
	case SB_RIGHT:
		m_fStartTime = m_fTotalTime - m_fDispTime;
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
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		m_fStartTime = m_fTotalTime * nPos / SCROLL_RANGE;
		break;
	}

	DispGraphWindow();
}

void CGraphFactor::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* /*pScrollBar*/)
{
	switch (nSBCode) {
	case SB_LEFT:
		m_fStartValue = m_fMinValue + m_fTotalValue - m_fDispValue;
		break;
	case SB_RIGHT:
		m_fStartValue = m_fMinValue;
		break;
	case SB_LINELEFT:
		m_fStartValue += m_fDispValue / 32;
		break;
	case SB_LINERIGHT:
		m_fStartValue -= m_fDispValue / 32;
		break;
	case SB_PAGELEFT:
		m_fStartValue += m_fDispValue;
		break;
	case SB_PAGERIGHT:
		m_fStartValue -= m_fDispValue;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		m_fStartValue = m_fMinValue + m_fTotalValue * int(SCROLL_RANGE - nPos - m_nVScrollSize) / SCROLL_RANGE;
		break;
	}

	DispGraphWindow();
}

void CGraphFactor::CalcData()
{
	int i;
	double fMaxValue = 0, fMinValue = 0;

	switch (m_nFactor) {
	case FACTOR_PHI0:
		for (i = 0; i < m_nData; i++) {
			m_pfData[i] = m_pAcfFactor[i].fPhi0 + m_fAdjustPhi0;
			if (!_isnan(m_pfData[i]) && _finite(m_pfData[i])) {
				if (i == 0 || fMaxValue < m_pfData[i])
					fMaxValue = m_pfData[i];
				if (i == 0 || fMinValue > m_pfData[i])
					fMinValue = m_pfData[i];
			}
		}
		if (fMaxValue == 0 && fMinValue == 0)
			fMaxValue = 1;
		else if (fMaxValue >= 0 && fMinValue >= 0) {
			fMinValue = 0;
			fMaxValue = GetCeilValue(fMaxValue);
		} else if (fMaxValue <= 0 && fMinValue <= 0){
			fMaxValue = 0;
			fMinValue = GetCeilValue(fMinValue);
		} else {
			fMaxValue = GetCeilValue(fMaxValue);
			fMinValue = -fMaxValue;
		}
		m_sAxis.LoadString(IDS_PHI0);
		m_sAxis += " [dB]";
		m_nZoomMode = ZOOM_CENTER;
		break;
	case FACTOR_TAUE:
		for (i = 0; i < m_nData; i++) {
			m_pfData[i] = m_pAcfFactor[i].fTauE * 1000;
			if (!_isnan(m_pfData[i]) && _finite(m_pfData[i])) {
				if (i == 0 || fMaxValue < m_pfData[i])
					fMaxValue = m_pfData[i];
			}
		}
		fMaxValue = GetCeilValue(fMaxValue);
		fMinValue = 0;
		m_sAxis.LoadString(IDS_TAUE);
		m_sAxis += " [ms]";
		m_nZoomMode = ZOOM_LOWER;
		break;
	case FACTOR_TAU1:
		for (i = 0; i < m_nData; i++) {
			m_pfData[i] = m_pAcfFactor[i].fTau1 * 1000;
			if (!_isnan(m_pfData[i]) && _finite(m_pfData[i])) {
				if (i == 0 || fMaxValue < m_pfData[i])
					fMaxValue = m_pfData[i];
			}
		}
		fMaxValue = GetCeilValue(fMaxValue);
		fMinValue = 0;
		m_sAxis.LoadString(IDS_TAU1);
		m_sAxis += " [ms]";
		m_nZoomMode = ZOOM_LOWER;
		break;
	case FACTOR_PHI1:
		for (i = 0; i < m_nData; i++)
			m_pfData[i] = m_pAcfFactor[i].fPhi1;
		fMaxValue = 1;
		fMinValue = 0;
		m_sAxis.LoadString(IDS_PHI1);
		m_nZoomMode = ZOOM_LOWER;
		break;
	case FACTOR_SPL:
		for (i = 0; i < m_nData; i++) {
			m_pfData[i] = m_pIAcfFactor[i].fSPL + m_fAdjustPhi0;
			if (!_isnan(m_pfData[i]) && _finite(m_pfData[i])) {
				if (i == 0 || fMaxValue < m_pfData[i])
					fMaxValue = m_pfData[i];
				if (i == 0 || fMinValue > m_pfData[i])
					fMinValue = m_pfData[i];
			}
		}
		if (fMaxValue == 0 && fMinValue == 0)
			fMaxValue = 1;
		else if (fMaxValue >= 0 && fMinValue >= 0) {
			fMinValue = 0;
			fMaxValue = GetCeilValue(fMaxValue);
		} else if (fMaxValue <= 0 && fMinValue <= 0){
			fMaxValue = 0;
			fMinValue = GetCeilValue(fMinValue);
		} else {
			fMaxValue = GetCeilValue(fMaxValue);
			fMinValue = -fMaxValue;
		}
		m_sAxis = "SPL";
		m_sAxis += " [dB]";
		m_nZoomMode = ZOOM_CENTER;
		break;
	case FACTOR_IACC:
		for (i = 0; i < m_nData; i++)
			m_pfData[i] = m_pIAcfFactor[i].fIACC;
		fMaxValue = 1;
		fMinValue = 0;
		m_sAxis = "IACC";
		m_nZoomMode = ZOOM_UPPER;
		break;
	case FACTOR_TIACC:
		for (i = 0; i < m_nData; i++) {
			m_pfData[i] = m_pIAcfFactor[i].fTauIACC * 1000;
			if (!_isnan(m_pfData[i]) && _finite(m_pfData[i])) {
				if (fMaxValue < m_pfData[i])
					fMaxValue = m_pfData[i];
				if (fMinValue > m_pfData[i])
					fMinValue = m_pfData[i];
			}
		}
		fMaxValue = GetCeilValue(fMaxValue);
		fMinValue = GetCeilValue(fMinValue);
		m_sAxis.LoadString(IDS_TIACC);
		m_sAxis += " [ms]";
		m_nZoomMode = ZOOM_CENTER;
		break;
	case FACTOR_WIACC:
		for (i = 0; i < m_nData; i++) {
			m_pfData[i] = m_pIAcfFactor[i].fWIACC * 1000;
			if (!_isnan(m_pfData[i]) && _finite(m_pfData[i])) {
				if (i == 0 || fMaxValue < m_pfData[i])
					fMaxValue = m_pfData[i];
			}
		}
		fMaxValue = GetCeilValue(fMaxValue);
		fMinValue = 0;
		m_sAxis = "W_IACC";
		m_sAxis += " [ms]";
		m_nZoomMode = ZOOM_LOWER;
		break;
	}

	if (fMaxValue <= fMinValue)
		fMaxValue = fMinValue + 1;

	m_fMinValue = fMinValue;
	m_fTotalValue = fMaxValue - fMinValue;
}

void CGraphFactor::DispGraphWindow()
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

	if (m_fDispValue == 0)
		m_fDispValue = m_fTotalValue;
	else if (m_fDispValue < m_fTotalValue / MAX_ZOOM_VALUE)
		m_fDispValue = m_fTotalValue / MAX_ZOOM_VALUE;
	else if (m_fDispValue > m_fTotalValue)
		m_fDispValue = m_fTotalValue;

	if (m_fStartValue < m_fMinValue)
		m_fStartValue = m_fMinValue;
	else if (m_fStartValue > m_fMinValue + m_fTotalValue - m_fDispValue)
		m_fStartValue = m_fMinValue + m_fTotalValue - m_fDispValue;

	SetHScroll();
	SetVScroll();

	m_cGraphFactor.DispGraph(m_fStepTime, m_fStartTime, m_fDispTime, m_pfData, m_nData, m_fStartValue + m_fDispValue, m_fStartValue, m_sAxis, m_nSel
			, m_fDispTime < m_fTotalTime, m_fDispValue < m_fTotalValue, NULL, 0, 0, FALSE);

	m_cZoomH = int(m_fTotalTime / m_fDispTime);
	m_cZoomV = int(m_fTotalValue / m_fDispValue);
}

void CGraphFactor::SetHScroll()
{
	const int nScrollSize = (int)(m_fDispTime / m_fTotalTime * SCROLL_RANGE);
	const int nScrollPos = (int)(m_fStartTime / m_fTotalTime * SCROLL_RANGE);

	if (nScrollSize != m_nHScrollSize || nScrollPos != m_nHScrollPos) {
		m_nHScrollSize = nScrollSize;
		m_nHScrollPos = nScrollPos;

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
}

void CGraphFactor::SetVScroll()
{
	const int nScrollSize = (int)(m_fDispValue / m_fTotalValue * SCROLL_RANGE);
	const int nScrollPos = (int)((m_fStartValue - m_fMinValue) / m_fTotalValue * SCROLL_RANGE);

	if (nScrollSize != m_nVScrollSize || nScrollPos != m_nVScrollPos) {
		m_nVScrollSize = nScrollSize;
		m_nVScrollPos = nScrollPos;

		SCROLLINFO si{
			.cbSize = sizeof(SCROLLINFO),
			.fMask = SIF_ALL,
			.nMin = 0,
			.nMax = SCROLL_RANGE,
			.nPage = (UINT)nScrollSize,
			.nPos = SCROLL_RANGE - nScrollPos - m_nVScrollSize
		};
		m_cScrollV.SetScrollInfo(&si, TRUE);
	}

	CToolBarCtrl &tc = m_tbZoomV.GetToolBarCtrl();
	if (m_fDispValue == m_fTotalValue / MAX_ZOOM_VALUE) {
		tc.EnableButton(IDM_ZOOM_FULL_V, TRUE);
		tc.EnableButton(IDM_ZOOM_IN_V, FALSE);
		tc.EnableButton(IDM_ZOOM_OUT_V, TRUE);
	} else if (m_fDispValue == m_fTotalValue) {
		tc.EnableButton(IDM_ZOOM_FULL_V, FALSE);
		tc.EnableButton(IDM_ZOOM_IN_V, TRUE);
		tc.EnableButton(IDM_ZOOM_OUT_V, FALSE);
	} else {
		tc.EnableButton(IDM_ZOOM_FULL_V, TRUE);
		tc.EnableButton(IDM_ZOOM_IN_V, TRUE);
		tc.EnableButton(IDM_ZOOM_OUT_V, TRUE);
	}
}

void CGraphFactor::OnOK()
{
}

void CGraphFactor::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogExt::OnShowWindow(bShow, nStatus);

	if (bShow && m_bReDraw) {
		DispGraphWindow();
		m_bReDraw = FALSE;
	}
}

void CGraphFactor::ReDraw(int nSel)
{
	m_nSel = nSel;

	if (m_hWnd != NULL) {
		if (IsWindowVisible()) {
			DispGraphWindow();
		} else
			m_bReDraw = TRUE;
	}
}

void CALLBACK CGraphFactor::GraphCallBack(WPARAM code, LPARAM data, double param1, double param2)
{
	CGraphFactor *pWnd = (CGraphFactor *)data;

	switch (code) {
	case CC_SCROLL:
		pWnd->m_fStartTime -= param1 * pWnd->m_fDispTime;
		pWnd->m_fStartValue += param2 * pWnd->m_fDispValue;
		pWnd->DispGraphWindow();
		break;
	case CC_DBLCLK:
		const double fTime = (double)(pWnd->m_fStartTime + param1 * pWnd->m_fDispTime);
		pWnd->GetParent()->SendMessage(WM_SELECT_DATA, (WPARAM)&fTime, 0);
		break;
	}
}

void CGraphFactor::OnDestroy()
{
	CDialogExt::OnDestroy();

	if (m_pfData != NULL) {
		delete [] m_pfData;
		m_pfData = NULL;
	}
}

BOOL CGraphFactor::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static constexpr UINT aIDs[] = {
		IDC_GRAPH_FACTOR, IDH_GRAPH_FACTOR,
		IDC_ZOOM_V, IDH_GRAPH_FACTOR_ZOOM_V,
		IDC_SCROLL_V, IDH_GRAPH_FACTOR_SCROLL_V,
		IDC_ZOOM_H, IDH_GRAPH_FACTOR_ZOOM_H,
		IDC_SCROLL_H, IDH_GRAPH_FACTOR_SCROLL_H,
		ID_ZOOM_H_FULL, IDH_GRAPH_FACTOR_H_FULL,
		ID_ZOOM_H_IN, IDH_GRAPH_FACTOR_H_IN,
		ID_ZOOM_H_OUT, IDH_GRAPH_FACTOR_H_OUT,
		ID_ZOOM_V_FULL, IDH_GRAPH_FACTOR_ZOOM_V_FULL,
		ID_ZOOM_V_IN, IDH_GRAPH_FACTOR_ZOOM_V_IN,
		ID_ZOOM_V_OUT, IDH_GRAPH_FACTOR_ZOOM_V_OUT,
		0
	};

	::DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}
