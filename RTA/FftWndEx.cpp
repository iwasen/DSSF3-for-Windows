// FftWndEx.cpp : 実装ファイル
//

#include "stdafx.h"
#include "RTA.h"
#include "FftWndEx.h"

static CString s_sTab2("\t\t");

// CFftWndEx

CFftWndEx::CFftWndEx(CFftWnd *pFftWnd, int nMode)
{
	m_pFftWnd = pFftWnd;
	m_nMode = nMode;
	m_bLButtonDown = FALSE;
}

BEGIN_MESSAGE_MAP(CFftWndEx, CWnd)
	ON_MESSAGE(WM_FREQ_LEVEL_LEFT, OnFreqLevelLeft)
	ON_MESSAGE(WM_FREQ_LEVEL_RIGHT, OnFreqLevelRight)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_MOVE()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_THEMECHANGED()
END_MESSAGE_MAP()

// CFftWndEx メッセージ ハンドラ

int CFftWndEx::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_wndStatusBar.Create(WS_CHILD | CCS_BOTTOM | SBARS_SIZEGRIP, CRect(0,0,0,0), this, AFX_IDW_STATUS_BAR);
	SetStatusPane();

	InitStatusPane();

	m_wndStatusBar.ShowWindow(SW_SHOW);

	return 0;
}

BOOL CFftWndEx::OnEraseBkgnd(CDC* pDC)
{
	m_pFftWnd->EraseBkgnd(pDC, this);

	return TRUE;
}

void CFftWndEx::OnPaint()
{
	m_pFftWnd->Paint(this);
}

void CFftWndEx::PostNcDestroy()
{
	CWnd::PostNcDestroy();

	delete this;
}

void CFftWndEx::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	m_wndStatusBar.SendMessage(WM_SIZE);
	SetStatusPane();

	WINDOWINFO wi;
	GetWindowInfo(&wi);
	if (wi.dwStyle & WS_VISIBLE) {
		m_pFftWnd->SetBitmap(this);
		Invalidate();
	}

	SaveWindowPos();
}

void CFftWndEx::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// ウィンドウの最小サイズを設定
	lpMMI->ptMinTrackSize.x = 350;
	lpMMI->ptMinTrackSize.y = 250;

	CWnd::OnGetMinMaxInfo(lpMMI);
}

void CFftWndEx::OnMove(int x, int y)
{
	CWnd::OnMove(x, y);

	SaveWindowPos();
}

LRESULT CFftWndEx::OnFreqLevelLeft(WPARAM wParam, LPARAM lParam)
{
	m_wndStatusBar.SetText(s_sTab2 + (LPCSTR)wParam, 1, m_nBorderType);
	m_wndStatusBar.SetText(s_sTab2 + (LPCSTR)lParam, 3, m_nBorderType);

	return 0;
}

LRESULT CFftWndEx::OnFreqLevelRight(WPARAM wParam, LPARAM lParam)
{
	m_wndStatusBar.SetText(s_sTab2 + (LPCSTR)wParam, 7, m_nBorderType);
	m_wndStatusBar.SetText(s_sTab2 + (LPCSTR)lParam, 9, m_nBorderType);

	return 0;
}

void CFftWndEx::InitStatusPane()
{
	CString sUnitX, sUnitY;
	GetAxisUnit(m_nMode, sUnitX, sUnitY);

	m_nBorderType = g_dllUxTheme.IsThemeActive() ? SBT_NOBORDERS : 0;

	if (m_nMode == FFT_MODE_PHS || m_nMode == FFT_MODE_CRS || m_nMode == FFT_MODE_COH) {
		m_wndStatusBar.SetText("", 0, SBT_NOBORDERS);
		m_wndStatusBar.SetText("", 1, m_nBorderType);
		m_wndStatusBar.SetText(sUnitX, 2, SBT_NOBORDERS);
		m_wndStatusBar.SetText("", 3, m_nBorderType);
		m_wndStatusBar.SetText(sUnitY, 4, SBT_NOBORDERS);
	} else {
		m_wndStatusBar.SetText("\t\tL", 0, SBT_NOBORDERS);
		m_wndStatusBar.SetText("", 1, m_nBorderType);
		m_wndStatusBar.SetText(sUnitX, 2, SBT_NOBORDERS);
		m_wndStatusBar.SetText("", 3, m_nBorderType);
		m_wndStatusBar.SetText(sUnitY, 4, SBT_NOBORDERS);
		m_wndStatusBar.SetText("", 5, SBT_NOBORDERS);
		m_wndStatusBar.SetText("\t\tR", 6, SBT_NOBORDERS);
		m_wndStatusBar.SetText("", 7, m_nBorderType);
		m_wndStatusBar.SetText(sUnitX, 8, SBT_NOBORDERS);
		m_wndStatusBar.SetText("", 9, m_nBorderType);
		m_wndStatusBar.SetText(sUnitY, 10, SBT_NOBORDERS);
	}
}

void CFftWndEx::SetStatusPane()
{
	static const int widthsLR[] = { 15, 50, 25, 50, 25, -1, 15, 50, 25, 50, 25};
	static const int widths[] = { 15, 50, 25, 50, 25 };
	int parts[sizeof(widthsLR) / sizeof(int)];
	CRect rect;
	int size;
	const int *pWidth;
	int nWidth;

	if (m_nMode == FFT_MODE_PHS || m_nMode == FFT_MODE_CRS || m_nMode == FFT_MODE_COH) {
		pWidth = widths;
		nWidth = sizeof(widths) / sizeof(int);
	} else {
		pWidth = widthsLR;
		nWidth = sizeof(widthsLR) / sizeof(int);
	}

	int pos = 0;
	for (int i = 0; i < nWidth; i++) {
		size = pWidth[i];
		if (size == -1) {
			m_wndStatusBar.GetClientRect(rect);
			size = rect.Width() - pos - 20;
			for (int j = i + 1; j < nWidth; j++)
				size -= pWidth[j];
			if (size < 0)
				size = 0;
		}

		pos += size;
		parts[i] = pos;
	}
	m_wndStatusBar.SetParts(nWidth, parts);
}

void CFftWndEx::SaveWindowPos()
{
	GetWindowRect(&g_oSetData.Fft.rectFftWindow[m_nMode]);

	CRect rect;
	m_wndStatusBar.GetWindowRect(rect);
	g_oSetData.Fft.rectFftWindow[m_nMode].bottom -= rect.Height();
}

void CFftWndEx::OnDestroy()
{
	CWnd::OnDestroy();

	m_pFftWnd->CloseFftWindow(this, m_nMode);
}

void CFftWndEx::OnLButtonDown(UINT nFlags, CPoint point)
{
	FFTWINDOW *pFftWindow = m_pFftWnd->GetFftWindow(this);

	m_pFftWnd->SetDispFreq(pFftWindow, point);

	m_bLButtonDown = TRUE;
	SetCapture();

	CWnd::OnLButtonDown(nFlags, point);
}

void CFftWndEx::OnRButtonDown(UINT nFlags, CPoint point)
{
	FFTWINDOW *pFftWindow = m_pFftWnd->GetFftWindow(this);

	m_pFftWnd->ResetDispFreq(pFftWindow);

	CWnd::OnRButtonDown(nFlags, point);
}

void CFftWndEx::OnMouseMove(UINT nFlags, CPoint point)
{
	FFTWINDOW *pFftWindow = m_pFftWnd->GetFftWindow(this);

	if (m_bLButtonDown) {
		if (nFlags & MK_LBUTTON)
			m_pFftWnd->SetDispFreq(pFftWindow, point);
	}

	CWnd::OnMouseMove(nFlags, point);
}

void CFftWndEx::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bLButtonDown = FALSE;
	ReleaseCapture();

	CWnd::OnLButtonUp(nFlags, point);
}

LRESULT CFftWndEx::OnThemeChanged()
{
	InitStatusPane();

	return 1;
}
