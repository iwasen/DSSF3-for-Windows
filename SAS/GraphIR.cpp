// GraphIR.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Sas.h"
#include "GraphIR.h"
#include "ImpFactor.h"
#include "Calc.h"
#include "Help\ContextHelp.h"

#define COLOR_LEFT			RGB(0, 128, 255)
#define COLOR_RIGHT			RGB(255, 64, 192)

#define SCROLL_RANGE 16384
#define MAX_ZOOM_TIME	512

/////////////////////////////////////////////////////////////////////////////
// CGraphIR ダイアログ

CGraphIR::CGraphIR(CWnd* pParent /*=NULL*/)
	: CDialogExt(CGraphIR::IDD, pParent)
{
	m_bReDraw = FALSE;
	m_nScrollSize = -1;
	m_nScrollPos = -1;
	m_fStartTime = 0;
	m_fDispTime = 0;
}

void CGraphIR::DoDataExchange(CDataExchange* pDX)
{
	CDialogExt::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ZOOM_H, m_cZoomH);
	DDX_Control(pDX, IDC_RIGHT_T1, m_cRightT1);
	DDX_Control(pDX, IDC_RIGHT_T0, m_cRightT0);
	DDX_Control(pDX, IDC_LEFT_T1, m_cLeftT1);
	DDX_Control(pDX, IDC_LEFT_T0, m_cLeftT0);
	DDX_Control(pDX, IDC_CHANGE, m_cChange);
	DDX_Control(pDX, IDC_SCROLL_H, m_cScrollH);
	DDX_Control(pDX, IDC_IR_RIGHT, m_cGraphRight);
	DDX_Control(pDX, IDC_IR_LEFT, m_cGraphLeft);
}

BEGIN_MESSAGE_MAP(CGraphIR, CDialogExt)
	ON_WM_HSCROLL()
	ON_COMMAND(IDM_ZOOM_FULL_H, OnZoomFullH)
	ON_COMMAND(IDM_ZOOM_IN_H, OnZoomInH)
	ON_COMMAND(IDM_ZOOM_OUT_H, OnZoomOutH)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_CHANGE, OnChange)
	ON_EN_CHANGE(IDC_LEFT_T0, OnChangeLeftT0)
	ON_EN_CHANGE(IDC_LEFT_T1, OnChangeLeftT1)
	ON_EN_CHANGE(IDC_RIGHT_T0, OnChangeRightT0)
	ON_EN_CHANGE(IDC_RIGHT_T1, OnChangeRightT1)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphIR メッセージ ハンドラ

BOOL CGraphIR::OnInitDialog()
{
	CWaitCursor waitCursor;

	CDialogExt::OnInitDialog();

	if (m_tbZoomH.Create(this, WS_CHILD | WS_VISIBLE | CBRS_SIZE_FIXED | CBRS_ALIGN_LEFT | CBRS_TOOLTIPS, ID_ZOOM_H) &&
		m_tbZoomH.LoadToolBar(IDR_ZOOM_H)) {
		WINDOWPLACEMENT wp;
		m_cZoomH.GetWindowPlacement(&wp);

		CSize sizeToolBar;
		sizeToolBar = m_tbZoomH.CalcFixedLayout(FALSE, FALSE);
		m_tbZoomH.MoveWindow(0, wp.rcNormalPosition.top - 8, sizeToolBar.cx, sizeToolBar.cy);
	}

	CString str;
	if (m_pDbImpulseRec->nChannel == 1) {
		str.LoadString(IDS_IR);
		m_cGraphLeft.Initialize(90, str, m_pDbAcParamRec != NULL ? 3 : 1, COLOR_LEFT, LeftGraphCallBack, (LPARAM)this);
	} else {
		str.LoadString(IDS_LEFTIR);
		m_cGraphLeft.Initialize(90, str, m_pDbAcParamRec != NULL ? 3 : 1, COLOR_LEFT, LeftGraphCallBack, (LPARAM)this);
		str.LoadString(IDS_RIGHTIR);
		m_cGraphRight.Initialize(90, str, m_pDbAcParamRec != NULL ? 3 : 1, COLOR_RIGHT, RightGraphCallBack, (LPARAM)this);
	}

	m_fTotalTime = (double)m_nData / m_fRate;

	m_cGraphLeft.EnableWindow(TRUE);
	if (m_pDbImpulseRec->nChannel == 2)
		m_cGraphRight.EnableWindow(TRUE);

	if (m_pDbAcParamRec == NULL) {
		m_cLeftT0.EnableWindow(FALSE);
		m_cLeftT1.EnableWindow(FALSE);
		m_cRightT0.EnableWindow(FALSE);
		m_cRightT1.EnableWindow(FALSE);
		m_cChange.EnableWindow(FALSE);
	} else {
		m_cGraphLeft.SetDeltaT1(m_pDbAcParamRec->dbAcParamResult.fDeltaT0L, m_pDbAcParamRec->dbAcParamResult.fDeltaT1L);
		m_cLeftT0 = m_pDbAcParamRec->dbAcParamResult.fDeltaT0L;
		m_cLeftT1 = m_pDbAcParamRec->dbAcParamResult.fDeltaT1L;
		if (m_pDbImpulseRec->nChannel == 1) {
			m_cRightT0.EnableWindow(FALSE);
			m_cRightT1.EnableWindow(FALSE);
		} else {
			m_cGraphRight.SetDeltaT1(m_pDbAcParamRec->dbAcParamResult.fDeltaT0R, m_pDbAcParamRec->dbAcParamResult.fDeltaT1R);
			m_cRightT0 = m_pDbAcParamRec->dbAcParamResult.fDeltaT0R;
			m_cRightT1 = m_pDbAcParamRec->dbAcParamResult.fDeltaT1R;
		}
	}

	DispGraphWindow();

	m_cChange.EnableWindow(FALSE);

	return TRUE;
}

void CGraphIR::DispGraphWindow()
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

	int nScrollSize = (int)(m_fDispTime / m_fTotalTime * SCROLL_RANGE);
	int nScrollPos = (int)(m_fStartTime / m_fTotalTime * SCROLL_RANGE);

	if (nScrollSize != m_nScrollSize || nScrollPos != m_nScrollPos) {
		m_nScrollSize = nScrollSize;
		m_nScrollPos = nScrollPos;

		SCROLLINFO si;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_ALL;
		si.nMin = 0;
		si.nMax = SCROLL_RANGE;
		si.nPage = nScrollSize;
		si.nPos = nScrollPos;
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
	m_cGraphLeft.DispImpulse(m_fTotalTime, m_fStartTime, m_fDispTime, m_pIRLeft, m_nData, bScroll);
	m_cGraphRight.DispImpulse(m_fTotalTime, m_fStartTime, m_fDispTime, m_pIRRight, m_nData, bScroll);

	m_cZoomH = int(m_fTotalTime / m_fDispTime);
}

void CGraphIR::OnZoomFullH()
{
	m_fDispTime = m_fTotalTime;
	DispGraphWindow();
}

void CGraphIR::OnZoomInH()
{
	m_fDispTime /= 2;
	DispGraphWindow();
}

void CGraphIR::OnZoomOutH()
{
	m_fDispTime *= 2;
	DispGraphWindow();
}

void CGraphIR::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* /*pScrollBar*/)
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

void CGraphIR::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogExt::OnShowWindow(bShow, nStatus);

	if (bShow && m_bReDraw) {
		DispGraphWindow();
		m_bReDraw = FALSE;
	}
}

void CGraphIR::ReDraw()
{
	if (m_hWnd != NULL) {
		if (IsWindowVisible())
			DispGraphWindow();
		else
			m_bReDraw = TRUE;
	}
}

void CGraphIR::OnChange()
{
	CWaitCursor waitCursor;

	m_pDbAcParamRec->dbAcParamResult.fDeltaT0L = m_cLeftT0;
	m_pDbAcParamRec->dbAcParamResult.fDeltaT1L = m_cLeftT1;
	m_pDbAcParamRec->dbAcParamResult.fDeltaT0R = m_cRightT0;
	m_pDbAcParamRec->dbAcParamResult.fDeltaT1R = m_cRightT1;
	CalcTsub(m_pDbImpulseRec, m_hWaveData, m_pDbAcParamRec, m_fRate);
	m_cGraphLeft.SetDeltaT1(m_pDbAcParamRec->dbAcParamResult.fDeltaT0L, m_pDbAcParamRec->dbAcParamResult.fDeltaT1L);
	m_cGraphRight.SetDeltaT1(m_pDbAcParamRec->dbAcParamResult.fDeltaT0R, m_pDbAcParamRec->dbAcParamResult.fDeltaT1R);
	m_pParent->ChangeData();
	m_cChange.EnableWindow(FALSE);
}

void CALLBACK CGraphIR::LeftGraphCallBack(WPARAM code, LPARAM data, double param1, double param2)
{
	CGraphIR *pWnd = (CGraphIR *)data;

	switch (code) {
	case CC_SCROLL:
		pWnd->m_fStartTime -= param1 * pWnd->m_fDispTime;
		pWnd->DispGraphWindow();
		break;
	case CC_DT1_START:
	case CC_DT1_END:
	case CC_DT1_MOVE:
		pWnd->m_cLeftT0.Format("%.2lf", param1 * 1000);
		pWnd->m_cLeftT0.UpdateWindow();
		pWnd->m_cLeftT1.Format("%.2lf", param2 * 1000);
		pWnd->m_cLeftT1.UpdateWindow();
		pWnd->m_cChange.EnableWindow(TRUE);
		break;
	}
}

void CALLBACK CGraphIR::RightGraphCallBack(WPARAM code, LPARAM data, double param1, double param2)
{
	CGraphIR *pWnd = (CGraphIR *)data;

	switch (code) {
	case CC_SCROLL:
		pWnd->m_fStartTime -= param1 * pWnd->m_fDispTime;
		pWnd->DispGraphWindow();
		break;
	case CC_DT1_START:
	case CC_DT1_END:
	case CC_DT1_MOVE:
		pWnd->m_cRightT0.Format("%.2lf", param1 * 1000);
		pWnd->m_cRightT0.UpdateWindow();
		pWnd->m_cRightT1.Format("%.2lf", param2 * 1000);
		pWnd->m_cRightT1.UpdateWindow();
		pWnd->m_cChange.EnableWindow(TRUE);
		break;
	}
}

void CGraphIR::OnChangeLeftT0()
{
	m_cGraphLeft.SetDeltaT1((double)m_cLeftT0, (double)m_cLeftT1);
	m_cGraphLeft.Invalidate(FALSE);
	m_cChange.EnableWindow(TRUE);
}

void CGraphIR::OnChangeLeftT1()
{
	m_cGraphLeft.SetDeltaT1((double)m_cLeftT0, (double)m_cLeftT1);
	m_cGraphLeft.Invalidate(FALSE);
	m_cChange.EnableWindow(TRUE);
}

void CGraphIR::OnChangeRightT0()
{
	m_cGraphRight.SetDeltaT1((double)m_cRightT0, (double)m_cRightT1);
	m_cGraphRight.Invalidate(FALSE);
	m_cChange.EnableWindow(TRUE);
}

void CGraphIR::OnChangeRightT1()
{
	m_cGraphRight.SetDeltaT1((double)m_cRightT0, (double)m_cRightT1);
	m_cGraphRight.Invalidate(FALSE);
	m_cChange.EnableWindow(TRUE);
}

BOOL CGraphIR::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static const UINT aIDs[] = {
		IDC_IR_LEFT, IDH_GRAPH_IR_LEFT,
		IDC_ZOOM_H, IDH_GRAPH_IR_ZOOM_H,
		IDC_SCROLL_H, IDH_GRAPH_IR_SCROLL_H,
		IDC_IR_RIGHT, IDH_GRAPH_IR_RIGHT,
		IDC_LEFT_T0, IDH_GRAPH_IR_LEFT_T0,
		IDC_RIGHT_T0, IDH_GRAPH_IR_RIGHT_T0,
		IDC_LEFT_T1, IDH_GRAPH_IR_LEFT_T1,
		IDC_RIGHT_T1, IDH_GRAPH_IR_RIGHT_T1,
		IDC_CHANGE, IDH_GRAPH_IR_CHANGE,
		ID_ZOOM_H_FULL, IDH_GRAPH_IR_H_FULL,
		ID_ZOOM_H_IN, IDH_GRAPH_IR_H_IN,
		ID_ZOOM_H_OUT, IDH_GRAPH_IR_H_OUT,
		0
	};

	DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}
