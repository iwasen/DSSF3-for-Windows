// AcfWnd.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Sas.h"
#include "AcfWnd.h"

#define MOUSE_SELECT_NONE		0
#define MOUSE_SELECT_START		1
#define MOUSE_SELECT_END		2
#define MOUSE_SCROLL			3

/////////////////////////////////////////////////////////////////////////////
// CAcfWnd

CAcfWnd::CAcfWnd()
{
	m_bInitialize = FALSE;
	m_nMouseMode = MOUSE_SELECT_NONE;
	m_pCallBackFunc = NULL;
	m_hCursorArrow = NULL;
	m_bHScroll = FALSE;
}

BEGIN_MESSAGE_MAP(CAcfWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAcfWnd メッセージ ハンドラ

void CAcfWnd::OnPaint()
{
	CPaintDC dc(this); // 描画用のデバイス コンテキスト

	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
	dc.BitBlt(0, 0, m_nWidth, m_nHeight, &m_dcMem, 0, 0, SRCCOPY);
	m_dcMem.SelectObject(pBitmapOrg);
}

BOOL CAcfWnd::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void CAcfWnd::Initialize(int nFontSize, LPCTSTR pTitle, PFNGRAPH pCallBackFunc, LPARAM nCallBackData)
{
	if (m_bInitialize)
		return;

	m_bInitialize = TRUE;

	m_pCallBackFunc = pCallBackFunc;
	m_dwCallBackData = nCallBackData;

	RECT rect;
	GetClientRect(&rect);

	m_nWidth = rect.right;
	m_nHeight = rect.bottom;

	CDC *pDC = GetDC();
	m_dcMem.CreateCompatibleDC(pDC);
	m_bitmapMem.CreateCompatibleBitmap(pDC, m_nWidth, m_nHeight);
	ReleaseDC(pDC);

	m_penData.CreatePen(PS_SOLID, 0, RGB(0, 0, 255));
	m_penLine1.CreatePen(PS_DOT, 0, RGB(255, 0, 0));
	m_penLine2.CreatePen(PS_DOT, 0, RGB(0, 255, 0));
	m_penBlack.CreatePen(PS_SOLID, 0, RGB(0, 0, 0));
	m_penGray.CreatePen(PS_SOLID, 0, RGB(128, 128, 128));
	m_penLightGray.CreatePen(PS_SOLID, 0, RGB(192, 192, 192));

	m_brushWhite.CreateSolidBrush(RGB(255, 255, 255));

	CString str;
	LOGFONT font;
	memset(&font, 0, sizeof(font));
	font.lfHeight = CDpi::OriginalDpi(nFontSize, 88);
	font.lfCharSet = DEFAULT_CHARSET;
	str.LoadString(IDS_DEFAULTFONT);
	strcpy_s(font.lfFaceName, str);
	m_Font.CreatePointFontIndirect(&font, &m_dcMem);
	font.lfEscapement = 900;
	m_Font2.CreatePointFontIndirect(&font, &m_dcMem);

	m_dcMem.SetBkMode(TRANSPARENT);
	m_dcMem.SetTextColor(RGB(64, 64, 64));

	CFont *pFontOrg = m_dcMem.SelectObject(&m_Font);
	TEXTMETRIC tm;
	m_dcMem.GetTextMetrics(&tm);
	m_dcMem.SelectObject(pFontOrg);

	m_nScaleLeft = tm.tmAveCharWidth * 4 + tm.tmHeight + 5;
	m_nScaleTop = 10;
	m_nScaleRight = m_nWidth - 0;
	m_nScaleBottom = m_nHeight - ((tm.tmHeight + 1) * 2 + 1);
	m_nScaleWidth = m_nScaleRight - m_nScaleLeft;
	m_nScaleHeight = m_nScaleBottom - m_nScaleTop;

	m_Rgn.CreateRectRgn(m_nScaleLeft, m_nScaleTop, m_nScaleRight, m_nScaleBottom);

	m_sTitle = pTitle;
	m_wndRemark.Create(this);

	CWinApp *pWinApp = ::AfxGetApp();
	m_hCursorArrow = pWinApp->LoadStandardCursor(IDC_ARROW);
	m_hCursorHScroll = pWinApp->LoadStandardCursor(IDC_SIZEWE);
}

void CAcfWnd::SetBitmap(double startTime, double dispTime, int nMaxLevel, int nMinLevel, int nStep, int nDiv, LPCTSTR pStr)
{
	CString sText;
	CSize size;
	int x, y;
	int ns;
	int nLevelRange = nMaxLevel - nMinLevel;

	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
	CPen *pPenOrg = m_dcMem.SelectObject(&m_penBlack);
	CFont *pFontOrg = m_dcMem.SelectObject(&m_Font);

	m_nScaleZero = m_nScaleTop + m_nScaleHeight * nMaxLevel / nLevelRange;

	CRect rect;
	rect.SetRect(0, 0, m_nWidth, m_nHeight);
	m_dcMem.FillRect(rect, &m_brushWhite);

	m_dcMem.MoveTo(m_nScaleLeft - 1, m_nScaleTop);
	m_dcMem.LineTo(m_nScaleLeft - 1, m_nScaleBottom);

	for (int i = nMaxLevel; i >= nMinLevel; i -= nStep) {
		y = m_nScaleZero - (i * m_nScaleHeight / nLevelRange);
		if (i == 0)
			m_dcMem.SelectObject(&m_penBlack);
		else if (i == nMinLevel)
			m_dcMem.SelectObject(&m_penGray);
		else
			m_dcMem.SelectObject(&m_penLightGray);

		m_dcMem.MoveTo(m_nScaleLeft, y);
		m_dcMem.LineTo(m_nScaleRight, y);

		sText.Format("%g", (double)i / nDiv);
		size = m_dcMem.GetOutputTextExtent(sText);
		m_dcMem.TextOut(m_nScaleLeft - size.cx - 2, y - size.cy / 2, sText);
	}

	double t1 = dispTime / 7;
	double step = pow(10.0, floor(log10(t1)));
	double t2 = t1 / step;
	if (t2 < 2) {
		step *= 0.5;
		ns = 2;
	} else if (t2 < 5)
		ns = 2;
	else
		ns = 5;

	for (double t = floor(startTime / step) * step; t < startTime + dispTime; t += step) {
		x = m_nScaleLeft + int((t - startTime) * m_nScaleWidth / dispTime);
		if (x >= m_nScaleLeft) {
			if (int(t / step + 0.5) % ns == 0) {
				if (x > m_nScaleLeft) {
					m_dcMem.SelectObject(&m_penGray);
					m_dcMem.MoveTo(x, m_nScaleTop + 1);
					m_dcMem.LineTo(x, m_nScaleBottom - 1);
				}
				sText.Format("%g", t * 1000);
				size = m_dcMem.GetOutputTextExtent(sText);
				m_dcMem.TextOut(x - size.cx / 2, m_nScaleBottom + 1, sText);
			} else {
				if (x > m_nScaleLeft) {
					m_dcMem.SelectObject(&m_penLightGray);
					m_dcMem.MoveTo(x, m_nScaleTop + 1);
					m_dcMem.LineTo(x, m_nScaleBottom - 1);
				}
			}
		}
	}

	sText.LoadString(IDS_TIME);
	sText += " [ms]";
	size = m_dcMem.GetOutputTextExtent(sText);
	m_dcMem.TextOut(m_nScaleLeft + (m_nScaleWidth - size.cx) / 2, m_nHeight - (size.cy + 1), sText);

	m_dcMem.SelectObject(&m_Font2);
	size = m_dcMem.GetOutputTextExtent(pStr);
	m_dcMem.TextOut(1, m_nScaleTop + (m_nScaleHeight + size.cx) / 2, pStr);

	m_dcMem.SelectObject(pBitmapOrg);
	m_dcMem.SelectObject(pPenOrg);
	m_dcMem.SelectObject(pFontOrg);
}

void CAcfWnd::DispGraph1(double totalTime, double startTime, double dispTime, const double *pData, int nData, double fTau1, double fPhi1, BOOL bHScroll)
{
	m_bHScroll = bHScroll;

	CString str;
	str.LoadString(IDS_PHI_TAU);
	SetBitmap(startTime, dispTime, 10, -10, 5, 10, str);

	m_dcMem.SelectClipRgn(&m_Rgn);
	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
	CPen *pPenOrg = m_dcMem.SelectObject(&m_penData);

	double fOffset = startTime / totalTime * nData;
	int i2 = int(fOffset);

	int x, y;
	for (int i = i2; i < nData - 1; i++) {
		x = m_nScaleLeft + int((i - fOffset) * totalTime / dispTime * m_nScaleWidth / nData + 0.5);
		if (x >= m_nScaleRight)
			break;

		y = m_nScaleZero - (int)(pData[i] * m_nScaleHeight / 2 + 0.5);
		if (i == i2)
			m_dcMem.MoveTo(x, y);
		else
			m_dcMem.LineTo(x, y);
	}

	if (fTau1 != 0) {
		m_dcMem.SelectObject(&m_penLine2);
		x = m_nScaleLeft + (int)((fTau1 - startTime) / dispTime * m_nScaleWidth);
		m_dcMem.MoveTo(x, m_nScaleTop);
		m_dcMem.LineTo(x, m_nScaleBottom);
	}

	if (fPhi1 != 0) {
		m_dcMem.SelectObject(&m_penLine1);
		y = m_nScaleZero - (int)(fPhi1 * m_nScaleHeight / 2);
		m_dcMem.MoveTo(m_nScaleLeft, y);
		m_dcMem.LineTo(m_nScaleRight, y);
	}

	m_dcMem.SelectObject(pBitmapOrg);
	m_dcMem.SelectObject(pPenOrg);

	m_dcMem.SelectClipRgn(NULL);

	if (!m_sTitle.IsEmpty()) {
		CString str1, str2;
		REMARK_INFO remark;
		int n = 0;

		remark.remarks[n].pen = &m_penData;
		remark.remarks[n++].text = m_sTitle;

		remark.remarks[n].pen = &m_penLine1;
		str1.LoadString(IDS_PHI1);
		remark.remarks[n++].text = str1;

		remark.remarks[n].pen = &m_penLine2;
		str2.LoadString(IDS_TAU1);
		remark.remarks[n++].text = str2;

		remark.nRemark = n;
		m_wndRemark.DispRemarks(&remark);
	}

	Invalidate(FALSE);
	UpdateWindow();
}

void CAcfWnd::DispGraph2(double totalTime, double startTime, double dispTime, const double *pData, int nData, double fTauE, double fTauE0, int nMaxLevel, int nMinLevel, BOOL bHScroll)
{
	int nLevelRange = nMaxLevel - nMinLevel;

	m_bHScroll = bHScroll;

	CString str;
	str.LoadString(IDS_PHI_TAU);
	SetBitmap(startTime, dispTime, nMaxLevel, nMinLevel, 5, 1, "log |" + str + "| [dB]");

	m_dcMem.SelectClipRgn(&m_Rgn);
	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
	CPen *pPenOrg = m_dcMem.SelectObject(&m_penData);

	int i2 = int(startTime / totalTime * nData);
	int x, y;
	for (int i = i2; i < nData - 1; i++) {
		x = m_nScaleLeft + int((i - i2) * totalTime / dispTime * m_nScaleWidth / nData + 0.5);
		if (x >= m_nScaleRight)
			break;

		y = m_nScaleZero - (int)(pData[i] * m_nScaleHeight / nLevelRange + 0.5);
		if (i == i2)
			m_dcMem.MoveTo(x, y);
		else
			m_dcMem.LineTo(x, y);
	}

	if (fTauE != 0) {
		double dev = (-10 - fTauE0) / fTauE;
		m_dcMem.SelectObject(&m_penLine1);
		x = m_nScaleLeft + (int)(-startTime / dispTime * m_nScaleWidth);
		y = m_nScaleZero - (int)(fTauE0 * m_nScaleHeight / nLevelRange + 0.5);
		m_dcMem.MoveTo(x, y);
		x = m_nScaleLeft + (int)(((nMinLevel - fTauE0) / dev - startTime) / dispTime * m_nScaleWidth);
		y = m_nScaleBottom;
		m_dcMem.LineTo(x, y);

		m_dcMem.SelectObject(&m_penLine2);
		x = m_nScaleLeft + (int)((fTauE - startTime) / dispTime * m_nScaleWidth);
		m_dcMem.MoveTo(x, m_nScaleTop);
		m_dcMem.LineTo(x, m_nScaleBottom);
	}

	m_dcMem.SelectObject(pBitmapOrg);
	m_dcMem.SelectObject(pPenOrg);

	m_dcMem.SelectClipRgn(NULL);

	if (!m_sTitle.IsEmpty()) {
		CString str1, str2;
		REMARK_INFO remark;
		int n = 0;

		remark.remarks[n].pen = &m_penData;
		remark.remarks[n++].text = m_sTitle;

		remark.remarks[n].pen = &m_penLine1;
		str1.LoadString(IDS_REGRESSIONLINE);
		remark.remarks[n++].text = str1;

		remark.remarks[n].pen = &m_penLine2;
		str2.LoadString(IDS_TAUE);
		remark.remarks[n++].text = str2;

		remark.nRemark = n;
		m_wndRemark.DispRemarks(&remark);
	}

	Invalidate(FALSE);
	UpdateWindow();
}

void CAcfWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_nMouseMode == MOUSE_SELECT_NONE) {
		m_nScrollPos = point.x;
		m_nMouseMode = MOUSE_SCROLL;
		SetCapture();
	}

	SetMouseCursor();

	CWnd::OnLButtonDown(nFlags, point);
}

void CAcfWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_nMouseMode != MOUSE_SELECT_NONE) {
		m_nMouseMode = MOUSE_SELECT_NONE;
		ReleaseCapture();

		SetMouseCursor();
	}

	CWnd::OnLButtonUp(nFlags, point);
}

void CAcfWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	switch (m_nMouseMode) {
	case MOUSE_SCROLL:
		m_pCallBackFunc(CC_SCROLL, m_dwCallBackData, (double)(point.x - m_nScrollPos) / m_nScaleWidth, 0);
		m_nScrollPos = point.x;
		break;
	}

	SetMouseCursor();

	CWnd::OnMouseMove(nFlags, point);
}

BOOL CAcfWnd::OnSetCursor(CWnd* /*pWnd*/, UINT /*nHitTest*/, UINT /*message*/)
{
	return TRUE;
}

void CAcfWnd::SetMouseCursor()
{
	HCURSOR hCursor;

	switch (m_nMouseMode) {
	case MOUSE_SCROLL:
		if (m_bHScroll)
			hCursor = m_hCursorHScroll;
		else
			hCursor = m_hCursorArrow;
		break;
	default:
		hCursor = m_hCursorArrow;
		break;
	}

	::SetCursor(hCursor);
}
