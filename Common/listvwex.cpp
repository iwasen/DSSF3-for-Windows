// ListVwEx.cpp : CListViewEx クラスのインプリメンテーション
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1998 Microsoft Corporation
// All rights reserved.
//
// このソース コードは Microsoft Foundation Class リファレンスおよび、
// 関連するオンライン ドキュメントの補足のみを目的としています。
// Microsoft Foundation Class 製品に関する詳しい情報はこれらのドキュメント
// を参照してください。
//

#include "stdafx.h"
#include "ListVwEx.h"

/////////////////////////////////////////////////////////////////////////////
// CListViewEx

IMPLEMENT_DYNCREATE(CListViewEx, CListView)

BEGIN_MESSAGE_MAP(CListViewEx, CListView)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_MESSAGE(LVM_SETIMAGELIST, OnSetImageList)
	ON_MESSAGE(LVM_SETTEXTCOLOR, OnSetTextColor)
	ON_MESSAGE(LVM_SETTEXTBKCOLOR, OnSetTextBkColor)
	ON_MESSAGE(LVM_SETBKCOLOR, OnSetBkColor)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListViewEx クラスの構築/消滅

CListViewEx::CListViewEx()
{
	m_bFullRowSel = FALSE;
	m_bClientWidthSel = TRUE;

	m_cxClient = 0;
	m_cxStateImageOffset = 0;

	m_clrText = ::GetSysColor(COLOR_WINDOWTEXT);
	m_clrTextBk = ::GetSysColor(COLOR_WINDOW);
	m_clrBkgnd = ::GetSysColor(COLOR_WINDOW);
}

BOOL CListViewEx::PreCreateWindow(CREATESTRUCT& cs)
{
	// デフォルトではレポート ビューおよび行全体の選択です。
	cs.style &= ~LVS_TYPEMASK;
	cs.style |= LVS_REPORT | LVS_OWNERDRAWFIXED;
	m_bFullRowSel = TRUE;

	return(CListView::PreCreateWindow(cs));
}

BOOL CListViewEx::SetFullRowSel(BOOL bFullRowSel)
{
	// 変更中は描画しません。
	LockWindowUpdate();

	m_bFullRowSel = bFullRowSel;

	BOOL bRet;

	if (m_bFullRowSel)
		bRet = ModifyStyle(0L, LVS_OWNERDRAWFIXED);
	else
		bRet = ModifyStyle(LVS_OWNERDRAWFIXED, 0L);

	// ビュー タイプの変更中でなければ、ウィンドウを再描画します。
	if (bRet && (GetStyle() & LVS_TYPEMASK) == LVS_REPORT)
		Invalidate();

	// 変更を再描画します。
	UnlockWindowUpdate();

	return(bRet);
}

BOOL CListViewEx::GetFullRowSel() const
{
	return(m_bFullRowSel);
}

/////////////////////////////////////////////////////////////////////////////
// CListViewEx クラスの描画

// 最初とその他の列用のオフセット
#define OFFSET_FIRST	2
#define OFFSET_OTHER	6

void CListViewEx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CListCtrl& ListCtrl=GetListCtrl();
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rcItem(lpDrawItemStruct->rcItem);
	UINT uiFlags = ILD_TRANSPARENT;
	CImageList* pImageList;
	int nItem = lpDrawItemStruct->itemID;
	BOOL bFocus = (GetFocus() == this);
	COLORREF clrTextSave, clrBkSave = 0;
	COLORREF clrImage = m_clrBkgnd;
	static _TCHAR szBuff[MAX_PATH];
	LPCTSTR pszText;

// アイテム データを取得します。

	LV_ITEM lvi{
		.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_INDENT,
		.iItem = nItem,
		.iSubItem = 0,
		.stateMask = 0xFFFF,
		.pszText = szBuff,
		.cchTextMax = sizeof(szBuff)
	};
	ListCtrl.GetItem(&lvi);

	BOOL bSelected = (bFocus || (GetStyle() & LVS_SHOWSELALWAYS)) && lvi.state & LVIS_SELECTED;
	bSelected = bSelected || (lvi.state & LVIS_DROPHILITED);

// アイテムが選択された場合、色を設定します。

	CRect rcAllLabels;
	ListCtrl.GetItemRect(nItem, rcAllLabels, LVIR_BOUNDS);

	CRect rcLabel;
	ListCtrl.GetItemRect(nItem, rcLabel, LVIR_LABEL);

	rcAllLabels.left = rcLabel.left;
	if (m_bClientWidthSel && rcAllLabels.right<m_cxClient)
		rcAllLabels.right = m_cxClient;

	if (bSelected)
	{
		clrTextSave = pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		clrBkSave = pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));

		CBrush brush(::GetSysColor(COLOR_HIGHLIGHT));
		pDC->FillRect(rcAllLabels, &brush);
	} else {
		clrTextSave = pDC->SetTextColor(RGB(
				(lvi.iIndent & ITEM_COLOR_RED) ? 255 : 0,
				(lvi.iIndent & ITEM_COLOR_GREEN) ? 255 : 0,
				(lvi.iIndent & ITEM_COLOR_BLUE) ? 255 : 0));

		CBrush brush(m_clrTextBk);
		pDC->FillRect(rcAllLabels, &brush);
	}

// 色を設定してアイコンをマスクします。

	if (lvi.state & LVIS_CUT)
	{
		clrImage = m_clrBkgnd;
		uiFlags |= ILD_BLEND50;
	}
	else if (bSelected)
	{
		clrImage = ::GetSysColor(COLOR_HIGHLIGHT);
		uiFlags |= ILD_BLEND50;
	}

// 状態アイコンを描画します。

	UINT nStateImageMask = lvi.state & LVIS_STATEIMAGEMASK;
	if (nStateImageMask)
	{
		int nImage = (nStateImageMask>>12) - 1;
		pImageList = ListCtrl.GetImageList(LVSIL_STATE);
		if (pImageList)
		{
			pImageList->Draw(pDC, nImage,
				CPoint(rcItem.left, rcItem.top), ILD_TRANSPARENT);
		}
	}

// 通常のアイコンとオーバーレイ アイコンを描画します。

	CRect rcIcon;
	ListCtrl.GetItemRect(nItem, rcIcon, LVIR_ICON);

	pImageList = ListCtrl.GetImageList(LVSIL_SMALL);
	if (pImageList)
	{
		UINT nOvlImageMask=lvi.state & LVIS_OVERLAYMASK;
		if (rcItem.left<rcItem.right-1)
		{
			ImageList_DrawEx(pImageList->m_hImageList, lvi.iImage,
					pDC->m_hDC,rcIcon.left,rcIcon.top, 16, 16,
					m_clrBkgnd, clrImage, uiFlags | nOvlImageMask);
		}
	}

// アイテムのラベルを描きます。

	ListCtrl.GetItemRect(nItem, rcItem, LVIR_LABEL);
	rcItem.right -= m_cxStateImageOffset;

	pszText = MakeShortString(pDC, szBuff,
				rcItem.right-rcItem.left, 2*OFFSET_FIRST);

	rcLabel = rcItem;
	rcLabel.left += OFFSET_FIRST;
	rcLabel.right -= OFFSET_FIRST;

	pDC->DrawText(pszText,-1,rcLabel,DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER);

// 特別なカラム用のラベルを描きます。

	LV_COLUMN lvc{
		.mask = LVCF_FMT | LVCF_WIDTH
	};

	for(int nColumn = 1; ListCtrl.GetColumn(nColumn, &lvc); nColumn++)
	{
		rcItem.left = rcItem.right;
		rcItem.right += lvc.cx;

		int nRetLen = ListCtrl.GetItemText(nItem, nColumn,
						szBuff, sizeof(szBuff));
		if (nRetLen == 0)
			continue;

		pszText = MakeShortString(pDC, szBuff,
			rcItem.right - rcItem.left, 2*OFFSET_OTHER);

		UINT nJustify = DT_LEFT;

		if(pszText == szBuff)
		{
			switch(lvc.fmt & LVCFMT_JUSTIFYMASK)
			{
			case LVCFMT_RIGHT:
				nJustify = DT_RIGHT;
				break;
			case LVCFMT_CENTER:
				nJustify = DT_CENTER;
				break;
			default:
				break;
			}
		}

		rcLabel = rcItem;
		rcLabel.left += OFFSET_OTHER;
		rcLabel.right -= OFFSET_OTHER;

		pDC->DrawText(pszText, -1, rcLabel,
			nJustify | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER);
	}

// アイテムがフォーカスを持っている場合に、フォーカスのある矩形を描画します。

	if (lvi.state & LVIS_FOCUSED && bFocus)
		pDC->DrawFocusRect(rcAllLabels);

// アイテムが選択された場合、オリジナル カラーに設定します。

	if (bSelected)
	{
        pDC->SetTextColor(clrTextSave);
		pDC->SetBkColor(clrBkSave);
	}
}

LPCTSTR CListViewEx::MakeShortString(CDC* pDC, LPCTSTR lpszLong, int nColumnLen, int nOffset)
{
	static const _TCHAR szThreeDots[] = _T("...");

	int nStringLen = lstrlen(lpszLong);

	if(nStringLen == 0 ||
		(pDC->GetTextExtent(lpszLong, nStringLen).cx + nOffset) <= nColumnLen)
	{
		return(lpszLong);
	}

	static _TCHAR szShort[MAX_PATH];

	lstrcpy(szShort,lpszLong);
	int nAddLen = pDC->GetTextExtent(szThreeDots,sizeof(szThreeDots)).cx;

	for(int i = nStringLen-1; i > 0; i--)
	{
		szShort[i] = 0;
		if((pDC->GetTextExtent(szShort, i).cx + nOffset + nAddLen)
			<= nColumnLen)
		{
			break;
		}
	}

	lstrcat(szShort, szThreeDots);
	return(szShort);
}

void CListViewEx::RepaintSelectedItems()
{
	CListCtrl& ListCtrl = GetListCtrl();
	CRect rcItem, rcLabel;

// フォーカスされたアイテムを無効にすることにより、正しく再描画できます。

	int nItem = ListCtrl.GetNextItem(-1, LVNI_FOCUSED);

	if(nItem != -1)
	{
		ListCtrl.GetItemRect(nItem, rcItem, LVIR_BOUNDS);
		ListCtrl.GetItemRect(nItem, rcLabel, LVIR_LABEL);
		rcItem.left = rcLabel.left;

		InvalidateRect(rcItem, FALSE);
	}

// 選択されたアイテムが保存されない場合は、それらを無効にします。

	if(!(GetStyle() & LVS_SHOWSELALWAYS))
	{
		for(nItem = ListCtrl.GetNextItem(-1, LVNI_SELECTED);
			nItem != -1; nItem = ListCtrl.GetNextItem(nItem, LVNI_SELECTED))
		{
			ListCtrl.GetItemRect(nItem, rcItem, LVIR_BOUNDS);
			ListCtrl.GetItemRect(nItem, rcLabel, LVIR_LABEL);
			rcItem.left = rcLabel.left;

			InvalidateRect(rcItem, FALSE);
		}
	}

// 変更を更新します。

	UpdateWindow();
}

/////////////////////////////////////////////////////////////////////////////
// CListViewEx クラスの診断

#ifdef _DEBUG

void CListViewEx::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);

	dc << "m_bFullRowSel = " << (UINT)m_bFullRowSel;
	dc << "\n";
	dc << "m_cxStateImageOffset = " << m_cxStateImageOffset;
	dc << "\n";
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CListViewEx メッセージ ハンドラ

LRESULT CListViewEx::OnSetImageList(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	// Windows のバージョンが 4 以上であればアイテム テキストの位置に
	// オフセットを加える必要はありません。
	/*
	OSVERSIONINFO info;
	info.dwOSVersionInfoSize = sizeof(info);
	::GetVersionEx(&info);

	if( (int) wParam == LVSIL_STATE && info.dwMajorVersion < 4)
	{
		int cx, cy;

		if(::ImageList_GetIconSize((HIMAGELIST)lParam, &cx, &cy))
			m_cxStateImageOffset = cx;
		else
			m_cxStateImageOffset = 0;
	}
	*/
	return(Default());
}

LRESULT CListViewEx::OnSetTextColor(WPARAM /*wParam*/, LPARAM lParam)
{
	m_clrText = (COLORREF)lParam;
	return(Default());
}

LRESULT CListViewEx::OnSetTextBkColor(WPARAM /*wParam*/, LPARAM lParam)
{
	m_clrTextBk = (COLORREF)lParam;
	return(Default());
}

LRESULT CListViewEx::OnSetBkColor(WPARAM /*wParam*/, LPARAM lParam)
{
	m_clrBkgnd = (COLORREF)lParam;
	return(Default());
}

void CListViewEx::OnSize(UINT nType, int cx, int cy)
{
	m_cxClient = cx;
	CListView::OnSize(nType, cx, cy);
}

void CListViewEx::OnPaint()
{
	// 行全体の選択モードでは、クリップしている領域を拡張する必要があります。
	// それによって選択部分を正しく塗りつぶすことができます。
	if (m_bClientWidthSel &&
		(GetStyle() & LVS_TYPEMASK) == LVS_REPORT &&
		GetFullRowSel())
	{
		CRect rcAllLabels;
		GetListCtrl().GetItemRect(0, rcAllLabels, LVIR_BOUNDS);

		if(rcAllLabels.right < m_cxClient)
		{
			// 正しいクリップ領域を取得するために BeginPaint (CPaintDC c-tor 内の)
			// を呼び出す必要があります。
			CPaintDC dc(this);

			CRect rcClip;
			dc.GetClipBox(rcClip);

			rcClip.left = min(rcAllLabels.right-1, rcClip.left);
			rcClip.right = m_cxClient;

			InvalidateRect(rcClip, FALSE);
			// EndPaint は CPaintDC d-tor 内で呼び出されます。
		}
	}

	CListView::OnPaint();
}

void CListViewEx::OnSetFocus(CWnd* pOldWnd)
{
	CListView::OnSetFocus(pOldWnd);

	// ラベル エディット ボックスからフォーカスを取得中かどうかを確認します。
	if(pOldWnd!=NULL && pOldWnd->GetParent()==this)
		return;

	// アイテムを再描画して変更を表示します。
	if(m_bFullRowSel && (GetStyle() & LVS_TYPEMASK)==LVS_REPORT)
		RepaintSelectedItems();
}

void CListViewEx::OnKillFocus(CWnd* pNewWnd)
{
	CListView::OnKillFocus(pNewWnd);

	// ラベル エディット ボックスへのフォーカスが失われているかどうかを確認します。
	if(pNewWnd != NULL && pNewWnd->GetParent() == this)
		return;

	// アイテムを再描画して変更を表示します。
	if(m_bFullRowSel && (GetStyle() & LVS_TYPEMASK) == LVS_REPORT)
		RepaintSelectedItems();
}

void CListViewEx::SetItemColor(int nItem, int nColor)
{
	LVITEM lvi{
		.mask = LVIF_INDENT,
		.iItem = nItem,
		.iSubItem = 0,
		.iIndent = nColor
	};
	GetListCtrl().SetItem(&lvi);
}
