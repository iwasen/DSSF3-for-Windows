// EchoWnd.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Sas.h"
#include "EchoWnd.h"

#define MOUSE_SELECT_NONE		0
#define MOUSE_SELECT_START		1
#define MOUSE_SELECT_END		2
#define MOUSE_SCROLL			3

/////////////////////////////////////////////////////////////////////////////
// CEchoWnd

CEchoWnd::CEchoWnd()
{
	m_bInitialize = FALSE;
	m_nMouseMode = MOUSE_SELECT_NONE;
	m_pCallBackFunc = NULL;
	m_hCursorArrow = NULL;
	m_bHScroll = FALSE;
}

BEGIN_MESSAGE_MAP(CEchoWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEchoWnd メッセージ ハンドラ

void CEchoWnd::OnPaint()
{
	CPaintDC dc(this); // 描画用のデバイス コンテキスト

	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
	dc.BitBlt(0, 0, m_nWidth, m_nHeight, &m_dcMem, 0, 0, SRCCOPY);
	m_dcMem.SelectObject(pBitmapOrg);
}

BOOL CEchoWnd::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void CEchoWnd::Initialize(int nFontSize, LPCSTR pTitle, PFNGRAPH pCallBackFunc, LPARAM nCallBackData)
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

	m_penData.CreatePen(PS_SOLID, 0, RGB(255, 0, 0));
	m_penData2.CreatePen(PS_SOLID, 0, RGB(255, 0, 255));
	m_penTsub.CreatePen(PS_SOLID, 0, RGB(0, 0, 255));
	m_penDot.CreatePen(PS_DOT, 0, RGB(0, 0, 255));
	m_penBlack.CreatePen(PS_SOLID, 0, RGB(0, 0, 0));
	m_penGray.CreatePen(PS_SOLID, 0, RGB(128, 128, 128));
	m_penLightGray.CreatePen(PS_SOLID, 0, RGB(192, 192, 192));
	m_penT20.CreatePen(PS_SOLID, 0, RGB(224, 224, 0));
	m_penT30.CreatePen(PS_SOLID, 0, RGB(128, 224, 0));
	m_penEDT.CreatePen(PS_SOLID, 0, RGB(0, 224, 224));

	m_brushWhite.CreateSolidBrush(RGB(255, 255, 255));

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

void CEchoWnd::SetBitmap(double startTime, double dispTime, int nMaxLevel, int nMinLevel)
{
	CString sText;
	CSize size;
	int x, y;
	int ns;
	const int nLevelRange = nMaxLevel - nMinLevel;

	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
	CPen *pPenOrg = m_dcMem.SelectObject(&m_penBlack);
	CFont *pFontOrg = m_dcMem.SelectObject(&m_Font);

	m_nScaleZero = m_nScaleTop + m_nScaleHeight * nMaxLevel / nLevelRange;

	CRect rect;
	rect.SetRect(0, 0, m_nWidth, m_nHeight);
	m_dcMem.FillRect(rect, &m_brushWhite);

	m_dcMem.MoveTo(m_nScaleLeft - 1, m_nScaleTop);
	m_dcMem.LineTo(m_nScaleLeft - 1, m_nScaleBottom);

	for (int i = nMaxLevel; i >= nMinLevel; i -= 10) {
		y = m_nScaleZero - (i * m_nScaleHeight / nLevelRange);
		if (i == 0)
			m_dcMem.SelectObject(&m_penBlack);
		else if (i == nMinLevel)
			m_dcMem.SelectObject(&m_penGray);
		else
			m_dcMem.SelectObject(&m_penLightGray);

		m_dcMem.MoveTo(m_nScaleLeft, y);
		m_dcMem.LineTo(m_nScaleRight, y);

		sText.Format("%d", i);
		size = m_dcMem.GetOutputTextExtent(sText);
		m_dcMem.TextOut(m_nScaleLeft - size.cx - 2, y - size.cy / 2, sText);
	}

	const double t1 = dispTime / 7;
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

	sText.LoadString(IDS_TIME);
	sText += " [ms]";
	size = m_dcMem.GetOutputTextExtent(sText);
	m_dcMem.TextOut(m_nScaleLeft + (m_nScaleWidth - size.cx) / 2, m_nHeight - (size.cy + 1), sText);

	m_dcMem.SelectObject(&m_Font2);
	sText.LoadString(IDS_REVLEVEL);
	sText += " [dB]";
	size = m_dcMem.GetOutputTextExtent(sText);
	m_dcMem.TextOut(1, m_nScaleTop + (m_nScaleHeight + size.cx) / 2, sText);

	m_dcMem.SelectObject(pBitmapOrg);
	m_dcMem.SelectObject(pPenOrg);
	m_dcMem.SelectObject(pFontOrg);
}

void CEchoWnd::DispGraph(double totalTime, double startTime, double dispTime, const double *pData, const double *pData2, int nData, double fRate, double t0, double t1, double dev, double fEndTime, int nMaxLevel, int nMinLevel, BOOL bHScroll, double fT20Reg0, double fT20Reg1, double fT30Reg0, double fT30Reg1, double fEDTReg0, double fEDTReg1)
{
	int i, i2;
	int x, x2 = 0;
	int y, y2 = 0;
	const int nLevelRange = nMaxLevel - nMinLevel;
	double fOffset;

	m_bHScroll = bHScroll;

	SetBitmap(startTime, dispTime, nMaxLevel, nMinLevel);

	m_dcMem.SelectClipRgn(&m_Rgn);
	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
	CPen *pPenOrg = m_dcMem.SelectObject(&m_penDot);

	y = m_nScaleZero - (-60 * m_nScaleHeight / nLevelRange);
	m_dcMem.MoveTo(m_nScaleLeft, y);
	m_dcMem.LineTo(m_nScaleRight, y);

	if (dev != 0) {
		const double t1s = t1 / 1000;
		m_dcMem.SelectObject(&m_penTsub);
		x = m_nScaleLeft + (int)((t1s - startTime) / dispTime * m_nScaleWidth);
		y = m_nScaleZero;
		m_dcMem.MoveTo(x, y);
		x = m_nScaleLeft + (int)((t1s + (nMinLevel / dev - startTime)) / dispTime * m_nScaleWidth);
		y = m_nScaleBottom;
		m_dcMem.LineTo(x, y);

		m_dcMem.SelectObject(&m_penDot);
		x = m_nScaleLeft + (int)((t1s - startTime) / dispTime * m_nScaleWidth);
		m_dcMem.MoveTo(x, m_nScaleTop);
		m_dcMem.LineTo(x, m_nScaleBottom);
		x = m_nScaleLeft + (int)((fEndTime - startTime) / dispTime * m_nScaleWidth);
		m_dcMem.MoveTo(x, m_nScaleTop);
		m_dcMem.LineTo(x, m_nScaleBottom);
	}

	const int nT0Pos = (int)(t0 / 1000 * fRate);
	const int nT1Pos = (int)(t1 / 1000 * fRate);
	if (fT20Reg1 != 0) {
		fOffset = -(fT20Reg0 + pData[nT0Pos] - pData[nT1Pos]) / fT20Reg1;
		m_dcMem.SelectObject(&m_penT20);
		x = m_nScaleLeft + (int)((fOffset - startTime) / dispTime * m_nScaleWidth);
		y = m_nScaleZero;
		m_dcMem.MoveTo(x, y);
		x = m_nScaleLeft + (int)((fOffset + (nMinLevel / fT20Reg1 - startTime)) / dispTime * m_nScaleWidth);
		y = m_nScaleBottom;
		m_dcMem.LineTo(x, y);
	}

	if (fT30Reg1 != 0) {
		fOffset = -(fT30Reg0 + pData[nT0Pos] - pData[nT1Pos]) / fT30Reg1;
		m_dcMem.SelectObject(&m_penT30);
		x = m_nScaleLeft + (int)((fOffset - startTime) / dispTime * m_nScaleWidth);
		y = m_nScaleZero;
		m_dcMem.MoveTo(x, y);
		x = m_nScaleLeft + (int)((fOffset + (nMinLevel / fT30Reg1 - startTime)) / dispTime * m_nScaleWidth);
		y = m_nScaleBottom;
		m_dcMem.LineTo(x, y);
	}

	if (fEDTReg1 != 0) {
		fOffset = -(fEDTReg0 + pData[nT0Pos] - pData[nT1Pos]) / fEDTReg1;
		m_dcMem.SelectObject(&m_penEDT);
		x = m_nScaleLeft + (int)((fOffset - startTime) / dispTime * m_nScaleWidth);
		y = m_nScaleZero;
		m_dcMem.MoveTo(x, y);
		x = m_nScaleLeft + (int)((fOffset + (nMinLevel / fEDTReg1 - startTime)) / dispTime * m_nScaleWidth);
		y = m_nScaleBottom;
		m_dcMem.LineTo(x, y);
	}

	m_dcMem.SelectObject(&m_penData);

	fOffset = startTime / totalTime * nData;
	i2 = int(fOffset);

	if (pData != NULL) {
		for (i = i2; i < nData - 1; i++) {
			x = m_nScaleLeft + int((i - fOffset) * totalTime / dispTime * m_nScaleWidth / nData);
			y = m_nScaleZero - (int)((double)pData[i] * m_nScaleHeight / nLevelRange + 0.5);
			if (x >= m_nScaleRight)
				break;

			if (i == i2)
				m_dcMem.MoveTo(x, y);
			else {
				if (x != x2 || y != y2)
					m_dcMem.LineTo(x, y);
			}
			x2 = x;
			y2 = y;
		}
	}

	if (pData2 != NULL) {
		m_dcMem.SelectObject(&m_penData2);
		for (i = i2; i < nData - 1; i++) {
			x = m_nScaleLeft + int((i - i2) * totalTime / dispTime * m_nScaleWidth / nData + 0.5);
			y = m_nScaleZero - (int)((double)pData2[i] * m_nScaleHeight / nLevelRange + 0.5);
			if (x >= m_nScaleRight)
				break;

			if (i == i2)
				m_dcMem.MoveTo(x, y);
			else {
				if (x != x2 || y != y2)
					m_dcMem.LineTo(x, y);
			}
			x2 = x;
			y2 = y;
		}
	}

	m_dcMem.SelectObject(pBitmapOrg);
	m_dcMem.SelectObject(pPenOrg);

	m_dcMem.SelectClipRgn(NULL);

	if (!m_sTitle.IsEmpty()) {
		CString str1, str2, str3, str4, str5, str6;
		REMARK_INFO remark;
		int n = 0;
		remark.remarks[n].pen = NULL;
		remark.remarks[n++].text = m_sTitle;
		if (pData2 != NULL) {
			remark.remarks[n].pen = &m_penData2;
			str1.LoadString(IDS_WITHOUTNOISE);
			remark.remarks[n++].text = str1;
		}
		if (pData != NULL) {
			remark.remarks[n].pen = &m_penData;
			str2.LoadString(IDS_WITHNOISE);
			remark.remarks[n++].text = str2;
		}
		if (dev != 0) {
			remark.remarks[n].pen = &m_penTsub;
			str3.LoadString(IDS_REGRESSIONLINE);
			str3 = "Tsub " + str3;
			remark.remarks[n++].text = str3;
			//remark.remarks[n].pen = &m_penDot;
			//remark.remarks[n++].text = "Tsub";
		}
		if (fT20Reg1 != 0) {
			remark.remarks[n].pen = &m_penT20;
			str4.LoadString(IDS_REGRESSIONLINE);
			str4 = "T20 " + str4;
			remark.remarks[n++].text = str4;
		}
		if (fT30Reg1 != 0) {
			remark.remarks[n].pen = &m_penT30;
			str5.LoadString(IDS_REGRESSIONLINE);
			str5 = "T30 " + str5;
			remark.remarks[n++].text = str5;
		}
		if (fEDTReg1 != 0) {
			remark.remarks[n].pen = &m_penEDT;
			str6.LoadString(IDS_REGRESSIONLINE);
			str6 = "EDT " + str6;
			remark.remarks[n++].text = str6;
		}
		remark.nRemark = n;
		m_wndRemark.DispRemarks(&remark);
	}

	Invalidate(FALSE);
	UpdateWindow();
}

void CEchoWnd::DispEnergy(double totalTime, double startTime, double dispTime, const double *pData, int nData, int nMaxLevel, int nMinLevel, BOOL bHScroll)
{
	int i, i2;
	int x, x2 = 0;
	int y;
	const int nLevelRange = nMaxLevel - nMinLevel;
	double data;
	int nCount;
	BOOL bFirst = TRUE;

	m_bHScroll = bHScroll;

	SetBitmap(startTime, dispTime, nMaxLevel, nMinLevel);

	m_dcMem.SelectClipRgn(&m_Rgn);
	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
	CPen *pPenOrg = m_dcMem.SelectObject(&m_penData);

	data= 0;
	nCount = 0;
	i2 = int(startTime / totalTime * nData);
	for (i = i2; i < nData - 1; i++) {
		x = m_nScaleLeft + int((i - i2) * totalTime / dispTime * m_nScaleWidth / nData + 0.5);
		if (x >= m_nScaleRight)
			break;

		if (i == i2)
			x2 = x;

		if (x != x2) {
			if (data != 0)
				data = log10(data / nCount) * 10;
			else
				data = -1000;
			y = m_nScaleZero - (int)(data * m_nScaleHeight / nLevelRange + 0.5);
			if (bFirst) {
				m_dcMem.MoveTo(x2, y);
				bFirst = FALSE;
			}
			m_dcMem.LineTo(x2, y);
			x2 = x;
			data = 0;
			nCount = 0;
		}

		data += pData[i];
		nCount++;
	}

	m_dcMem.SelectClipRgn(NULL);

	m_dcMem.SelectObject(pBitmapOrg);
	m_dcMem.SelectObject(pPenOrg);

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

void CEchoWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (point.y > m_nScaleTop && point.y < m_nScaleBottom) {
		if (m_nMouseMode == MOUSE_SELECT_NONE) {
			m_nScrollPos = point.x;
			m_nMouseMode = MOUSE_SCROLL;
			SetCapture();
		}

		SetMouseCursor(point);
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void CEchoWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_nMouseMode != MOUSE_SELECT_NONE) {
		m_nMouseMode = MOUSE_SELECT_NONE;
		ReleaseCapture();

		SetMouseCursor(point);
	}

	CWnd::OnLButtonUp(nFlags, point);
}

void CEchoWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	switch (m_nMouseMode) {
	case MOUSE_SCROLL:
		m_pCallBackFunc(CC_SCROLL, m_dwCallBackData, (double)(point.x - m_nScrollPos) / m_nScaleWidth, 0);
		m_nScrollPos = point.x;
		break;
	}

	SetMouseCursor(point);

	CWnd::OnMouseMove(nFlags, point);
}

BOOL CEchoWnd::OnSetCursor(CWnd* /*pWnd*/, UINT /*nHitTest*/, UINT /*message*/)
{
	return TRUE;
}

void CEchoWnd::SetMouseCursor(CPoint /*point*/) const
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
