// FreqWnd.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "FreqWnd.h"
#include "common.h"
#include "comres.h"
#include "Dpi.h"

/////////////////////////////////////////////////////////////////////////////
// CFreqWnd

CFreqWnd::CFreqWnd()
{
	m_bInitialized = FALSE;
}

BEGIN_MESSAGE_MAP(CFreqWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFreqWnd メッセージ ハンドラ

void CFreqWnd::Initialize(int nFontSize, LPCSTR pTitle, int nRemark, COLORREF freqColor, COLORREF phaseColor)
{
	if (m_bInitialized)
		return;

	m_bInitialized = TRUE;

	RECT rect;
	GetClientRect(&rect);

	m_nWidth = rect.right;
	m_nHeight = rect.bottom;

	CDC *pDC = GetDC();
	m_dcMem.CreateCompatibleDC(pDC);
	m_bitmapMem.CreateCompatibleBitmap(pDC, m_nWidth, m_nHeight);
	ReleaseDC(pDC);

	m_penFreq.CreatePen(PS_SOLID, 0, freqColor);
	m_penPhase.CreatePen(PS_SOLID, 0, phaseColor);
	m_penBlack.CreatePen(PS_SOLID, 0, RGB(0, 0, 0));
	m_penGray.CreatePen(PS_SOLID, 0, RGB(128, 128, 128));

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
	font.lfEscapement = -900;
	m_Font3.CreatePointFontIndirect(&font, &m_dcMem);

	m_dcMem.SetBkMode(TRANSPARENT);
	m_dcMem.SetTextColor(RGB(64, 64, 64));

	if (pTitle != NULL) {
		CString str1, str2;
		str1.LoadString(IDS_FREQRESP);
		str2.LoadString(IDS_PHASE);

		REMARK_INFO remark{
			.nRemark = nRemark,
			.remarks{
				{
					.pen = NULL,
					.text = pTitle
				},{
					.pen = &m_penFreq,
					.text = str1
				},{
					.pen = &m_penPhase,
					.text = str2
				}
			}
		};
		m_wndRemark.Create(this);
		m_wndRemark.DispRemarks(&remark);
	}
}

void CFreqWnd::SetBitmap(int nScale, int nMinFreq, int nMaxFreq, int nMinLevel, int nMaxLevel, BOOL bPhase)
{
	int	nFreq, nLevel;
	int x, y;
	CString sText;
	CSize size;

	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
	CFont *pFontOrg = m_dcMem.SelectObject(&m_Font);
	CPen *pPenOrg = m_dcMem.SelectObject(&m_penBlack);

	TEXTMETRIC tm;
	m_dcMem.GetTextMetrics(&tm);

	m_nScaleLeft = tm.tmAveCharWidth * 4 + tm.tmHeight + 6;
	m_nScaleTop = 10;
	if (bPhase)
		m_nScaleRight = m_nWidth - (tm.tmAveCharWidth * 4 + tm.tmHeight + 5);
	else
		m_nScaleRight = m_nWidth - 10;
	m_nScaleBottom = m_nHeight - ((tm.tmHeight + 1) * 2 + 2);
	m_nScaleWidth = m_nScaleRight - m_nScaleLeft;
	m_nScaleHeight = m_nScaleBottom - m_nScaleTop;
	m_nScaleCenter = m_nScaleTop + m_nScaleHeight / 2;

	CRect rect;
	rect.SetRect(0, 0, m_nWidth, m_nHeight);
	m_dcMem.FillRect(rect, &m_brushWhite);

	rect.SetRect(m_nScaleLeft, m_nScaleTop, m_nScaleRight, m_nScaleBottom);
	m_dcMem.Rectangle(rect);

	m_dcMem.SelectObject(&m_penGray);

	if (nScale == 0) {
		const double fMaxFreqLog = log((double)nMaxFreq);
		const double fMinFreqLog = log((double)nMinFreq);
		const double fSpanFreq = fMaxFreqLog - fMinFreqLog;
		for (nFreq = nMinFreq; nFreq <= nMaxFreq; ) {
			x =  m_nScaleLeft + (int)((log((double)nFreq) - fMinFreqLog) * m_nScaleWidth / fSpanFreq + 0.5);

			if (x > m_nScaleLeft && x < m_nScaleRight) {
				m_dcMem.MoveTo(x, m_nScaleTop);
				m_dcMem.LineTo(x, m_nScaleBottom);
			}

			if (nFreq < 1000)
				sText.Format("%d", nFreq);
			else
				sText.Format("%dk", nFreq / 1000);
			if (sText[0] == '1' || sText[0] == '2' || sText[0] == '5') {
				size = m_dcMem.GetOutputTextExtent(sText);
				m_dcMem.TextOut(x - size.cx / 2, m_nScaleBottom + 2, sText);
			}

			if (nFreq < 10)
				nFreq += 1;
			else if (nFreq < 100)
				nFreq += 10;
			else if (nFreq < 1000)
				nFreq += 100;
			else if (nFreq < 10000)
				nFreq += 1000;
			else
				nFreq += 10000;
		}
	} else {
		int step = (nMaxFreq - nMinFreq) / 5;
		if (step < 9)
			step = 1;
		else if (step < 99)
			step = 10;
		else if (step < 999)
			step = 100;
		else if (step < 9999)
			step = 1000;
		else
			step = 10000;

		const int start = (nMinFreq + step - 1) / step * step;
		const double fSpanFreq = nMaxFreq - nMinFreq;
		for (nFreq = start; nFreq <= nMaxFreq; nFreq += step) {
			x = m_nScaleLeft + (int)((nFreq - nMinFreq) * m_nScaleWidth / fSpanFreq);

			if (x > m_nScaleLeft && x < m_nScaleRight) {
				m_dcMem.MoveTo(x, m_nScaleTop + 1);
				m_dcMem.LineTo(x, m_nScaleBottom - 1);
			}

			if (nFreq < 1000)
				sText.Format("%d", nFreq);
			else
				sText.Format("%dk", nFreq / 1000);
			size = m_dcMem.GetOutputTextExtent(sText);
			m_dcMem.TextOut(x - size.cx / 2, m_nScaleBottom + 2, sText);
		}
	}

	if (bPhase) {
		for (int phase = 180; phase >= -180; phase -= 90) {
			y = m_nScaleCenter - phase * m_nScaleHeight / 2 / 180;
			m_dcMem.MoveTo(m_nScaleRight - 8, y);
			m_dcMem.LineTo(m_nScaleRight, y);

			sText.Format("%+d", phase);
			size = m_dcMem.GetOutputTextExtent(sText);
			m_dcMem.TextOut(m_nScaleRight + 2, y - size.cy / 2, sText);
		}
	}

	const int step = ((nMaxLevel - nMinLevel) / 120 + 1) * 10;
	const double fSpanLevel = nMaxLevel - nMinLevel;
	for (nLevel = nMaxLevel; nLevel >= nMinLevel; nLevel -= step) {
		y = m_nScaleBottom - (int)((nLevel - nMinLevel) * m_nScaleHeight / fSpanLevel);

		if (y > m_nScaleTop && y < m_nScaleBottom) {
			m_dcMem.MoveTo(m_nScaleLeft, y);
			m_dcMem.LineTo(m_nScaleRight, y);
		}

		sText.Format("%d", nLevel);
		size = m_dcMem.GetOutputTextExtent(sText);
		m_dcMem.TextOut(m_nScaleLeft - size.cx - 2, y - size.cy / 2, sText);
	}

	sText.LoadString(IDS_FREQUENCY);
	sText += " [Hz]";
	size = m_dcMem.GetOutputTextExtent(sText);
	m_dcMem.TextOut(m_nScaleLeft + (m_nScaleWidth - size.cx) / 2, m_nHeight - (size.cy + 1), sText);

	m_dcMem.SelectObject(&m_Font2);
	sText.LoadString(IDS_LEVEL);
	sText += " [dB]";
	size = m_dcMem.GetOutputTextExtent(sText);
	m_dcMem.TextOut(1, m_nScaleTop + (m_nScaleHeight + size.cx) / 2, sText);

	if (bPhase) {
		m_dcMem.SelectObject(&m_Font3);
		sText.LoadString(IDS_PHASE);
		sText += " [deg.]";
		size = m_dcMem.GetOutputTextExtent(sText);
		m_dcMem.TextOut(m_nWidth - 1, m_nScaleTop + (m_nScaleHeight - size.cx) / 2, sText);
	}

	m_dcMem.SelectObject(pBitmapOrg);
	m_dcMem.SelectObject(pFontOrg);
	m_dcMem.SelectObject(pPenOrg);
}

void CFreqWnd::DispGraph(const double *pFreqData, int nData, double fRate, int nMinFreq, int nMaxFreq, int nMinLevel, int nMaxLevel, const double *pPhaseData, BOOL bMean)
{
	const double fMaxFreqLog = log((double)nMaxFreq);
	const double fMinFreqLog = log((double)nMinFreq);
	int nScale = 0;
	int i;
	int x, x2 = 0;
	int y, y2 = 0;

	SetBitmap(nScale, nMinFreq, nMaxFreq, nMinLevel, nMaxLevel, pPhaseData != NULL);

	CRgn rgn;
	rgn.CreateRectRgn(m_nScaleLeft, m_nScaleTop, m_nScaleRight - 1, m_nScaleBottom);
	m_dcMem.SelectClipRgn(&rgn);

	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
	CPen *pPenOrg = m_dcMem.SelectObject(&m_penBlack);

	const double fFreqStep = fRate / nData;
	const double fStepY = (double)m_nScaleHeight / (nMinLevel - nMaxLevel);
	const double fTmp1 = (double)m_nScaleWidth / (fMaxFreqLog - fMinFreqLog);
	const double fTmp2 = (double)m_nScaleWidth / (nMaxFreq - nMinFreq);

	if (pPhaseData != NULL) {
		m_dcMem.SelectObject(&m_penPhase);
		for (i = 1; i < nData / 2; i++) {
			if (nScale == 0)
				x = m_nScaleLeft + (int)((log(i * fFreqStep) - fMinFreqLog) * fTmp1 + 0.5);
			else
				x = m_nScaleLeft + (int)((i * fFreqStep - nMinFreq) * fTmp2 + 0.5);

			y = m_nScaleCenter - (int)(pPhaseData[i] / M_PI * m_nScaleHeight / 2);

			if (i == 1)
				m_dcMem.MoveTo(x, y);
			else {
				if (x != x2 || y != y2)
					m_dcMem.LineTo(x, y);
			}
			x2 = x;
			y2 = y;
		}
	}

	m_dcMem.SelectObject(&m_penFreq);
	if (bMean) {
		double fLevel= 0;
		int nCount = 0;
		BOOL bFirst = TRUE;
		for (i = 1; i < nData / 2; i++) {
			if (nScale == 0)
				x = m_nScaleLeft + (int)((log(i * fFreqStep) - fMinFreqLog) * fTmp1 + 0.5);
			else
				x = m_nScaleLeft + (int)((i * fFreqStep - nMinFreq) * fTmp2 + 0.5);

			if (i == 1)
				x2 = x;

			if (x != x2) {
				fLevel /= nCount;
				if (fLevel != 0)
					y = m_nScaleTop + (int)((log10(fLevel) * 10 - nMaxLevel) * fStepY + 0.5);
				else
					y = m_nScaleBottom;
				if (bFirst) {
					m_dcMem.MoveTo(x2, y);
					bFirst = FALSE;
				}
				m_dcMem.LineTo(x2, y);
				x2 = x;
				fLevel = 0;
				nCount = 0;
			}

			fLevel += pFreqData[i];
			nCount++;
		}
	} else {
		for (i = 1; i < nData / 2; i++) {
			if (nScale == 0)
				x = m_nScaleLeft + (int)((log(i * fFreqStep) - fMinFreqLog) * fTmp1 + 0.5);
			else
				x = m_nScaleLeft + (int)((i * fFreqStep - nMinFreq) * fTmp2 + 0.5);

			if (pFreqData[i] != 0)
				y = m_nScaleTop + (int)((log10(pFreqData[i]) * 10 - nMaxLevel) * fStepY + 0.5);
			else
				y = m_nScaleBottom;

			if (i == 1)
				m_dcMem.MoveTo(x, y);
			else {
				if (x != x2 || y != y2)
					m_dcMem.LineTo(x, y);
			}
			x2 = x;
			y2 = y;
		}
	}

	m_dcMem.SelectClipRgn(NULL);

	m_dcMem.SelectObject(pBitmapOrg);
	m_dcMem.SelectObject(pPenOrg);

	Invalidate(FALSE);
	UpdateWindow();
}

void CFreqWnd::OnPaint()
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

BOOL CFreqWnd::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}
