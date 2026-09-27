// DialogEx.cpp : 実装ファイル
//

#include "stdafx.h"
#include "DialogEx.h"

// CDialogExt ダイアログ

CDialogExt::CDialogExt(UINT nIDTemplate, CWnd* pParentWnd)
	: CDialogAF(nIDTemplate, pParentWnd)
{
}

void CDialogExt::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDialogExt, CDialogAF)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipText)
END_MESSAGE_MAP()

// CDialogExt メッセージ ハンドラ

BOOL CDialogExt::OnInitDialog()
{
	CDialogAF::OnInitDialog();

	EnableToolTips(TRUE);

	return TRUE;
}

BOOL CDialogExt::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* /*pResult*/)
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
