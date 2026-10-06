// BaseDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "Rta.h"
#include "BaseDlg.h"

// CBaseDlg ダイアログ

CBaseDlg::CBaseDlg(UINT nID, CWnd* pParent)
	: CDialogExt(nID, pParent)
{
	m_bStart = FALSE;
	m_sizeWindow = 0;
	m_sizeClient = 0;
	m_nWindowKind = 0;
	m_hAccel = NULL;
}

BEGIN_MESSAGE_MAP(CBaseDlg, CDialogExt)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipText)
	ON_WM_GETMINMAXINFO()
	ON_WM_SYSCOMMAND()
	ON_WM_INITMENU()
END_MESSAGE_MAP()

// CBaseDlg メッセージ ハンドラ

void CBaseDlg::PostNcDestroy()
{
	CDialogExt::PostNcDestroy();

	((CRtaApp *)::AfxGetApp())->CloseFuncWindow(this);

	delete this;
}

BOOL CBaseDlg::Create(UINT nIDTemplate, int nLeft, int nTop, int nFontSize, CWnd* pParentWnd)
{
	BOOL bResult = CDialogExt::CreateIndirect(GetDialogResource(nIDTemplate, nFontSize), pParentWnd);
	if (bResult) {
		if (nLeft == 0 && nTop == 0)
			ShowWindow(SW_SHOW);
		else {
			if (::GetSystemMetrics(SM_CMONITORS) == 1) {
				CRect rectWindow;
				GetWindowRect(&rectWindow);

				CRect rectScreen;
				::SystemParametersInfo(SPI_GETWORKAREA, 0, &rectScreen, 0);

				if (nLeft < rectScreen.left)
					nLeft = rectScreen.left;

				if (nTop < rectScreen.top)
					nTop = rectScreen.top;

				if (nLeft + rectWindow.Width() > rectScreen.right)
					nLeft = rectScreen.right - rectWindow.Width();

				if (nTop + rectWindow.Height() > rectScreen.bottom)
					nTop = rectScreen.bottom - rectWindow.Height();
			}

			SetWindowPos(NULL, nLeft, nTop, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
		}
	}
	return bResult;
}

void CBaseDlg::OnCancel()
{
	DestroyWindow();
}

void CBaseDlg::OnOK()
{
}

BOOL CBaseDlg::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* /*pResult*/)
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

BOOL CBaseDlg::OnInitDialog()
{
	CDialogExt::OnInitDialog();

	if (m_nWindowKind != 0) {
		m_hAccel = LoadAccelerators(NULL, MAKEINTRESOURCE(IDR_ACCELERATOR1));

		CMenu* pSysMenu = GetSystemMenu(FALSE);
		if (pSysMenu != NULL) {
			CString sText;
			sText.LoadString(IDS_FONT_ENLARGE);
			sText += "\tAlt+UP";
			pSysMenu->InsertMenu(0, MF_BYPOSITION, IDM_FONT_ENLARGE, sText);
			sText.LoadString(IDS_FONT_REDUCE);
			sText += "\tAlt+DOWN";
			pSysMenu->InsertMenu(1, MF_BYPOSITION, IDM_FONT_REDUCE, sText);
			pSysMenu->InsertMenu(2, MF_BYPOSITION, MF_SEPARATOR);
		}
	}

	EnableToolTips(TRUE);

	return TRUE;
}

void CBaseDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	if (m_sizeWindow.cx != 0) {
		lpMMI->ptMinTrackSize.x = m_sizeWindow.cx;
		lpMMI->ptMinTrackSize.y = m_sizeWindow.cy;
	}

	CDialogExt::OnGetMinMaxInfo(lpMMI);
}

BOOL CBaseDlg::PreTranslateMessage(MSG* pMsg)
{
	if (m_hAccel != NULL) {
		if (TranslateAccelerator(GetSafeHwnd(), m_hAccel, pMsg))
			return TRUE;
	}

	return CDialogExt::PreTranslateMessage(pMsg);
}

void CBaseDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	int nFontIndex;
	int nFontSize;

	switch (nID & 0xfff0) {
	case IDM_FONT_ENLARGE:
		nFontIndex = GetFontIndex();
		nFontSize = g_tFontSize[nFontIndex == -1 ? 0 : nFontIndex + 1];
		((CWnd *)((CRtaApp *)::AfxGetApp())->m_pWndRta)->PostMessage(WM_CHANGE_FONT_SIZE, m_nWindowKind, nFontSize);
		break;
	case IDM_FONT_REDUCE:
		nFontIndex = GetFontIndex();
		nFontSize = g_tFontSize[nFontIndex == -1 ? 0 : nFontIndex - 1];
		((CWnd *)((CRtaApp *)::AfxGetApp())->m_pWndRta)->PostMessage(WM_CHANGE_FONT_SIZE, m_nWindowKind, nFontSize);
		break;
	}

	CDialogExt::OnSysCommand(nID, lParam);
}

int CBaseDlg::GetFontIndex()
{
	int nFontSize;

	switch (m_nWindowKind) {
	case WINDOW_SG:
		nFontSize = g_oSetData2.nFontSizeSg;
		break;
	case WINDOW_FFT:
		nFontSize = g_oSetData2.nFontSizeFft;
		break;
	case WINDOW_OS:
		nFontSize = g_oSetData2.nFontSizeOs;
		break;
	case WINDOW_IMP:
		nFontSize = g_oSetData2.nFontSizeImp;
		break;
	case WINDOW_ACF:
		nFontSize = g_oSetData2.nFontSizeAcf;
		break;
	case WINDOW_DST:
		nFontSize = g_oSetData2.nFontSizeDst;
		break;
	case WINDOW_FRE:
		nFontSize = g_oSetData2.nFontSizeFre;
		break;
	default:
		return -1;
	}

	for (int i = 0; g_tFontSize[i] != 0; i++) {
		if (g_tFontSize[i] == nFontSize)
			return i;
	}

	return -1;
}

void CBaseDlg::OnInitMenu(CMenu* pMenu)
{
	CDialogExt::OnInitMenu(pMenu);

	const int nFontIndex = GetFontIndex();

	pMenu->EnableMenuItem(IDM_FONT_ENLARGE, g_tFontSize[nFontIndex + 1] != 0 ? MF_BYCOMMAND : (MF_BYCOMMAND | MF_GRAYED));
	pMenu->EnableMenuItem(IDM_FONT_REDUCE, nFontIndex != 0 ? MF_BYCOMMAND : (MF_BYCOMMAND | MF_GRAYED));
}
