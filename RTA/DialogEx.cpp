// CDialogExt.cpp : 実装ファイル
//

#include "stdafx.h"
#include "Rta.h"
#include "DialogEx.h"

// CDialogExt ダイアログ

CDialogExt::CDialogExt(UINT nID, CWnd* pParent)
	: CDialogAF(nID, pParent)
{
	m_sizeWindow = 0;
	m_sizeClient = 0;
	m_hMaximunCtrl = NULL;
}

BEGIN_MESSAGE_MAP(CDialogExt, CDialogAF)
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CDialogExt メッセージ ハンドラ

void CDialogExt::SetCtlPosition(UINT nID, double fLeft, double fTop, double fRight, double fBottom)
{
	SetCtlPosition(GetDlgItem(nID), fLeft, fTop, fRight, fBottom);
}

void CDialogExt::SetCtlPosition(CWnd *pWnd, double fLeft, double fTop, double fRight, double fBottom)
{
	HWND hWnd = pWnd->GetSafeHwnd();
	if (hWnd == NULL)
		return;

	// 設定情報を構造体に設定
	SDlgCtlPosition cDlgCtlPosition;
	cDlgCtlPosition.hWnd = hWnd;
	cDlgCtlPosition.fLeft = fLeft;
	cDlgCtlPosition.fTop = fTop;
	cDlgCtlPosition.fRight = fRight;
	cDlgCtlPosition.fBottom = fBottom;

	// 現在のウィンドウ位置を取得
	WINDOWPLACEMENT wp;
	pWnd->GetWindowPlacement(&wp);
	cDlgCtlPosition.rectCtl = wp.rcNormalPosition;

	int nSize = (int)m_aDlgCtlPosition.GetSize();
	for (int i = 0; i < nSize; i++) {
		if (m_aDlgCtlPosition[i].hWnd == hWnd) {
			// 配列に設定情報を上書き
			m_aDlgCtlPosition[i] = cDlgCtlPosition;
			return;
		}
	}

	// 配列に設定情報を追加
	m_aDlgCtlPosition.Add(cDlgCtlPosition);
}

void CDialogExt::SaveWindowSize()
{
	CRect rect;
	GetClientRect(rect);
	m_sizeClient = rect.Size();
	GetWindowRect(rect);
	m_sizeWindow = rect.Size();
}

BOOL CDialogExt::OnInitDialog()
{
	CDialogAF::OnInitDialog();

	SaveWindowSize();

	return TRUE;
}

void CDialogExt::OnSize(UINT nType, int cx, int cy)
{
	CDialogAF::OnSize(nType, cx, cy);

	RepositionControls(cx, cy);
}

void CDialogExt::RepositionControls(int cx, int cy)
{
	// ステータスバーを再配置
	if (m_wndStatusBar.m_hWnd) {
//		RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
		CRect rectStatusBar;
		m_wndStatusBar.GetWindowRect(rectStatusBar);
		cy -= rectStatusBar.Height();
	}

	// ダイアログコントロールを適切な位置に再配置
	if (m_sizeClient.cx && cx != 0) {
		// サイズの差分
		int nx = cx - m_sizeClient.cx;
		int ny = cy - m_sizeClient.cy;

		// 変更されたサイズ分ダイアログ内のコントロールの大きさと位置を変更する
		int nDlgCtlPosition = (int)m_aDlgCtlPosition.GetSize();
		for (int i = 0; i < nDlgCtlPosition; i++) {
			// 子ウィンドウを取得
			SDlgCtlPosition &cDlgCtlPosition = m_aDlgCtlPosition[i];
			CWnd *pWnd = FromHandle(m_aDlgCtlPosition[i].hWnd);

			if (m_hMaximunCtrl == pWnd->m_hWnd) {
				// 子ウィンドウを移動
				pWnd->SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOZORDER);
			} else {
				// 子ウィンドウの位置、大きさを取得
				CRect rect(cDlgCtlPosition.rectCtl);

				// 位置と大きさを調整
				rect.left += (int)(nx * cDlgCtlPosition.fLeft);
				rect.top += (int)(ny * cDlgCtlPosition.fTop);
				rect.right += (int)(nx * cDlgCtlPosition.fRight);
				rect.bottom += (int)(ny * cDlgCtlPosition.fBottom);

				// 子ウィンドウを移動
				pWnd->SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOZORDER);
			}
			pWnd->UpdateWindow();
		}

		Invalidate();
	}

	if (m_wndStatusBar.m_hWnd)
		RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
}

void CDialogExt::SetMaximunControl(HWND hMaximunCtrl)
{
	m_hMaximunCtrl = hMaximunCtrl;

	CRect rect;
	GetClientRect(rect);
	RepositionControls(rect.right, rect.bottom);
}

void CDialogExt::CreateStatusBar()
{
	if (m_wndStatusBar.Create(this)) {
		const UINT ind[2] = {0};

		m_wndStatusBar.SetIndicators(ind, 1);
		m_wndStatusBar.SetPaneInfo(0, 0, SBPS_STRETCH, 0);

		RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

		CRect rectStatusBar;
		m_wndStatusBar.GetWindowRect(rectStatusBar);

		CRect rectDialog;
		GetWindowRect(rectDialog);
		rectDialog.bottom += rectStatusBar.Height();
		m_sizeWindow = rectDialog.Size();
		MoveWindow(rectDialog, FALSE);
	}
}

void CDialogExt::SetStatusBar(UINT nID)
{
	CString sText;
	sText.LoadString(nID);
	SetStatusBar(sText);
}

void CDialogExt::SetStatusBar(LPCTSTR pText)
{
	m_wndStatusBar.SetPaneText(0, pText);
}
