// MyCtrl.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Common.h"
#include "MyCtrl.h"

#define TS_MARGIN	10

/////////////////////////////////////////////////////////////////////////////
// CMyButton

BEGIN_MESSAGE_MAP(CMyButton, CButton)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyButton メッセージ ハンドラ

BOOL CMyButton::operator=(BOOL param)
{
	if (GetCheck() != param)
		SetCheck(param);

	return param;
}

CMyButton::operator BOOL()
{
	return GetCheck();
}

/////////////////////////////////////////////////////////////////////////////
// CMyEdit

CMyEdit::CMyEdit()
{
	m_bSetTextColor = FALSE;
	m_nTextColor = (COLORREF)-1;
	m_hNotifyWnd = NULL;
	m_nMaxLength = 0;
}

BEGIN_MESSAGE_MAP(CMyEdit, CEdit)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_CHAR()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyEdit メッセージ ハンドラ

int CMyEdit::operator=(int param)
{
	CString str1, str2;

	str1.Format("%d", param);
	GetWindowText(str2);

	if (str1 != str2)
		SetWindowText(str1);

	return param;
}

long CMyEdit::operator=(long nParam)
{
	CString sStr1, sStr2;

	sStr1.Format("%ld", nParam);
	GetWindowText(sStr2);

	if (sStr1 != sStr2)
		SetWindowText(sStr1);

	return nParam;
}

float CMyEdit::operator=(float param)
{
	CString str1, str2;

	str1.Format("%g", param);
	GetWindowText(str2);

	if (str1 != str2)
		SetWindowText(str1);

	return param;
}

double CMyEdit::operator=(double param)
{
	CString str1, str2;

	str1.Format("%g", param);
	GetWindowText(str2);

	if (str1 != str2)
		SetWindowText(str1);

	return param;
}

CString &CMyEdit::operator=(CString &param)
{
	CString str2;

	GetWindowText(str2);

	if (param != str2)
		SetWindowText(param);

	return param;
}

LPCTSTR CMyEdit::operator=(LPCTSTR param)
{
	CString str2;

	GetWindowText(str2);

	if (str2 != param)
		SetWindowText(param);

	return param;
}

CMyEdit &CMyEdit::operator=(CMyEdit &param)
{
	CString str;

	param.GetWindowText(str);
	SetWindowText(str);

	return param;
}

int CMyEdit::operator==(LPCTSTR param)
{
	GetWindowText(m_str);
	return m_str == param;
}

int CMyEdit::operator==(int param)
{
	GetWindowText(m_str);
	return atoi(m_str) == param;
}

int CMyEdit::operator!=(int param)
{
	GetWindowText(m_str);
	return atoi(m_str) != param;
}

CMyEdit::operator int()
{
	GetWindowText(m_str);
	return atoi(m_str);
}

CMyEdit::operator float()
{
	GetWindowText(m_str);
	return (float)atof(m_str);
}

CMyEdit::operator double()
{
	GetWindowText(m_str);
	return atof(m_str);
}

CMyEdit::operator CString()
{
	GetWindowText(m_str);
	return m_str;
}

void CMyEdit::Format(LPCTSTR lpszFormat, ...)
{
	va_list vl;
	char str[256];
	CString str2;

	va_start(vl, lpszFormat);
	vsprintf_s(str, lpszFormat, vl);

	GetWindowText(str2);

	if (str2 != str)
		SetWindowText(str);
}

void CMyEdit::Blank()
{
	CString str;

	GetWindowText(str);
	if (!str.IsEmpty())
		SetWindowText("");
}

void CMyEdit::LoadString(UINT nStringID)
{
	CString str1, str2;

	str1.LoadString(nStringID);

	GetWindowText(str2);

	if (str2 != str1)
		SetWindowText(str1);
}

void CMyEdit::FormatMessage(UINT nStringID, ...)
{
	va_list vl;
	char str[256];
	CString str2;

	va_start(vl, nStringID);
	str2.LoadString(nStringID);
	vsprintf_s(str, (LPCSTR)str2, vl);

	GetWindowText(str2);

	if (str2 != str)
		SetWindowText(str);
}

void CMyEdit::SetTextColor(COLORREF color)
{
	if (!m_bSetTextColor) {
		m_brush.CreateSolidBrush(::GetSysColor(COLOR_3DFACE));
		m_bSetTextColor = TRUE;
	}

	if (color != m_nTextColor) {
		m_nTextColor = color;
		Invalidate();
	}
}

HBRUSH CMyEdit::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	if (m_bSetTextColor) {
		pDC->SetTextColor(m_nTextColor);
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)m_brush;
	}

	return NULL;
}

void CMyEdit::SetValidChar(UINT nValidChar)
{
	m_sValidStr = "\b\x03\x16\x18";		// BS,CTRL+C,CTRL+V,CTRL+X
	if (nValidChar & VC_NUM)
		m_sValidStr += "0123456789";
	if (nValidChar & VC_MINUS)
		m_sValidStr += "-";
	if (nValidChar & VC_POINT)
		m_sValidStr += ".";
}

void CMyEdit::SetMaxLength(int nMaxLength)
{
	m_nMaxLength = nMaxLength;
}

void CMyEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (!m_sValidStr.IsEmpty()) {
		if (strchr(m_sValidStr, nChar) == NULL) {
			MessageBeep(0);
			return;
		}
	}

	if (m_nMaxLength != 0) {
		CString str1, str2;
		int nStartChar, nEndChar;

		GetWindowText(str1);
		GetSel(nStartChar, nEndChar);

		CEdit::OnChar(nChar, nRepCnt, nFlags);

		GetWindowText(str2);
		if (str2.GetLength() > m_nMaxLength) {
			SetWindowText(str1);
			SetSel(nStartChar, nEndChar);
			MessageBeep(0);
		}
	} else
		CEdit::OnChar(nChar, nRepCnt, nFlags);
}

void CMyEdit::SetNotifyWnd(HWND hNotifyWnd)
{
	m_hNotifyWnd = hNotifyWnd;
}

void CMyEdit::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (m_hNotifyWnd) {
		ClientToScreen(&point);
		::SendMessage(m_hNotifyWnd, WM_MYCTRL_RBUTTONDOWN, GetDlgCtrlID(), point.x | (point.y << 16));
	} else
		CEdit::OnRButtonDown(nFlags, point);
}

BOOL CMyEdit::IsEmpty()
{
	return GetWindowTextLength() == 0;
}

/////////////////////////////////////////////////////////////////////////////
// CMyTab

CMyTab::CMyTab()
{
	m_nFontSize = 0;
	m_nClickedTab = -1;
}

BEGIN_MESSAGE_MAP(CMyTab, CTabCtrl)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyTab メッセージ ハンドラ

int CMyTab::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTabCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_TabData.RemoveAll();

	return 0;
}

BOOL CMyTab::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult)
{
	NMHDR* pNMHDR = (NMHDR*)lParam;
	if (pNMHDR->code == TCN_SELCHANGE) {
		int	n;

		if ((n = GetCurSel()) != -1)
			DisplayChildDlg(n);
	}

	return CTabCtrl::OnChildNotify(message, wParam, lParam, pLResult);
}

int CMyTab::AddPage(CDialog *pDialog, UINT id, LPCTSTR title, BOOL bCreate, BOOL bSort)
{
	int n = (int)m_TabData.GetSize();
	if (bSort) {
		for (int i = 0; i < n; i++) {
			if (id < m_TabData[i].id) {
				n = i;
				break;
			}
		}
	}
	InsertItem(n, title);

	TabData tabData = {pDialog, id, FALSE};
	m_TabData.InsertAt(n, tabData);

	if (bCreate)
		CreateChildDlg(n);

	return n;
}

int CMyTab::AddPage(CDialog *pDialog, UINT id, UINT titleID, BOOL bCreate, BOOL bSort)
{
	CString str;

	str.LoadString(titleID);
	return AddPage(pDialog, id, (LPCSTR)str, bCreate, bSort);
}

void CMyTab::DeleteAllPages()
{
	for (int i = 0; i < m_TabData.GetSize(); i++) {
		if (m_TabData[i].pDialog != NULL) {
			if (m_TabData[i].pDialog->m_hWnd != NULL)
				m_TabData[i].pDialog->DestroyWindow();
		}
	}

	m_TabData.RemoveAll();
	DeleteAllItems();
}

void CMyTab::DeletePage(int n)
{
	if (m_TabData[n].pDialog != NULL) {
		if (m_TabData[n].pDialog->m_hWnd != NULL)
			m_TabData[n].pDialog->DestroyWindow();
	}

	m_TabData.RemoveAt(n);
	DeleteItem(n);
/*
	int nSize = m_TabData.GetSize();
	if (nSize > 0) {
		if (n >= nSize)
			n = 0;
		SetCurSel(n);
		//SetPage(n);
	}
*/
}

void CMyTab::SetPage(int n)
{
	if (n >= 0 && n < m_TabData.GetSize()) {
		SetCurSel(n);
		DisplayChildDlg(n);
	}
}

void CMyTab::DisplayChildDlg(int n)
{
	int nTab = (int)m_TabData.GetSize();

	for (int i = 0; i < nTab; i++) {
		if (m_TabData[i].pDialog != NULL) {
			if (n != i) {
				if (m_TabData[i].pDialog->m_hWnd != NULL)
					m_TabData[i].pDialog->ShowWindow(SW_HIDE);
			}
		}
	}

	if (m_TabData[n].pDialog != NULL) {
		if (m_TabData[n].pDialog->m_hWnd == NULL) {
			UpdateWindow();		// SW_HIDEした後が表示されるまで灰色に残るのを防ぐ
			CreateChildDlg(n);
		}

		if (m_TabData[n].pDialog->m_hWnd != NULL)
			m_TabData[n].pDialog->ShowWindow(SW_SHOW);
	}
}

void CMyTab::CreateChildDlg(int n)
{
	CRect rect1, rect2;
	CWnd *pParent = GetParent();
	CDialog *pDialog = m_TabData[n].pDialog;

	if (m_nFontSize == 0)
		pDialog->Create(m_TabData[n].id, pParent);
	else
		pDialog->CreateIndirect(GetDialogResource(m_TabData[n].id, m_nFontSize), pParent);

	// ダイアログの背景色をタブコントロールに合わせる
	g_dllUxTheme.EnableThemeDialogTexture(pDialog->m_hWnd, ETDT_ENABLETAB);

	// ダイアログをタブコントロール全体に広げる（タブとダイアログの背景色が微妙に違うので）
	GetWindowRect(rect1);
	AdjustRect(FALSE, rect1);
	pDialog->GetWindowRect(rect2);
	POINT point{
		.x = (rect1.Width() - rect2.Width()) / 2 - 1,
		.y = (rect1.Height() - rect2.Height()) / 2
	};
	pParent->ScreenToClient(rect1);
	pDialog->MoveWindow(rect1, FALSE);

	// ダイアログ内のコントロールを再配置
	CWnd *pWnd = pDialog->GetTopWindow();
	while (pWnd != NULL) {
		pWnd->GetWindowRect(rect1);
		rect1 += point;
		pDialog->ScreenToClient(rect1);
		pWnd->MoveWindow(rect1, FALSE);

		pWnd = pWnd->GetNextWindow(GW_HWNDNEXT);
	}

	pDialog->SendMessage(WM_TAB_INIT_DIALOG);
}

void CMyTab::NotifyChildDlg(WPARAM nCode)
{
	for (int i = 0; i < m_TabData.GetSize(); i++) {
		if (m_TabData[i].pDialog != NULL && m_TabData[i].pDialog->m_hWnd != NULL)
			m_TabData[i].pDialog->SendMessage(WM_TAB_NOTIFY, nCode);
	}
}

CDialog *CMyTab::GetCurrentDlg()
{
	int n = GetCurSel();
	if (n != -1)
		return m_TabData[n].pDialog;
	else
		return NULL;
}

void CMyTab::SetFontSize(int nFontSize)
{
	m_nFontSize = nFontSize;
}

void CMyTab::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect rect;

	CTabCtrl::OnLButtonDown(nFlags, point);

	for (int i = 0; i < GetItemCount(); i++) {
		GetItemRect(i, rect);

		if (rect.PtInRect(point)) {
			if (m_TabData[i].bDrag && GetCurSel() == i) {
				m_nClickedTab = i;
				m_pointClick = point;
				SetCapture();
			}
			break;
		}
	}
}

void CMyTab::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_nClickedTab >= 0){
		ReleaseCapture();
		m_nClickedTab = -1;
	}

	CTabCtrl::OnLButtonUp(nFlags, point);
}

void CMyTab::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_nClickedTab >= 0) {
		CRect rect;
		GetItemRect(m_nClickedTab, rect);

		if (!rect.PtInRect(point)) {
			CRect rect2;
			GetClientRect(rect2);
			CRectTracker cRectTracker(rect2, CRectTracker::hatchedBorder);

			m_nClickedTab = -1;
			ReleaseCapture();

			if (cRectTracker.Track(this, m_pointClick, FALSE, CWnd::GetDesktopWindow())) {
				ClientToScreen(cRectTracker.m_rect);
				GetParent()->SendMessage(WM_TAB_DRAG, cRectTracker.m_rect.left, cRectTracker.m_rect.top);
			}
		}
	}

	CTabCtrl::OnMouseMove(nFlags, point);
}

void CMyTab::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	int n = GetCurSel();
	if (n != -1) {
		CRect rect;
		GetItemRect(n, rect);
		if (rect.PtInRect(point))
			GetParent()->SendMessage(WM_TAB_DBLCLICK, n, 0);
	}

	CTabCtrl::OnLButtonDblClk(nFlags, point);
}

void CMyTab::SetDrag(int n, BOOL bDrag)
{
	m_TabData[n].bDrag = bDrag;
}

void CMyTab::SetGrayed(int n, BOOL bGray)
{
	SetItemState(n, ODS_GRAYED, bGray ? ODS_GRAYED : 0);
}

void CMyTab::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);

	char text[32];
	TCITEM ti{
		.mask = TCIF_TEXT | TCIF_STATE,
		.dwStateMask = TCIS_HIGHLIGHTED,
		.pszText = text,
		.cchTextMax = sizeof(text)
	};
	GetItem(lpDrawItemStruct->itemID, &ti);
	CString str = text;

	COLORREF color;
	if (ti.dwState & ODS_GRAYED)
		color = dc.SetTextColor(GetSysColor(COLOR_GRAYTEXT));
	else if (lpDrawItemStruct->itemState & ODS_SELECTED)
		color = dc.SetTextColor(RGB(0x00, 0x00, 0x99));
	else
		color = dc.SetTextColor(GetSysColor(COLOR_BTNTEXT));

	dc.FillSolidRect(&lpDrawItemStruct->rcItem, GetSysColor(COLOR_BTNFACE));
	CSize size = dc.GetOutputTextExtent(str);
	CRect rect(lpDrawItemStruct->rcItem);
	dc.TextOut(rect.left + (rect.Width() - size.cx) / 2, rect.top + 3, str);

	dc.SetTextColor(color);

	dc.Detach();
}

void CMyTab::GetCurrentTitle(CString &sTitle)
{
	int n = GetCurSel();
	if (n != -1) {
		TCITEM tci{
			.mask = TCIF_TEXT,
			.pszText = sTitle.GetBuffer(256),
			.cchTextMax = 256
		};
		GetItem(n, &tci);
		sTitle.ReleaseBuffer();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMySliderString

CMySliderString::~CMySliderString()
{
	RemoveAll();
}

BEGIN_MESSAGE_MAP(CMySliderString, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CMySliderString::OnPaint()
{
	CPaintDC dc(this);

	dc.SetBkMode(TRANSPARENT);
	CFont *pFont = dc.SelectObject(GetParent()->GetFont());

	CSize size;
	for (POSITION pos = m_StringList.GetHeadPosition(); pos != NULL; ) {
		CTicString *pTicString = (CTicString *)m_StringList.GetNext(pos);

		size = dc.GetTextExtent(pTicString->text);
		dc.TextOut(TS_MARGIN + pTicString->x - size.cx / 2 + 1, 0, pTicString->text);
	}

	dc.SelectObject(pFont);
}

void CMySliderString::AddTicString(int x, LPCTSTR pText)
{
	CTicString *pTicString = new CTicString;
	pTicString->x = x;
	pTicString->text = pText;
	m_StringList.AddTail(pTicString);

	Invalidate(FALSE);

	WINDOWPLACEMENT wp;
	GetWindowPlacement(&wp);
	GetParent()->InvalidateRect(&wp.rcNormalPosition);
}

void CMySliderString::RemoveAll()
{
	CTicString *pTicString;

	while (!m_StringList.IsEmpty()) {
		pTicString = (CTicString *)m_StringList.RemoveHead();
		delete pTicString;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMySliderCtrl

BEGIN_MESSAGE_MAP(CMySliderCtrl, CSliderCtrl)
END_MESSAGE_MAP()

void CMySliderCtrl::AddTicString(int nTic, LPCTSTR pStr)
{
	if (m_MySliderString.m_hWnd == NULL) {
		WINDOWPLACEMENT wp;
		GetWindowPlacement(&wp);

		LOGFONT logFont;
		GetParent()->GetFont()->GetLogFont(&logFont);
		int nTop = wp.rcNormalPosition.top + 25 + abs(logFont.lfHeight) / 2;
		int nBottom = nTop + abs(logFont.lfHeight);

		CRect rect;
		rect.SetRect(wp.rcNormalPosition.left - TS_MARGIN, nTop, wp.rcNormalPosition.right + TS_MARGIN, nBottom);
		m_MySliderString.Create(::AfxRegisterWndClass(0), "SliderString", WS_CHILD | WS_VISIBLE, rect, GetParent(), 0xffff);
	}

	m_MySliderString.AddTicString(GetTicPos(nTic), pStr);
}

void CMySliderCtrl::ClearTics(BOOL bRedraw)
{
	m_MySliderString.RemoveAll();
	CSliderCtrl::ClearTics(bRedraw);
}

void CMySliderCtrl::PreSubclassWindow()
{
	CRect rectWindow, rectThumb;

	GetWindowRect(rectWindow);
	GetThumbRect(rectThumb);
	SetWindowPos(NULL, 0, 0, rectWindow.Width(), rectThumb.bottom + 7, SWP_NOMOVE | SWP_NOZORDER);

	CSliderCtrl::PreSubclassWindow();
}

// XPスタイルでグリップが正常に表示されないバグの対処
//////////////////////////////////////////////////////
// CMyToolBar

BEGIN_MESSAGE_MAP(CMyToolBar, CToolBar)
	ON_WM_NCPAINT()
END_MESSAGE_MAP()

void CMyToolBar::OnNcPaint()
{
	EraseNonClient();
}
void CMyToolBar::EraseNonClient()
{
	// Get window DC that is clipped to the non-client area.
	CWindowDC dc(this);
	CRect rectClient;
	GetClientRect(rectClient);
	CRect rectWindow;
	GetWindowRect(rectWindow);
	ScreenToClient(rectWindow);
	rectClient.OffsetRect(-rectWindow.left, -rectWindow.top);
	dc.ExcludeClipRect(rectClient);

	// Draw the borders in the non-client area.
	rectWindow.OffsetRect(-rectWindow.left, -rectWindow.top);
	DrawBorders(&dc, rectWindow);

	// Erase the parts that are not drawn.
	dc.IntersectClipRect(rectWindow);
	SendMessage(WM_ERASEBKGND, (WPARAM)dc.m_hDC);

	// Draw the gripper in the non-client area.
	DrawGripper(&dc, rectWindow);
}

void CMyToolBar::DoPaint(CDC* pDC)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pDC);

	// Paint inside the client area.
	CRect rect;
	GetClientRect(rect);
	DrawBorders(pDC, rect);
	DrawGripper(pDC, rect);
}

void CMyToolBar::DrawGripper(CDC* pDC, const CRect& rect)
{
	pDC->FillSolidRect( &rect, ::GetSysColor(COLOR_BTNFACE)); // Fill in the background.
	CToolBar::DrawGripper(pDC,rect);
}
