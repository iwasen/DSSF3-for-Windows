// Impulse.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Common.h"
#include "ImpWnd.h"
#include "comres.h"
#include "Dpi.h"

#define MOUSE_SELECT_NONE		0
#define MOUSE_SELECT_START		1
#define MOUSE_SELECT_END		2
#define MOUSE_T0				3
#define MOUSE_T1				4
#define MOUSE_SCROLL			5

/////////////////////////////////////////////////////////////////////////////
// CImpWnd

CImpWnd::CImpWnd()
{
	m_bInitialized = FALSE;
	m_fT0 = -1;
	m_fT1 = -1;
	m_fStartSelectTime = -1;
	m_fEndSelectTime = -1;
	m_nMouseMode = MOUSE_SELECT_NONE;
	m_pCallBackFunc = NULL;
	m_hCursorArrow = NULL;
	m_hCursorHScroll = NULL;
	m_hCursorHMove = NULL;
	m_bHScroll = FALSE;
	m_pPoint = NULL;
}

CImpWnd::~CImpWnd()
{
	if (m_pPoint != NULL)
		delete [] m_pPoint;
}

BEGIN_MESSAGE_MAP(CImpWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_ENABLE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImpWnd メッセージ ハンドラ

void CImpWnd::Initialize(int nFontSize, LPCSTR pTitle, int nRemark, COLORREF rgbWaveColor, PFNGRAPH pCallBackFunc, LPARAM nCallBackData)
{
	if (m_bInitialized)
		return;

	m_bInitialized = TRUE;

	m_pCallBackFunc = pCallBackFunc;
	m_dwCallBackData = nCallBackData;

	CDC *pDC = GetDC();
	m_dcMem.CreateCompatibleDC(pDC);
	ReleaseDC(pDC);

	m_penWave.CreatePen(PS_SOLID, 0, rgbWaveColor);
//	m_penLine.CreatePen(PS_SOLID, 0, rgbLineColor);
	m_penBlack.CreatePen(PS_SOLID, 0, RGB(0, 0, 0));
	m_penGray.CreatePen(PS_SOLID, 0, RGB(128, 128, 128));
	m_penLightGray.CreatePen(PS_SOLID, 0, RGB(192, 192, 192));
	m_penT0.CreatePen(PS_DOT, 0, RGB(0, 255, 0));
	m_penT1.CreatePen(PS_DOT, 0, RGB(255, 0, 0));

	m_brushWhite.CreateSolidBrush(RGB(255, 255, 255));
	m_brushSelectArea.CreateSolidBrush(RGB(255, 255, 128));

	CString str;
	LOGFONT font{
		.lfHeight = CDpi::OriginalDpi(nFontSize, 88),
		.lfCharSet = DEFAULT_CHARSET
	};
	str.LoadString(IDS_DEFAULTFONT);
	strcpy_s(font.lfFaceName, str);
	m_fontNormal.CreatePointFontIndirect(&font, &m_dcMem);
	font.lfEscapement = 900;
	m_fontRotation.CreatePointFontIndirect(&font, &m_dcMem);

	m_dcMem.SetBkMode(TRANSPARENT);
	m_dcMem.SetTextColor(RGB(64, 64, 64));

	if (pTitle != NULL) {
		CString str1, str2;
		str1.LoadString(IDS_DIRECTSOUND);
		str2.LoadString(IDS_FIRSTREFLECTION);

		REMARK_INFO remark{
			.nRemark = nRemark,
			.remarks{
				{
					.pen = (nRemark > 1) ? &m_penWave : NULL,
					.text = pTitle
				},{
					.pen = &m_penT0,
					.text = str1
				},{
					.pen = &m_penT1,
					.text = str2
				}
			}
		};

		m_wndRemark.Create(this);
		m_wndRemark.DispRemarks(&remark);
	}

	CWinApp *pWinApp = ::AfxGetApp();
	m_hCursorArrow = pWinApp->LoadStandardCursor(IDC_ARROW);
	m_hCursorHScroll = pWinApp->LoadStandardCursor(IDC_SIZEWE);
	m_hCursorHMove = pWinApp->LoadCursor(IDC_HMOVE_CUR);

	Initialize2();
}

void CImpWnd::Initialize2()
{
	RECT rect;
	GetClientRect(&rect);

	m_nWidth = rect.right;
	m_nHeight = rect.bottom;

	CDC *pDC = GetDC();
	m_bitmapMem.CreateCompatibleBitmap(pDC, m_nWidth, m_nHeight);
	ReleaseDC(pDC);

	CFont *pFontOrg = m_dcMem.SelectObject(&m_fontNormal);
	TEXTMETRIC tm;
	m_dcMem.GetTextMetrics(&tm);
	m_dcMem.SelectObject(pFontOrg);

	m_nScaleLeft = tm.tmAveCharWidth * 4 + tm.tmHeight + 6;
	m_nScaleTop = 10;
	m_nScaleRight = m_nWidth - 0;
	m_nScaleBottom = ((m_nHeight - ((tm.tmHeight + 1) * 2 + 2)) & ~1) + 1;
	m_nScaleWidth = m_nScaleRight - m_nScaleLeft;
	m_nScaleHeight = m_nScaleBottom - m_nScaleTop;
	m_nScaleCenter = m_nScaleTop + m_nScaleHeight / 2;

	m_rgnScale.CreateRectRgn(m_nScaleLeft, m_nScaleTop, m_nScaleRight, m_nScaleBottom);

	m_pPoint = new POINT[m_nScaleWidth * 2 + 4];
}

void CImpWnd::Uninitialize2()
{
	m_bitmapMem.DeleteObject();
	m_rgnScale.DeleteObject();

	if (m_pPoint != NULL) {
		delete [] m_pPoint;
		m_pPoint = NULL;
	}
}

void CImpWnd::Resize()
{
	Uninitialize2();
	Initialize2();
}

void CImpWnd::SetBitmap(double startTime, double dispTime)
{
	CRect rect;
	CString sText;
	CSize size;
	int x, y;
	int ns;

	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
	CPen *pPenOrg = m_dcMem.SelectObject(&m_penBlack);
	CFont *pFontOrg = m_dcMem.SelectObject(&m_fontNormal);

	rect.SetRect(0, 0, m_nWidth, m_nHeight);
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

	const double t1 = dispTime / m_nScaleWidth * 75;
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

	m_dcMem.SelectObject(&m_penBlack);
	m_dcMem.MoveTo(m_nScaleLeft - 1, m_nScaleTop);
	m_dcMem.LineTo(m_nScaleLeft - 1, m_nScaleBottom);
	m_dcMem.MoveTo(m_nScaleLeft, m_nScaleCenter);
	m_dcMem.LineTo(m_nScaleRight, m_nScaleCenter);

	sText.LoadString(IDS_TIME);
	sText += " [ms]";
	size = m_dcMem.GetOutputTextExtent(sText);
	m_dcMem.TextOut(m_nScaleLeft + (m_nScaleWidth - size.cx) / 2, m_nHeight - (size.cy + 2), sText);

	m_dcMem.SelectObject(&m_fontRotation);
	sText.LoadString(IDS_AMPLITUDE);
	size = m_dcMem.GetOutputTextExtent(sText);
	m_dcMem.TextOut(2, m_nScaleTop + (m_nScaleHeight + size.cx) / 2, sText);

	m_dcMem.SelectObject(pBitmapOrg);
	m_dcMem.SelectObject(pPenOrg);
	m_dcMem.SelectObject(pFontOrg);
}

void CImpWnd::OnPaint()
{
	CPaintDC dc(this); // 描画用のデバイス コンテキスト

	if (IsWindowEnabled()) {
		int x;
		CBitmap *pBitmapOrg;
		CPen *pPenOrg;

		pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
		dc.BitBlt(0, 0, m_nWidth, m_nHeight, &m_dcMem, 0, 0, SRCCOPY);

		if (m_fT0 >= 0) {
			x = (int)((m_fT0 - m_fStartTime) / m_fDispTime * m_nScaleWidth + 0.5) + m_nScaleLeft;
			if (x >= m_nScaleLeft && x < m_nScaleRight) {
				dc.SetBkMode(TRANSPARENT);
				pPenOrg = dc.SelectObject(&m_penT0);
				dc.MoveTo(x, m_nScaleTop);
				dc.LineTo(x, m_nScaleBottom);
				dc.SelectObject(pPenOrg);
			}
		}
		if (m_fT1 >= 0) {
			x = (int)((m_fT1 - m_fStartTime) / m_fDispTime * m_nScaleWidth + 0.5) + m_nScaleLeft;
			if (x >= m_nScaleLeft && x < m_nScaleRight) {
				dc.SetBkMode(TRANSPARENT);
				pPenOrg = dc.SelectObject(&m_penT1);
				dc.MoveTo(x, m_nScaleTop);
				dc.LineTo(x, m_nScaleBottom);
				dc.SelectObject(pPenOrg);
			}
		}
		m_dcMem.SelectObject(pBitmapOrg);
	} else {
		CBrush brush;
		RECT rect;
		GetClientRect(&rect);
		brush.CreateSysColorBrush(COLOR_3DFACE);
		dc.FillRect(&rect, &brush);
	}
}

void CImpWnd::DispImpulse(double totalTime, double startTime, double dispTime, const double *pData, int nData, BOOL bHScroll)
{
	int i, i2;
	int x, x2;
	int y;
	int ymin, ymax;

	if (!IsWindowEnabled())
		return;

	m_fStartTime = startTime;
	m_fDispTime = dispTime;
	m_nData = nData;
	m_bHScroll = bHScroll;

	SetBitmap(startTime, dispTime);

	if (pData == NULL) {
		Invalidate(FALSE);
		UpdateWindow();
		return;
	}

	const double fMaxVal = GetMaxData(pData, nData);

	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
	CPen *pPenOrg = m_dcMem.SelectObject(&m_penWave);

	int nPoint = 0;
	x2 = 0;
	ymin = ymax = m_nScaleCenter;
	i2 = int(startTime / totalTime * nData + 0.99);
	for (i = i2; i < nData - 1; i++) {
		x = (int)((i * totalTime / nData - startTime) / dispTime * m_nScaleWidth + 0.5) + m_nScaleLeft;

		if (x2 == 0)
			x2 = x;

		if (x != x2) {
			if (ymin == m_nScaleCenter && ymax == m_nScaleCenter) {
				m_pPoint[nPoint].x = x2;
				m_pPoint[nPoint].y = m_nScaleCenter;
				nPoint++;
			} else {
				if (ymin != m_nScaleCenter) {
					m_pPoint[nPoint].x = x2;
					m_pPoint[nPoint].y = ymin;
					nPoint++;
				}

				if (ymax != m_nScaleCenter) {
					m_pPoint[nPoint].x = x2;
					m_pPoint[nPoint].y = ymax;
					nPoint++;
				}
			}

			if (x2 > m_nScaleRight)
				break;

			ymin = ymax = m_nScaleCenter;
			x2 = x;
		}

		y = (int)(m_nScaleCenter - pData[i] * m_nScaleHeight / 2 / fMaxVal + 0.5);
		if (y < ymin)
			ymin = y;
		if (y > ymax)
			ymax = y;
	}

	if (nPoint != 0)
		m_dcMem.Polyline(m_pPoint, nPoint);

	if (m_fStartSelectTime != m_fEndSelectTime)
		SetSelectArea(m_fStartSelectTime, m_fEndSelectTime, FALSE);

	m_dcMem.SelectObject(pBitmapOrg);
	m_dcMem.SelectObject(pPenOrg);

	Invalidate(FALSE);
	UpdateWindow();
}

void CImpWnd::OnEnable(BOOL bEnable)
{
	CWnd::OnEnable(bEnable);

	Invalidate(FALSE);
}

BOOL CImpWnd::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void CImpWnd::SetDeltaT1(double t0, double t1)
{
	m_fT0 = t0 / 1000;
	m_fT1 = t1 / 1000;
}

void CImpWnd::SetSelectArea(double startTime, double endTime, BOOL bErase)
{
	int x1, x2;

	m_dcMem.SelectClipRgn(&m_rgnScale);

	const int ropModeOrg = m_dcMem.SetROP2(R2_NOTXORPEN);
	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
	CPen *pPenOrg = m_dcMem.SelectObject(&m_penBlack);
	CBrush *pBrushOrg = m_dcMem.SelectObject(&m_brushSelectArea);

	if (bErase && m_fStartSelectTime != m_fEndSelectTime) {
		x1 = (int)((m_fStartSelectTime - m_fStartTime) / m_fDispTime * m_nScaleWidth + 0.5) + m_nScaleLeft;
		x2 = (int)((m_fEndSelectTime - m_fStartTime) / m_fDispTime * m_nScaleWidth + 0.5) + m_nScaleLeft;
		if (x1 != x2)
			m_dcMem.Rectangle(x1, m_nScaleTop, x2 + 1, m_nScaleBottom);
		else {
			m_dcMem.MoveTo(x1, m_nScaleTop);
			m_dcMem.LineTo(x1, m_nScaleBottom);
		}
	}

	m_fStartSelectTime = startTime;
	m_fEndSelectTime = endTime;

	x1 = (int)((m_fStartSelectTime - m_fStartTime) / m_fDispTime * m_nScaleWidth + 0.5) + m_nScaleLeft;
	x2 = (int)((m_fEndSelectTime - m_fStartTime) / m_fDispTime * m_nScaleWidth + 0.5) + m_nScaleLeft;
	if (x1 != x2)
		m_dcMem.Rectangle(x1, m_nScaleTop, x2 + 1, m_nScaleBottom);
	else {
		m_dcMem.MoveTo(x1, m_nScaleTop);
		m_dcMem.LineTo(x1, m_nScaleBottom);
	}

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

void CImpWnd::GetSelectArea(double *startTime, double *endTime) const
{
	*startTime = m_fStartSelectTime;
	*endTime = m_fEndSelectTime;
}

void CImpWnd::OnLButtonDown(UINT nFlags, CPoint point)
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

		if (m_fT0 >= 0) {
			const int x1 = (int)((m_fT0 - m_fStartTime) / m_fDispTime * m_nScaleWidth + 0.5) + m_nScaleLeft;
			const int x2 = (int)((m_fT1 - m_fStartTime) / m_fDispTime * m_nScaleWidth + 0.5) + m_nScaleLeft;

			if (abs(point.x - x2) < 2) {
				m_nMouseMode = MOUSE_T1;
				m_nMouseOffset = point.x - x2;
				if (m_pCallBackFunc != NULL)
					m_pCallBackFunc(CC_DT1_START, m_dwCallBackData, m_fT0, m_fT1);
			} else if (abs(point.x - x1) < 2) {
				m_nMouseMode = MOUSE_T0;
				m_nMouseOffset = point.x - x1;
				if (m_pCallBackFunc != NULL)
					m_pCallBackFunc(CC_DT1_START, m_dwCallBackData, m_fT0, m_fT1);
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

void CImpWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_nMouseMode != MOUSE_SELECT_NONE) {
		switch (m_nMouseMode) {
		case MOUSE_SELECT_START:
		case MOUSE_SELECT_END:
			if (m_pCallBackFunc != NULL)
				m_pCallBackFunc(CC_SELECT_END, m_dwCallBackData, m_fStartSelectTime, m_fEndSelectTime);
			break;
		case MOUSE_T0:
		case MOUSE_T1:
			if (m_pCallBackFunc != NULL)
				m_pCallBackFunc(CC_DT1_END, m_dwCallBackData, m_fT0, m_fT1);
			break;
		}

		m_nMouseMode = MOUSE_SELECT_NONE;
		ReleaseCapture();

		SetMouseCursor(point);
	}

	CWnd::OnLButtonUp(nFlags, point);
}

void CImpWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	int x = point.x - m_nMouseOffset - m_nScaleLeft;
	if (x < 0)
		x = 0;
	if (x > m_nScaleWidth)
		x = m_nScaleWidth;

	const double t = x * m_fDispTime / m_nScaleWidth + m_fStartTime;

	switch (m_nMouseMode) {
	case MOUSE_SELECT_START:
	case MOUSE_SELECT_END:
		if (m_nMouseMode == MOUSE_SELECT_START) {
			if (t < m_fEndSelectTime) {
				SetSelectArea(t, m_fEndSelectTime);
				if (m_pCallBackFunc != NULL)
					m_pCallBackFunc(CC_SELECT_MOVE, m_dwCallBackData, m_fStartSelectTime, m_fEndSelectTime);
			}
		} else {
			if (t > m_fStartSelectTime) {
				SetSelectArea(m_fStartSelectTime, t);
				if (m_pCallBackFunc != NULL)
					m_pCallBackFunc(CC_SELECT_MOVE, m_dwCallBackData, m_fStartSelectTime, m_fEndSelectTime);
			}
		}
		break;
	case MOUSE_T0:
	case MOUSE_T1:
		if (m_nMouseMode == MOUSE_T0) {
			if (t < m_fT1) {
				m_fT0 = t;
				Invalidate(FALSE);
				UpdateWindow();
				if (m_pCallBackFunc != NULL)
					m_pCallBackFunc(CC_DT1_MOVE, m_dwCallBackData, m_fT0, m_fT1);
			}
		} else {
			if (t > m_fT0) {
				m_fT1 = t;
				Invalidate(FALSE);
				UpdateWindow();
				if (m_pCallBackFunc != NULL)
					m_pCallBackFunc(CC_DT1_MOVE, m_dwCallBackData, m_fT0, m_fT1);
			}
		}
		break;
	case MOUSE_SCROLL:
		m_pCallBackFunc(CC_SCROLL, m_dwCallBackData, (double)(point.x - m_nScrollPos) / m_nScaleWidth, 0);
		m_nScrollPos = point.x;
		break;
	}

	SetMouseCursor(point);

	CWnd::OnMouseMove(nFlags, point);
}

void CImpWnd::SetMouseCursor(CPoint point) const
{
	HCURSOR hCursor = m_hCursorArrow;

	switch (m_nMouseMode) {
	case MOUSE_SELECT_START:
	case MOUSE_SELECT_END:
	case MOUSE_T0:
	case MOUSE_T1:
		hCursor = m_hCursorHMove;
		break;
	case MOUSE_SCROLL:
		if (m_bHScroll)
			hCursor = m_hCursorHScroll;
		break;
	default:
		if (m_fStartSelectTime >= 0) {
			const int x1 = (int)((m_fStartSelectTime - m_fStartTime) / m_fDispTime * m_nScaleWidth + 0.5) + m_nScaleLeft;
			const int x2 = (int)((m_fEndSelectTime - m_fStartTime) / m_fDispTime * m_nScaleWidth + 0.5) + m_nScaleLeft;
			if ((abs(point.x - x2) < 2 || abs(point.x - x1) < 2) && point.y > m_nScaleTop && point.y < m_nScaleBottom)
				hCursor = m_hCursorHMove;
		}
		if (m_fT0 >= 0) {
			const int x1 = (int)((m_fT0 - m_fStartTime) / m_fDispTime * m_nScaleWidth + 0.5) + m_nScaleLeft;
			const int x2 = (int)((m_fT1 - m_fStartTime) / m_fDispTime * m_nScaleWidth + 0.5) + m_nScaleLeft;
			if ((abs(point.x - x2) < 2 || abs(point.x - x1) < 2) && point.y > m_nScaleTop && point.y < m_nScaleBottom)
				hCursor = m_hCursorHMove;
		}
		break;
	}

	::SetCursor(hCursor);
}

BOOL CImpWnd::OnSetCursor(CWnd* /*pWnd*/, UINT /*nHitTest*/, UINT /*message*/)
{
	return TRUE;
}
