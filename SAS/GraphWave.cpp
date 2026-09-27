// GraphWave.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Sas.h"
#include "GraphWave.h"
#include "Help\ContextHelp.h"

#define SCROLL_RANGE 16384
#define MAX_ZOOM_TIME	512
#define MAX_ZOOM_VALUE	512
#define ZOOM_LOWER		0
#define ZOOM_CENTER		1
#define ZOOM_UPPER		2

/////////////////////////////////////////////////////////////////////////////
// CGraphWave ダイアログ

CGraphWave::CGraphWave(CWnd* pParent /*=NULL*/)
	: CDialogExt(CGraphWave::IDD, pParent)
{
	m_bReDraw = FALSE;
	m_nHScrollSize = -1;
	m_nHScrollPos = -1;
	m_nVScrollSize = -1;
	m_nVScrollPos = -1;
	m_fStartTime = 0;
	m_fDispTime = 0;
	m_fStartValue = 0;
	m_fDispValue = 0;
	m_nSel = -1;
	m_pZoomGraph = NULL;
	m_pTempBuf = NULL;
	m_nData = 0;
	m_nZoomChannel = 0;
}

CGraphWave::~CGraphWave()
{
	if (m_pTempBuf != NULL)
		delete [] m_pTempBuf;
}

void CGraphWave::DoDataExchange(CDataExchange* pDX)
{
	CDialogExt::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRAPH_LEFT, m_cGraphLeft);
	DDX_Control(pDX, IDC_GRAPH_RIGHT, m_cGraphRight);
	DDX_Control(pDX, IDC_SCROLL_H, m_cScrollH);
	DDX_Control(pDX, IDC_SCROLL_V, m_cScrollV);
	DDX_Control(pDX, IDC_ZOOM_H, m_cZoomH);
	DDX_Control(pDX, IDC_ZOOM_V, m_cZoomV);
}

BEGIN_MESSAGE_MAP(CGraphWave, CDialogExt)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_SHOWWINDOW()
	ON_COMMAND(IDM_ZOOM_FULL_H, OnZoomFullH)
	ON_COMMAND(IDM_ZOOM_IN_H, OnZoomInH)
	ON_COMMAND(IDM_ZOOM_OUT_H, OnZoomOutH)
	ON_COMMAND(IDM_ZOOM_FULL_V, OnZoomFullV)
	ON_COMMAND(IDM_ZOOM_IN_V, OnZoomInV)
	ON_COMMAND(IDM_ZOOM_OUT_V, OnZoomOutV)
	ON_WM_HELPINFO()
	ON_MESSAGE(WM_MARKER_CURSOR, OnMarkerCursor)
	ON_MESSAGE(WM_SELECT_MARKER, OnSelectMarker)
	ON_MESSAGE(WM_GRAPH_DRAG, OnGraphDrag)
	ON_MESSAGE(WM_RESIZE, OnResize)
	ON_MESSAGE(WM_CLOSE_GRAPH, OnCloseGraph)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphWave メッセージ ハンドラ

BOOL CGraphWave::OnInitDialog()
{
	CDialogExt::OnInitDialog();

	PWAVEDATA pWaveData = (PWAVEDATA)::GlobalLock(m_hWaveData);
	m_fTotalTime = (double)(pWaveData->dataSize / (pWaveData->nBitsPerSample / 8) / pWaveData->nChannels) / pWaveData->nSamplesPerSec;
	m_fTotalValue = 2;
	m_fMinValue = -1;
	int nBytesPerSample = pWaveData->nBitsPerSample / 8;
	int nSampleNum = pWaveData->dataSize / nBytesPerSample;
	m_pTempBuf = new double[nSampleNum];
	CopyWaveToDouble(pWaveData->waveData, m_pTempBuf, NULL, nSampleNum, pWaveData->nBitsPerSample);
	m_nData = nSampleNum / pWaveData->nChannels;
	m_nChannels = pWaveData->nChannels;
	::GlobalUnlock(m_hWaveData);

	CString str;
	if (m_nChannels == 2) {
		str.LoadString(IDS_LEFTCHANNEL);
		m_cGraphLeft.Initialize(0, str, GraphCallBack, (LPARAM)this);
		str.LoadString(IDS_RIGHTCHANNEL);
		m_cGraphRight.Initialize(1, str, GraphCallBack, (LPARAM)this);
	} else {
		RECT rectLeft, rectRight;
		m_cGraphLeft.GetWindowRect(&rectLeft);
		m_cGraphRight.GetWindowRect(&rectRight);
		m_cGraphLeft.SetWindowPos(NULL, 0, 0, rectLeft.right - rectLeft.left, rectRight.bottom - rectLeft.top, SWP_NOMOVE | SWP_NOZORDER);
		m_cGraphLeft.Initialize(0, NULL, GraphCallBack, (LPARAM)this);
		m_cGraphRight.ShowWindow(SW_HIDE);
	}

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

	DispGraphWindow();

	return TRUE;
}

void CGraphWave::OnZoomFullH()
{
	m_fDispTime = m_fTotalTime;
	DispGraphWindow();
}

void CGraphWave::OnZoomInH()
{
	m_fDispTime /= 2;
	DispGraphWindow();
}

void CGraphWave::OnZoomOutH()
{
	m_fDispTime *= 2;
	DispGraphWindow();
}

void CGraphWave::OnZoomFullV()
{
	m_fDispValue = m_fTotalValue;
	DispGraphWindow();
}

void CGraphWave::OnZoomInV()
{
	m_fDispValue /= 2;
	m_fStartValue += m_fDispValue / 2;

	DispGraphWindow();
}

void CGraphWave::OnZoomOutV()
{
	m_fStartValue -= m_fDispValue / 2;
	m_fDispValue *= 2;
	DispGraphWindow();
}

void CGraphWave::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* /*pScrollBar*/)
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

void CGraphWave::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* /*pScrollBar*/)
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

void CGraphWave::DispGraphWindow()
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

	double fSelStart = m_nSel * m_pAcfCondition->fRunningStep;
	double fSelEnd = fSelStart + m_pAcfCondition->fIntegrationTime;

	SetZoomArea();

	m_cGraphLeft.DispGraph(m_fTotalTime, m_fStartTime, m_fDispTime, m_pTempBuf, m_nData, m_nChannels, m_fStartValue + m_fDispValue, m_fStartValue, m_fDispTime < m_fTotalTime, m_fDispValue < m_fTotalValue, fSelStart, fSelEnd, *m_pAcfMarkerRec, *m_nAcfMarkerRec);

	if (m_nChannels == 2)
		m_cGraphRight.DispGraph(m_fTotalTime, m_fStartTime, m_fDispTime, m_pTempBuf + 1, m_nData, m_nChannels, m_fStartValue + m_fDispValue, m_fStartValue, m_fDispTime < m_fTotalTime, m_fDispValue < m_fTotalValue, fSelStart, fSelEnd, *m_pAcfMarkerRec, *m_nAcfMarkerRec);

	if (m_pZoomGraph != NULL)
		DispZoomGraph(m_pZoomGraph);

	m_cZoomH = int(m_fTotalTime / m_fDispTime);
	m_cZoomV = int(m_fTotalValue / m_fDispValue);
}

void CGraphWave::SetHScroll()
{
	int nScrollSize = (int)(m_fDispTime / m_fTotalTime * SCROLL_RANGE);
	int nScrollPos = (int)(m_fStartTime / m_fTotalTime * SCROLL_RANGE);

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

void CGraphWave::SetVScroll()
{
	int nScrollSize;
	int nScrollPos;

	nScrollSize = (int)(m_fDispValue / m_fTotalValue * SCROLL_RANGE);
	nScrollPos = (int)((m_fStartValue - m_fMinValue) / m_fTotalValue * SCROLL_RANGE);

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

void CGraphWave::OnOK()
{
}

void CGraphWave::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogExt::OnShowWindow(bShow, nStatus);

	if (bShow && m_bReDraw) {
		DispGraphWindow();
		m_bReDraw = FALSE;
	}
}

void CGraphWave::ReDraw(int nSel)
{
	if (nSel != -1)
		m_nSel = nSel;

	if (m_hWnd != NULL) {
		if (IsWindowVisible()) {
			DispGraphWindow();
		} else
			m_bReDraw = TRUE;
	}
}

void CALLBACK CGraphWave::GraphCallBack(WPARAM code, LPARAM data, double param1, double param2)
{
	CGraphWave *pWnd = (CGraphWave *)data;
	double fTime;

	switch (code) {
	case CC_SCROLL:
		pWnd->m_fStartTime -= param1 * pWnd->m_fDispTime;
		pWnd->m_fStartValue += param2 * pWnd->m_fDispValue;
		pWnd->DispGraphWindow();
		break;
	case CC_DBLCLK:
		fTime = (double)(pWnd->m_fStartTime + param1 * pWnd->m_fDispTime);
		pWnd->GetParent()->SendMessage(WM_SELECT_DATA, (WPARAM)&fTime, 0);
		break;
	case CC_RBUTTONDOWN:
		fTime = (double)(pWnd->m_fStartTime + param1 * pWnd->m_fDispTime);
		pWnd->GetParent()->SendMessage(WM_RIGHT_BUTTON, (WPARAM)&fTime, 0);
		break;
	}
}

BOOL CGraphWave::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static const UINT aIDs[] = {
		IDC_GRAPH_LEFT, IDH_GRAPH_WAVE_LEFT,
		IDC_GRAPH_RIGHT, IDH_GRAPH_WAVE_RIGHT,
		IDC_ZOOM_V, IDH_GRAPH_WAVE_ZOOM_V,
		IDC_SCROLL_V, IDH_GRAPH_WAVE_SCROLL_V,
		IDC_ZOOM_H, IDH_GRAPH_WAVE_ZOOM_H,
		IDC_SCROLL_H, IDH_GRAPH_WAVE_SCROLL_H,
		ID_ZOOM_H_FULL, IDH_GRAPH_WAV_H_FULL,
		ID_ZOOM_H_IN, IDH_GRAPH_WAV_H_IN,
		ID_ZOOM_H_OUT, IDH_GRAPH_WAV_H_OUT,
		ID_ZOOM_V_FULL, IDH_GRAPH_WAV_ZOOM_V_FULL,
		ID_ZOOM_V_IN, IDH_GRAPH_WAV_ZOOM_V_IN,
		ID_ZOOM_V_OUT, IDH_GRAPH_WAV_ZOOM_V_OUT,
		0
	};

	DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}

LRESULT CGraphWave::OnMarkerCursor(WPARAM wParam, LPARAM lParam)
{
	if (wParam) {
		m_cGraphLeft.DispMarkerCursor(m_fStartTime, m_fDispTime, *(double *)lParam);
		m_cGraphRight.DispMarkerCursor(m_fStartTime, m_fDispTime, *(double *)lParam);
	} else {
		m_cGraphLeft.HideMarkerCursor();
		m_cGraphRight.HideMarkerCursor();
	}

	return 0;
}

LRESULT CGraphWave::OnSelectMarker(WPARAM wParam, LPARAM lParam)
{
	return GetParent()->SendMessage(WM_SELECT_MARKER, wParam, lParam);
}

void CGraphWave::OnDestroy()
{
	CloseZoomWnd();

	CDialogExt::OnDestroy();
}

LRESULT CGraphWave::OnGraphDrag(WPARAM wParam, LPARAM lParam)
{
	SGraphDrag *pGraphDrag = (SGraphDrag *)wParam;
	m_fZoomStartTime = m_fStartTime + m_fDispTime * pGraphDrag->fStartX;
	m_fZoomDispTime = m_fDispTime * (pGraphDrag->fEndX - pGraphDrag->fStartX);
	m_fZoomMinValue = m_fStartValue + m_fDispValue * (1 - pGraphDrag->fEndY);
	m_fZoomMaxValue = m_fZoomMinValue + m_fDispValue * (pGraphDrag->fEndY - pGraphDrag->fStartY);

	if (m_fZoomStartTime < 0)
		m_fZoomStartTime = 0;
	else if (m_fZoomStartTime + m_fZoomDispTime > m_fTotalTime)
		m_fZoomStartTime = m_fTotalTime - m_fZoomDispTime;

	if (m_fZoomMinValue < m_fStartValue) {
		m_fZoomMaxValue += m_fStartValue - m_fZoomMinValue;
		m_fZoomMinValue = m_fStartValue;
	} else if (m_fZoomMaxValue > m_fStartValue + m_fTotalValue) {
		m_fZoomMinValue -= m_fZoomMaxValue - (m_fStartValue + m_fTotalValue);
		m_fZoomMaxValue = m_fStartValue + m_fTotalValue;
	}

	if (m_pZoomGraph == NULL) {
		CString sTitle, sText;
		GetParent()->GetWindowText(sTitle);
		sTitle += " - ";
		sText.LoadString(IDS_WAVE);
		sTitle += sText;

		m_nZoomChannel = (int)lParam;

		m_pZoomGraph = new CWaveWnd;
		m_pZoomGraph->CreateEx(0, ::AfxRegisterWndClass(0, NULL, 0, NULL),
				sTitle, WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
				::GetDesktopWindow(), 0);
		m_pZoomGraph->Initialize(m_nZoomChannel, NULL, GraphCallBack, (LPARAM)this, this);
		SetZoomArea();
		DispZoomGraph(m_pZoomGraph);
		m_pZoomGraph->SetWindowPos(GetParent(), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
	} else {
		if (m_nZoomChannel != (int)lParam) {
			m_nZoomChannel = (int)lParam;
			m_pZoomGraph->ChangeChannel(m_nZoomChannel);
		}

		SetZoomArea();
		DispZoomGraph(m_pZoomGraph);
	}

	return 0;
}

void CGraphWave::DispZoomGraph(CWaveWnd *pWnd) const
{
	double fSelStart = m_nSel * m_pAcfCondition->fRunningStep;
	double fSelEnd = fSelStart + m_pAcfCondition->fIntegrationTime;

	pWnd->DispGraph(m_fTotalTime, m_fZoomStartTime, m_fZoomDispTime, m_pTempBuf + m_nZoomChannel, m_nData, m_nChannels, m_fZoomMaxValue, m_fZoomMinValue, FALSE, FALSE, fSelStart, fSelEnd, *m_pAcfMarkerRec, *m_nAcfMarkerRec);
}

LRESULT CGraphWave::OnResize(WPARAM wParam, LPARAM /*lParam*/)
{
	CWaveWnd *pWnd = (CWaveWnd *)wParam;

	pWnd->SetSize(90);
	DispZoomGraph(pWnd);
	
	return 0;
}

LRESULT CGraphWave::OnCloseGraph(WPARAM wParam, LPARAM /*lParam*/)
{
	CWaveWnd *pWnd = (CWaveWnd *)wParam;

	if (pWnd == m_pZoomGraph) {
		delete m_pZoomGraph;
		m_pZoomGraph = NULL;

		m_cGraphLeft.ResetZoomRect();
		m_cGraphRight.ResetZoomRect();
	}

	return 0;
}

void CGraphWave::CloseZoomWnd()
{
	if (m_pZoomGraph != NULL) {
		m_pZoomGraph->DestroyWindow();
	}
}

void CGraphWave::SetZoomArea()
{
	if (m_pZoomGraph != NULL) {
		double fLeft = (m_fZoomStartTime - m_fStartTime) / m_fDispTime;
		double fTop = 1 - (m_fZoomMaxValue - m_fStartValue) / m_fDispValue;
		double fRight = (m_fZoomStartTime + m_fZoomDispTime - m_fStartTime) / m_fDispTime;
		double fBottom = 1 - (m_fZoomMinValue - m_fStartValue) / m_fDispValue;

		if (m_nZoomChannel == 0) {
			m_cGraphLeft.SetZoomRect(fLeft, fTop, fRight, fBottom);
			m_cGraphRight.ResetZoomRect();
		} else {
			m_cGraphRight.SetZoomRect(fLeft, fTop, fRight, fBottom);
			m_cGraphLeft.ResetZoomRect();
		}
	} else {
		m_cGraphLeft.ResetZoomRect();
		m_cGraphRight.ResetZoomRect();
	}
}
