// IfaSave.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Rta.h"
#include "IfaSave.h"
#include "FileIO.h"
#include "Help\ContextHelp.h"

#define SCROLL_RANGE 16384
#define MAX_ZOOM_TIME		1024

/////////////////////////////////////////////////////////////////////////////
// CIfaSave ダイアログ

CIfaSave::CIfaSave(CWnd* pParent /*=NULL*/)
	: CDialogAF(CIfaSave::IDD, pParent)
{
	m_bPhaseAdj = FALSE;

	m_pFreqData = NULL;
	m_pPhaseData = NULL;
	m_nScrollSize = -1;
	m_nScrollPos = -1;
}

void CIfaSave::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MAX_ADJ_LEVEL, m_cMaxAdjLevel);
	DDX_Control(pDX, IDC_START_POS, m_cStartPos);
	DDX_Control(pDX, IDC_END_POS, m_cEndPos);
	DDX_Control(pDX, IDC_FREQ, m_cFreq);
	DDX_Control(pDX, IDC_IMP_SCROLL, m_cScroll);
	DDX_Control(pDX, IDC_IMPALSE, m_cImpulse);
	DDX_Text(pDX, IDC_COMMENT, m_sComment);
	DDX_Text(pDX, IDC_TITLE, m_sTitle);
	DDX_Check(pDX, IDC_PHASE_ADJ, m_bPhaseAdj);
	DDX_Control(pDX, IDC_ZOOM_H, m_cZoomH);
}

BEGIN_MESSAGE_MAP(CIfaSave, CDialogAF)
	ON_COMMAND(IDM_ZOOM_IN_H, OnZoomInH)
	ON_COMMAND(IDM_ZOOM_OUT_H, OnZoomOutH)
	ON_COMMAND(IDM_ZOOM_FULL_H, OnZoomFullH)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_WM_DESTROY()
	ON_EN_CHANGE(IDC_MAX_ADJ_LEVEL, OnChangeMaxAdjLevel)
	ON_BN_CLICKED(IDC_PHASE_ADJ, OnPhaseAdj)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipText)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIfaSave メッセージ ハンドラ

BOOL CIfaSave::OnInitDialog()
{
	m_bPhaseAdj = TRUE;

	CDialogAF::OnInitDialog();

	if (m_tbZoomH.Create(this, WS_CHILD | WS_VISIBLE | CBRS_SIZE_FIXED | CBRS_ALIGN_LEFT | CBRS_TOOLTIPS, ID_ZOOM_H) &&
		m_tbZoomH.LoadToolBar(IDR_ZOOM_H)) {
		WINDOWPLACEMENT wp;
		m_cZoomH.GetWindowPlacement(&wp);

		CSize sizeToolBar;
		sizeToolBar = m_tbZoomH.CalcFixedLayout(FALSE, FALSE);
		m_tbZoomH.MoveWindow(wp.rcNormalPosition.left - sizeToolBar.cx - 4, wp.rcNormalPosition.top - 8, sizeToolBar.cx, sizeToolBar.cy);
	}

	m_cImpulse.Initialize(90, NULL, 0, COLOR_LEFT, GraphCallBack, (LPARAM)this);
	m_cImpulse.EnableWindow();

	m_cFreq.Initialize(90, NULL, 0, RGB(255, 0, 0), RGB(0, 255, 0));
	m_cFreq.EnableWindow();

	m_cMaxAdjLevel = 30;

	m_nData = 1 << m_nStage;

	int i;
	double data;
	double max, min;
	double start, end;

	max = 0;
	for (i = 0; i < m_nData; i++) {
		data = fabs(m_pIRData[i]);
		if (data > max)
			max = data;
	}

	for (i = 0; i < m_nData; i++) {
		if (fabs(m_pIRData[i]) > max / 5)
			break;
	}
	start = (i - 0.5) / m_iSampling;

	i += (int)(m_iSampling * 0.001);
	min = fabs(m_pIRData[i]);
	for ( ; i < m_nData; i++) {
		if (fabs(m_pIRData[i]) < min)
			min = fabs(m_pIRData[i]);
		else
			break;
	}
	end = (i + 0.5) / m_iSampling;
	m_cImpulse.SetSelectArea(start, end);

	m_fTotalTime = (double)m_nData / m_iSampling;
	m_fDispTime = m_fTotalTime / 8;
	m_fStartTime = 0;
	DispIRWindow();

	m_pFreqData = new double[m_nData];
	m_pPhaseData = new double[m_nData];
	CalcPowerSpectrum(start, end);
	DispFreqWindow();

	m_cMaxAdjLevel.SetValidChar(VC_NUM);

	return TRUE;
}

void CIfaSave::OnDestroy()
{
	CDialogAF::OnDestroy();

	if (m_pFreqData != NULL) {
		delete [] m_pFreqData;
		m_pFreqData = NULL;
	}

	if (m_pPhaseData != NULL) {
		delete [] m_pPhaseData;
		m_pPhaseData = NULL;
	}
}

void CIfaSave::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* /*pScrollBar*/)
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

	DispIRWindow();
}

void CIfaSave::OnZoomFullH()
{
	m_fDispTime = m_fTotalTime;
	DispIRWindow();
}

void CIfaSave::OnZoomInH()
{
	m_fDispTime /= 2;
	DispIRWindow();
}

void CIfaSave::OnZoomOutH()
{
	m_fDispTime *= 2;
	DispIRWindow();
}

void CIfaSave::OnSave()
{
	if (::AfxMessageBox(IDS_MSG_SAVEIR, MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL)
		return;

	UpdateData();
	if (m_sTitle.IsEmpty()) {
		::AfxMessageBox(IDS_MSG_TITLE);
		return;
	}

	CDbIFilter dbIFilter;
	if (!dbIFilter.Open())
		return;

	DbIFilterRec dbIFilterRec;
	long id;
	if (!dbIFilter.GetNewID(&id))
		return;

	double start, end;
	m_cImpulse.GetSelectArea(&start, &end);

	dbIFilterRec.nIFilterID = id;
	dbIFilterRec.sTitle = m_sTitle;
	dbIFilterRec.sComment = m_sComment;
	dbIFilterRec.nSampling = m_iSampling;
	dbIFilterRec.fStartPos = start * 1000;
	dbIFilterRec.fEndPos = end * 1000;
	dbIFilterRec.nMaxLevel = (int)m_cMaxAdjLevel;
	dbIFilterRec.nPhaseAdj = m_bPhaseAdj;

	if (!dbIFilter.StoreRec(&dbIFilterRec))
		return;

	HWAVEDATA hWaveData = MakeWaveDataHandle(m_iSampling, 16, 1, m_nData, m_pIRData, NULL, TRUE);

	if (MMWaveWriteDB(hWaveData, dbIFilter.GetFCB(), "IRDATA", MMTYPE_WAV) != 0) {
		::AfxMessageBox(IDS_ERR_WRITEIRDATA);
		::GlobalFree(hWaveData);
		return;
	}

	::GlobalFree(hWaveData);

	EndDialog(IDOK);

	return;
}

void CIfaSave::DispIRWindow()
{
	SCROLLINFO si;
	int nScrollSize;
	int nScrollPos;

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

	nScrollSize = (int)(m_fDispTime / m_fTotalTime * SCROLL_RANGE);
	nScrollPos = (int)(m_fStartTime / m_fTotalTime * SCROLL_RANGE);

	if (nScrollSize != m_nScrollSize || nScrollPos != m_nScrollPos) {
		m_nScrollSize = nScrollSize;
		m_nScrollPos = nScrollPos;

		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_ALL;
		si.nMin = 0;
		si.nMax = SCROLL_RANGE;
		si.nPage = nScrollSize;
		si.nPos = nScrollPos;
		m_cScroll.SetScrollInfo(&si, TRUE);
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

	m_cImpulse.DispImpulse(m_fTotalTime, m_fStartTime, m_fDispTime, m_pIRData, m_nData, m_fDispTime < m_fTotalTime);

	m_cZoomH = int(m_fTotalTime / m_fDispTime);
}

void CIfaSave::DispFreqWindow()
{
	m_cFreq.DispGraph(m_pFreqData, m_nData, m_iSampling, 20, (int)(m_iSampling / 2), (int)(dB10(m_fMinLevel) - 5) / 10 * 10 - 10, 0, m_pPhaseData, FALSE);
}

void CIfaSave::CalcPowerSpectrum(double start, double end)
{
	int i, j;
	int nLoop = m_nData / 2;
	double max;
	double min;

	int nStartPos = (int)(start / m_fTotalTime * m_nData);
	int nEndPos = (int)(end / m_fTotalTime * m_nData);
	int nData = nEndPos - nStartPos;

	memcpy(m_pFreqData, m_pIRData + nStartPos + 1, nData * sizeof(double));
	memset(m_pFreqData + nData, 0, (m_nData - nData) * sizeof(double));

	m_oRFFT.fft(m_nData, m_pFreqData);

	if (m_bPhaseAdj) {
		for (i = 0; i < nLoop; i++) {
			j = i * 2;
			m_pPhaseData[i] = atan2(m_pFreqData[j + 1], m_pFreqData[j]);
		}
	} else {
		for (i = 0; i < nLoop; i++)
			m_pPhaseData[i] = 0;
	}

	max = 0;
	m_pFreqData[0] = 0;
	for (i = 1; i < nLoop; i++) {
		j = i * 2;
		m_pFreqData[i] = m_pFreqData[j] * m_pFreqData[j] + m_pFreqData[j + 1] * m_pFreqData[j + 1];
		if (m_pFreqData[i] > max)
			max = m_pFreqData[i];
	}

	double limit = max / pow(10.0, (double)m_cMaxAdjLevel / 10);
	max = 0;
	for (i = 1; i < nLoop; i++) {
		if (m_pFreqData[i] < limit)
			m_pFreqData[i] = limit;

		m_pFreqData[i] = 1 / m_pFreqData[i];
		if (m_pFreqData[i] > max)
			max = m_pFreqData[i];
	}

	min = 1.0;
	for (i = 1; i < nLoop; i++) {
		m_pFreqData[i] /= max;

		if (m_pFreqData[i] < min)
			min = m_pFreqData[i];
	}
	m_fMinLevel = min;
}

void CALLBACK CIfaSave::GraphCallBack(WPARAM code, LPARAM data, double param1, double param2)
{
	CIfaSave *pWnd = (CIfaSave *)data;
	CString str;

	switch (code) {
	case CC_SELECT_SET:
	case CC_SELECT_START:
	case CC_SELECT_END:
	case CC_SELECT_MOVE:
		str.Format("%.2f", param1 * 1000);
		pWnd->m_cStartPos = str;
		pWnd->m_cStartPos.UpdateWindow();

		str.Format("%.2f", param2 * 1000);
		pWnd->m_cEndPos = str;
		pWnd->m_cEndPos.UpdateWindow();

		if (code == CC_SELECT_END) {
			pWnd->CalcPowerSpectrum(param1, param2);
			pWnd->DispFreqWindow();
		}
		break;
	case CC_SCROLL:
		pWnd->m_fStartTime -= param1 * pWnd->m_fDispTime;
		pWnd->DispIRWindow();
		break;
	}
}

void CIfaSave::OnChangeMaxAdjLevel()
{
	if (m_pIRData != NULL && m_pFreqData != NULL) {
		double start, end;
		m_cImpulse.GetSelectArea(&start, &end);
		CalcPowerSpectrum(start, end);
		DispFreqWindow();
	}
}

void CIfaSave::OnPhaseAdj()
{
	UpdateData(TRUE);

	double start, end;
	m_cImpulse.GetSelectArea(&start, &end);
	CalcPowerSpectrum(start, end);
	DispFreqWindow();
}

BOOL CIfaSave::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static const UINT aIDs[] = {
		IDC_IMPALSE, IDH_IMP_IFSAVE_IMPALSE,
		IDC_FREQ, IDH_IMP_IFSAVE_FREQ,
		IDC_TITLE, IDH_IMP_IFSAVE_TITLE,
		IDC_COMMENT, IDH_IMP_IFSAVE_COMMENT,
		IDC_MAX_ADJ_LEVEL, IDH_IMP_IFSAVE_MAX_ADJ_LEVEL,
		IDC_PHASE_ADJ, IDH_IMP_IFSAVE_PHASE_ADJ,
		IDC_START_POS, IDH_IMP_IFSAVE_START_POS,
		IDC_END_POS, IDH_IMP_IFSAVE_END_POS,
		IDC_SAVE, IDH_IMP_IFSAVE_SAVE,
		IDCANCEL, IDH_IMP_IFSAVE_CLOSE,
		IDC_IMP_SCROLL, IDH_IMP_SCROLL_H,
		IDC_ZOOM_H, IDH_IMP_ZOOM_H,
		ID_ZOOM_H_FULL, IDH_IMP_ZOOM_H_FULL,
		ID_ZOOM_H_IN, IDH_IMP_ZOOM_H_IN,
		ID_ZOOM_H_OUT, IDH_IMP_ZOOM_H_OUT,
		0
	};

	DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}

BOOL CIfaSave::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* /*pResult*/)
{
	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR;
	UINT_PTR nID = pNMHDR->idFrom;

	if (pTTT->uFlags & TTF_IDISHWND)
		nID = ::GetDlgCtrlID((HWND)nID);

	if(nID) {
		pTTT->lpszText = MAKEINTRESOURCE(nID);
		pTTT->hinst = ::AfxGetResourceHandle();
		return(TRUE);
	}

	return(FALSE);
}
