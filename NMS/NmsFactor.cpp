// NmsFactor.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Nms.h"
#include "NmsFactor.h"
#include "SetData.h"
#include "Help\ContextHelp.h"

#define CLM_TIME	0
#define CLM_PHI0	1
#define CLM_TAUE	2
#define CLM_TAU1	3
#define CLM_PHI1	4
#define CLM_SPL		5
#define CLM_IACC	6
#define CLM_TIACC	7
#define CLM_WIACC	8

/////////////////////////////////////////////////////////////////////////////
// CNmsFactor ダイアログ

CNmsFactor::CNmsFactor(CWnd* pParent /*=NULL*/)
	: CDialogML(CNmsFactor::IDD, pParent)
{
	m_pAcfFactor = NULL;
	m_pIAcfFactor = NULL;
	m_pNmsFactorData = NULL;
	m_pGraphPhi0 = NULL;
	m_pGraphTauE = NULL;
	m_pGraphTau1 = NULL;
	m_pGraphPhi1 = NULL;
	m_pGraphIACC = NULL;
	m_pGraphSPL = NULL;
	m_pGraphTIACC = NULL;
	m_pGraphWIACC = NULL;
	m_nCurrentPage = 0;
	m_nSortItem = 0;
	m_nCurrentDataNo = 0;
	m_hWaveData = NULL;

	Create((CWnd *)&wndTop);
}

CNmsFactor::~CNmsFactor()
{
	DeleteGraphWindow();
	FreeBuffers();

	if (m_hWaveData != NULL)
		::GlobalFree(m_hWaveData);
}

void CNmsFactor::DoDataExchange(CDataExchange* pDX)
{
	CDialogML::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRAPH, m_cGraph);
	DDX_Control(pDX, IDC_FACTOR_LIST, m_cFactorList);
	DDX_Control(pDX, IDC_PLAY_BACK, m_cPlayBackBtn);
}

BEGIN_MESSAGE_MAP(CNmsFactor, CDialogML)
	ON_WM_DESTROY()
	ON_NOTIFY(TCN_SELCHANGE, IDC_GRAPH, OnSelchangeGraph)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_FACTOR_LIST, OnItemchangedFactorList)
	ON_BN_CLICKED(IDC_PLAY_BACK, OnPlayBack)
	ON_MESSAGE(WM_SELECT_DATA, OnSelectData)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNmsFactor メッセージ ハンドラ

void CNmsFactor::OpenFactorWnd(const CNmsItem *pNmsItem)
{
	if (g_pNmsFactor == NULL)
		g_pNmsFactor = new CNmsFactor;

	g_pNmsFactor->ShowFactor(pNmsItem);
	g_pNmsFactor->m_nIndex = pNmsItem->m_nIndex;
}

void CNmsFactor::CloseFactorWnd()
{
	if (g_pNmsFactor != NULL)
		g_pNmsFactor->DestroyWindow();
}

void CNmsFactor::ShowFactor(const CNmsItem *pNmsItem)
{
	m_pNmsConditionData = &pNmsItem->m_NmsConditionData;
	m_nNmsFactorData = pNmsItem->m_nNmsFactorData;
	m_pNmsFactorData = pNmsItem->m_pNmsFactorData;

	DispNmsFactorList();
	DispGraph();
}

BOOL CNmsFactor::OnInitDialog()
{
	CDialogML::OnInitDialog();

	m_cFactorList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	InitNmsFactorList();

	SetWindowPos(NULL, g_oSetData.FactorWindow.left, g_oSetData.FactorWindow.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

	return TRUE;
}

void CNmsFactor::DispGraph()
{
	int i;

	m_cGraph.DeleteAllPages();
	DeleteGraphWindow();
	FreeBuffers();

	m_pAcfFactor = new AcfFactor[m_nNmsFactorData];
	for (i = 0; i < m_nNmsFactorData; i++)
		m_pAcfFactor[i] = m_pNmsFactorData[i].Acf;

	m_pIAcfFactor = new IAcfFactor[m_nNmsFactorData];
	for (i = 0; i < m_nNmsFactorData; i++)
		m_pIAcfFactor[i] = m_pNmsFactorData[i].IAcf;

	m_pGraphPhi0 = new CGraphFactor;
	m_pGraphPhi0->m_nFactor = FACTOR_PHI0;
	m_pGraphPhi0->m_pAcfFactor = m_pAcfFactor;
	m_pGraphPhi0->m_nData = m_nNmsFactorData;
	m_pGraphPhi0->m_fStepTime = m_pNmsConditionData->AcfCondition.fRunningStep;
	m_pGraphPhi0->m_fAdjustPhi0 = 0;
	m_cGraph.AddPage(m_pGraphPhi0, IDD_GRAPH_FACTOR, IDS_PHI0);

	m_pGraphTauE = new CGraphFactor;
	m_pGraphTauE->m_nFactor = FACTOR_TAUE;
	m_pGraphTauE->m_pAcfFactor = m_pAcfFactor;
	m_pGraphTauE->m_nData = m_nNmsFactorData;
	m_pGraphTauE->m_fStepTime = m_pNmsConditionData->AcfCondition.fRunningStep;
	m_pGraphTauE->m_fAdjustPhi0 = 0;
	m_cGraph.AddPage(m_pGraphTauE, IDD_GRAPH_FACTOR, IDS_TAUE);

	m_pGraphTau1 = new CGraphFactor;
	m_pGraphTau1->m_nFactor = FACTOR_TAU1;
	m_pGraphTau1->m_pAcfFactor = m_pAcfFactor;
	m_pGraphTau1->m_nData = m_nNmsFactorData;
	m_pGraphTau1->m_fStepTime = m_pNmsConditionData->AcfCondition.fRunningStep;
	m_pGraphTau1->m_fAdjustPhi0 = 0;
	m_cGraph.AddPage(m_pGraphTau1, IDD_GRAPH_FACTOR, IDS_TAU1);

	m_pGraphPhi1 = new CGraphFactor;
	m_pGraphPhi1->m_nFactor = FACTOR_PHI1;
	m_pGraphPhi1->m_pAcfFactor = m_pAcfFactor;
	m_pGraphPhi1->m_nData = m_nNmsFactorData;
	m_pGraphPhi1->m_fStepTime = m_pNmsConditionData->AcfCondition.fRunningStep;
	m_pGraphPhi1->m_fAdjustPhi0 = 0;
	m_cGraph.AddPage(m_pGraphPhi1, IDD_GRAPH_FACTOR, IDS_PHI1);

	m_pGraphSPL = new CGraphFactor;
	m_pGraphSPL->m_nFactor = FACTOR_SPL;
	m_pGraphSPL->m_pIAcfFactor = m_pIAcfFactor;
	m_pGraphSPL->m_nData = m_nNmsFactorData;
	m_pGraphSPL->m_fStepTime = m_pNmsConditionData->AcfCondition.fRunningStep;
	m_pGraphSPL->m_fAdjustPhi0 = 0;
	m_cGraph.AddPage(m_pGraphSPL, IDD_GRAPH_FACTOR, "SPL");

	m_pGraphIACC = new CGraphFactor;
	m_pGraphIACC->m_nFactor = FACTOR_IACC;
	m_pGraphIACC->m_pIAcfFactor = m_pIAcfFactor;
	m_pGraphIACC->m_nData = m_nNmsFactorData;
	m_pGraphIACC->m_fStepTime = m_pNmsConditionData->AcfCondition.fRunningStep;
	m_pGraphIACC->m_fAdjustPhi0 = 0;
	m_cGraph.AddPage(m_pGraphIACC, IDD_GRAPH_FACTOR, "IACC");

	m_pGraphTIACC = new CGraphFactor;
	m_pGraphTIACC->m_nFactor = FACTOR_TIACC;
	m_pGraphTIACC->m_pIAcfFactor = m_pIAcfFactor;
	m_pGraphTIACC->m_nData = m_nNmsFactorData;
	m_pGraphTIACC->m_fStepTime = m_pNmsConditionData->AcfCondition.fRunningStep;
	m_pGraphTIACC->m_fAdjustPhi0 = 0;
	m_cGraph.AddPage(m_pGraphTIACC, IDD_GRAPH_FACTOR, IDS_TIACC);

	m_pGraphWIACC = new CGraphFactor;
	m_pGraphWIACC->m_nFactor = FACTOR_WIACC;
	m_pGraphWIACC->m_pIAcfFactor = m_pIAcfFactor;
	m_pGraphWIACC->m_nData = m_nNmsFactorData;
	m_pGraphWIACC->m_fStepTime = m_pNmsConditionData->AcfCondition.fRunningStep;
	m_pGraphWIACC->m_fAdjustPhi0 = 0;
	m_cGraph.AddPage(m_pGraphWIACC, IDD_GRAPH_FACTOR, "W_IACC");

	if (m_nCurrentPage >= m_cGraph.GetItemCount())
		m_nCurrentPage = 0;
	m_cGraph.SetPage(m_nCurrentPage);
}

void CNmsFactor::InitNmsFactorList()
{
	LV_COLUMN lvc;
	struct LISTCOLUMN {
		UINT nID;
		char *text;
		int width;
	};
	static const LISTCOLUMN tColumnHeader[] = {
		{0, "Time", 53},
		{IDS_PHI0, NULL, 53},
		{IDS_TAUE, NULL, 53},
		{IDS_TAU1, NULL, 53},
		{IDS_PHI1, NULL, 53},
		{0, "SPL", 53},
		{0, "IACC", 53},
		{IDS_TIACC, NULL, 53},
		{0, "W_IACC", 53}
	};
	static const char *tUnit[] = {" [sec]", " [dB]", " [ms]", " [ms]", "", "", "", ""};
	CString str;

	const LISTCOLUMN *pColumnHeader = tColumnHeader;
	int size = sizeof(tColumnHeader) / sizeof(LISTCOLUMN);

	for (int i = 0; i < size; i++, pColumnHeader++) {
		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
		lvc.fmt = LVCFMT_RIGHT;
		if (pColumnHeader->nID == 0)
			str = pColumnHeader->text;
		else
			str.LoadString(pColumnHeader->nID);
//		str += tUnit[i];
		lvc.pszText = (LPTSTR)(LPCSTR)str;

		lvc.cx = CDpi::AdjustDpi(pColumnHeader->width, 92);
		m_cFactorList.InsertColumn(i, &lvc);
	}
}

void CNmsFactor::DispNmsFactorList()
{
	CString str;

	m_cFactorList.DeleteAllItems();

	double time = 0;
	for (int i = 0; i < m_nNmsFactorData; i++) {
		str.Format("%g", time);
		m_cFactorList.InsertItem(i, str);

		str.Format("%.2lf", m_pNmsFactorData[i].Acf.fPhi0);
		m_cFactorList.SetItemText(i, CLM_PHI0, str);

		str.Format("%.2lf", m_pNmsFactorData[i].Acf.fTauE * 1000);
		m_cFactorList.SetItemText(i, CLM_TAUE, str);

		str.Format("%.2lf", m_pNmsFactorData[i].Acf.fTau1 * 1000);
		m_cFactorList.SetItemText(i, CLM_TAU1, str);

		str.Format("%.2lf", m_pNmsFactorData[i].Acf.fPhi1);
		m_cFactorList.SetItemText(i, CLM_PHI1, str);

		str.Format("%.2lf", m_pNmsFactorData[i].IAcf.fSPL);
		m_cFactorList.SetItemText(i, CLM_SPL, str);

		str.Format("%.2lf", m_pNmsFactorData[i].IAcf.fIACC);
		m_cFactorList.SetItemText(i, CLM_IACC, str);

		str.Format("%.2lf", m_pNmsFactorData[i].IAcf.fTauIACC * 1000);
		m_cFactorList.SetItemText(i, CLM_TIACC, str);

		str.Format("%.2lf", m_pNmsFactorData[i].IAcf.fWIACC * 1000);
		m_cFactorList.SetItemText(i, CLM_WIACC, str);

		m_cFactorList.SetItemData(i, i);

		time += m_pNmsConditionData->AcfCondition.fRunningStep;
	}

	m_cFactorList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
}

void CNmsFactor::DeleteGraphWindow()
{
	if (m_pGraphPhi0 != NULL) {
		delete m_pGraphPhi0;
		m_pGraphPhi0 = NULL;
	}

	if (m_pGraphTauE != NULL) {
		delete m_pGraphTauE;
		m_pGraphTauE = NULL;
	}

	if (m_pGraphTau1 != NULL) {
		delete m_pGraphTau1;
		m_pGraphTau1 = NULL;
	}

	if (m_pGraphPhi1 != NULL) {
		delete m_pGraphPhi1;
		m_pGraphPhi1 = NULL;
	}

	if (m_pGraphSPL != NULL) {
		delete m_pGraphSPL;
		m_pGraphSPL = NULL;
	}

	if (m_pGraphIACC != NULL) {
		delete m_pGraphIACC;
		m_pGraphIACC = NULL;
	}

	if (m_pGraphTIACC != NULL) {
		delete m_pGraphTIACC;
		m_pGraphTIACC = NULL;
	}

	if (m_pGraphWIACC != NULL) {
		delete m_pGraphWIACC;
		m_pGraphWIACC = NULL;
	}
}

void CNmsFactor::FreeBuffers()
{
	if (m_pAcfFactor != NULL) {
		delete [] m_pAcfFactor;
		m_pAcfFactor = NULL;
	}

	if (m_pIAcfFactor != NULL) {
		delete [] m_pIAcfFactor;
		m_pIAcfFactor = NULL;
	}
}

void CNmsFactor::PostNcDestroy()
{
	g_pNmsFactor  = NULL;

	CDialogML::PostNcDestroy();
}

void CNmsFactor::OnCancel()
{
	DestroyWindow();
}

void CNmsFactor::OnDestroy()
{
	WINDOWPLACEMENT winpl;

	winpl.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(&winpl);			// ダイアログ表示位置取得
	g_oSetData.FactorWindow.left = winpl.rcNormalPosition.left;
	g_oSetData.FactorWindow.top = winpl.rcNormalPosition.top;

	CDialogML::OnDestroy();
}

void CNmsFactor::OnSelchangeGraph(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	m_nCurrentPage = m_cGraph.GetCurSel();

	*pResult = 0;
}

void CNmsFactor::OnItemchangedFactorList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	m_cFactorList.UpdateWindow();
	if (pNMListView->uNewState & LVIS_SELECTED && m_nCurrentDataNo != pNMListView->lParam) {
		m_nCurrentDataNo = (int)pNMListView->lParam;

		m_cFactorList.UpdateWindow();

		if (m_pGraphPhi0 != NULL)
			m_pGraphPhi0->ReDraw(m_nCurrentDataNo);

		if (m_pGraphTauE != NULL)
			m_pGraphTauE->ReDraw(m_nCurrentDataNo);

		if (m_pGraphTau1 != NULL)
			m_pGraphTau1->ReDraw(m_nCurrentDataNo);

		if (m_pGraphPhi1 != NULL)
			m_pGraphPhi1->ReDraw(m_nCurrentDataNo);

		if (m_pGraphSPL != NULL)
			m_pGraphSPL->ReDraw(m_nCurrentDataNo);

		if (m_pGraphIACC != NULL)
			m_pGraphIACC->ReDraw(m_nCurrentDataNo);

		if (m_pGraphTIACC != NULL)
			m_pGraphTIACC->ReDraw(m_nCurrentDataNo);

		if (m_pGraphWIACC != NULL)
			m_pGraphWIACC->ReDraw(m_nCurrentDataNo);
	}

	*pResult = 0;
}

void CNmsFactor::OnPlayBack()
{
	if (m_hWaveData != NULL) {
		::GlobalFree(m_hWaveData);
		m_hWaveData = NULL;
	}

	CString tempFileName;
	tempFileName.Format("%s\\%d.WAV", (LPCSTR)g_sTempDir, m_nIndex);
	if (MMWaveReadFile(&m_hWaveData, (LPTSTR)(LPCSTR)tempFileName) == 0)
		m_cPlayBackBtn.Play(m_hWaveData);
}

LRESULT CNmsFactor::OnSelectData(WPARAM wParam, LPARAM /*lParam*/)
{
	int nIndex = (int)(*(double *)wParam / m_pNmsConditionData->AcfCondition.fRunningStep + 0.5);

	if (nIndex >= 0 && nIndex < m_cFactorList.GetItemCount()) {
		m_cFactorList.SetItemState(nIndex, LVIS_SELECTED, LVIS_SELECTED);
		m_cFactorList.EnsureVisible(nIndex, FALSE);
	}

	return 0;
}

BOOL CNmsFactor::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static const UINT aIDs[] = {
		IDC_GRAPH, IDH_NMS_FACTOR_GRAPH,
		IDC_FACTOR_LIST, IDH_NMS_FACTOR_LIST,
		IDC_PLAY_BACK, IDH_NMS_FACTOR_PLAY_BACK,
		IDCANCEL, IDH_NMS_FACTOR_CANCEL,
		0
	};

	DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}
