// PrefWnd.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Sas.h"
#include "PrefWnd.h"

/////////////////////////////////////////////////////////////////////////////
// CPrefWnd

CPrefWnd::CPrefWnd()
{
	m_bInitialize = FALSE;
}

BEGIN_MESSAGE_MAP(CPrefWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrefWnd メッセージ ハンドラ

void CPrefWnd::OnPaint()
{
	CPaintDC dc(this);

	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
	dc.BitBlt(0, 0, m_nWidth, m_nHeight, &m_dcMem, 0, 0, SRCCOPY);
	m_dcMem.SelectObject(pBitmapOrg);
}

BOOL CPrefWnd::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void CPrefWnd::Initialize(int nFontSize)
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

	m_penData.CreatePen(PS_SOLID, 0, RGB(255, 0, 0));
	m_penTauE.CreatePen(PS_DOT, 0, RGB(0, 0, 255));
	m_penS.CreatePen(PS_DOT, 0, RGB(255, 0, 255));
	m_penBlack.CreatePen(PS_SOLID, 0, RGB(0, 0, 0));
	m_penGray.CreatePen(PS_SOLID, 0, RGB(128, 128, 128));
	m_penLightGray.CreatePen(PS_SOLID, 0, RGB(192, 192, 192));

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

	CFont *pFontOrg = m_dcMem.SelectObject(&m_Font);
	TEXTMETRIC tm;
	m_dcMem.GetTextMetrics(&tm);
	m_dcMem.SelectObject(pFontOrg);

	m_nScaleLeft = tm.tmAveCharWidth * 3 + tm.tmHeight + 4;
	m_nScaleTop = 10;
	m_nScaleRight = m_nWidth - 10;
	m_nScaleBottom = m_nHeight - ((tm.tmHeight + 1) * 2 + 1);
	m_nScaleWidth = m_nScaleRight - m_nScaleLeft;
	m_nScaleHeight = m_nScaleBottom - m_nScaleTop;

	CString str1, str2, str3;
	REMARK_INFO remark;
	remark.nRemark = 3;
	remark.remarks[0].pen = &m_penData;
	str1.LoadString(IDS_TAUEPREF);
	remark.remarks[0].text = str1;
	remark.remarks[1].pen = &m_penTauE;
	str2.LoadString(IDS_OPTIMUMTAUE);
	remark.remarks[1].text = str2;
	remark.remarks[2].pen = &m_penS;
	str3.LoadString(IDS_MAXPREF);
	remark.remarks[2].text = str3;
	m_wndRemark.Create(this);
	m_wndRemark.DispRemarks(&remark);
}

void CPrefWnd::SetBitmap(int minS, int maxTe)
{
	int	t, s;
	int x, y;
	CString text;
	CSize size;

	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
	CPen *pPenOrg = m_dcMem.SelectObject(&m_penGray);
	CFont *pFontOrg = m_dcMem.SelectObject(&m_Font);

	CRect rect;
	rect.SetRect(0, 0, m_nWidth, m_nHeight);
	m_dcMem.FillRect(rect, &m_brushWhite);

	for (t = 0; t <= maxTe; t += 10) {
		if (t % 20 == 0)
			m_dcMem.SelectObject(&m_penGray);
		else
			m_dcMem.SelectObject(&m_penLightGray);

		x = m_nScaleLeft + t * m_nScaleWidth / maxTe;
		m_dcMem.MoveTo(x, m_nScaleTop);
		m_dcMem.LineTo(x, m_nScaleBottom);

		if (t % 20 == 0) {
			text.Format("%d", t);
			size = m_dcMem.GetOutputTextExtent(text);
			m_dcMem.TextOut(x - size.cx / 2, m_nScaleBottom + 1, text);
		}
	}

	for (s = 0; s >= minS * 2; s--) {
		if (s % 2 == 0)
			m_dcMem.SelectObject(&m_penGray);
		else
			m_dcMem.SelectObject(&m_penLightGray);

		y = m_nScaleTop + s * m_nScaleHeight / minS / 2;
		m_dcMem.MoveTo(m_nScaleLeft, y);
		m_dcMem.LineTo(m_nScaleRight, y);

		if (s % 2 == 0) {
			text.Format("%d", s / 2);
			size = m_dcMem.GetOutputTextExtent(text);
			m_dcMem.TextOut(m_nScaleLeft - size.cx - 2, y - size.cy / 2, text);
		}
	}

	m_dcMem.SelectObject(&m_penBlack);
	m_dcMem.MoveTo(m_nScaleLeft, m_nScaleTop);
	m_dcMem.LineTo(m_nScaleRight, m_nScaleTop);
	m_dcMem.LineTo(m_nScaleRight, m_nScaleBottom);
	m_dcMem.LineTo(m_nScaleLeft, m_nScaleBottom);
	m_dcMem.LineTo(m_nScaleLeft, m_nScaleTop);

	text.LoadString(IDS_TAUE);
	text += " [ms]";
	size = m_dcMem.GetOutputTextExtent(text);
	m_dcMem.TextOut(m_nScaleLeft + (m_nScaleWidth - size.cx) / 2, m_nHeight - (size.cy + 1), text);

	m_dcMem.SelectObject(&m_Font2);
	text.LoadString(IDS_PREFVALUE);
	size = m_dcMem.GetOutputTextExtent(text);
	m_dcMem.TextOut(1, m_nScaleTop + (m_nScaleHeight + size.cx) / 2, text);

	m_dcMem.SelectObject(pBitmapOrg);
	m_dcMem.SelectObject(pPenOrg);
	m_dcMem.SelectObject(pFontOrg);
}

#define MIN_S	-10
void CPrefWnd::DispGraph(const double *pData, int nData, double maxTauE, double maxS)
{
	int i;
	int x, y;

	SetBitmap(MIN_S, nData);

	CRgn rgn;
	rgn.CreateRectRgn(m_nScaleLeft, m_nScaleTop, m_nScaleRight - 1, m_nScaleBottom);
	m_dcMem.SelectClipRgn(&rgn);

	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
	CPen *pPenOrg = m_dcMem.SelectObject(&m_penTauE);

	x = m_nScaleLeft + (int)(maxTauE * m_nScaleWidth / nData + 0.5);
	m_dcMem.MoveTo(x, m_nScaleTop);
	m_dcMem.LineTo(x, m_nScaleBottom);

	m_dcMem.SelectObject(&m_penS);
	y = m_nScaleTop + (int)(maxS * m_nScaleHeight / MIN_S + 0.5);
	m_dcMem.MoveTo(m_nScaleLeft, y);
	m_dcMem.LineTo(m_nScaleRight, y);

	m_dcMem.SelectObject(&m_penData);

	for (i = 0; i < nData; i++) {
		x = m_nScaleLeft + (i + 1) * m_nScaleWidth / nData;
		y = m_nScaleTop + (int)(pData[i] * m_nScaleHeight / MIN_S + 0.5);

		if (i == 0)
			m_dcMem.MoveTo(x, y);
		else
			m_dcMem.LineTo(x, y);
	}

	m_dcMem.SelectObject(pBitmapOrg);
	m_dcMem.SelectObject(pPenOrg);

	m_dcMem.SelectClipRgn(NULL);

	Invalidate(FALSE);
	UpdateWindow();
}
