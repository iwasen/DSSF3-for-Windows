// FactorWnd.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "comres.h"
#include "common.h"
#include "FactorWnd.h"
#include "Dpi.h"

#define COLOR_DATA	RGB(0, 0, 255)
#define COLOR_SEL	RGB(255, 0, 0)
#define COLOR_SEL_LINE	RGB(255, 128, 128)
#define COLOR_MARKER	RGB(0, 192, 0)
#define COLOR_DRAG	RGB(192, 0, 0)
#define COLOR_ZOOMAREA	RGB(255, 240, 240)

#define MOUSE_NORMAL			0
#define MOUSE_SELECT_START		1
#define MOUSE_SELECT_END		2
#define MOUSE_SCROLL			3
#define MOUSE_DRAG				4
#define MOUSE_ZOOM_AREA			5

/////////////////////////////////////////////////////////////////////////////
// CFactorWnd

CFactorWnd::CFactorWnd()
{
	m_bInitialize = FALSE;
	m_nMouseMode = MOUSE_NORMAL;
	m_pCallBackFunc = NULL;
	m_hCursorArrow = NULL;
	m_bHScroll = FALSE;
	m_bVScroll = FALSE;
	m_nMarkerCursorX = 0;
	m_nMarkerIndex = -1;
	m_pNotifyWnd = NULL;
	m_sizeCurrent = 0;
	m_rectZoom.SetRect(0, 0, 0, 0);
	m_hCursor = NULL;
}

BEGIN_MESSAGE_MAP(CFactorWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_SIZE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFactorWnd メッセージ ハンドラ

void CFactorWnd::OnPaint()
{
	CPaintDC dc(this); // 描画用のデバイス コンテキスト

	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);

	if (m_nMarkerCursorX != 0 || m_rectZoom.Width() != 0 || m_nMouseMode == MOUSE_DRAG) {
		CBitmap *pBitmapOrg2 = m_dcMem2.SelectObject(&m_bitmapMem2);
		m_dcMem2.BitBlt(0, 0, m_nWidth, m_nHeight, &m_dcMem, 0, 0, SRCCOPY);

		m_dcMem2.SelectClipRgn(&m_Rgn);

		if (m_nMarkerCursorX != 0) {
			CPen *pPenOrg = m_dcMem2.SelectObject(&m_penMarkerDot);
			m_dcMem2.MoveTo(m_nMarkerCursorX, m_nScaleTop);
			m_dcMem2.LineTo(m_nMarkerCursorX, m_nScaleBottom);
			m_dcMem2.SelectObject(pPenOrg);
		}

		if (m_rectZoom.Width() != 0) {
			CPen *pPenOrg = m_dcMem2.SelectObject(&m_penZoomArea);
			CBrush *pBrushOrg = m_dcMem2.SelectObject(&m_brushZoomArea);
			const int ropModeOrg = m_dcMem2.SetROP2(R2_MASKPEN);

			m_dcMem2.Rectangle(m_rectZoom);

			m_dcMem2.SelectObject(pPenOrg);
			m_dcMem2.SelectObject(pBrushOrg);
			m_dcMem2.SetROP2(ropModeOrg);
		}

		if (m_nMouseMode == MOUSE_DRAG) {
			CPoint point[5];
			point[0] = m_pointDragStart;
			point[1].SetPoint(m_pointDragStart.x, m_pointDragEnd.y);
			point[2] = m_pointDragEnd;
			point[3].SetPoint(m_pointDragEnd.x, m_pointDragStart.y);
			point[4] = m_pointDragStart;
			CPen *pPenOrg = m_dcMem2.SelectObject(&m_penDrag);
			m_dcMem2.Polyline(point, 5);
			m_dcMem2.SelectObject(pPenOrg);
		}

		m_dcMem2.SelectClipRgn(NULL);

		dc.BitBlt(0, 0, m_nWidth, m_nHeight, &m_dcMem2, 0, 0, SRCCOPY);
		m_dcMem2.SelectObject(pBitmapOrg2);
	} else
		dc.BitBlt(0, 0, m_nWidth, m_nHeight, &m_dcMem, 0, 0, SRCCOPY);

	m_dcMem.SelectObject(pBitmapOrg);
}

BOOL CFactorWnd::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void CFactorWnd::Initialize(int nFontSize, LPCSTR pTitle, PFNGRAPH pCallBackFunc, LPARAM nCallBackData, CWnd *pNotifyWnd)
{
	if (m_bInitialize)
		return;

	m_bInitialize = TRUE;

	m_pNotifyWnd = pNotifyWnd;

	SetSize(nFontSize);

	m_penData.CreatePen(PS_SOLID, 0, COLOR_DATA);
	m_penSel.CreatePen(PS_SOLID, 0, COLOR_SEL);
	m_penSelLine.CreatePen(PS_DOT, 0, COLOR_SEL_LINE);
	m_penBlack.CreatePen(PS_SOLID, 0, RGB(0, 0, 0));
	m_penGray.CreatePen(PS_SOLID, 0, RGB(128, 128, 128));
	m_penLightGray.CreatePen(PS_SOLID, 0, RGB(192, 192, 192));
	m_penMarkerDot.CreatePen(PS_DOT, 0, COLOR_MARKER);
	m_penMarkerSolid.CreatePen(PS_SOLID, 0, COLOR_MARKER);
	m_penDrag.CreatePen(PS_DOT, 0, COLOR_DRAG);
	m_penZoomArea.CreatePen(PS_SOLID, 0, COLOR_DRAG);

	m_brushWhite.CreateSolidBrush(RGB(255, 255, 255));
	m_brushData.CreateSolidBrush(COLOR_DATA);
	m_brushSel.CreateSolidBrush(COLOR_SEL);
	m_brushMarker.CreateSolidBrush(COLOR_MARKER);
	m_brushZoomArea.CreateSolidBrush(COLOR_ZOOMAREA);

	m_tooltip.Create(this);
	m_tooltip.AddTool(this);
	m_tooltip.SetDelayTime(TTDT_INITIAL, 10);
	m_tooltip.SetDelayTime(TTDT_AUTOPOP, 30000);

	m_pCallBackFunc = pCallBackFunc;
	m_dwCallBackData = nCallBackData;

	m_sTitle = pTitle;
	m_wndRemark.Create(this);

	CWinApp *pWinApp = ::AfxGetApp();
	m_hCursorArrow = pWinApp->LoadStandardCursor(IDC_ARROW);
	m_hCursorHScroll = pWinApp->LoadStandardCursor(IDC_SIZEWE);
	m_hCursorVScroll = pWinApp->LoadStandardCursor(IDC_SIZENS);
	m_hCursorAScroll = pWinApp->LoadStandardCursor(IDC_SIZEALL);
}

void CFactorWnd::SetSize(int nFontSize)
{
	CRect rect;

	GetClientRect(&rect);

	if (rect.Size() == m_sizeCurrent)
		return;

	m_sizeCurrent = rect.Size();

	m_nWidth = rect.right;
	m_nHeight = rect.bottom;

	CDC *pDC = GetDC();
	m_dcMem.DeleteDC();
	m_dcMem.CreateCompatibleDC(pDC);
	m_bitmapMem.DeleteObject();
	m_bitmapMem.CreateCompatibleBitmap(pDC, m_nWidth, m_nHeight);

	m_dcMem2.DeleteDC();
	m_dcMem2.CreateCompatibleDC(pDC);
	m_bitmapMem2.DeleteObject();
	m_bitmapMem2.CreateCompatibleBitmap(pDC, m_nWidth, m_nHeight);
	ReleaseDC(pDC);

	CString str;
	LOGFONT font{
		.lfHeight = CDpi::OriginalDpi(nFontSize, 88),
		.lfCharSet = DEFAULT_CHARSET
	};
	str.LoadString(IDS_DEFAULTFONT);
	strcpy_s(font.lfFaceName, str);
	m_Font.DeleteObject();
	m_Font.CreatePointFontIndirect(&font, &m_dcMem);
	font.lfEscapement = 900;
	m_Font2.DeleteObject();
	m_Font2.CreatePointFontIndirect(&font, &m_dcMem);

	m_dcMem.SetBkMode(TRANSPARENT);
	m_dcMem.SetTextColor(RGB(64, 64, 64));

	CFont *pFontOrg = m_dcMem.SelectObject(&m_Font);
	TEXTMETRIC tm;
	m_dcMem.GetTextMetrics(&tm);
	m_dcMem.SelectObject(pFontOrg);

	m_nScaleLeft = tm.tmAveCharWidth * 5 + tm.tmHeight + 5;
	m_nScaleTop = 12;
	m_nScaleRight = m_nWidth - 0;
	m_nScaleBottom = m_nHeight - ((tm.tmHeight + 1) * 2 + 2);
	m_nScaleWidth = m_nScaleRight - m_nScaleLeft;
	m_nScaleHeight = m_nScaleBottom - m_nScaleTop;

	m_Rgn.DeleteObject();
	m_Rgn.CreateRectRgn(m_nScaleLeft, m_nScaleTop, m_nScaleRight + 1, m_nScaleBottom + 1);
}

void CFactorWnd::SetBitmap(double fStartTime, double fDispTime, double fMaxValue, double fMinValue, LPCSTR pAxisStr, BOOL bLogScale)
{
	CString sText;
	CSize size;
	int x, y;
	double step, t, t1, t2;
	double fValue;
	int ns;
	const double fLevelRange = fMaxValue - fMinValue;

	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
	CPen *pPenOrg = m_dcMem.SelectObject(&m_penBlack);
	CFont *pFontOrg = m_dcMem.SelectObject(&m_Font);

	m_fScaleZero = m_nScaleTop + m_nScaleHeight * fMaxValue / fLevelRange;

	CRect rect;
	rect.SetRect(0, 0, m_nWidth, m_nHeight);
	m_dcMem.FillRect(rect, &m_brushWhite);

	rect.SetRect(m_nScaleLeft, m_nScaleTop, m_nScaleRight, m_nScaleBottom);
	m_dcMem.SelectStockObject(NULL_BRUSH);
	m_dcMem.Rectangle(rect);

	if (bLogScale) {
		step = pow(10, floor(log10((fMinValue)) + 1e-3));
		const double fLogMaxValue = log(fMaxValue);
		const double fLogMinValue = log(fMinValue);
		const double fSpan = fLogMaxValue - fLogMinValue;
		for (fValue = floor(fMinValue / step + 1e-3) * step; fValue <= fMaxValue; fValue += pow(10, floor(log10((fValue)) + 1e-3))) {
			y =  m_nScaleBottom - (int)((log(fValue) - fLogMinValue) * m_nScaleHeight / fSpan + 0.5);

			sText.Format("%g", fValue);

			if (sText[0] == '1' || sText[0] == '2' || sText[0] == '5') {
				size = m_dcMem.GetOutputTextExtent(sText);
				m_dcMem.TextOut(m_nScaleLeft - size.cx - 2, y - size.cy / 2, sText);
			}

			if (y > m_nScaleTop && y < m_nScaleBottom) {
				m_dcMem.SelectObject(sText[0] == '1' ? m_penGray : m_penLightGray);
				m_dcMem.MoveTo(m_nScaleLeft + 1, y);
				m_dcMem.LineTo(m_nScaleRight - 1, y);
			}
		}
	} else {
		t1 = fLevelRange / 4 + fLevelRange / 1000;
		step = pow(10.0, floor(log10(t1)));
		t2 = t1 / step;
		if (t2 < 2) {
			step *= 0.5;
			ns = 2;
		} else if (t2 < 5)
			ns = 2;
		else
			ns = 5;

		if (step == 0)
			step = 1;

		for (fValue = floor(fMinValue / step) * step; fValue <= fMaxValue + step * 1e-7; fValue += step) {
			y = (int)(m_fScaleZero - fValue * m_nScaleHeight / fLevelRange + 0.5);

			if (y <= m_nScaleBottom) {
				if (int(fabs(fValue) / step + 0.5) % ns == 0) {
					if (y > m_nScaleTop && y < m_nScaleBottom) {
						m_dcMem.SelectObject(&m_penGray);
						m_dcMem.MoveTo(m_nScaleLeft + 1, y);
						m_dcMem.LineTo(m_nScaleRight - 1, y);
					}
					if (fabs(fValue) < step / 10)
						fValue = 0;
					sText.Format("%g", fValue);
					size = m_dcMem.GetOutputTextExtent(sText);
					m_dcMem.TextOut(m_nScaleLeft - size.cx - 2, y - size.cy / 2, sText);
				} else {
					if (y > m_nScaleTop && y < m_nScaleBottom) {
						m_dcMem.SelectObject(&m_penLightGray);
						m_dcMem.MoveTo(m_nScaleLeft + 1, y);
						m_dcMem.LineTo(m_nScaleRight - 1, y);
					}
				}
			}
		}
	}

	t1 = fDispTime / 7;
	step = pow(10.0, floor(log10(t1)));
	t2 = t1 / step;
	if (t2 < 2) {
		step *= 0.5;
		ns = 2;
	} else if (t2 < 5)
		ns = 2;
	else
		ns = 5;

	if (step == 0)
		step = 1;

	for (t = floor(fStartTime / step) * step; t < fStartTime + fDispTime; t += step) {
		x = m_nScaleLeft + int((t - fStartTime) * m_nScaleWidth / fDispTime + 0.5);
		if (x >= m_nScaleLeft) {
			if (int(t / step + 0.5) % ns == 0) {
				if (x > m_nScaleLeft && x < m_nScaleRight) {
					m_dcMem.SelectObject(&m_penGray);
					m_dcMem.MoveTo(x, m_nScaleTop + 1);
					m_dcMem.LineTo(x, m_nScaleBottom - 1);
				}
				sText.Format("%g", t);
				size = m_dcMem.GetOutputTextExtent(sText);
				m_dcMem.TextOut(x - size.cx / 2, m_nScaleBottom + 2, sText);
			} else {
				if (x > m_nScaleLeft && x < m_nScaleRight) {
					m_dcMem.SelectObject(&m_penLightGray);
					m_dcMem.MoveTo(x, m_nScaleTop + 1);
					m_dcMem.LineTo(x, m_nScaleBottom - 1);
				}
			}
		}
	}

	sText.LoadString(IDS_TIME);
	sText += " [sec]";
	size = m_dcMem.GetOutputTextExtent(sText);
	m_dcMem.TextOut(m_nScaleLeft + (m_nScaleWidth - size.cx) / 2, m_nHeight - (size.cy + 1), sText);

	m_dcMem.SelectObject(&m_Font2);
	size = m_dcMem.GetOutputTextExtent(pAxisStr);
	m_dcMem.TextOut(1, m_nScaleTop + (m_nScaleHeight + size.cx) / 2, pAxisStr);

	m_dcMem.SelectObject(pBitmapOrg);
	m_dcMem.SelectObject(pPenOrg);
	m_dcMem.SelectObject(pFontOrg);
}

void CFactorWnd::DispGraph(double fStepTime, double fStartTime, double fDispTime, const double *pData, int nData, double fMaxValue, double fMinValue, LPCSTR pAxisStr, int nSel, BOOL bHScroll, BOOL bVScroll, DbAcfMarkerRec *pAcfMarkerRec, int nAcfMarkerRec, double fOffsetTime, BOOL bLogScale)
{
	int i, i2;
	int x;
	int y;
	const double fLevelRange = fMaxValue - fMinValue;
	BOOL bFirst = TRUE;
	BOOL bSel = FALSE;
	int nSelX = 0, nSelY = 0;

	m_bHScroll = bHScroll;
	m_bVScroll = bVScroll;

	SetBitmap(fStartTime + fOffsetTime, fDispTime, fMaxValue, fMinValue, pAxisStr, bLogScale);

	m_dcMem.SelectClipRgn(&m_Rgn);
	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
	CPen *pPenOrg = m_dcMem.SelectObject(&m_penData);
	CBrush *pBrushOrg = m_dcMem.SelectObject(&m_brushData);

	const int nDotSize1 = min(int(m_nScaleWidth * fStepTime / fDispTime) / 4, 2);
	const int nDotSize2 = nDotSize1 ? nDotSize1 + 1 : 0;

	const double fOffset = fStartTime / fStepTime;
	i2 = int(fOffset);
	for (i = i2; i < nData; i++) {
		if (!_isnan(pData[i]) && _finite(pData[i])) {
			x = m_nScaleLeft + int((i - fOffset) * fStepTime / fDispTime * m_nScaleWidth + 0.5);

			if (bLogScale)
				y = m_nScaleBottom - (int)((log(pData[i] < fMinValue ? fMinValue : pData[i]) - log(fMinValue)) * m_nScaleHeight / (log(fMaxValue) - log(fMinValue)) + 0.5);
			else
				y = (int)(m_fScaleZero - pData[i] * m_nScaleHeight / fLevelRange + 0.5);

			if (bFirst) {
				m_dcMem.MoveTo(x, y);
				bFirst = FALSE;
			}

			m_dcMem.LineTo(x, y);

			if (i == nSel) {
				bSel = TRUE;
				nSelX = x;
				nSelY = y;
			} else
				m_dcMem.Ellipse(x - nDotSize1, y - nDotSize1, x + nDotSize2, y + nDotSize2);

			if (x >= m_nScaleRight)
				break;
		}
	}

	if (bSel && nSelX >= m_nScaleLeft && nSelX <= m_nScaleRight) {
		m_dcMem.SelectObject(&m_penSel);
		m_dcMem.SelectObject(&m_brushSel);

		m_dcMem.Ellipse(nSelX - 2, nSelY - 2, nSelX + 3, nSelY + 3);

		m_dcMem.SelectObject(&m_penSelLine);

		m_dcMem.MoveTo(nSelX, m_nScaleTop);
		m_dcMem.LineTo(nSelX, m_nScaleBottom);
		m_dcMem.MoveTo(m_nScaleLeft, nSelY);
		m_dcMem.LineTo(m_nScaleRight, nSelY);

		m_dcMem.SelectObject(&m_penData);
		m_dcMem.SelectObject(&m_brushData);
	}

	m_dcMem.SelectClipRgn(NULL);

	m_arrayMarker.RemoveAll();
	if (nAcfMarkerRec != 0) {
		const double fEndTime = fStartTime + fDispTime;
		for (i = 0; i < nAcfMarkerRec; i++) {
			if (pAcfMarkerRec[i].fTime > fStartTime && pAcfMarkerRec[i].fTime < fEndTime) {
				x = m_nScaleLeft + (int)((pAcfMarkerRec[i].fTime - fStartTime) / fDispTime * m_nScaleWidth + 0.5);

				m_dcMem.SelectObject(&m_penMarkerDot);
				m_dcMem.MoveTo(x, m_nScaleTop);
				m_dcMem.LineTo(x, m_nScaleBottom);

				CPoint point[3];
				point[0].x = x;
				point[0].y = m_nScaleTop - 2;
				point[1].x = x - 5;
				point[1].y = m_nScaleTop - 9;
				point[2].x = x + 5;
				point[2].y = m_nScaleTop - 9;
				m_dcMem.SelectObject(&m_penGray);
				m_dcMem.SelectObject(&m_brushMarker);
				m_dcMem.Polygon(point, 3);

				SMarker marker;
				marker.nMarkerID = pAcfMarkerRec[i].nMarkerID;
				marker.sTitle = pAcfMarkerRec[i].sTitle;
				marker.rectMarker.SetRect(point[1].x, point[1].y, point[2].x, point[0].y);
				m_arrayMarker.Add(marker);
			}
		}
	}

	m_dcMem.SelectObject(pBitmapOrg);
	m_dcMem.SelectObject(pPenOrg);
	m_dcMem.SelectObject(pBrushOrg);

	if (!m_sTitle.IsEmpty()) {
		REMARK_INFO remark{
			.nRemark = 1,
			.remarks{
				{
					.pen = NULL,
					.text = m_sTitle
				}
			}
		};
		m_wndRemark.DispRemarks(&remark);
	}

	Invalidate(FALSE);
	UpdateWindow();
}

void CFactorWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_pNotifyWnd == NULL) {
		if (m_nMouseMode == MOUSE_NORMAL) {
			if (nFlags & MK_SHIFT) {
				m_pointDragStart = point;
				m_pointDragEnd = point;
				m_nMouseMode = MOUSE_DRAG;
			} else {
				if (m_rectZoom.PtInRect(point)) {
					m_pointZoomArea = point - m_rectZoom.TopLeft();
					m_nMouseMode = MOUSE_ZOOM_AREA;
				} else {
					m_nScrollPosX = point.x;
					m_nScrollPosY = point.y;
					m_nMouseMode = MOUSE_SCROLL;
				}
			}

			SetMouseCursor();

			SetCapture();
			if (m_hCursor != NULL)
				::SetCursor(m_hCursor);
		}
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void CFactorWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_pNotifyWnd == NULL) {
		switch (m_nMouseMode) {
		case MOUSE_SCROLL:
			if (m_nMouseMode != MOUSE_NORMAL) {
				m_nMouseMode = MOUSE_NORMAL;

				SetMouseCursor();
			}
			break;
		case MOUSE_DRAG:
			m_nMouseMode = MOUSE_NORMAL;

			if (m_pointDragStart.x != m_pointDragEnd.x && m_pointDragStart.y != m_pointDragEnd.y) {
				CPoint pointStart, pointEnd;
				pointStart.SetPoint(min(m_pointDragStart.x, m_pointDragEnd.x), min(m_pointDragStart.y, m_pointDragEnd.y));
				pointEnd.SetPoint(max(m_pointDragStart.x, m_pointDragEnd.x), max(m_pointDragStart.y, m_pointDragEnd.y));
				SGraphDrag drag{
					.fStartX = (double)(pointStart.x - m_nScaleLeft) / m_nScaleWidth,
					.fStartY = (double)(pointStart.y - m_nScaleTop) / m_nScaleHeight,
					.fEndX = (double)(pointEnd.x - m_nScaleLeft) / m_nScaleWidth,
					.fEndY = (double)(pointEnd.y - m_nScaleTop) / m_nScaleHeight
				};
				GetParent()->SendMessage(WM_GRAPH_DRAG, (WPARAM)&drag);
			}

			Invalidate(FALSE);
			break;
		case MOUSE_ZOOM_AREA:
			m_nMouseMode = MOUSE_NORMAL;
			break;
		}

		ReleaseCapture();
	}

	CWnd::OnLButtonUp(nFlags, point);
}

void CFactorWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_pNotifyWnd == NULL) {
		switch (m_nMouseMode) {
		case MOUSE_SCROLL:
			if (m_pCallBackFunc != NULL)
				m_pCallBackFunc(CC_SCROLL, m_dwCallBackData, (double)(point.x - m_nScrollPosX) / m_nScaleWidth, (double)(point.y - m_nScrollPosY) / m_nScaleHeight);
			m_nScrollPosX = point.x;
			m_nScrollPosY = point.y;
			break;
		case MOUSE_DRAG:
			m_pointDragEnd = point;
			Invalidate(FALSE);
			break;
		case MOUSE_ZOOM_AREA:
			{
				CPoint pointStart, pointEnd;
				pointStart = point - m_pointZoomArea;
				pointEnd = pointStart + m_rectZoom.Size();

				SGraphDrag drag{
					.fStartX = (double)(pointStart.x - m_nScaleLeft) / m_nScaleWidth,
					.fStartY = (double)(pointStart.y - m_nScaleTop) / m_nScaleHeight,
					.fEndX = (double)(pointEnd.x - m_nScaleLeft) / m_nScaleWidth,
					.fEndY = (double)(pointEnd.y - m_nScaleTop) / m_nScaleHeight
				};
				GetParent()->SendMessage(WM_GRAPH_DRAG, (WPARAM)&drag);
			}
			break;
		}

		SetMouseCursor();
	}

	CWnd::OnMouseMove(nFlags, point);
}

BOOL CFactorWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (m_hCursor != NULL) {
		::SetCursor(m_hCursor);
		return TRUE;
	}

	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CFactorWnd::SetMouseCursor()
{
	m_hCursor = NULL;

	switch (m_nMouseMode) {
	case MOUSE_SCROLL:
		if (m_bHScroll) {
			if (m_bVScroll)
				m_hCursor = m_hCursorAScroll;
			else
				m_hCursor = m_hCursorHScroll;
		} else {
			if (m_bVScroll)
				m_hCursor = m_hCursorVScroll;
			else
				m_hCursor = NULL;
		}
		break;
	}
}

void CFactorWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (m_pCallBackFunc != NULL)
		m_pCallBackFunc(CC_DBLCLK, m_dwCallBackData, (double)(point.x - m_nScaleLeft) / m_nScaleWidth, (double)(point.y - m_nScaleTop) / m_nScaleHeight);

	CWnd::OnLButtonDblClk(nFlags, point);
}

void CFactorWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (m_pCallBackFunc != NULL) {
		int i;
		for (i = 0; i < m_arrayMarker.GetCount(); i++) {
			if (m_arrayMarker[i].rectMarker.PtInRect(point))
				break;
		}
		if (i < m_arrayMarker.GetCount()) {
			GetParent()->SendMessage(WM_SELECT_MARKER, m_arrayMarker[i].nMarkerID);
		} else {
			CRect rectScale(m_nScaleLeft, m_nScaleTop, m_nScaleRight, m_nScaleBottom);
			if (rectScale.PtInRect(point))
				m_pCallBackFunc(CC_RBUTTONDOWN, m_dwCallBackData, (double)(point.x - m_nScaleLeft) / m_nScaleWidth, (double)(point.y - m_nScaleTop) / m_nScaleHeight);
		}
	}

	CWnd::OnRButtonDown(nFlags, point);
}

void CFactorWnd::DispMarkerCursor(double fStartTime, double fDispTime, double fOffsetTime)
{
	const int x = (int)((fOffsetTime - fStartTime) / fDispTime * m_nScaleWidth + 0.5);
	if (x >= 0 && x < m_nScaleWidth)
		m_nMarkerCursorX = x + m_nScaleLeft;
	else
		m_nMarkerCursorX = 0;

	Invalidate(FALSE);
	UpdateWindow();
}

void CFactorWnd::HideMarkerCursor()
{
	m_nMarkerCursorX = 0;

	Invalidate(FALSE);
	UpdateWindow();
}

BOOL CFactorWnd::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_MOUSEMOVE) {
		int i;
		CPoint point(GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam));
		for (i = 0; i < m_arrayMarker.GetCount(); i++) {
			if (m_arrayMarker[i].rectMarker.PtInRect(point)) {
				if (i != m_nMarkerIndex) {
					m_nMarkerIndex = i;
					m_tooltip.UpdateTipText(m_arrayMarker[i].sTitle, this);
				}
				m_tooltip.RelayEvent(pMsg);
				break;
			}
		}
		if (m_nMarkerIndex != -1 && i == m_arrayMarker.GetCount()) {
			m_nMarkerIndex = -1;
			m_tooltip.Pop();
		}

	}

	return CWnd::PreTranslateMessage(pMsg);
}

void CFactorWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if (m_pNotifyWnd != NULL && cx != 0 && cy != 0)
		m_pNotifyWnd->SendMessage(WM_RESIZE, (WPARAM)this);
}

void CFactorWnd::PostNcDestroy()
{
	CWnd::PostNcDestroy();

	if (m_pNotifyWnd != NULL)
		m_pNotifyWnd->SendMessage(WM_CLOSE_GRAPH, (WPARAM)this);
}

void CFactorWnd::SetZoomRect(double fLeft, double fTop, double fRight, double fBottom)
{
	m_rectZoom.left = m_nScaleLeft + (int)(m_nScaleWidth * fLeft + 0.5);
	m_rectZoom.top = m_nScaleTop + (int)(m_nScaleHeight * fTop + 0.5);
	m_rectZoom.right = m_nScaleLeft + (int)(m_nScaleWidth * fRight + 0.5);
	m_rectZoom.bottom = m_nScaleTop + (int)(m_nScaleHeight * fBottom + 0.5);

	Invalidate(FALSE);
}

void CFactorWnd::ResetZoomRect()
{
	m_rectZoom.SetRect(0, 0, 0, 0);

	Invalidate(FALSE);
}
