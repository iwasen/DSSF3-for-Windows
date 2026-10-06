// MtfWnd.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "MtfWnd.h"
#include "comres.h"
#include "resource.h"
#include "common.h"
#include "Calc.h"

#define MIN_FREQ	0.5
#define MAX_FREQ	15

/////////////////////////////////////////////////////////////////////////////
// CMtfWnd

CMtfWnd::CMtfWnd()
{
	m_bInitialize = FALSE;
}

BEGIN_MESSAGE_MAP(CMtfWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMtfWnd メッセージ ハンドラ

void CMtfWnd::Initialize(int nFontSize, LPCSTR pTitle, int nRemark, COLORREF color)
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

	m_penMtf.CreatePen(PS_SOLID, 0, color);
	m_penBlack.CreatePen(PS_SOLID, 0, RGB(0, 0, 0));
	m_penGray.CreatePen(PS_SOLID, 0, RGB(128, 128, 128));

	m_brushWhite.CreateSolidBrush(RGB(255, 255, 255));
	m_brushData.CreateSolidBrush(color);

	CString str;
	LOGFONT font{
		.lfHeight = nFontSize,
		.lfCharSet = DEFAULT_CHARSET
	};
	str.LoadString(IDS_DEFAULTFONT);
	strcpy_s(font.lfFaceName, str);
	m_Font.CreatePointFontIndirect(&font, &m_dcMem);
	font.lfEscapement = 900;
	m_Font2.CreatePointFontIndirect(&font, &m_dcMem);

	m_dcMem.SetBkMode(TRANSPARENT);
	m_dcMem.SetTextColor(RGB(64, 64, 64));

	if (pTitle != NULL) {
		CString str1;
		str1.LoadString(IDS_FREQRESP);

		REMARK_INFO remark{
			.nRemark = nRemark,
			.remarks{
				{
					.pen = NULL,
					.text = pTitle
				},{
					.pen = &m_penMtf,
					.text = str1
				}
			}
		};
		m_wndRemark.Create(this);
		m_wndRemark.DispRemarks(&remark);
	}
}

void CMtfWnd::SetBitmap()
{
	CString text;
	CSize size;

	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
	CFont *pFontOrg = m_dcMem.SelectObject(&m_Font);
	CPen *pPenOrg = m_dcMem.SelectObject(&m_penBlack);

	TEXTMETRIC tm;
	m_dcMem.GetTextMetrics(&tm);

	m_nScaleLeft = tm.tmAveCharWidth * 4 + tm.tmHeight + 5;
	m_nScaleTop = 10;
	m_nScaleRight = m_nWidth - 10;
	m_nScaleBottom = m_nHeight - ((tm.tmHeight + 1) * 2 + 1);
	m_nScaleWidth = m_nScaleRight - m_nScaleLeft;
	m_nScaleHeight = m_nScaleBottom - m_nScaleTop;
	m_nScaleCenter = m_nScaleTop + m_nScaleHeight / 2;

	CRect rect;
	rect.SetRect(0, 0, m_nWidth, m_nHeight);
	m_dcMem.FillRect(rect, &m_brushWhite);

	rect.SetRect(m_nScaleLeft, m_nScaleTop, m_nScaleRight, m_nScaleBottom);
	m_dcMem.Rectangle(rect);

	m_dcMem.SelectObject(&m_penGray);

	const int minFreq = (int)(MIN_FREQ * 10);
	const int maxFreq = (int)(MAX_FREQ * 10);
	const double fMaxFreqLog = log((double)maxFreq);
	const double fMinFreqLog = log((double)minFreq);
	for (int nFreq = minFreq; nFreq <= maxFreq; ) {
		const int x =  (int)((log((double)nFreq) - fMinFreqLog) *
					m_nScaleWidth / (fMaxFreqLog - fMinFreqLog) + m_nScaleLeft + 0.5);

		if (x > m_nScaleLeft && x < m_nScaleRight) {
			m_dcMem.MoveTo(x, m_nScaleTop);
			m_dcMem.LineTo(x, m_nScaleBottom);
		}

		if (nFreq != minFreq) {
			text.Format("%g", (double)nFreq / 10);
			if (text[0] == '0')
				text = text.Mid(1);
			size = m_dcMem.GetOutputTextExtent(text);
			m_dcMem.TextOut(x - size.cx / 2, m_nScaleBottom + 2, text);
		}

		if (nFreq < 10)
			nFreq += 1;
		else if (nFreq < 100)
			nFreq += 10;
		else
			nFreq += 100;
	}

	for (int nLevel = 0; nLevel <= 10; nLevel++) {
		const int y = m_nScaleBottom - nLevel * m_nScaleHeight / 10;

		if (y > m_nScaleTop && y < m_nScaleBottom) {
			m_dcMem.MoveTo(m_nScaleLeft, y);
			m_dcMem.LineTo(m_nScaleRight, y);
		}

		text.Format("%g", (double)nLevel / 10);
		size = m_dcMem.GetOutputTextExtent(text);
		m_dcMem.TextOut(m_nScaleLeft - size.cx - 2, y - size.cy / 2, text);
	}

	text.LoadString(IDS_MOD_FREQUENCY);
	text += " [Hz]";
	size = m_dcMem.GetOutputTextExtent(text);
	m_dcMem.TextOut(m_nScaleLeft + (m_nScaleWidth - size.cx) / 2, m_nHeight - (size.cy + 1), text);

	m_dcMem.SelectObject(&m_Font2);
	text = "MTF";
	size = m_dcMem.GetOutputTextExtent(text);
	m_dcMem.TextOut(1, m_nScaleTop + (m_nScaleHeight + size.cx) / 2, text);

	m_dcMem.SelectObject(pBitmapOrg);
	m_dcMem.SelectObject(pFontOrg);
	m_dcMem.SelectObject(pPenOrg);
}

void CMtfWnd::DispGraph(const double *pMtfData, const double *pMtfFreq)
{
	const double fMaxFreqLog = log((double)MAX_FREQ);
	const double fMinFreqLog = log((double)MIN_FREQ);
	int x, y;

	SetBitmap();

	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
	CPen *pPenOrg = m_dcMem.SelectObject(&m_penMtf);
	CBrush *pBrushOrg = m_dcMem.SelectObject(&m_brushData);

	const double fTmp1 = (double)m_nScaleWidth / (fMaxFreqLog - fMinFreqLog);

	for (int i = 0; i < N_MTF_FREQ; i++) {
		x = m_nScaleLeft + (int)((log(pMtfFreq[i]) - fMinFreqLog) * fTmp1 + 0.5);
		y = m_nScaleBottom - (int)(pMtfData[i] * m_nScaleHeight + 0.5);

		if (i == 0)
			m_dcMem.MoveTo(x, y);
		else
			m_dcMem.LineTo(x, y);

		m_dcMem.Ellipse(x - 2, y - 2, x + 3, y + 3);
	}

	m_dcMem.SelectObject(pBitmapOrg);
	m_dcMem.SelectObject(pPenOrg);
	m_dcMem.SelectObject(pBrushOrg);

	Invalidate(FALSE);
	UpdateWindow();
}

void CMtfWnd::OnPaint()
{
	CPaintDC dc(this); // 描画用のデバイス コンテキスト

	if (IsWindowEnabled()) {
		CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
		dc.BitBlt(0, 0, m_nWidth, m_nHeight, &m_dcMem, 0, 0, SRCCOPY);
		m_dcMem.SelectObject(pBitmapOrg);
	} else {
		CBrush brush;
		RECT rect;
		GetClientRect(&rect);
		brush.CreateSysColorBrush(COLOR_MENU);
		dc.FillRect(&rect, &brush);
	}
}

BOOL CMtfWnd::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}
