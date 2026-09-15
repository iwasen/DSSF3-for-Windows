// TimeWnd.cpp : 実装ファイル
//

#include "stdafx.h"
#include "Rta.h"
#include "TimeWnd.h"

#define TIMER_UPDATE 1
#define XMARGIN		4
#define YMARGIN		2

typedef DWORD (WINAPI *FWINLAYER)(HWND hwnd,DWORD crKey,BYTE bAlpha,DWORD dwFlags);

// CTimeWnd

CTimeWnd::CTimeWnd()
{
	m_bDisp = FALSE;
	m_bMoving = FALSE;
	bLayered = FALSE;

	Create();
}

BEGIN_MESSAGE_MAP(CTimeWnd, CWnd)
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(IDM_CLOSE, OnClose)
	ON_COMMAND(IDM_SIZE_8_POINT, OnSize8Point)
	ON_COMMAND(IDM_SIZE_9_POINT, OnSize9Point)
	ON_COMMAND(IDM_SIZE_10_POINT, OnSize10Point)
	ON_COMMAND(IDM_SIZE_11_POINT, OnSize11Point)
	ON_COMMAND(IDM_SIZE_12_POINT, OnSize12Point)
	ON_COMMAND(IDM_FORMAT_DATE, OnFormatDate)
	ON_COMMAND(IDM_FORMAT_TIME, OnFormatTime)
	ON_COMMAND(IDM_FORMAT_DATE_TIME, OnFormatDateTime)
	ON_COMMAND(IDM_TRANSPARENCY_0, OnTransparency0)
	ON_COMMAND(IDM_TRANSPARENCY_1, OnTransparency1)
	ON_COMMAND(IDM_TRANSPARENCY_2, OnTransparency2)
	ON_COMMAND(IDM_TRANSPARENCY_3, OnTransparency3)
END_MESSAGE_MAP()

// CTimeWnd メッセージ ハンドラ

void CTimeWnd::Create()
{
	if (!CreateEx(WS_EX_DLGMODALFRAME | WS_EX_LAYERED | WS_EX_TOPMOST, ::AfxRegisterWndClass(0), NULL, WS_POPUP, 0, 0, 0, 0, ::AfxGetMainWnd()->m_hWnd, NULL))
		return;

	CDC *pDC = GetDC();
	m_dcMem.CreateCompatibleDC(pDC);
	m_dcMem.SetBkMode(TRANSPARENT);
	ReleaseDC(pDC);

	Init(g_oSetData.Tim.nPosLeft, g_oSetData.Tim.nPosTop);

	CreateBitmap();

	SetTimer(TIMER_UPDATE, 1000, NULL);

	return;
}

void CTimeWnd::Init(int x, int y)
{
	CreateFont();

	GetDateFormat();

	GetBitmapSize();

	CDC *pDC = GetDC();
	m_bitmapMem.DeleteObject();
	m_bitmapMem.CreateCompatibleBitmap(pDC, m_nWidth, m_nHeight);
	ReleaseDC(pDC);

	CRect rect(0, 0, m_nWidth, m_nHeight);
	CalcWindowRect(rect);
	UINT nFlags = SWP_NOZORDER | SWP_SHOWWINDOW;
	if (x < 0 || y < 0)
		nFlags |= SWP_NOMOVE;
	SetWindowPos(NULL, x, y, rect.Width(), rect.Height(), nFlags);

	HMODULE hDll = LoadLibrary("user32.dll");
	FWINLAYER pfWin = (FWINLAYER)GetProcAddress(hDll, "SetLayeredWindowAttributes");
	if (pfWin != NULL) {
		pfWin(m_hWnd, 0, (BYTE)(255 - g_oSetData.Tim.nTransparency * 255 / 100), LWA_ALPHA);
		bLayered = TRUE;
	}
	FreeLibrary(hDll);
}

void CTimeWnd::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIMER_UPDATE) {
		CreateBitmap();
		Invalidate(FALSE);
	}

	CWnd::OnTimer(nIDEvent);
}

void CTimeWnd::OnPaint()
{
	CPaintDC dc(this);

	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
	dc.BitBlt(0, 0, m_nWidth, m_nHeight, &m_dcMem, 0, 0, SRCCOPY);
	m_dcMem.SelectObject(pBitmapOrg);
}

BOOL CTimeWnd::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void CTimeWnd::OnDestroy()
{
	CRect rect;
	GetWindowRect(rect);
	g_oSetData.Tim.nPosLeft = rect.left;
	g_oSetData.Tim.nPosTop = rect.top;

	CWnd::OnDestroy();
}

void CTimeWnd::CreateFont()
{
	CString str;

	str.LoadString(IDS_DEFAULTFONT);
	m_Font.DeleteObject();
	m_Font.CreatePointFont(g_oSetData.Tim.nFontSize, str, NULL);
}

void CTimeWnd::GetDateFormat()
{
	switch (g_oSetData.Tim.nFormat) {
	case 0:
		m_sDispFormat.LoadString(IDS_DATE_FORMAT);
		break;
	case 1:
		m_sDispFormat.LoadString(IDS_TIME_FORMAT);
		break;
	case 2:
		CString date, time;
		date.LoadString(IDS_DATE_FORMAT);
		time.LoadString(IDS_TIME_FORMAT);
		m_sDispFormat = date + " " + time;
		break;
	}
}

CString CTimeWnd::GetTimeStr()
{
	return CTime::GetCurrentTime().Format(m_sDispFormat);
}

void CTimeWnd::GetBitmapSize()
{
	CFont *pFontOrg = m_dcMem.SelectObject(&m_Font);

	CString time = GetTimeStr();
	CSize size = m_dcMem.GetOutputTextExtent(time);

	m_nWidth = size.cx + XMARGIN * 2;
	m_nHeight = size.cy + YMARGIN * 2;

	m_dcMem.SelectObject(pFontOrg);
}

void CTimeWnd::CreateBitmap()
{
	CFont *pFontOrg = m_dcMem.SelectObject(&m_Font);
	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);

	CString time = GetTimeStr();

	CBrush brush;
	brush.CreateSolidBrush(RGB(255, 255, 225));
	CRect rect(0, 0, m_nWidth, m_nHeight);
	m_dcMem.FillRect(rect, &brush);

	m_dcMem.TextOut(XMARGIN, YMARGIN, time);

	m_dcMem.SelectObject(pFontOrg);
	m_dcMem.SelectObject(pBitmapOrg);
}

void CTimeWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	WINDOWPLACEMENT wpl;

	GetWindowPlacement(&wpl);
	m_Rect = wpl.rcNormalPosition;
	m_Point = point;

	m_bMoving = TRUE;
	SetCapture();

	CWnd::OnLButtonDown(nFlags, point);
}

void CTimeWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bMoving && point != m_Point) {
		m_Rect += point - m_Point;
		SetWindowPos(NULL, m_Rect.left, m_Rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		UpdateWindow();
	}

	CWnd::OnMouseMove(nFlags, point);
}

void CTimeWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bMoving = FALSE;
	ReleaseCapture();

	CWnd::OnLButtonUp(nFlags, point);
}

void CTimeWnd::PostNcDestroy()
{
	((CRtaApp *)::AfxGetApp())->CloseFuncWindow(this);

	CWnd::PostNcDestroy();

	delete this;
}

void CTimeWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
	POINT posScreen;

	::GetCursorPos(&posScreen);

	CMenu menu;
	CMenu *pSubMenu;
	menu.LoadMenu(IDR_TIME_MENU);
	CMenu* pPopup = menu.GetSubMenu(0);

	pSubMenu = pPopup->GetSubMenu(2);
	pSubMenu->CheckMenuItem(g_oSetData.Tim.nFontSize / 10 - 8, MF_BYPOSITION | MF_CHECKED);

	pSubMenu = pPopup->GetSubMenu(3);
	pSubMenu->CheckMenuItem(g_oSetData.Tim.nFormat, MF_BYPOSITION | MF_CHECKED);

	if (bLayered) {
		pSubMenu = pPopup->GetSubMenu(4);
		pSubMenu->CheckMenuItem(g_oSetData.Tim.nTransparency / 20, MF_BYPOSITION | MF_CHECKED);
	} else
		pPopup->EnableMenuItem(4, MF_BYPOSITION | MF_GRAYED);

	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, posScreen.x, posScreen.y, this);

	CWnd::OnRButtonDown(nFlags, point);
}

void CTimeWnd::OnClose()
{
	DestroyWindow();
}

void CTimeWnd::ChangeFontSize(int nFontSize)
{
	g_oSetData.Tim.nFontSize = nFontSize;
	Init();
	CreateBitmap();
}

void CTimeWnd::OnSize8Point()
{
	ChangeFontSize(80);
}

void CTimeWnd::OnSize9Point()
{
	ChangeFontSize(90);
}

void CTimeWnd::OnSize10Point()
{
	ChangeFontSize(100);
}

void CTimeWnd::OnSize11Point()
{
	ChangeFontSize(110);
}

void CTimeWnd::OnSize12Point()
{
	ChangeFontSize(120);
}

void CTimeWnd::ChangeFormat(int nFormat)
{
	g_oSetData.Tim.nFormat = nFormat;
	GetDateFormat();
	Init();
	CreateBitmap();
}

void CTimeWnd::OnFormatDate()
{
	ChangeFormat(0);
}

void CTimeWnd::OnFormatTime()
{
	ChangeFormat(1);
}

void CTimeWnd::OnFormatDateTime()
{
	ChangeFormat(2);
}

void CTimeWnd::ChangeTransparency(int nTransparency)
{
	g_oSetData.Tim.nTransparency = nTransparency;
	Init();
	CreateBitmap();
}

void CTimeWnd::OnTransparency0()
{
	ChangeTransparency(0);
}

void CTimeWnd::OnTransparency1()
{
	ChangeTransparency(20);
}

void CTimeWnd::OnTransparency2()
{
	ChangeTransparency(40);
}

void CTimeWnd::OnTransparency3()
{
	ChangeTransparency(60);
}
