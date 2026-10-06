// IfaSet.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Rta.h"
#include "IfaSet.h"
#include "FileIO.h"
#include "Help\ContextHelp.h"

#define SCROLL_RANGE 16384
#define MAX_ZOOM_TIME		1024

/////////////////////////////////////////////////////////////////////////////
// CIfaSet ダイアログ

CIfaSet::CIfaSet(CWnd* pParent /*=NULL*/)
	: CDialogAF(CIfaSet::IDD, pParent)
{
	m_bPhaseAdj = FALSE;

	m_pIRData = NULL;
	m_nIFilterID = -1;
	m_pFreqData = NULL;
	m_pPhaseData = NULL;
	m_nScrollSize = -1;
	m_nScrollPos = -1;
	m_nSortItem = -1;
}

CIfaSet::~CIfaSet()
{
	if (m_pIRData != NULL)
		delete [] m_pIRData;

	if (m_pFreqData != NULL)
		delete [] m_pFreqData;

	if (m_pPhaseData != NULL)
		delete [] m_pPhaseData;
}

void CIfaSet::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MAX_ADJ_LEVEL, m_cMaxAdjLevel);
	DDX_Control(pDX, IDC_START_POS, m_cStartPos);
	DDX_Control(pDX, IDC_FREQ, m_cFreq);
	DDX_Control(pDX, IDC_END_POS, m_cEndPos);
	DDX_Control(pDX, IDC_SET, m_cSet);
	DDX_Control(pDX, IDC_DELETE, m_cDelete);
	DDX_Control(pDX, IDC_CHANGE, m_cChange);
	DDX_Control(pDX, IDC_IMP_SCROLL, m_cScroll);
	DDX_Control(pDX, IDC_IMPALSE, m_cImpulse);
	DDX_Control(pDX, IDC_IR_LIST, m_cIRList);
	DDX_Text(pDX, IDC_COMMENT, m_sComment);
	DDX_Text(pDX, IDC_TITLE, m_sTitle);
	DDX_Check(pDX, IDC_PHASE_ADJ, m_bPhaseAdj);
	DDX_Control(pDX, IDC_ZOOM_H, m_cZoomH);
}

BEGIN_MESSAGE_MAP(CIfaSet, CDialogAF)
	ON_WM_HSCROLL()
	ON_COMMAND(IDM_ZOOM_IN_H, OnZoomInH)
	ON_COMMAND(IDM_ZOOM_OUT_H, OnZoomOutH)
	ON_COMMAND(IDM_ZOOM_FULL_H, OnZoomFullH)
	ON_BN_CLICKED(IDC_SET, OnSet)
	ON_BN_CLICKED(IDC_CHANGE, OnChange)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_EN_CHANGE(IDC_MAX_ADJ_LEVEL, OnChangeMaxAdjLevel)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_IR_LIST, OnItemchangedIrList)
	ON_BN_CLICKED(IDC_PHASE_ADJ, OnPhaseAdj)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_IR_LIST, OnColumnclickIrList)
	ON_NOTIFY(NM_DBLCLK, IDC_IR_LIST, OnDblclkIrList)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipText)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIfaSet メッセージ ハンドラ

BOOL CIfaSet::OnInitDialog()
{
	CDialogAF::OnInitDialog();

	if (m_tbZoomH.Create(this, WS_CHILD | WS_VISIBLE | CBRS_SIZE_FIXED | CBRS_ALIGN_LEFT | CBRS_TOOLTIPS, ID_ZOOM_H) &&
		m_tbZoomH.LoadToolBar(IDR_ZOOM_H)) {
		WINDOWPLACEMENT wp;
		m_cZoomH.GetWindowPlacement(&wp);

		CSize sizeToolBar;
		sizeToolBar = m_tbZoomH.CalcFixedLayout(FALSE, FALSE);
		m_tbZoomH.MoveWindow(wp.rcNormalPosition.left - sizeToolBar.cx - 4, wp.rcNormalPosition.top - 8, sizeToolBar.cx, sizeToolBar.cy);

		CToolBarCtrl &tc = m_tbZoomH.GetToolBarCtrl();
		tc.EnableButton(IDM_ZOOM_FULL_H, FALSE);
		tc.EnableButton(IDM_ZOOM_IN_H, FALSE);
		tc.EnableButton(IDM_ZOOM_OUT_H, FALSE);
	}

	struct LISTCOLUMN {
		int textID;
		int width;
	};
	static constexpr LISTCOLUMN tColumn[] = {
		{IDS_TITLE, 200},
		{IDS_COMMENT, 200},
		{IDS_SAMPLING, 65}
	};
	CString str;

	m_cIRList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	for (int i = 0; i < ARRAY_SIZE(tColumn); i++) {
		str.LoadString(tColumn[i].textID);
		LV_COLUMN lvc{
			.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT,
			.fmt = LVCFMT_LEFT,
			.cx = CDpi::AdjustDpi(tColumn[i].width, 98),
			.pszText = (LPSTR)(LPCSTR)str
		};
		m_cIRList.InsertColumn(i, &lvc);
	}

	SetIRList();

	m_cImpulse.Initialize(90, NULL, 0, COLOR_LEFT, GraphCallBack, (LPARAM)this);
	m_cFreq.Initialize(90, NULL, 0, RGB(255, 0, 0), RGB(0, 255, 0));

	if (m_nIFilterID != -1)
		ReadIRData();

	m_cMaxAdjLevel.SetValidChar(VC_NUM);

	return TRUE;
}

void CIfaSet::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* /*pScrollBar*/)
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

void CIfaSet::OnZoomFullH()
{
	m_fDispTime = m_fTotalTime;
	DispIRWindow();
}

void CIfaSet::OnZoomInH()
{
	m_fDispTime /= 2;
	DispIRWindow();
}

void CIfaSet::OnZoomOutH()
{
	m_fDispTime *= 2;
	DispIRWindow();
}

void CIfaSet::OnSet()
{
	double start, end;
	m_cImpulse.GetSelectArea(&start, &end);

	m_nStartPos = (int)(start * m_iSampling);
	m_nEndPos = (int)(end * m_iSampling);
	m_nMaxAdjLevel = m_cMaxAdjLevel;

	OnOK();
}

void CIfaSet::OnChange()
{
	if (::AfxMessageBox(IDS_MSG_CHANGESELECTDATA, MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL)
		return;

	CDbIFilter dbIFilter;
	if (!dbIFilter.Open())
		return;

	double start, end;
	m_cImpulse.GetSelectArea(&start, &end);

	UpdateData(TRUE);

	DbIFilterRec dbIFilterRec;
	dbIFilterRec.nIFilterID = m_nIFilterID;
	dbIFilterRec.sTitle = m_sTitle;
	dbIFilterRec.sComment = m_sComment;
	dbIFilterRec.nSampling = m_iSampling;
	dbIFilterRec.fStartPos = start * 1000;
	dbIFilterRec.fEndPos = end * 1000;
	dbIFilterRec.nMaxLevel = (int)m_cMaxAdjLevel;
	dbIFilterRec.nPhaseAdj = m_bPhaseAdj;

	if (!dbIFilter.UpdateRec(&dbIFilterRec))
		return;

	SetIRList();
}

void CIfaSet::OnDelete()
{
	if (::AfxMessageBox(IDS_MSG_DELETESELECTDATA, MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL)
		return;

	CDbIFilter dbIFilter;
	if (!dbIFilter.Open())
		return;

	if (!dbIFilter.DeleteID(m_nIFilterID))
		return;

	SetIRList();

	m_sTitle.Empty();
	m_sComment.Empty();
	m_iSampling = 0;
	UpdateData(FALSE);

	m_cImpulse.EnableWindow(FALSE);
	m_cFreq.EnableWindow(FALSE);
	m_cSet.EnableWindow(FALSE);
	m_cChange.EnableWindow(FALSE);
	m_cDelete.EnableWindow(FALSE);
}

void CIfaSet::DispIRWindow()
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

void CIfaSet::DispFreqWindow()
{
	m_cFreq.DispGraph(m_pFreqData, m_nData, m_iSampling, 20, (int)(m_iSampling / 2), (int)(dB10(m_fMinLevel) - 5) / 10 * 10 - 10, 0, m_pPhaseData, FALSE);
}

void CIfaSet::ReadIRData()
{
	CDbIFilter dbIFilter;
	if (!dbIFilter.Open())
		return;

	DbIFilterRec dbIFilterRec;
	if (!dbIFilter.ReadRecID(m_nIFilterID, &dbIFilterRec))
		return;

	HWAVEDATA hWaveData;

	if (MMWaveReadDB(&hWaveData, dbIFilter.GetFCB(), "IRDATA") != 0) {
		::AfxMessageBox(IDS_ERR_READIRDATA);
		return;
	}

	if (m_pIRData != NULL) {
		delete [] m_pIRData;
		m_pIRData = NULL;
	}

	PWAVEDATA pWaveData = (PWAVEDATA)::GlobalLock(hWaveData);
	m_nData = pWaveData->dataSize / (pWaveData->nBitsPerSample / 8);
	::GlobalUnlock(hWaveData);

	m_pIRData = new double[m_nData];
	ReadWaveData(hWaveData, m_pIRData, NULL, m_nData);
	::GlobalFree(hWaveData);

	m_fTotalTime = (double)m_nData / dbIFilterRec.nSampling;

	m_sTitle = dbIFilterRec.sTitle;
	m_sComment = dbIFilterRec.sComment;
	m_iSampling = dbIFilterRec.nSampling;
	m_cMaxAdjLevel = dbIFilterRec.nMaxLevel;
	m_bPhaseAdj = dbIFilterRec.nPhaseAdj;
	UpdateData(FALSE);

	m_fTotalTime = (double)m_nData / m_iSampling;
	m_fDispTime = m_fTotalTime / 8;
	m_fStartTime = 0;
	m_cImpulse.EnableWindow(TRUE);
	DispIRWindow();

	const double start = dbIFilterRec.fStartPos / 1000;
	const double end = dbIFilterRec.fEndPos / 1000;
	m_cImpulse.SetSelectArea(start, end);

	if (m_pFreqData != NULL)
		delete [] m_pFreqData;
	m_pFreqData = new double[m_nData];

	if (m_pPhaseData != NULL)
		delete [] m_pPhaseData;
	m_pPhaseData = new double[m_nData];

	CalcPowerSpectrum(start, end);
	m_cFreq.EnableWindow(TRUE);
	DispFreqWindow();

	m_cSet.EnableWindow(TRUE);
	m_cChange.EnableWindow(TRUE);
	m_cDelete.EnableWindow(TRUE);
}

void CIfaSet::SetIRList()
{
	CDbIFilter dbIFilter;
	if (!dbIFilter.Open())
		return;

	m_cIRList.DeleteAllItems();

	CString str;
	DbIFilterRec dbIFilterRec;
	for (int i = 0; ; i++) {
		if (!dbIFilter.ReadRecNext(&dbIFilterRec))
			break;

		m_cIRList.InsertItem(i, dbIFilterRec.sTitle);
		m_cIRList.SetItemText(i, 1, dbIFilterRec.sComment);
		str.Format("%d", dbIFilterRec.nSampling);
		m_cIRList.SetItemText(i, 2, str);

		m_cIRList.SetItemData(i, dbIFilterRec.nIFilterID);
	}
}

void CIfaSet::CalcPowerSpectrum(double start, double end)
{
	int i, j;
	const int nLoop = m_nData / 2;
	double max;
	double min;

	const int nStartPos = (int)(start / m_fTotalTime * m_nData);
	const int nEndPos = (int)(end / m_fTotalTime * m_nData);
	const int nData = nEndPos - nStartPos;

	memcpy(m_pFreqData, m_pIRData + nStartPos + 1, nData * sizeof(double));
	memset(m_pFreqData + nData, 0, (m_nData - nData) * sizeof(double));

	m_oRFFT.fft(m_nData, m_pFreqData);

	if (m_bPhaseAdj) {
		for (i = 0; i < nLoop; i++) {
			j = i * 2;
			m_pPhaseData[i] = atan2(m_pFreqData[j + 1], m_pFreqData[j]);
		}
	} else {
		memset(m_pPhaseData, 0, nLoop * sizeof(double));
//#		for (i = 0; i < nLoop; i++)
//#			m_pPhaseData[i] = 0;
	}

	max = 0;
	m_pFreqData[0] = 0;
	for (i = 1; i < nLoop; i++) {
		j = i * 2;
		m_pFreqData[i] = m_pFreqData[j] * m_pFreqData[j] + m_pFreqData[j + 1] * m_pFreqData[j + 1];
		if (m_pFreqData[i] > max)
			max = m_pFreqData[i];
	}

	const double limit = max / pow(10.0, (double)m_cMaxAdjLevel / 10);
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

void CALLBACK CIfaSet::GraphCallBack(WPARAM code, LPARAM data, double param1, double param2)
{
	CIfaSet *pWnd = (CIfaSet *)data;
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

void CIfaSet::OnChangeMaxAdjLevel()
{
	if (m_pIRData != NULL && m_pFreqData != NULL) {
		double start, end;
		m_cImpulse.GetSelectArea(&start, &end);
		CalcPowerSpectrum(start, end);
		DispFreqWindow();
	}
}

void CIfaSet::OnItemchangedIrList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (pNMListView->uNewState & LVIS_SELECTED) {
		m_nIFilterID = (long)pNMListView->lParam;
		ReadIRData();
	}

	*pResult = 0;
}

void CIfaSet::OnPhaseAdj()
{
	UpdateData(TRUE);

	if (m_pIRData != NULL && m_pFreqData != NULL) {
		double start, end;
		m_cImpulse.GetSelectArea(&start, &end);
		CalcPowerSpectrum(start, end);
		DispFreqWindow();
	}
}

void CIfaSet::OnColumnclickIrList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (m_nSortItem != pNMListView->iSubItem)
		m_nSortDir = 1;
	else
		m_nSortDir *= -1;

	m_nSortItem = pNMListView->iSubItem;
	m_cIRList.SortItems(CompareItems, (LPARAM)this);

	*pResult = 0;
}

int CALLBACK CIfaSet::CompareItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CIfaSet *pDlg = (CIfaSet *)lParamSort;
	CListCtrl &dataList = pDlg->m_cIRList;

	LV_FINDINFO findInfo1{
		.flags = LVFI_PARAM,
		.lParam = lParam1
	};
	const int index1 = dataList.FindItem(&findInfo1);

	LV_FINDINFO findInfo2{
		.flags = LVFI_PARAM,
		.lParam = lParam2
	};
	const int index2 = dataList.FindItem(&findInfo2);

	return dataList.GetItemText(index1, pDlg->m_nSortItem).Compare(dataList.GetItemText(index2, pDlg->m_nSortItem)) * pDlg->m_nSortDir;
}

void CIfaSet::OnDblclkIrList(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	OnSet();

	*pResult = 0;
}

BOOL CIfaSet::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static constexpr UINT aIDs[] = {
		IDC_IR_LIST, IDH_IMP_IFSET_LIST,
		IDC_IMPALSE, IDH_IMP_IFSET_IMPALSE,
		IDC_FREQ, IDH_IMP_IFSET_FREQ,
		IDC_TITLE, IDH_IMP_IFSET_TITLE,
		IDC_COMMENT, IDH_IMP_IFSET_COMMENT,
		IDC_MAX_ADJ_LEVEL, IDH_IMP_IFSET_MAX_ADJ_LEVEL,
		IDC_PHASE_ADJ, IDH_IMP_IFSET_PHASE_ADJ,
		IDC_START_POS, IDH_IMP_IFSET_START_POS,
		IDC_END_POS, IDH_IMP_IFSET_END_POS,
		IDC_SET, IDH_IMP_IFSET_SET,
		IDCANCEL, IDH_IMP_IFSET_CANCEL,
		IDC_CHANGE, IDH_IMP_IFSET_CHANGE,
		IDC_DELETE, IDH_IMP_IFSET_DELETE,
		IDC_IMP_SCROLL, IDH_IMP_SCROLL_H,
		IDC_ZOOM_H, IDH_IMP_ZOOM_H,
		ID_ZOOM_H_FULL, IDH_IMP_ZOOM_H_FULL,
		ID_ZOOM_H_IN, IDH_IMP_ZOOM_H_IN,
		ID_ZOOM_H_OUT, IDH_IMP_ZOOM_H_OUT,
		0
	};

	::DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}

BOOL CIfaSet::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* /*pResult*/)
{
	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR;
	UINT_PTR nID =pNMHDR->idFrom;

	if (pTTT->uFlags & TTF_IDISHWND)
		nID = ::GetDlgCtrlID((HWND)nID);

	if(nID) {
		pTTT->lpszText = MAKEINTRESOURCE(nID);
		pTTT->hinst = ::AfxGetResourceHandle();
		return(TRUE);
	}

	return(FALSE);
}
