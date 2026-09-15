// ParamWnd.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Sas.h"
#include "ParamWnd.h"
#include "Calc.h"

#define COLOR_ALL	RGB(224, 0, 255)
#define COLOR_LEFT	RGB(0, 0, 255)
#define COLOR_RIGHT	RGB(255, 0, 0)

/////////////////////////////////////////////////////////////////////////////
// CParamWnd

CParamWnd::CParamWnd()
{
	m_bInitialize = FALSE;
}

BEGIN_MESSAGE_MAP(CParamWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParamWnd メッセージ ハンドラ

void CParamWnd::OnPaint()
{
	CPaintDC dc(this);

	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
	dc.BitBlt(0, 0, m_nWidth, m_nHeight, &m_dcMem, 0, 0, SRCCOPY);
	m_dcMem.SelectObject(pBitmapOrg);
}

BOOL CParamWnd::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void CParamWnd::Initialize(int nFontSize)
{
	if (m_bInitialize)
		return;

	m_bInitialize = TRUE;

	RECT rect;
	GetClientRect(&rect);

	m_nWidth = rect.right;
	m_nHeight = rect.bottom;

	CDC *pDC = GetDC();
	m_dcMem.CreateCompatibleDC(pDC);
	m_bitmapMem.CreateCompatibleBitmap(pDC, m_nWidth, m_nHeight);
	ReleaseDC(pDC);

	m_penAll.CreatePen(PS_SOLID, 0, COLOR_ALL);
	m_penLeft.CreatePen(PS_SOLID, 0, COLOR_LEFT);
	m_penRight.CreatePen(PS_SOLID, 0, COLOR_RIGHT);
	m_penBlack.CreatePen(PS_SOLID, 0, RGB(0, 0, 0));
	m_penGray.CreatePen(PS_SOLID, 0, RGB(128, 128, 128));

	m_brushAll.CreateSolidBrush(COLOR_ALL);
	m_brushLeft.CreateSolidBrush(COLOR_LEFT);
	m_brushRight.CreateSolidBrush(COLOR_RIGHT);
	m_brushWhite.CreateSolidBrush(RGB(255, 255, 255));

	CString str;
	LOGFONT font;
	memset(&font, 0, sizeof(font));
	font.lfHeight = nFontSize;
	font.lfCharSet = DEFAULT_CHARSET;
	str.LoadString(IDS_DEFAULTFONT);
	strcpy_s(font.lfFaceName, str);
	m_Font.CreatePointFontIndirect(&font, &m_dcMem);
	font.lfEscapement = 900;
	m_Font2.CreatePointFontIndirect(&font, &m_dcMem);

	m_dcMem.SetBkMode(TRANSPARENT);
	m_dcMem.SetTextColor(RGB(64, 64, 64));

	m_wndRemark.Create(this);
}

void CParamWnd::SetBitmap(const int *pDataFreq, int nFreqBand, int nData, double scaleMin, double scaleMax, double scaleStep, LPCTSTR vAxis)
{
	int	i;
	int x, y;
	double a;
	CString text;
	CSize size;
	int nLine = (nData > 16) ? 2 : 1;

	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
	CPen *pPenOrg = m_dcMem.SelectObject(&m_penGray);
	CFont *pFontOrg = m_dcMem.SelectObject(&m_Font);

	TEXTMETRIC tm;
	m_dcMem.GetTextMetrics(&tm);

	m_nScaleLeft = tm.tmAveCharWidth * 4 + tm.tmHeight + 5;
	m_nScaleTop = 10;
	m_nScaleRight = m_nWidth - 10;
	m_nScaleBottom = m_nHeight - ((tm.tmHeight + 1) * (nLine + 1) + 1);
	m_nScaleWidth = m_nScaleRight - m_nScaleLeft;
	m_nScaleHeight = m_nScaleBottom - m_nScaleTop;

	CRect rect;
	rect.SetRect(0, 0, m_nWidth, m_nHeight);
	m_dcMem.FillRect(rect, &m_brushWhite);

	for (i = 0; i < nData; i++) {
		x = m_nScaleLeft + (i + 1) * m_nScaleWidth / (nData + 1);
		m_dcMem.MoveTo(x, m_nScaleTop);
		m_dcMem.LineTo(x, m_nScaleBottom);

		GetDispFreq(pDataFreq[i], nFreqBand, text, TRUE);
		size = m_dcMem.GetOutputTextExtent(text);
		m_dcMem.TextOut(x - size.cx / 2, m_nScaleBottom + (i % nLine) * size.cy + nLine, text);
	}

	for (a = scaleMin; a <= scaleMax; a += scaleStep) {
		y = m_nScaleBottom - (int)((a - scaleMin) * m_nScaleHeight / (scaleMax - scaleMin) + 0.5);
		m_dcMem.MoveTo(m_nScaleLeft, y);
		m_dcMem.LineTo(m_nScaleRight, y);

		if (scaleStep >= 1.0)
			text.Format("%g", a);
		else
			text.Format("%.1lf", a);
		size = m_dcMem.GetOutputTextExtent(text);
		m_dcMem.TextOut(m_nScaleLeft - size.cx - 2, y - size.cy / 2, text);
	}

	m_dcMem.SelectObject(&m_penBlack);
	m_dcMem.MoveTo(m_nScaleLeft, m_nScaleTop);
	m_dcMem.LineTo(m_nScaleRight, m_nScaleTop);
	m_dcMem.LineTo(m_nScaleRight, m_nScaleBottom);
	m_dcMem.LineTo(m_nScaleLeft, m_nScaleBottom);
	m_dcMem.LineTo(m_nScaleLeft, m_nScaleTop);

	text.LoadString(IDS_FREQUENCY);
	text += " [Hz]";
	size = m_dcMem.GetOutputTextExtent(text);
	m_dcMem.TextOut(m_nScaleLeft + (m_nScaleWidth - size.cx) / 2, m_nHeight - (size.cy + 1), text);

	m_dcMem.SelectObject(&m_Font2);
	size = m_dcMem.GetOutputTextExtent(vAxis);
	m_dcMem.TextOut(1, m_nScaleTop + (m_nScaleHeight + size.cx) / 2, vAxis);

	m_dcMem.SelectObject(pBitmapOrg);
	m_dcMem.SelectObject(pPenOrg);
	m_dcMem.SelectObject(pFontOrg);
}

void CParamWnd::DispGraph(const int *pDataFreq, int nFreqBand, const double *pDataAll, const double *pDataLeft, const double *pDataRight, int nData, double scaleMin, double scaleMax, double scaleStep, LPCTSTR vAxis)
{
	SetBitmap(pDataFreq, nFreqBand, nData, scaleMin, scaleMax, scaleStep, vAxis);

	CRgn rgn;
	rgn.CreateRectRgn(m_nScaleLeft, m_nScaleTop, m_nScaleRight - 1, m_nScaleBottom);
	m_dcMem.SelectClipRgn(&rgn);

	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
	CPen *pPenOrg = m_dcMem.SelectObject(&m_penAll);
	CBrush *pBrushOrg = m_dcMem.SelectObject(&m_brushAll);

	if (pDataLeft != NULL)
		DispGraphSub(pDataFreq, pDataLeft, nData, scaleMin, scaleMax, 1);

	if (pDataRight != NULL)
		DispGraphSub(pDataFreq, pDataRight, nData, scaleMin, scaleMax, 2);

	if (pDataAll != NULL)
		DispGraphSub(pDataFreq, pDataAll, nData, scaleMin, scaleMax, 0);

	m_dcMem.SelectObject(pBitmapOrg);
	m_dcMem.SelectObject(pPenOrg);
	m_dcMem.SelectObject(pBrushOrg);

	m_dcMem.SelectClipRgn(NULL);

	Invalidate(FALSE);
	UpdateWindow();
}

void CParamWnd::DispGraphSub(const int *pDataFreq, const double *pData, int nData, double scaleMin, double scaleMax, int channel)
{
	int x, y;
	POINT point[3];
	BOOL bFirst = TRUE;

	for (int i = 0; i < nData; i++) {
		x = m_nScaleLeft + (i + 1) * m_nScaleWidth / (nData + 1);
		y = m_nScaleBottom - (int)((pData[i] - scaleMin) * m_nScaleHeight / (scaleMax - scaleMin) + 0.5);

		switch (channel) {
		case 0:
			m_dcMem.SelectObject(&m_penAll);
			m_dcMem.SelectObject(&m_brushAll);
			m_dcMem.Ellipse(x - 2, y - 2, x + 3, y + 3);
			break;
		case 1:
			m_dcMem.SelectObject(&m_penLeft);
			m_dcMem.SelectObject(&m_brushLeft);
			m_dcMem.Rectangle(x - 2, y - 2, x + 3, y + 3);
			break;
		case 2:
			m_dcMem.SelectObject(&m_penRight);
			m_dcMem.SelectObject(&m_brushRight);
			point[0].x = x - 2;
			point[0].y = y + 2;
			point[1].x = x;
			point[1].y = y - 2;
			point[2].x = x + 2;
			point[2].y = y + 2;
			m_dcMem.Polygon(point, 3);
			break;
		}

		if (pDataFreq[i] > 0) {
			if (bFirst) {
				m_dcMem.MoveTo(x, y);
				bFirst = FALSE;
			} else
				m_dcMem.LineTo(x, y);
		}
	}
}

void CParamWnd::DispRemark(LPCTSTR pTitle, int nRemark)
{
	CString str1, str2, str3;
	REMARK_INFO remark;

	remark.remarks[0].pen = NULL;
	remark.remarks[0].text = pTitle;

	remark.nRemark = nRemark;
	remark.remarks[1].pen = &m_penAll;
	str1.LoadString(IDS_LRAVERAGE);
	remark.remarks[1].text = str1;
	remark.remarks[2].pen = &m_penLeft;
	str2.LoadString(IDS_LEFTCHANNEL);
	remark.remarks[2].text = str2;
	remark.remarks[3].pen = &m_penRight;
	str3.LoadString(IDS_RIGHTCHANNEL);
	remark.remarks[3].text = str3;

	m_wndRemark.DispRemarks(&remark);
}
