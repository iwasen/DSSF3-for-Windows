// AcfSpgWnd.cpp : 実装ファイル
//

#include "stdafx.h"
#include "RTA.h"
#include "AcfSpgWnd.h"

#define COLOR_GRAPH_SCALE	RGB(0x00,0x00,0x00)
#define COLOR_LIGHT_GRAY	RGB(0xb0,0xb0,0xb0)
#define COLOR_MID_GRAY		RGB(0xaa,0xaa,0xaa)
#define COLOR_WHITE			RGB(0xff,0xff,0xff)

// CAcfSpgWnd

CAcfSpgWnd::CAcfSpgWnd()
{
	m_bDispGraph = FALSE;
	m_pSpgLevel = NULL;
}

CAcfSpgWnd::~CAcfSpgWnd()
{
	if (m_pSpgLevel != NULL)
		delete [] m_pSpgLevel;
}

BEGIN_MESSAGE_MAP(CAcfSpgWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CAcfSpgWnd メッセージ ハンドラ

void CAcfSpgWnd::Initialize()
{
	// ビューサイズ保存
	GetClientRect(m_rectView);

	// ペンの作成
	m_penScale.CreatePen(PS_SOLID, 1, COLOR_GRAPH_SCALE);
	m_penGray.CreatePen(PS_SOLID, 1, COLOR_GRAY);
	m_penLightGray.CreatePen(PS_SOLID, 1, COLOR_LIGHT_GRAY);
	m_penMidGray.CreatePen(PS_SOLID, 1, COLOR_MID_GRAY);

	// メモリデバイスコンテキストを作成
	CDC *pDC = GetDC();
	m_dcGraph.CreateCompatibleDC(pDC);
	m_bmpGraph.CreateCompatibleBitmap(pDC, m_rectView.Width(), m_rectView.Height());

	CString str;
	LOGFONT font{};
	font.lfHeight = CDpi::OriginalDpi(90, 88);
	font.lfCharSet = DEFAULT_CHARSET;
	str.LoadString(IDS_DEFAULTFONT);
	strcpy_s(font.lfFaceName, str);
	m_fontGraph.CreatePointFontIndirect(&font, &m_dcGraph);
	font.lfEscapement = 900;
	m_fontGraph2.CreatePointFontIndirect(&font, &m_dcGraph);

	// テキストサイズ取得
	CFont *pFontOrg = m_dcGraph.SelectObject(&m_fontGraph);
	TEXTMETRIC tm;
	m_dcGraph.GetTextMetrics(&tm);
	m_dcGraph.SelectObject(pFontOrg);

	// グラフのスケールの位置と大きさを設定
	m_rectScale.left = m_rectView.left + tm.tmAveCharWidth * 4 + tm.tmHeight * 2;
	m_rectScale.top = m_rectView.top + tm.tmHeight;
	m_rectScale.right = m_rectView.right - (tm.tmHeight * 3 + tm.tmAveCharWidth * 5 + 10);
	m_rectScale.bottom = m_rectView.bottom - tm.tmHeight * 3;

	// メモリデバイスコンテキストを作成
	m_dcGraph2.CreateCompatibleDC(pDC);
	m_bmpGraph2.CreateCompatibleBitmap(pDC, m_rectScale.Width(), m_rectScale.Height());

	ReleaseDC(pDC);

	m_pSpgLevel = new double[m_rectScale.Height()];
}

void CAcfSpgWnd::OnPaint()
{
	CPaintDC dc(this);

	if (m_bDispGraph) {
		// グラフのビットマップを画面に転送
		CBitmap *pBitmapOrg = m_dcGraph.SelectObject(&m_bmpGraph);
		dc.BitBlt(0, 0, m_rectView.Width(), m_rectView.Height(), &m_dcGraph, 0, 0, SRCCOPY);
		m_dcGraph.SelectObject(pBitmapOrg);
	}
}

BOOL CAcfSpgWnd::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void CAcfSpgWnd::DispGraph(const double *pData, int nFreqData, int nTimeData, int nTimeCount, int nSampleRate, double fTimeMin, double fTimeMax, double fTimeOffset, int nFreqMin, int nFreqMax, BOOL bFreqLog, int nLevelMin, int nLevelMax)
{
	if (m_hWnd == NULL)
		return;

	// メモリデバイスコンテキストにビットマップを設定
	CBitmap *pBitmapOrg = m_dcGraph.SelectObject(&m_bmpGraph);

	// 背景全体を黒で塗りつぶす
	m_dcGraph.FillSolidRect(m_rectView, COLOR_WHITE);

	// 透過モードに設定
	m_dcGraph.SetBkMode(TRANSPARENT);

	// データを描画
	DrawData(m_dcGraph, pData, nFreqData, nTimeData, nTimeCount, nSampleRate, nFreqMin, nFreqMax, bFreqLog, nLevelMin, nLevelMax);

	// スケールを描画
	DrawScale(m_dcGraph, fTimeMin + fTimeOffset, fTimeMax + fTimeOffset, nFreqMin, nFreqMax, bFreqLog, nLevelMin, nLevelMax);

	// メモリデバイスコンテキストからビットマップを解放
	m_dcGraph.SelectObject(pBitmapOrg);

	// グラフ表示フラグセット
	m_bDispGraph = TRUE;

	// ウィンドウの無効化
	Invalidate(FALSE);

	// ウィンドウを更新
	UpdateWindow();
}

void CAcfSpgWnd::DrawScale(CDC &dc, double fTimeMin, double fTimeMax, int nFreqMin, int nFreqMax, BOOL bFreqLog, int nLevelMin, int nLevelMax)
{
	int	nFreq;
	double fTime;
	double fStep;
	int x, y;
	CString sText;
	CSize sizeText;
	double fTimeSpan = fTimeMax - fTimeMin;
	int ns;
	double fLevelRange = nLevelMax - nLevelMin;
	double fLevel;
	int nHeight = m_rectScale.Height();

	// 描画オブジェクトをセット
	CPen *pPenOrg = dc.SelectObject(&m_penMidGray);
	CBrush *pBrushOrg = (CBrush *)dc.SelectStockObject(NULL_BRUSH);
	CFont *pFontOrg = dc.SelectObject(&m_fontGraph);

	// テキストサイズ取得
	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);

	// 横軸を描画
	fStep = GetLinearScaleStep(fTimeSpan, 8, &ns);
	for (fTime = GetScaleStartValue(fTimeMin, fStep); fTime <= fTimeMin + fTimeSpan; fTime += fStep) {
		x = m_rectScale.left + int((fTime - fTimeMin) * m_rectScale.Width() / fTimeSpan);
		if (x >= m_rectScale.left && x <= m_rectScale.right) {
			if (int(fTime / fStep + 0.5) % ns == 0) {
				if (x > m_rectScale.left && x < m_rectScale.right) {
					dc.MoveTo(x, m_rectScale.top);
					dc.LineTo(x, m_rectScale.bottom);
				}
				sText.Format("%g", fTime);
				sizeText = dc.GetOutputTextExtent(sText);
				dc.TextOut(x - sizeText.cx / 2, m_rectScale.bottom + tm.tmHeight / 2, sText);
			}
		}
	}

	// 縦軸を描画
	if (bFreqLog) {
		int step = GetLogScaleStep(nFreqMin);
		double fLogFreqMax = log((double)nFreqMax);
		double fLogFreqMin = log((double)nFreqMin);
		double fSpanFreq = fLogFreqMax - fLogFreqMin;
		for (nFreq = GetScaleStartValue(nFreqMin, step); nFreq <= nFreqMax; nFreq += GetLogScaleStep(nFreq)) {
			y = m_rectScale.top + (int)((fLogFreqMax - log((double)nFreq)) * nHeight / fSpanFreq + 0.5);

			if (nFreq < 1000)
				sText.Format("%d", nFreq);
			else
				sText.Format("%gk", (double)nFreq / 1000);

			if (sText[0] == '1' || sText[0] == '2' || sText[0] == '5') {
				CSize size = dc.GetOutputTextExtent(sText);
				dc.TextOut(m_rectScale.left - size.cx - 2, y - size.cy / 2, sText);
			}
		}
	} else {
		int step = GetLinearScaleStep(nFreqMax, nFreqMin, nHeight);
		double fSpanFreq = nFreqMax - nFreqMin;
		for (nFreq = GetScaleStartValue(nFreqMin, step); nFreq <= nFreqMax; nFreq += step) {
			y = m_rectScale.top + (int)((nFreqMax - nFreq) * nHeight / fSpanFreq + 0.5);

			if (nFreq < 1000)
				sText.Format("%d", nFreq);
			else
				sText.Format("%gk", (double)nFreq / 1000);

			CSize size = dc.GetOutputTextExtent(sText);
			dc.TextOut(m_rectScale.left - size.cx - 2, y - size.cy / 2, sText);
		}
	}

	// 外枠を描画
	dc.SelectObject(&m_penScale);
	dc.Rectangle(m_rectScale.left - 1, m_rectScale.top - 1, m_rectScale.right + 1, m_rectScale.bottom + 1);

	// 横軸の文字を描画
	sText.LoadString(IDS_TIME);
	sText += " [sec]";
	sizeText = dc.GetOutputTextExtent(sText);
	dc.TextOut(m_rectScale.left + (m_rectScale.Width() - sizeText.cx) / 2, m_rectView.bottom - (sizeText.cy + 4), sText);

	// 縦軸の文字を描画
	dc.SelectObject(&m_fontGraph2);
	sText.LoadString(IDS_FREQUENCY);
	sText += " [Hz]";
	sizeText = dc.GetOutputTextExtent(sText);
	dc.TextOut(m_rectView.left + 3, m_rectScale.top + (m_rectScale.Height() + sizeText.cx) / 2, sText);

	dc.SelectObject(&m_fontGraph);

	// カラースケールの枠を描画
	CRect rectColorScale(m_rectScale.right + tm.tmHeight * 2 + tm.tmAveCharWidth, m_rectScale.top - 1, m_rectScale.right + tm.tmHeight * 3 + 4, m_rectScale.bottom + 1);
	dc.Rectangle(rectColorScale);

	// カラースケールの文字を描画
	fStep = GetLinearScaleStep(fLevelRange, 2);
	for (fLevel = GetScaleStartValue((double)nLevelMin, fStep); fLevel <= nLevelMax; fLevel += fStep) {
		y = m_rectScale.top - (int)((fLevel - nLevelMax) * m_rectScale.Height() / fLevelRange + 0.5);

		sText.Format("%g", fLevel);
		sizeText = dc.GetOutputTextExtent(sText);
		dc.TextOut(rectColorScale.right + tm.tmAveCharWidth, y - sizeText.cy / 2, sText);
	}

	// カラースケールの色を描画
	for (y = m_rectScale.top; y <= m_rectScale.bottom; y++)
		dc.FillSolidRect(rectColorScale.left + 1, y, rectColorScale.Width() - 2, 1, GetLevelColor(nLevelMax - (y - m_rectScale.top) * fLevelRange / m_rectScale.Height(), nLevelMin, nLevelMax));

	sText.LoadString(IDS_LEVEL);
	sText += " [dB]";
	dc.SelectObject(&m_fontGraph2);
	sizeText = dc.GetOutputTextExtent(sText);
	dc.TextOut(rectColorScale.left - sizeText.cy - tm.tmAveCharWidth, rectColorScale.top + (rectColorScale.Height() + sizeText.cx) / 2, sText);

	// 描画オブジェクトを元に戻す
	dc.SelectObject(pPenOrg);
	dc.SelectObject(pBrushOrg);
	dc.SelectObject(pFontOrg);
}

COLORREF CAcfSpgWnd::GetLevelColor(double fLevel, double fLevelMin, double fLevelMax)
{
	int r, g, b;

	if (fLevel < fLevelMin)
		fLevel = fLevelMin;

	if (fLevel > fLevelMax)
		fLevel = fLevelMax;

	fLevel = (fLevel - fLevelMin) / (fLevelMax - fLevelMin);

	if (fLevel >= 0.8) {
		r = 255;
		g = 255 - (int)((fLevel - 0.8) * 5 * 255 + 0.5);
		b = 0;
	} else if (fLevel >= 0.6) {
		r = (int)((fLevel - 0.6) * 5 * 255 + 0.5);
		g = 255;
		b = 0;
	} else if (fLevel >= 0.4) {
		r = 0;
		g = 255;
		b = 255 - (int)((fLevel - 0.4) * 5 * 255 + 0.5);
	} else if (fLevel >= 0.2) {
		r = 0;
		g = (int)((fLevel - 0.2) * 5 * 255 + 0.5);
		b = 255;
	} else {
		r = 0;
		g = 0;
		b = (int)(fLevel * 5 * 255 + 0.5);
	}

	return RGB(r, g, b);
}

void CAcfSpgWnd::DrawData(CDC &dc, const double *pData, int nFreqData, int nTimeData, int nTimeCount, int nSampleRate, int nFreqMin, int nFreqMax, BOOL bFreqLog, int nLevelMin, int nLevelMax)
{
	int i;
	int x, y;
	int y2 = 0;
	int nScaleWidth = m_rectScale.Width();
	int nScaleHeight = m_rectScale.Height();
	double fLogFreqMax = log((double)nFreqMax);
	double fLogFreqMin = log((double)nFreqMin);
	double fTmp1 = (double)nScaleHeight / (fLogFreqMax - fLogFreqMin);
	double fTmp2 = (double)nScaleHeight / (nFreqMax - nFreqMin);
	double fTmp3 = exp(0.5 / fTmp1);
	double nLevel = 0;
	int nCount = 0;
	int t1, t2, t3;
	double fFreqStep = (double)nSampleRate / nFreqData / 2;
	int nTimeStep = (int)ceil((double)nScaleWidth / nTimeData);

	memset(m_pSpgLevel, 0, sizeof(double) * nScaleHeight);

	for (i = 1; i < nFreqData; i++) {
		if (bFreqLog)
			y = (int)((fLogFreqMax - log(fFreqStep * i)) * fTmp1 + 0.5);
		else
			y = (int)((nFreqMax - fFreqStep * i) * fTmp2 + 0.5);

		if (i == 1)
			y2 = y;

		if (y != y2) {
			if (y >= 0 && y < nScaleHeight) {
				if (nLevel == 0)
					m_pSpgLevel[y] = (double)nLevelMin;
				else {
					double tmp4, tmp5;

					if (g_oSetData.Fft.nFftScale == 0) {
						tmp4 = nFreqMax / exp(y / fTmp1);
						tmp5 = tmp4 * fTmp3 - tmp4 / fTmp3;
					} else
						tmp5 = 1 / fTmp2;

					m_pSpgLevel[y] = dB10(nLevel / nCount * tmp5 / fFreqStep);
				}
			}

			y2 = y;
			nLevel = 0;
			nCount = 0;
		}

		nLevel += pData[i];
		nCount++;
	}

	for (i = 0; i < nScaleHeight; i++) {
		if (m_pSpgLevel[i] == 0) {
			for (t1 = i; t1 >= 0 && m_pSpgLevel[t1] == 0; t1--)
				;
			for (t2 = i; t2 < nScaleHeight && m_pSpgLevel[t2] == 0; t2++)
				;
			if (t1 < 0)
				m_pSpgLevel[i] = m_pSpgLevel[t2];
			else if (t2 >= nScaleHeight)
				m_pSpgLevel[i] = m_pSpgLevel[t1];
			else {
				t3 = t2 - t1;
				m_pSpgLevel[i] = (m_pSpgLevel[t1] * (t2 - i) + m_pSpgLevel[t2] * (i - t1)) / t3;
			}
		}
	}

	CBitmap *pBitmapOrg = m_dcGraph2.SelectObject(&m_bmpGraph2);
	COLORREF color;

	if (nFreqData != 0) {
		if (nTimeCount < nTimeData)
			x = nTimeCount * nScaleWidth / nTimeData;
		else {
			CRect rect(0, 0, nScaleWidth, nScaleHeight);
			m_dcGraph2.ScrollDC(-nTimeStep, 0, rect, rect, NULL, NULL);
			x = nScaleWidth - nTimeStep;
		}

		for (y = 0; y < nScaleHeight; y++) {
			color = GetLevelColor(m_pSpgLevel[y], nLevelMin, nLevelMax);
			m_dcGraph2.FillSolidRect(x, y, nTimeStep, 1, color);
		}
	} else {
		CRect rect(0, 0, nScaleWidth, nScaleHeight);
		m_dcGraph2.FillSolidRect(rect, COLOR_BLACK);
	}

	dc.BitBlt(m_rectScale.left, m_rectScale.top, nScaleWidth, nScaleHeight, &m_dcGraph2, 0, 0, SRCCOPY);

	m_dcGraph2.SelectObject(pBitmapOrg);
}
