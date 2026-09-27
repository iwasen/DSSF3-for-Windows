// GraphEcho.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Sas.h"
#include "GraphEcho.h"
#include "ImpFactor.h"
#include "Calc.h"
#include "Help\ContextHelp.h"

#define COLOR_LEFT			RGB(0, 128, 255)
#define COLOR_RIGHT			RGB(255, 64, 192)

#define SCROLL_RANGE 16384
#define MAX_ZOOM_TIME	512

/////////////////////////////////////////////////////////////////////////////
// CGraphEcho ダイアログ

CGraphEcho::CGraphEcho(CWnd* pParent /*=NULL*/)
	: CDialogExt(CGraphEcho::IDD, pParent)
{
	m_iLeftRight = -1;

	m_pEchoTime = NULL;
	m_pEchoEnergy = NULL;
	m_pEchoData = NULL;
	m_pEchoData2 = NULL;
	m_bReDraw = FALSE;
	m_nScrollSize = -1;
	m_nScrollPos = -1;
	m_fStartTime = 0;
	m_fDispTime = 0;
	m_fT20Reg0 = 0;
	m_fT20Reg1 = 0;
	m_fT30Reg0 = 0;
	m_fT30Reg1 = 0;
	m_fEDTReg0 = 0;
	m_fEDTReg1 = 0;
}

void CGraphEcho::DoDataExchange(CDataExchange* pDX)
{
	CDialogExt::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ZOOM_H, m_cZoomH);
	DDX_Control(pDX, IDC_REGRESS, m_cRegress);
	DDX_Control(pDX, IDC_GRAPH_EE, m_cGraphEE);
	DDX_Control(pDX, IDC_GRAPH_IR, m_cGraphIR);
	DDX_Control(pDX, IDC_LEFT_RIGHT, m_cLeftRight);
	DDX_Control(pDX, IDC_LEFT, m_cLeft);
	DDX_Control(pDX, IDC_RIGHT, m_cRight);
	DDX_Control(pDX, IDC_GRAPH_ECHO, m_cGraphEcho);
	DDX_Control(pDX, IDC_SCROLL_H, m_cScrollH);
	DDX_Radio(pDX, IDC_LEFT_RIGHT, m_iLeftRight);
}

BEGIN_MESSAGE_MAP(CGraphEcho, CDialogExt)
	ON_COMMAND(IDM_ZOOM_FULL_H, OnZoomFullH)
	ON_COMMAND(IDM_ZOOM_IN_H, OnZoomInH)
	ON_COMMAND(IDM_ZOOM_OUT_H, OnZoomOutH)
	ON_WM_HSCROLL()
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_LEFT_RIGHT, OnLeftRight)
	ON_BN_CLICKED(IDC_LEFT, OnLeft)
	ON_BN_CLICKED(IDC_RIGHT, OnRight)
	ON_BN_CLICKED(IDC_REGRESS, OnRegress)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphEcho メッセージ ハンドラ

BOOL CGraphEcho::OnInitDialog()
{
	CWaitCursor waitCursor;
	CString str;

	CDialogExt::OnInitDialog();

	if (m_tbZoomH.Create(this, WS_CHILD | WS_VISIBLE | CBRS_SIZE_FIXED | CBRS_ALIGN_LEFT | CBRS_TOOLTIPS, ID_ZOOM_H) &&
		m_tbZoomH.LoadToolBar(IDR_ZOOM_H)) {
		WINDOWPLACEMENT wp;
		m_cZoomH.GetWindowPlacement(&wp);

		CSize sizeToolBar;
		sizeToolBar = m_tbZoomH.CalcFixedLayout(FALSE, FALSE);
		m_tbZoomH.MoveWindow(0, wp.rcNormalPosition.top - 8, sizeToolBar.cx, sizeToolBar.cy);
	}

	str.LoadString(IDS_ECHOTIMEPATTERN);
	m_cGraphIR.Initialize(80, str, 1, COLOR_LEFT, GraphCallBack, (LPARAM)this);
	str.LoadString(IDS_DECAYCURVE);
	m_cGraphEE.Initialize(80, str, GraphCallBack, (LPARAM)this);
	str.LoadString(IDS_SCHROEDER);
	m_cGraphEcho.Initialize(90, str, GraphCallBack, (LPARAM)this);

	m_fTotalTime = (double)m_nData / m_fRate;

	m_pEchoTime = new double[m_nData];
	m_pEchoEnergy = new double[m_nData];
	m_pEchoData = new double[m_nData];
	m_pEchoData2 = new double[m_nData];

	if (m_pDbImpulseRec->nChannel == 2) {
		m_iLeftRight = 0;
	} else {
		m_iLeftRight = 1;
		m_cLeft.EnableWindow(FALSE);
		m_cRight.EnableWindow(FALSE);
	}

	m_cRegress.EnableWindow(m_pDbAcParamRec != NULL);

	CalcGraphWindow();
	DispGraphWindow();

	UpdateData(FALSE);

	return TRUE;
}

void CGraphEcho::OnZoomFullH()
{
	m_fDispTime = m_fTotalTime;
	DispGraphWindow();
}

void CGraphEcho::OnZoomInH()
{
	m_fDispTime /= 2;
	DispGraphWindow();
}

void CGraphEcho::OnZoomOutH()
{
	m_fDispTime *= 2;
	DispGraphWindow();
}

void CGraphEcho::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* /*pScrollBar*/)
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

void CGraphEcho::CalcGraphWindow()
{
	double *p1, *p2;
	int i;

	switch (m_iLeftRight) {
	case 0:		// Left+Right
		p1 = m_pIRLeft;
		p2 = m_pIRRight;
		for (i = 0; i < m_nData; i++)
			m_pEchoTime[i] = m_pIRLeft[i] + m_pIRRight[i];
		break;
	case 1:		// Left
		p1 = m_pIRLeft;
		p2 = NULL;
		memcpy(m_pEchoTime, m_pIRLeft, m_nData * sizeof(double));
		break;
	case 2:		// Right
		p1 = m_pIRRight;
		p2 = NULL;
		memcpy(m_pEchoTime, m_pIRRight, m_nData * sizeof(double));
		break;
	default:
		return;
	}

	if (m_pDbAcParamRec != NULL) {
		switch (m_iLeftRight) {
		case 0:		// Left+Right
			m_fT0 = (m_pDbAcParamRec->dbAcParamResult.fDeltaT0L + m_pDbAcParamRec->dbAcParamResult.fDeltaT0R) / 2;
			m_fT1 = (m_pDbAcParamRec->dbAcParamResult.fDeltaT1L + m_pDbAcParamRec->dbAcParamResult.fDeltaT1R) / 2;
			break;
		case 1:		// Left
			m_fT0 = m_pDbAcParamRec->dbAcParamResult.fDeltaT0L;
			m_fT1 = m_pDbAcParamRec->dbAcParamResult.fDeltaT1L;
			break;
		case 2:		// Right
			m_fT0 = m_pDbAcParamRec->dbAcParamResult.fDeltaT0R;
			m_fT1 = m_pDbAcParamRec->dbAcParamResult.fDeltaT1R;
			break;
		}
		CalcParamTSub(p1, p2, m_nData, m_fRate, m_fT1,
				m_pDbAcParamRec->dbAcParamCond.fTsubEnd, m_pDbAcParamRec->dbAcParamCond.bTsubAuto, m_pDbAcParamRec->dbAcParamCond.fTsubNoise, &m_fDev, NULL, &m_fEndTime);

		CalcParamTx2(p1, p2, m_nData, m_fRate, (int)(m_fT0 / 1000 * m_fRate), &m_fT20Reg0, &m_fT20Reg1, &m_fT30Reg0, &m_fT30Reg1, &m_fEDTReg0, &m_fEDTReg1);

		m_fTsubNoise = m_pDbAcParamRec->dbAcParamCond.fTsubNoise;
	} else {
		m_fT1 = 0;
		m_fDev = 0;
		m_fTsubNoise = setData.CalcParam.fTsubNoise;
	}

	double power;
	for (i = 0; i < m_nData; i++) {
		if (p2 != NULL)
			power = p1[i] * p1[i] + p2[i] * p2[i];
		else
			power = p1[i] * p1[i];
			m_pEchoEnergy[i] = power;
	}

	double max = GetMaxData(m_pEchoEnergy, m_nData);
	for (i = 0; i < m_nData; i++)
		m_pEchoEnergy[i] /= max;

	if (m_fTsubNoise != 0)
		CalcEchoData(p1, p2, m_pEchoData, m_nData, m_fRate, m_fT1, m_fTsubNoise);
	CalcEchoData(p1, p2, m_pEchoData2, m_nData, m_fRate, m_fT1, 0);
}

void CGraphEcho::DispGraphWindow()
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
	m_cGraphIR.DispImpulse(m_fTotalTime, m_fStartTime, m_fDispTime, m_pEchoTime, m_nData, bScroll);
	m_cGraphEE.DispEnergy(m_fTotalTime, m_fStartTime, m_fDispTime, m_pEchoEnergy, m_nData, 0, -70, bScroll);
	m_cGraphEcho.DispGraph(m_fTotalTime, m_fStartTime, m_fDispTime, m_fTsubNoise == 0 ? NULL : m_pEchoData, m_pEchoData2, m_nData, m_fRate, m_fT0, m_fT1, m_fDev, m_fEndTime, 10, -70, bScroll, m_fT20Reg0, m_fT20Reg1, m_fT30Reg0, m_fT30Reg1, m_fEDTReg0, m_fEDTReg1);

	m_cZoomH = int(m_fTotalTime / m_fDispTime);
}

void CGraphEcho::OnDestroy()
{
	CDialogExt::OnDestroy();

	if (m_pEchoTime != NULL) {
		delete [] m_pEchoTime;
		m_pEchoTime = NULL;
	}

	if (m_pEchoEnergy != NULL) {
		delete [] m_pEchoEnergy;
		m_pEchoEnergy = NULL;
	}

	if (m_pEchoData != NULL) {
		delete [] m_pEchoData;
		m_pEchoData = NULL;
	}

	if (m_pEchoData2 != NULL) {
		delete [] m_pEchoData2;
		m_pEchoData2 = NULL;
	}
}

void CGraphEcho::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogExt::OnShowWindow(bShow, nStatus);

	if (bShow && m_bReDraw) {
		CalcGraphWindow();
		DispGraphWindow();
		m_bReDraw = FALSE;
	}
}

void CGraphEcho::ReDraw()
{
	if (m_hWnd != NULL) {
		if (IsWindowVisible()) {
			CalcGraphWindow();
			DispGraphWindow();
		} else
			m_bReDraw = TRUE;
	}
}

void CGraphEcho::OnLeftRight()
{
	UpdateData(TRUE);
	CalcGraphWindow();
	DispGraphWindow();
}

void CGraphEcho::OnLeft()
{
	UpdateData(TRUE);
	CalcGraphWindow();
	DispGraphWindow();
}

void CGraphEcho::OnRight()
{
	UpdateData(TRUE);
	CalcGraphWindow();
	DispGraphWindow();
}

void CGraphEcho::OnRegress()
{
	CChangeTsubDlg dlg(this);

	dlg.m_dTsubEnd = m_pDbAcParamRec->dbAcParamCond.fTsubEnd;
	dlg.m_bTsubAuto = m_pDbAcParamRec->dbAcParamCond.bTsubAuto;
	dlg.m_dTsubNoise = m_pDbAcParamRec->dbAcParamCond.fTsubNoise * 100;

	if (dlg.DoModal() == IDOK) {
		CWaitCursor waitCursor;
		m_pDbAcParamRec->dbAcParamCond.fTsubEnd = dlg.m_dTsubEnd;
		m_pDbAcParamRec->dbAcParamCond.bTsubAuto = dlg.m_bTsubAuto;
		m_pDbAcParamRec->dbAcParamCond.fTsubNoise = dlg.m_dTsubNoise / 100;
		CalcTsub(m_pDbImpulseRec, m_hWaveData, m_pDbAcParamRec, m_fRate);
		m_pParent->ChangeData();
	}
}

void CALLBACK CGraphEcho::GraphCallBack(WPARAM code, LPARAM data, double param1, double /*param2*/)
{
	CGraphEcho *pWnd = (CGraphEcho *)data;

	if (code == CC_SCROLL) {
		pWnd->m_fStartTime -= param1 * pWnd->m_fDispTime;
		pWnd->DispGraphWindow();
	}
}

BOOL CGraphEcho::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static const UINT aIDs[] = {
		IDC_GRAPH_IR, IDH_GRAPH_ECHO_GRAPH_IR,
		IDC_GRAPH_EE, IDH_GRAPH_ECHO_GRAPH_EE,
		IDC_ZOOM_H, IDH_GRAPH_ECHO_ZOOM_H,
		IDC_SCROLL_H, IDH_GRAPH_ECHO_SCROLL_H,
		IDC_GRAPH_ECHO, IDH_GRAPH_ECHO_GRAPH_ECHO,
		IDC_LEFT_RIGHT, IDH_GRAPH_ECHO_LEFT_RIGHT,
		IDC_LEFT, IDH_GRAPH_ECHO_LEFT,
		IDC_RIGHT, IDH_GRAPH_ECHO_RIGHT,
		IDC_REGRESS, IDH_GRAPH_ECHO_REGRESS,
		ID_ZOOM_H_FULL, IDH_GRAPH_ECHO_H_FULL,
		ID_ZOOM_H_IN, IDH_GRAPH_ECHO_H_IN,
		ID_ZOOM_H_OUT, IDH_GRAPH_ECHO_H_OUT,
		0
	};

	DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CChangeTsubDlg ダイアログ

CChangeTsubDlg::CChangeTsubDlg(CWnd* pParent /*=NULL*/)
	: CDialogAF(CChangeTsubDlg::IDD, pParent)
{
	m_dTsubEnd = 0.0;
	m_bTsubAuto = FALSE;
	m_dTsubNoise = 0.0;
}

void CChangeTsubDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TSUB_END, m_cTsubEnd);
	DDX_Text(pDX, IDC_TSUB_END, m_dTsubEnd);
	DDX_Check(pDX, IDC_TSUB_AUTO, m_bTsubAuto);
	DDX_Text(pDX, IDC_TSUB_NOISE, m_dTsubNoise);
}

BEGIN_MESSAGE_MAP(CChangeTsubDlg, CDialogAF)
	ON_BN_CLICKED(IDC_TSUB_AUTO, OnTsubAuto)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChangeTsubDlg メッセージ ハンドラ

BOOL CChangeTsubDlg::OnInitDialog()
{
	CDialogAF::OnInitDialog();

	EnableControls();

	return TRUE;
}

void CChangeTsubDlg::OnTsubAuto()
{
	UpdateData(TRUE);
	EnableControls();
}

void CChangeTsubDlg::EnableControls()
{
	m_cTsubEnd.EnableWindow(!m_bTsubAuto);
}

BOOL CChangeTsubDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static const UINT aIDs[] = {
		IDOK, IDH_CHANGE_TSUB_OK,
		IDCANCEL, IDH_CHANGE_TSUB_CANCEL,
		IDC_TSUB_END, IDH_CHANGE_TSUB_END,
		IDC_TSUB_AUTO, IDH_CHANGE_TSUB_AUTO,
		IDC_TSUB_NOISE, IDH_CHANGE_TSUB_NOISE,
		0
	};

	DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}
