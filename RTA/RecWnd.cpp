// RecWnd.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Rta.h"
#include "RecWnd.h"

#define MOUSE_SELECT_NONE		0
#define MOUSE_SELECT_START		1
#define MOUSE_SELECT_END		2
#define MOUSE_SCROLL			3

//#define MIN_SELECT_TIME		0.001

/////////////////////////////////////////////////////////////////////////////
// CRecWnd

CRecWnd::CRecWnd()
{
	m_bInitialized = FALSE;
	m_fStartSelectTime = -1;
	m_fEndSelectTime = -1;
	m_nMouseMode = MOUSE_SELECT_NONE;
	m_pCallBackFunc = NULL;
	m_hCursorArrow = NULL;
	m_hCursorScroll = NULL;
	m_hCursorHMove = NULL;
	m_pPointBuf = NULL;
	m_fReplayTime = -1;
}

CRecWnd::~CRecWnd()
{
	if (m_pPointBuf != NULL)
		delete [] m_pPointBuf;
}

BEGIN_MESSAGE_MAP(CRecWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRecWnd メッセージ ハンドラ

void CRecWnd::Initialize(int nFontSize, PFNGRAPH pCallBackFunc, LPARAM nCallBackData, BOOL bRemark)
{
	if (m_bInitialized)
		return;

	m_bInitialized = TRUE;

	m_pCallBackFunc = pCallBackFunc;
	m_dwCallBackData = nCallBackData;

	CDC *pDC = GetDC();
	m_dcMem.CreateCompatibleDC(pDC);
	ReleaseDC(pDC);

	m_penLeft.CreatePen(PS_SOLID, 0, COLOR_LEFT);
	m_penRight.CreatePen(PS_SOLID, 0, COLOR_RIGHT);
	m_penBlack.CreatePen(PS_SOLID, 0, RGB(0, 0, 0));
	m_penGray.CreatePen(PS_SOLID, 0, RGB(128, 128, 128));
	m_penLightGray.CreatePen(PS_SOLID, 0, RGB(192, 192, 192));
	m_penSelectBar.CreatePen(PS_SOLID, 0, RGB(0, 0, 0));
	m_penReplayBar.CreatePen(PS_SOLID, 0, RGB(0, 128, 0));

	m_brushWhite.CreateSolidBrush(RGB(255, 255, 255));
	m_brushSelectArea.CreateSolidBrush(RGB(192, 255, 255));

	CString str;
	LOGFONT font{
		.lfHeight = CDpi::OriginalDpi(nFontSize, 88),
		.lfCharSet = DEFAULT_CHARSET
	};
	str.LoadString(IDS_DEFAULTFONT);
	strcpy_s(font.lfFaceName, str);
	m_Font.CreatePointFontIndirect(&font, &m_dcMem);
	font.lfEscapement = 900;
	m_Font2.CreatePointFontIndirect(&font, &m_dcMem);

	m_dcMem.SetBkMode(TRANSPARENT);
	m_dcMem.SetTextColor(RGB(64, 64, 64));

	if (bRemark) {
		str.LoadString(IDS_INPUTWAVE);
		REMARK_INFO remark{
			.nRemark = 1,
			.remarks{
				{
					.pen = NULL,
					.text = str
				}
			}
		};
		m_wndRemark.Create(this);
		m_wndRemark.DispRemarks(&remark);
	}

	CWinApp *pWinApp = ::AfxGetApp();
	m_hCursorArrow = pWinApp->LoadStandardCursor(IDC_ARROW);
	m_hCursorScroll = pWinApp->LoadStandardCursor(IDC_SIZEWE);
	m_hCursorHMove = pWinApp->LoadCursor(IDC_HMOVE_CUR);

	Initialize2();
}

void CRecWnd::Initialize2()
{
	RECT rect;
	GetClientRect(&rect);

	m_nWidth = rect.right;
	m_nHeight = rect.bottom;

	CDC *pDC = GetDC();
	m_bitmapMem.CreateCompatibleBitmap(pDC, m_nWidth, m_nHeight);
	ReleaseDC(pDC);

	CFont *pFontOrg = m_dcMem.SelectObject(&m_Font);
	TEXTMETRIC tm;
	m_dcMem.GetTextMetrics(&tm);
	m_dcMem.SelectObject(pFontOrg);

	m_nScaleLeft = tm.tmAveCharWidth * 4 + tm.tmHeight + 6;
	m_nScaleTop = 10;
	m_nScaleRight = m_nWidth - 1;
	m_nScaleBottom = ((m_nHeight - ((tm.tmHeight + 1) * 2 + 2)) & ~1) + 1;
	m_nScaleWidth = m_nScaleRight - m_nScaleLeft;
	m_nScaleHeight = m_nScaleBottom - m_nScaleTop;
	m_nScaleCenter = m_nScaleTop + m_nScaleHeight / 2;

	m_Rgn.CreateRectRgn(m_nScaleLeft, m_nScaleTop, m_nScaleRight + 1, m_nScaleBottom);

	m_pPointBuf = new POINT[m_nScaleWidth * 2 + 2];
}

void CRecWnd::Uninitialize2()
{
	m_bitmapMem.DeleteObject();
	m_Rgn.DeleteObject();

	if (m_pPointBuf != NULL) {
		delete [] m_pPointBuf;
		m_pPointBuf = NULL;
	}
}

void CRecWnd::Resize()
{
	Uninitialize2();
	Initialize2();
}

void CRecWnd::SetBitmap(double startTime, double dispTime)
{
	CString sText;
	CSize size;
	int x, y;
	int ns;

	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
	CPen *pPenOrg = m_dcMem.SelectObject(&m_penBlack);
	CFont *pFontOrg = m_dcMem.SelectObject(&m_Font);

	CRect rect(0, 0, m_nWidth, m_nHeight);
	m_dcMem.FillRect(rect, &m_brushWhite);

	for (int i = 100; i >= -100; i -= 20) {
		y = m_nScaleCenter - (i * (m_nScaleHeight / 2) / 100);
		if (i == 0 || i == 100 || i == -100)
			m_dcMem.SelectObject(&m_penBlack);
		else
			m_dcMem.SelectObject(&m_penLightGray);

		m_dcMem.MoveTo(m_nScaleLeft, y);
		m_dcMem.LineTo(m_nScaleRight, y);

		sText.Format("%g", (double)i / 100);
		size = m_dcMem.GetOutputTextExtent(sText);
		m_dcMem.TextOut(m_nScaleLeft - size.cx - 2, y - size.cy / 2, sText);
	}

	const double t1 = dispTime / 5;
	double step = pow(10.0, floor(log10(t1)));
	const double t2 = t1 / step;
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
				if (t >= 0.1 || t == 0)
					sText.Format("%g", t);
				else
					sText.Format("%gm", t * 1000);
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

	m_dcMem.SelectObject(&m_penBlack);
	m_dcMem.MoveTo(m_nScaleLeft - 1, m_nScaleTop);
	m_dcMem.LineTo(m_nScaleLeft - 1, m_nScaleBottom);
	m_dcMem.MoveTo(m_nScaleLeft, m_nScaleCenter);
	m_dcMem.LineTo(m_nScaleRight, m_nScaleCenter);

	sText.LoadString(IDS_TIME);
	sText += " [sec]";
	size = m_dcMem.GetOutputTextExtent(sText);
	m_dcMem.TextOut(m_nScaleLeft + (m_nScaleWidth - size.cx) / 2, m_nHeight - (size.cy + 2), sText);

	m_dcMem.SelectObject(&m_Font2);
	sText.LoadString(IDS_AMPLITUDE);
	size = m_dcMem.GetOutputTextExtent(sText);
	m_dcMem.TextOut(2, m_nScaleTop + (m_nScaleHeight + size.cx) / 2, sText);

	m_dcMem.SelectObject(pBitmapOrg);
	m_dcMem.SelectObject(pPenOrg);
	m_dcMem.SelectObject(pFontOrg);
}

void CRecWnd::DispGraph(double totalTime, double startTime, double dispTime, const double *pData, int nData, int nChannel, int nLR, double fMaxData)
{
	nData /= nChannel;
	m_fStartTime = startTime;
	m_fDispTime = dispTime;
	m_nData = nData;

	SetBitmap(startTime, dispTime);

	if (pData == NULL) {
		Invalidate(FALSE);
		UpdateWindow();
		return;
	}

	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
	CPen *pPenOrg = m_dcMem.SelectObject(nLR == 0 ? &m_penLeft : &m_penRight);

	int nPoint = 0;
	int x2 = 0;
	int ymin = m_nScaleBottom;
	int ymax = m_nScaleTop;
	const int i2 = int(startTime / totalTime * nData + 0.99);
	for (int i = i2; i < nData; i++) {
		const int x = (int)((i * totalTime / nData - startTime) / dispTime * m_nScaleWidth + 0.5) + m_nScaleLeft;

		if (x2 == 0)
			x2 = x;

		if (x != x2) {
			m_pPointBuf[nPoint].x = x2;
			m_pPointBuf[nPoint].y = ymin;
			nPoint++;
			if (ymax != ymin) {
				m_pPointBuf[nPoint].x = x2;
				m_pPointBuf[nPoint].y = ymax;
				nPoint++;
			}
			ymin = m_nScaleBottom;
			ymax = m_nScaleTop;
			x2 = x;

			if (x > m_nScaleRight)
				break;
		}

		const int y = (int)(m_nScaleCenter - pData[i * nChannel + nLR] * m_nScaleHeight / 2 / (fMaxData != 0 ? fMaxData : 1));
		if (y < ymin)
			ymin = y;
		if (y > ymax)
			ymax = y;
	}

	if (nPoint >= 2)
		m_dcMem.Polyline(m_pPointBuf, nPoint);

	if (m_fStartSelectTime != m_fEndSelectTime)
		SetSelectArea(m_fStartSelectTime, m_fEndSelectTime, FALSE);

	m_dcMem.SelectObject(pBitmapOrg);
	m_dcMem.SelectObject(pPenOrg);

	Invalidate(FALSE);
	UpdateWindow();
}

void CRecWnd::DispGraph2(const double *pData, int nData, int nChannel, int nLR, int nOffset, int nLength)
{
	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
	CPen *pPenOrg = m_dcMem.SelectObject(nLR == 0 ? &m_penLeft : &m_penRight);

	nData /= nChannel;
	nOffset /= nChannel;
	nLength /= nChannel;

	int nPoint = 0;
	int x2 = 0;
	int ymin = m_nScaleBottom;
	int ymax = m_nScaleTop;
	for (int i = nOffset; i < nOffset + nLength; i++) {
		const int x = m_nScaleLeft + (int)((double)i * m_nScaleWidth / nData + 0.5);

		if (x2 == 0)
			x2 = x;

		if (x != x2) {
			m_pPointBuf[nPoint].x = x2;
			m_pPointBuf[nPoint].y = ymin;
			nPoint++;
			if (ymax != ymin) {
				m_pPointBuf[nPoint].x = x2;
				m_pPointBuf[nPoint].y = ymax;
				nPoint++;
			}
			ymin = m_nScaleBottom;
			ymax = m_nScaleTop;
			x2 = x;

			if (x > m_nScaleRight)
				break;
		}

		const int y = (int)(m_nScaleCenter - pData[i * nChannel + nLR] * m_nScaleHeight / 2);
		if (y < ymin)
			ymin = y;
		if (y > ymax)
			ymax = y;
	}

	if (nPoint > 0) {
		m_pPointBuf[nPoint].x = m_pPointBuf[nPoint - 1].x + 1;
		m_pPointBuf[nPoint].y = m_pPointBuf[nPoint - 1].y;
		nPoint++;
	}

	m_dcMem.Polyline(m_pPointBuf, nPoint);

	m_dcMem.SelectObject(pBitmapOrg);
	m_dcMem.SelectObject(pPenOrg);

//	CRect rect(m_nScaleLeft + nOffset * m_nScaleWidth / nData, m_nScaleTop, m_nScaleLeft + (nOffset + nLength) * m_nScaleWidth / nData, m_nScaleBottom);
//	InvalidateRect(rect, FALSE);
	Invalidate(FALSE);
	UpdateWindow();
}

void CRecWnd::SetSelectArea(double startTime, double endTime, BOOL bErase)
{
	int x1, x2;

	m_dcMem.SelectClipRgn(&m_Rgn);

	const int ropModeOrg = m_dcMem.SetROP2(R2_NOTXORPEN);
	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
	CPen *pPenOrg = (CPen *)m_dcMem.SelectStockObject(NULL_PEN);
	CBrush *pBrushOrg = m_dcMem.SelectObject(&m_brushSelectArea);

	if (bErase && m_fStartSelectTime != m_fEndSelectTime) {
		x1 = (int)((m_fStartSelectTime - m_fStartTime) / m_fDispTime * m_nScaleWidth + 0.5) + m_nScaleLeft;
		x2 = (int)((m_fEndSelectTime - m_fStartTime) / m_fDispTime * m_nScaleWidth + 0.5) + m_nScaleLeft;

		m_dcMem.Rectangle(x1, m_nScaleTop, x2, m_nScaleBottom);
		m_dcMem.SelectObject(&m_penSelectBar);
		m_dcMem.MoveTo(x1, m_nScaleTop);
		m_dcMem.LineTo(x1, m_nScaleBottom);
		m_dcMem.MoveTo(x2, m_nScaleTop);
		m_dcMem.LineTo(x2, m_nScaleBottom);
	}

	m_fStartSelectTime = startTime;
	m_fEndSelectTime = endTime;

	x1 = (int)((m_fStartSelectTime - m_fStartTime) / m_fDispTime * m_nScaleWidth + 0.5) + m_nScaleLeft;
	x2 = (int)((m_fEndSelectTime - m_fStartTime) / m_fDispTime * m_nScaleWidth + 0.5) + m_nScaleLeft;

	m_dcMem.SelectStockObject(NULL_PEN);
	m_dcMem.Rectangle(x1, m_nScaleTop, x2, m_nScaleBottom);
	m_dcMem.SelectObject(&m_penSelectBar);
	m_dcMem.MoveTo(x1, m_nScaleTop);
	m_dcMem.LineTo(x1, m_nScaleBottom);
	m_dcMem.MoveTo(x2, m_nScaleTop);
	m_dcMem.LineTo(x2, m_nScaleBottom);

	m_dcMem.SelectObject(pBitmapOrg);
	m_dcMem.SelectObject(pPenOrg);
	m_dcMem.SelectObject(pBrushOrg);
	m_dcMem.SetROP2(ropModeOrg);

	m_dcMem.SelectClipRgn(NULL);

	Invalidate(FALSE);
	UpdateWindow();

	if (m_pCallBackFunc != NULL)
		m_pCallBackFunc(CC_SELECT_SET, m_dwCallBackData, m_fStartSelectTime, m_fEndSelectTime);
}

void CRecWnd::GetSelectArea(double *startTime, double *endTime) const
{
	*startTime = m_fStartSelectTime;
	*endTime = m_fEndSelectTime;
}

void CRecWnd::SetReplayPosition(double fReplayTime)
{
	const int ropModeOrg = m_dcMem.SetROP2(R2_NOTXORPEN);
	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
	CPen *pPenOrg = (CPen *)m_dcMem.SelectObject(&m_penReplayBar);
	int x;

	m_dcMem.SelectClipRgn(&m_Rgn);

	if (m_fReplayTime != -1) {
		x = (int)((m_fReplayTime - m_fStartTime) / m_fDispTime * m_nScaleWidth + 0.5) + m_nScaleLeft;
		m_dcMem.MoveTo(x, m_nScaleTop);
		m_dcMem.LineTo(x, m_nScaleBottom);
	}

	if (fReplayTime != -1) {
		x = (int)((fReplayTime - m_fStartTime) / m_fDispTime * m_nScaleWidth + 0.5) + m_nScaleLeft;
		m_dcMem.MoveTo(x, m_nScaleTop);
		m_dcMem.LineTo(x, m_nScaleBottom);
	}

	m_fReplayTime = fReplayTime;

	m_dcMem.SelectObject(pBitmapOrg);
	m_dcMem.SelectObject(pPenOrg);
	m_dcMem.SetROP2(ropModeOrg);

	m_dcMem.SelectClipRgn(NULL);

	Invalidate(FALSE);
	UpdateWindow();
}

void CRecWnd::SetMouseCursor(CPoint point) const
{
	HCURSOR hCursor = m_hCursorArrow;

	switch (m_nMouseMode) {
	case MOUSE_SELECT_START:
	case MOUSE_SELECT_END:
		hCursor = m_hCursorHMove;
		break;
	case MOUSE_SCROLL:
		hCursor = m_hCursorScroll;
		break;
	default:
		if (m_fStartSelectTime >= 0) {
			const int x1 = (int)((m_fStartSelectTime - m_fStartTime) / m_fDispTime * m_nScaleWidth + 0.5) + m_nScaleLeft;
			const int x2 = (int)((m_fEndSelectTime - m_fStartTime) / m_fDispTime * m_nScaleWidth + 0.5) + m_nScaleLeft;
			if ((abs(point.x - x2) < 2 || abs(point.x - x1) < 2) && point.y > m_nScaleTop && point.y < m_nScaleBottom)
				hCursor = m_hCursorHMove;
		}
		break;
	}

	::SetCursor(hCursor);
}

void CRecWnd::OnPaint()
{
	CPaintDC dc(this); // 描画用のデバイス コンテキスト

	if (m_bInitialized) {
		CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
		dc.BitBlt(0, 0, m_nWidth, m_nHeight, &m_dcMem, 0, 0, SRCCOPY);
		m_dcMem.SelectObject(pBitmapOrg);
	}
}

BOOL CRecWnd::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void CRecWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (point.y > m_nScaleTop && point.y < m_nScaleBottom) {
		if (m_fStartSelectTime >= 0) {
			const int x1 = (int)((m_fStartSelectTime - m_fStartTime) / m_fDispTime * m_nScaleWidth + 0.5) + m_nScaleLeft;
			const int x2 = (int)((m_fEndSelectTime - m_fStartTime) / m_fDispTime * m_nScaleWidth + 0.5) + m_nScaleLeft;

			if (abs(point.x - x2) < 2) {
				m_nMouseMode = MOUSE_SELECT_END;
				m_nMouseOffset = point.x - x2;
				if (m_pCallBackFunc != NULL)
					m_pCallBackFunc(CC_SELECT_START, m_dwCallBackData, m_fStartSelectTime, m_fEndSelectTime);
			} else if (abs(point.x - x1) < 2) {
				m_nMouseMode = MOUSE_SELECT_START;
				m_nMouseOffset = point.x - x1;
				if (m_pCallBackFunc != NULL)
					m_pCallBackFunc(CC_SELECT_START, m_dwCallBackData, m_fStartSelectTime, m_fEndSelectTime);
			}
		}

		if (m_nMouseMode == MOUSE_SELECT_NONE) {
			m_nScrollPos = point.x;
			m_nMouseMode = MOUSE_SCROLL;
			m_nMouseOffset = 0;
		}

		SetCapture();

		SetMouseCursor(point);
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void CRecWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_nMouseMode != MOUSE_SELECT_NONE) {
		switch (m_nMouseMode) {
		case MOUSE_SELECT_START:
		case MOUSE_SELECT_END:
			if (m_pCallBackFunc != NULL)
				m_pCallBackFunc(CC_SELECT_END, m_dwCallBackData, m_fStartSelectTime, m_fEndSelectTime);
			break;
		}

		m_nMouseMode = MOUSE_SELECT_NONE;
		ReleaseCapture();

		SetMouseCursor(point);
	}

	CWnd::OnLButtonUp(nFlags, point);
}

void CRecWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	int x = point.x - m_nMouseOffset - m_nScaleLeft;
	if (x < 0)
		x = 0;
	if (x > m_nScaleWidth)
		x = m_nScaleWidth;

	const double t = x * m_fDispTime / m_nScaleWidth + m_fStartTime;
	double fStartSelectTime;
	double fEndSelectTime;

	switch (m_nMouseMode) {
	case MOUSE_SELECT_START:
	case MOUSE_SELECT_END:
		if (m_nMouseMode == MOUSE_SELECT_START) {
			if (t <= m_fEndSelectTime){
				fStartSelectTime = t;
				fEndSelectTime = m_fEndSelectTime;
			} else {
				fStartSelectTime = m_fEndSelectTime;
				fEndSelectTime = t;
				m_nMouseMode = MOUSE_SELECT_END;
			}
		} else {
			if (t >= m_fStartSelectTime) {
				fStartSelectTime = m_fStartSelectTime;
				fEndSelectTime = t;
			} else {
				fStartSelectTime = t;
				fEndSelectTime = m_fStartSelectTime;
				m_nMouseMode = MOUSE_SELECT_START;
			}
		}

		SetSelectArea(fStartSelectTime, fEndSelectTime);
		if (m_pCallBackFunc != NULL)
			m_pCallBackFunc(CC_SELECT_MOVE, m_dwCallBackData, m_fStartSelectTime, m_fEndSelectTime);
		break;
	case MOUSE_SCROLL:
		m_pCallBackFunc(CC_SCROLL, m_dwCallBackData, (double)(point.x - m_nScrollPos) / m_nScaleWidth, 0);
		m_nScrollPos = point.x;
		break;
	}

	SetMouseCursor(point);

	CWnd::OnMouseMove(nFlags, point);
}

BOOL CRecWnd::OnSetCursor(CWnd* /*pWnd*/, UINT /*nHitTest*/, UINT /*message*/)
{
	return TRUE;
}

void CRecWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (point.y > m_nScaleTop && point.y < m_nScaleBottom) {
		int x = point.x - m_nScaleLeft;
		if (x < 0)
			x = 0;
		if (x > m_nScaleWidth)
			x = m_nScaleWidth;

		const double t = x * m_fDispTime / m_nScaleWidth + m_fStartTime;

		SetSelectArea(t, t);
		m_nMouseMode = MOUSE_SELECT_END;
		m_nMouseOffset = 0;
		if (m_pCallBackFunc != NULL)
			m_pCallBackFunc(CC_SELECT_START, m_dwCallBackData, m_fStartSelectTime, m_fEndSelectTime);

		SetCapture();

		SetMouseCursor(point);
	}

	CWnd::OnRButtonDown(nFlags, point);
}

void CRecWnd::OnRButtonUp(UINT nFlags, CPoint point)
{
	if (m_nMouseMode != MOUSE_SELECT_NONE) {
		switch (m_nMouseMode) {
		case MOUSE_SELECT_START:
		case MOUSE_SELECT_END:
			if (m_pCallBackFunc != NULL)
				m_pCallBackFunc(CC_SELECT_END, m_dwCallBackData, m_fStartSelectTime, m_fEndSelectTime);
			break;
		}

		m_nMouseMode = MOUSE_SELECT_NONE;
		ReleaseCapture();

		SetMouseCursor(point);
	}

	CWnd::OnRButtonUp(nFlags, point);
}
