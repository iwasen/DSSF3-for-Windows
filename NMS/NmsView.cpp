// NmsView.cpp : CNmsView クラスの動作の定義を行います。
//

#include "stdafx.h"
#include "Nms.h"
#include "NmsDoc.h"
#include "NmsView.h"
#include "NmsFactor.h"
#include "ChangeNoiseSrcDlg.h"
#include "CalcAcf.h"
#include "SetData.h"

/////////////////////////////////////////////////////////////////////////////
// CNmsView

IMPLEMENT_DYNCREATE(CNmsView, CListViewEx)

BEGIN_MESSAGE_MAP(CNmsView, CListViewEx)
	ON_COMMAND(IDM_VIEW_FACTOR, OnViewFactor)
	ON_UPDATE_COMMAND_UI(IDM_VIEW_FACTOR, OnUpdateViewFactor)
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(IDM_CHANGE_NOISE_SRC, OnChangeNoiseSrc)
	ON_UPDATE_COMMAND_UI(IDM_CHANGE_NOISE_SRC, OnUpdateChangeNoiseSrc)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	// 標準印刷コマンド
	ON_COMMAND(ID_FILE_PRINT, CListViewEx::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CListViewEx::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CListViewEx::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNmsView クラスの構築/消滅

BOOL CNmsView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: この位置で CREATESTRUCT cs を修正して Window クラスまたはスタイルを
	//  修正してください。
	cs.style |= LVS_REPORT | LVS_OWNERDRAWFIXED | LVS_SHOWSELALWAYS;

	return CListViewEx::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CNmsView クラスの描画

void CNmsView::OnDraw(CDC* /*pDC*/)
{
	CNmsDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: この場所にネイティブ データ用の描画コードを追加します。
}

void CNmsView::OnInitialUpdate()
{
	CListViewEx::OnInitialUpdate();

	CNmsDoc* pDoc = GetDocument();
	pDoc->m_pListCtrl = &GetListCtrl();

	SetListHeader();
}

/////////////////////////////////////////////////////////////////////////////
// CNmsView クラスの印刷

BOOL CNmsView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// デフォルトの印刷準備
	return DoPreparePrinting(pInfo);
}

void CNmsView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 印刷前の特別な初期化処理を追加してください。
}

void CNmsView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 印刷後の後処理を追加してください。
}

/////////////////////////////////////////////////////////////////////////////
// CNmsView クラスの診断

#ifdef _DEBUG
CNmsDoc* CNmsView::GetDocument() // 非デバッグ バージョンはインラインです。
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CNmsDoc)));
	return (CNmsDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CNmsView クラスのメッセージ ハンドラ

void CNmsView::SetListHeader()
{
	struct LISTCOLUMN {
		UINT nID;
		LPCSTR text;
		int fmt;
		int width;
	};
	static constexpr LISTCOLUMN tColumnHeader[] = {
		{0, "No.", LVCFMT_LEFT, 50},
		{IDS_MEASUREMENT_TIME, NULL, LVCFMT_LEFT, 128},
		{IDS_STATUS, NULL, LVCFMT_LEFT, 70},
		{IDS_DECISION, NULL, LVCFMT_LEFT, 90},
//		{IDS_NOISE_VALUE, NULL, LVCFMT_RIGHT, 65},
		{IDS_MAX_SPL, NULL, LVCFMT_RIGHT, 65},
		{IDS_TAUE, NULL, LVCFMT_RIGHT, 53},
		{IDS_TAU1, NULL, LVCFMT_RIGHT, 53},
		{IDS_PHI1, NULL, LVCFMT_RIGHT, 53},
		{0, "IACC", LVCFMT_RIGHT, 53},
		{IDS_TIACC, NULL, LVCFMT_RIGHT, 53},
		{0, "WIACC", LVCFMT_RIGHT, 53}
	};
	CString str;

	const LISTCOLUMN *pColumnHeader = tColumnHeader;
	const int size = ARRAY_SIZE(tColumnHeader);

	for (int i = 0; i < size; i++, pColumnHeader++) {
		if (pColumnHeader->nID == 0)
			str = pColumnHeader->text;
		else
			str.LoadString(pColumnHeader->nID);
		LV_COLUMN lvc{
			.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT,
			.fmt = pColumnHeader->fmt,
			.cx = CDpi::AdjustDpi(pColumnHeader->width),
			.pszText = (LPSTR)(LPCSTR)str
		};
		GetListCtrl().InsertColumn(i, &lvc);
	}
}

void CNmsView::OnUpdate(CView* /*pSender*/, LPARAM lHint, CObject* pHint)
{
	switch (lHint) {
	case VIEWHINT_ADDITEM:
		AddItem((CNmsItem *)pHint);
		break;
	case VIEWHINT_UPDATEITEM:
		UpdateItem((CNmsItem *)pHint);
		break;
	case VIEWHINT_CLEARITEM:
		GetListCtrl().DeleteAllItems();
		break;
	}
}

void CNmsView::AddItem(const CNmsItem *pNmsItem)
{
	CListCtrl &lc = GetListCtrl();
	CString str;

	const int nItem = pNmsItem->m_nIndex;
	str.Format("%d", nItem + 1);
	lc.InsertItem(nItem, (LPCSTR)str);
	lc.SetItemData(nItem, (DWORD_PTR)pNmsItem);

	SetNmsItem(pNmsItem);

	lc.EnsureVisible(nItem, FALSE);
}

void CNmsView::UpdateItem(const CNmsItem *pNmsItem)
{
	SetNmsItem(pNmsItem);
}

void CNmsView::SetNmsItem(const CNmsItem *pNmsItem)
{
	CListCtrl &lc = GetListCtrl();
	static constexpr UINT status[] = {NULL, IDS_NOT_CALC, IDS_IN_CALC, IDS_FINISHED};
	const int nItem = pNmsItem->m_nIndex;
	CString str;
	int nItemColor;

	lc.SetItemText(nItem, 1, (LPCSTR)pNmsItem->m_cTime.Format("%Y/%m/%d %H:%M:%S"));
	str.LoadString(status[pNmsItem->m_nStatus]);
	lc.SetItemText(nItem, 2, str);
	str = pNmsItem->m_NoiseSrcData.aName;
	if (str.GetLength() > 0 && str[0] == '=')
		str = str.Mid(1);
	if (pNmsItem->m_bLearning)
		str = "*" + str;
	lc.SetItemText(nItem, 3, str);

	if (pNmsItem->m_nStatus == STATUS_FINISH_CALC) {
		float fMaxSPL = -100;
		AcfFactor *pAcfFactor = NULL;
		IAcfFactor *pIAcfFactor = NULL;

		const int nIdentPos = GetIdentPos(pNmsItem->m_pNmsFactorData, pNmsItem->m_nNmsFactorData, &g_oSetData.Measurement.IdentCondition);
		fMaxSPL = pNmsItem->m_pNmsFactorData[nIdentPos].IAcf.fSPL;
		pIAcfFactor = &pNmsItem->m_pNmsFactorData[nIdentPos].IAcf;
		pAcfFactor = &pNmsItem->m_pNmsFactorData[nIdentPos].Acf;

		if (pIAcfFactor != NULL) {
			str.Format("%.2f", pIAcfFactor->fSPL);
			lc.SetItemText(nItem, 4, str);

			str.Format("%.2f", pAcfFactor->fTauE * 1000);
			lc.SetItemText(nItem, 5, str);

			str.Format("%.2f", pAcfFactor->fTau1 * 1000);
			lc.SetItemText(nItem, 6, str);

			str.Format("%.2f", pAcfFactor->fPhi1);
			lc.SetItemText(nItem, 7, str);

			str.Format("%.2f", pIAcfFactor->fIACC);
			lc.SetItemText(nItem, 8, str);

			str.Format("%.2f", pIAcfFactor->fTauIACC * 1000);
			lc.SetItemText(nItem, 9, str);

			str.Format("%.2f", pIAcfFactor->fWIACC * 1000);
			lc.SetItemText(nItem, 10, str);
		}
	} else {
		lc.SetItemText(nItem, 5, "---");
		lc.SetItemText(nItem, 6, "---");
		lc.SetItemText(nItem, 7, "---");
		lc.SetItemText(nItem, 8, "---");
	}

	switch (pNmsItem->m_nStatus) {
	case STATUS_WAIT_CALC:
		nItemColor = ITEM_COLOR_BLUE;
		break;
	case STATUS_IN_CALC:
		nItemColor = ITEM_COLOR_RED;
		break;
	case STATUS_FINISH_CALC:
		nItemColor = ITEM_COLOR_BLACK;
		break;
	default:
		nItemColor = ITEM_COLOR_BLACK;
		break;
	}
	SetItemColor(nItem, nItemColor);
}

void CNmsView::OnViewFactor()
{
	const CListCtrl &lc = GetListCtrl();
	POSITION pos;
	int nIndex;
	CNmsItem *pNmsItem;

	if ((pos = lc.GetFirstSelectedItemPosition()) != NULL) {
		nIndex = lc.GetNextSelectedItem(pos);
		pNmsItem = (CNmsItem *)lc.GetItemData(nIndex);
		if (pNmsItem->m_nStatus == STATUS_FINISH_CALC)
			CNmsFactor::OpenFactorWnd(pNmsItem);
	}
}

void CNmsView::OnUpdateViewFactor(CCmdUI* pCmdUI)
{
	const CListCtrl &lc = GetListCtrl();
	POSITION pos;
	int nIndex;
	CNmsItem *pNmsItem;
	BOOL bEnable = FALSE;

	if ((pos = lc.GetFirstSelectedItemPosition()) != NULL) {
		nIndex = lc.GetNextSelectedItem(pos);
		pNmsItem = (CNmsItem *)lc.GetItemData(nIndex);
		if (pNmsItem->m_nStatus == STATUS_FINISH_CALC)
			bEnable = TRUE;
	}

	pCmdUI->Enable(bEnable);
}

void CNmsView::OnLButtonDblClk(UINT /*nFlags*/, CPoint /*point*/)
{
	OnViewFactor();
}

void CNmsView::OnChangeNoiseSrc()
{
	CNmsItem *pNmsItem;

	if ((pNmsItem = GetSelectedItem()) != NULL) {
		if (pNmsItem->m_nStatus == STATUS_FINISH_CALC) {
			CChangeNoiseSrcDlg dlg;
			dlg.m_pNmsItem = pNmsItem;
			if (dlg.DoModal() == IDOK)
				SetNmsItem(pNmsItem);
		}
	}
}

void CNmsView::OnUpdateChangeNoiseSrc(CCmdUI* pCmdUI)
{
	CNmsItem *pNmsItem;
	BOOL bEnable = FALSE;

	if ((pNmsItem = GetSelectedItem()) != NULL) {
		if (pNmsItem->m_nStatus == STATUS_FINISH_CALC)
			bEnable = TRUE;
	}

	pCmdUI->Enable(bEnable);
}

void CNmsView::OnRclick(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	OnChangeNoiseSrc();

	*pResult = 0;
}

CNmsItem *CNmsView::GetSelectedItem() const
{
	const CListCtrl &lc = GetListCtrl();
	POSITION pos;
	int nIndex;

	if ((pos = lc.GetFirstSelectedItemPosition()) == NULL)
		return NULL;

	nIndex = lc.GetNextSelectedItem(pos);
	return (CNmsItem *)lc.GetItemData(nIndex);
}
