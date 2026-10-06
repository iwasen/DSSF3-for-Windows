// FftWnd.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Rta.h"
#include "FftWnd.h"
#include "FftDlg.h"
#include "MakeFilter.h"
#include "FftWndEx.h"

#define COLOR_PHASE			RGB(0x00, 0x80, 0xc0)
#define COLOR_CROSS			RGB(0x50, 0xa0, 0x50)
#define COLOR_CROSS_PEAK	RGB(0x00, 0x80, 0x00)
#define COLOR_COHERENCE		RGB(0xa0, 0x50, 0x20)

/////////////////////////////////////////////////////////////////////////////
// CFftWnd

CFftWnd::CFftWnd()
{
	m_bClose = FALSE;
	m_pLogFreqTbl = NULL;
	m_pWindowFunc = NULL;
	m_pFftBufL = NULL;
	m_pFftBufR = NULL;
	m_pFftBufM = NULL;
	m_p3ddCheckTbl = NULL;
	m_p3ddWork = NULL;
	m_pCrossBufRe = NULL;
	m_pCrossBufIm = NULL;
	m_pPhsPoint = NULL;
	m_nPhsPointCount = 0;
	m_pCohPoint = NULL;
	m_nCohPointCount = 0;
	m_pCrsPoint = NULL;
	m_nCrsPointCount = 0;
	m_nCrsDispFreq = 0;
	m_nCrsFreqPos = 0;
	m_nCrsLevelPos = 0;
	m_pCrsPeakLevel = NULL;
	m_pCrsPeakPoint = NULL;
	m_pCrsBuf = NULL;
	m_nCrsPeakCount = 0;
	m_fCrossAllPower = 0;
	m_nPhsDispFreq = 0;
	m_nPhsFreqPos = 0;
	m_nPhsLevelPos = 0;
	m_nCohDispFreq = 0;
	m_nCohFreqPos = 0;
	m_nCohLevelPos = 0;
	m_bSmoothingReset = FALSE;
	m_hTheme = FALSE;
	m_bGetData = FALSE;
	m_bValidFftData = FALSE;

	memset(&m_aFftData, 0, sizeof(m_aFftData));

	m_oMicCalDataL.fInputSens = 0;
	m_oMicCalDataL.nFreqData = 0;
	m_oMicCalDataR.fInputSens = 0;
	m_oMicCalDataR.nFreqData = 0;
	m_pFilterTblL = NULL;
	m_pFilterTblR = NULL;
	m_bPeakReset = FALSE;
	m_pPhaseBuf = NULL;
	m_pCoherenceBuf = NULL;
	m_pSpgLevel = NULL;
	m_nChannel = 0;
	m_bLch = FALSE;
	m_bRch = FALSE;

	m_dcMem.CreateCompatibleDC(NULL);
	m_dcMem2.CreateCompatibleDC(NULL);
	m_dcMem3.CreateCompatibleDC(NULL);

	m_penLeft.CreatePen(PS_SOLID, 0, COLOR_LEFT);
	m_penRight.CreatePen(PS_SOLID, 0, COLOR_RIGHT);
	m_penLeftPeak.CreatePen(PS_SOLID, 0, COLOR_LEFT_PEAK);
	m_penRightPeak.CreatePen(PS_SOLID, 0, COLOR_RIGHT_PEAK);
	m_penLeftLine.CreatePen(PS_SOLID, 0, COLOR_LEFT_LINE);
	m_penRightLine.CreatePen(PS_SOLID, 0, COLOR_RIGHT_LINE);
	m_penGreen.CreatePen(PS_SOLID, 0, COLOR_GREEN);
	m_penLightGreen.CreatePen(PS_SOLID, 0, COLOR_LIGHTGREEN);
	m_penPhase.CreatePen(PS_SOLID, 0, COLOR_PHASE);
	m_penCross.CreatePen(PS_SOLID, 0, COLOR_CROSS);
	m_penCrossPeak.CreatePen(PS_SOLID, 0, COLOR_CROSS_PEAK);
	m_penCoherence.CreatePen(PS_SOLID, 0, COLOR_COHERENCE);
	m_penBlack.CreatePen(PS_SOLID, 0, COLOR_BLACK);
	m_penGray.CreatePen(PS_SOLID, 0, COLOR_GRAY);
	m_penLightGray.CreatePen(PS_SOLID, 0, COLOR_LIGHTGRAY);
	m_penMidGray.CreatePen(PS_SOLID, 0, RGB(0xaa, 0xaa, 0xaa));
	m_brushLeftHold.CreateSolidBrush(COLOR_BLUE);
	m_brushRightHold.CreateSolidBrush(COLOR_RED);
	m_brushGraph.CreateSolidBrush(COLOR_GRAPH);
	m_brushBlack.CreateSolidBrush(COLOR_BLACK);

	CString str;
	LOGFONT font{};
	if (g_oSetData2.nFontSizeFft == 0)
#ifdef _LANG_JPN
		font.lfHeight = 90;
#else
		font.lfHeight = 80;
#endif
	else
		font.lfHeight = g_oSetData2.nFontSizeFft * 10;
	font.lfHeight = CDpi::OriginalDpi(font.lfHeight, 88);
	font.lfCharSet = DEFAULT_CHARSET;
	str.LoadString(IDS_DEFAULTFONT);
	strcpy_s(font.lfFaceName, str);
	m_Font.CreatePointFontIndirect(&font, &m_dcMem);
	font.lfEscapement = 900;
	m_Font2.CreatePointFontIndirect(&font, &m_dcMem);
}

CFftWnd::~CFftWnd()
{
	RemoveFftWindowAll();

	FreeBuffer();
	FreeBuffer2(NULL);

	m_dcMem.DeleteDC();
	m_dcMem2.DeleteDC();
	m_dcMem3.DeleteDC();
	m_penLeft.DeleteObject();
	m_penRight.DeleteObject();
	m_penLeftPeak.DeleteObject();
	m_penRightPeak.DeleteObject();
	m_penLeftLine.DeleteObject();
	m_penRightLine.DeleteObject();
	m_penGreen.DeleteObject();
	m_penLightGreen.DeleteObject();
	m_penPhase.DeleteObject();
	m_penCross.DeleteObject();
	m_penCrossPeak.DeleteObject();
	m_penCoherence.DeleteObject();
	m_penBlack.DeleteObject();
	m_penGray.DeleteObject();
	m_penLightGray.DeleteObject();
	m_penMidGray.DeleteObject();
	m_Font.DeleteObject();
	m_Font2.DeleteObject();
	m_brushLeftHold.DeleteObject();
	m_brushRightHold.DeleteObject();
	m_brushGraph.DeleteObject();
	m_brushBlack.DeleteObject();
	m_bitmapLeft.DeleteObject();
	m_bitmapRight.DeleteObject();
	m_bitmapPeakLeft.DeleteObject();
	m_bitmapPeakRight.DeleteObject();

	if (m_hTheme != NULL)
		g_dllUxTheme.CloseThemeData(m_hTheme);
}

BEGIN_MESSAGE_MAP(CFftWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CREATE()
	ON_WM_THEMECHANGED()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFftWnd メッセージ ハンドラ

int CFftWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	GetThemaHandle();

	return 0;
}

void CFftWnd::InitFFT()
{
	int i;

	FreeBuffer();

	m_nFftSize = g_oSetData.Fft.nFftSize;
	m_nSamplingRate = g_oSetData.Fft.nSamplingRate;
	m_fFreqStep = (double)m_nSamplingRate / m_nFftSize;
	m_fTimeStep = 1.0 / m_fFreqStep;
	m_fTimeStep2 = m_fTimeStep / g_oSetData.Fft.nTimeRes;

	const int nFftBufSize = m_nFftSize / 2 + 1;

	m_pLogFreqTbl = new double[nFftBufSize];
	m_pLogFreqTbl[0] = 0;
	for (i = 1; i < nFftBufSize; i++)
		m_pLogFreqTbl[i] = log(i * m_fFreqStep);

	m_pFftBufL = new double[m_nFftSize];
	m_pFftBufR = new double[m_nFftSize];
	m_pFftBufM = new double[m_nFftSize];

	m_pPhaseBuf = new double[nFftBufSize]();

	m_pCrossBufRe = new double[nFftBufSize]();

	m_pCrossBufIm = new double[nFftBufSize]();

	m_pCoherenceBuf = new double[nFftBufSize]();

	const int nAverageNum = (g_oSetData.Fft.nAverage >= 0) ? g_oSetData.Fft.nAverage * m_nSamplingRate * g_oSetData.Fft.nTimeRes / m_nFftSize : -1;

	m_oCrossBufRe.Alloc(nFftBufSize, nAverageNum);
	m_oCrossBufIm.Alloc(nFftBufSize, nAverageNum);

	m_nSpectrogramRingBufSize = m_nFftSize / 2;
	m_nSpectrogramRingBufNum = (int)(1 / m_fTimeStep2 * g_oSetData.Fft.nTimeRange);
	m_nSpectrogramRingBufPtr = 0;
	m_bSpectrogramRingBufRound = FALSE;

	for (i = 0; i < 2; i++) {
		FFTDATA *pFftData = &m_aFftData[i];

		pFftData->m_pPowerSpecBuf = new double[nFftBufSize]();

		pFftData->m_oPowerSpecBuf.Alloc(nFftBufSize, nAverageNum);

		pFftData->m_pPwsPeakLevel = new double[nFftBufSize];

		pFftData->m_pCrfBuf = new double[m_nFftSize]();

		pFftData->m_pCepBuf = new double[m_nFftSize]();

		pFftData->m_pCepPeakLevel = new double[nFftBufSize];

		pFftData->m_pSpectrogramRingBuf = new double[m_nSpectrogramRingBufSize * m_nSpectrogramRingBufNum];
	}

	m_pCrsPeakLevel = new double[nFftBufSize];

	m_pCrsBuf = new double[nFftBufSize]();

	m_pWindowFunc = new double[m_nFftSize];
	MakeWindowFunc(g_oSetData.Fft.nFftWindowFunc, m_nFftSize, m_pWindowFunc, 2);

	m_pFilterTblL = new double[m_nFftSize];
	m_pFilterTblR = new double[m_nFftSize];
	MakeFilterTbl3(m_pFilterTblL, m_nFftSize, m_nSamplingRate, (FilterData *)m_oMicCalDataL.aFreq, m_oMicCalDataL.nFreqData, 20);
	MakeFilterTbl3(m_pFilterTblR, m_nFftSize, m_nSamplingRate, (FilterData *)m_oMicCalDataR.aFreq, m_oMicCalDataR.nFreqData, 20);
	const double sensL = pow(10.0, -m_oMicCalDataL.fInputSens / 20);
	const double sensR = pow(10.0, -m_oMicCalDataR.fInputSens / 20);
	m_pFilterTblL[0] = 0;
	m_pFilterTblR[0] = 0;
	for (i = 1; i < m_nFftSize; i++) {
		m_pFilterTblL[i] = sensL / m_pFilterTblL[i];
		m_pFilterTblR[i] = sensR / m_pFilterTblR[i];
	}

	if (g_oSetData.Fft.nFilter != 0) {
		double *pFilter = new double[m_nFftSize];
		MakeFilterTbl2(pFilter, m_nFftSize, m_nSamplingRate, g_oSetData.Fft.nFilter, 20);
		for (i = 0; i < m_nFftSize; i++) {
			m_pFilterTblL[i] *= pFilter[i];
			m_pFilterTblR[i] *= pFilter[i];
		}
		delete [] pFilter;
	}

	m_fLeftOffset = 0;
	m_fRightOffset = 0;

	m_bGetData = FALSE;
}

void CFftWnd::InitBuf()
{
	for (int i = 0; i < 2; i++) {
		FFTDATA *pFftData = &m_aFftData[i];

		pFftData->m_oPowerSpecBuf.Reset();

		if (pFftData->m_pSpgBmp != NULL) {
			BITMAP bm;
			pFftData->m_pSpgBmp->GetBitmap(&bm);
			CBitmap *pBitmapOrg = m_dcMem3.SelectObject(pFftData->m_pSpgBmp);
			m_dcMem3.FillSolidRect(0, 0, bm.bmWidth, bm.bmHeight, RGB(0, 0, 0));
			m_dcMem3.SelectObject(pBitmapOrg);
		}
	}

	m_oCrossBufRe.Reset();
	m_oCrossBufIm.Reset();

	m_fSpgTimeStep = 0;
	m_bPeakReset = TRUE;
	m_bSmoothingReset = TRUE;

	m_nSpectrogramRingBufPtr = 0;
	m_bSpectrogramRingBufRound = FALSE;
}

void CFftWnd::ChangeDispMode(FFTWINDOW *pFftWindow)
{
	if (pFftWindow == NULL) {
		SetBitmapAll();
		Redraw();
	} else {
		SetBitmap(pFftWindow);
		pFftWindow->m_pWnd->Invalidate();
	}
}

void CFftWnd::OnPaint()
{
	Paint(this);
}

void CFftWnd::Paint(CWnd *pWnd)
{
	CPaintDC dc(pWnd); // 描画用のデバイス コンテキスト

	FFTWINDOW *pFftWindow = GetFftWindow(pWnd);
	if (pFftWindow != NULL)
		PaintSub(dc, pFftWindow);
	else
		pFftWindow = NULL;
}

void CFftWnd::PaintSub(CDC &dc, const FFTWINDOW *pFftWindow)
{
	CBitmap *pBitmapOrg = m_dcMem.SelectObject(pFftWindow->m_pBitmap);
	CBitmap *pBitmapOrg2 = m_dcMem2.SelectObject(pFftWindow->m_pBitmap2);

	SetViewMode(pFftWindow);
	SetMinMax(pFftWindow);

	m_dcMem2.BitBlt(0, 0, pFftWindow->m_nScaleWidth, pFftWindow->m_nScaleHeight, &m_dcMem, pFftWindow->m_nFrameLeft + pFftWindow->m_sizeMargin.cx, pFftWindow->m_nFrameTop + pFftWindow->m_sizeMargin.cy, SRCCOPY);

	if (m_bLch)
		PaintData(0, pFftWindow);

	if (m_bRch && m_nViewMode != VM_SPLIT)
		PaintData(1, pFftWindow);

	dc.BitBlt(pFftWindow->m_nFrameLeft + pFftWindow->m_sizeMargin.cx, pFftWindow->m_nFrameTop + pFftWindow->m_sizeMargin.cy, pFftWindow->m_nScaleWidth, pFftWindow->m_nScaleHeight, &m_dcMem2, 0, 0, SRCCOPY);

	if (m_nViewMode == VM_SPLIT) {
		m_dcMem2.BitBlt(0, 0, pFftWindow->m_nScaleWidth, pFftWindow->m_nScaleHeight, &m_dcMem, pFftWindow->m_nFrameLeft + pFftWindow->m_sizeMargin.cx, pFftWindow->m_nFrameBottom - pFftWindow->m_nScaleHeight + pFftWindow->m_sizeMargin.cy, SRCCOPY);
		PaintData(1, pFftWindow);
		dc.BitBlt(pFftWindow->m_nFrameLeft + pFftWindow->m_sizeMargin.cx, pFftWindow->m_nFrameBottom - pFftWindow->m_nScaleHeight + pFftWindow->m_sizeMargin.cy, pFftWindow->m_nScaleWidth, pFftWindow->m_nScaleHeight, &m_dcMem2, 0, 0, SRCCOPY);
	}

	m_dcMem.SelectObject(pBitmapOrg);
	m_dcMem2.SelectObject(pBitmapOrg2);
}

void CFftWnd::PaintData(int nChannel, const FFTWINDOW *pFftWindow)
{
	switch (pFftWindow->m_nMode) {
	case FFT_MODE_PWS:
		PaintPws(pFftWindow, nChannel);
		break;
	case FFT_MODE_OCT:
		PaintOct(pFftWindow, nChannel);
		break;
	case FFT_MODE_3DD:
		Paint3dd(pFftWindow, nChannel);
		break;
	case FFT_MODE_CRF:
		PaintCrf(pFftWindow, nChannel);
		break;
	case FFT_MODE_PHS:
		PaintPhs(pFftWindow, nChannel);
		break;
	case FFT_MODE_SPG:
		PaintSpg(pFftWindow, nChannel);
		break;
	case FFT_MODE_CRS:
		PaintCrs(pFftWindow, nChannel);
		break;
	case FFT_MODE_COH:
		PaintCoh(pFftWindow, nChannel);
		break;
	case FFT_MODE_CEP:
		PaintCep(pFftWindow, nChannel);
		break;
	}
}

BOOL CFftWnd::OnEraseBkgnd(CDC* pDC)
{
	EraseBkgnd(pDC, this);

	return TRUE;
}

void CFftWnd::EraseBkgnd(CDC* pDC, CWnd *pWnd)
{
	FFTWINDOW *pFftWindow = GetFftWindow(pWnd);
	if (pFftWindow != NULL) {
		CBitmap *pBitmapOrg = m_dcMem.SelectObject(pFftWindow->m_pBitmap);
		pDC->BitBlt(0, 0, pFftWindow->m_sizeClient.cx, pFftWindow->m_sizeClient.cy, &m_dcMem, 0, 0, SRCCOPY);
		m_dcMem.SelectObject(pBitmapOrg);
	}
}

void CFftWnd::SetBitmapAll()
{
	for (POSITION pos = m_oFftWindow.GetHeadPosition(); pos != NULL; )
		SetBitmap((FFTWINDOW *)m_oFftWindow.GetNext(pos));
}

void CFftWnd::SetBitmap(CWnd *pWnd)
{
	FFTWINDOW *pFftWindow = GetFftWindow(pWnd);
	if (pFftWindow != NULL)
		SetBitmap(pFftWindow);
}

void CFftWnd::SetBitmap(FFTWINDOW *pFftWindow)
{
	FreeBuffer2(pFftWindow);

	CRect rect;
	pFftWindow->m_pWnd->GetClientRect(rect);
	rect.bottom -= pFftWindow->m_nStatusBarHeight;
	pFftWindow->m_sizeClient = rect.Size();

	CRect rectClient = rect;
	rect.DeflateRect(pFftWindow->m_sizeMargin);
	pFftWindow->m_nWidth = rect.Width();
	pFftWindow->m_nHeight = rect.Height();

	CDC *pDC = pFftWindow->m_pWnd->GetDC();
	pFftWindow->m_pBitmap->DeleteObject();
	pFftWindow->m_pBitmap->CreateCompatibleBitmap(pDC, pFftWindow->m_sizeClient.cx, pFftWindow->m_sizeClient.cy);

	SetViewMode(pFftWindow);
	SetMinMax(pFftWindow);

	CBitmap *pBitmapOrg = m_dcMem.SelectObject(pFftWindow->m_pBitmap);
	if (m_hTheme != NULL)
		g_dllUxTheme.DrawThemeBackground(m_hTheme, m_dcMem.m_hDC, 10, 0, &rectClient, NULL);
	else
		m_dcMem.FillSolidRect(rectClient, COLOR_BKGRND);

	m_dcMem.SetViewportOrg(pFftWindow->m_sizeMargin.cx, pFftWindow->m_sizeMargin.cy);

	switch (pFftWindow->m_nMode) {
	case FFT_MODE_PWS:
		SetBitmapPws(pFftWindow);
		break;
	case FFT_MODE_OCT:
		SetBitmapOct(pFftWindow);
		break;
	case FFT_MODE_3DD:
		SetBitmap3dd(pFftWindow);
		break;
	case FFT_MODE_CRF:
		SetBitmapCrf(pFftWindow);
		break;
	case FFT_MODE_PHS:
		SetBitmapPhs(pFftWindow);
		break;
	case FFT_MODE_SPG:
		SetBitmapSpg(pFftWindow);
		break;
	case FFT_MODE_CRS:
		SetBitmapCrs(pFftWindow);
		break;
	case FFT_MODE_COH:
		SetBitmapCoh(pFftWindow);
		break;
	case FFT_MODE_CEP:
		SetBitmapCep(pFftWindow);
		break;
	}

	m_dcMem.SetViewportOrg(0, 0);

	m_dcMem.SelectObject(pBitmapOrg);

	pFftWindow->m_pBitmap2->DeleteObject();
	pFftWindow->m_pBitmap2->CreateCompatibleBitmap(pDC, pFftWindow->m_nScaleWidth, pFftWindow->m_nScaleHeight);

	pFftWindow->m_pWnd->ReleaseDC(pDC);

	DispData(pFftWindow);
}

void CFftWnd::SetMinMax(const FFTWINDOW *pFftWindow)
{
	if (g_oSetData.Fft.bFftAutoFreq || pFftWindow->m_nMode == FFT_MODE_OCT) {
		m_nMaxFreq = g_oSetData.Fft.nSamplingRate / 2;
		m_nMinFreq = max(20, g_oSetData.Fft.nSamplingRate / g_oSetData.Fft.nFftSize);
	} else {
		m_nMaxFreq = g_oSetData.Fft.nFftMaxFreq;
		if (g_oSetData.Fft.nFftScale == 0 && g_oSetData.Fft.nFftMinFreq == 0)
			m_nMinFreq = 1;
		else
			m_nMinFreq = g_oSetData.Fft.nFftMinFreq;
	}

	if (m_nMinFreq >= m_nMaxFreq)
		m_nMaxFreq = m_nMinFreq + 1;

	m_fLogMaxFreq = log((double)m_nMaxFreq);
	m_fLogMinFreq = log((double)m_nMinFreq);

	if (g_oSetData.Fft.bFftAutoLevel) {
		switch (pFftWindow->m_nMode) {
		case FFT_MODE_PWS:
		case FFT_MODE_PHS:
		case FFT_MODE_CRS:
		case FFT_MODE_COH:
			m_nMaxLevel = ((int)(-m_oMicCalDataL.fInputSens) + 19) / 20 * 20;
			m_nMinLevel = m_nMaxLevel - 120;
			break;
		case FFT_MODE_OCT:
			m_nMaxLevel = ((int)(-m_oMicCalDataL.fInputSens) + 9) / 10 * 10;
			m_nMinLevel = m_nMaxLevel - 100;
			break;
		case FFT_MODE_3DD:
			m_nMaxLevel = ((int)(-m_oMicCalDataL.fInputSens) + 9) / 10 * 10;
			m_nMinLevel = m_nMaxLevel - 110;
			break;
		case FFT_MODE_SPG:
			m_nMaxLevel = ((int)(-m_oMicCalDataL.fInputSens) + 9) / 10 * 10 - 10;
			m_nMinLevel = m_nMaxLevel - 60;
			break;
		case FFT_MODE_CEP:
			m_nMaxLevel = 40;
			m_nMinLevel = -40;
			break;
		}
	} else {
		m_nMaxLevel = g_oSetData.Fft.nFftMaxLevel;
		m_nMinLevel = g_oSetData.Fft.nFftMinLevel;
		if (m_nMaxLevel == m_nMinLevel)
			m_nMinLevel = m_nMaxLevel - 1;
	}

	if (m_nMinLevel >= m_nMaxLevel)
		m_nMaxLevel = m_nMinLevel + 1;
}

void CFftWnd::GetWaveData(double *pData)
{
	int i;
	double *pData2;
	double fLeftData, fRightData;
	BOOL bLeft = FALSE, bRight = FALSE, bMono = FALSE;
	BOOL bStereo = FALSE;
	double *pFftBufL = NULL, *pFftBufR = NULL;

	m_bValidFftData = TRUE;

	double fLeftOffset = 0;
	double fRightOffset = 0;
	for (i = 0, pData2 = pData; i < m_nFftSize; i++) {
		fLeftOffset += *pData2++;
		fRightOffset += *pData2++;
	}
	m_fLeftOffset = (m_fLeftOffset * 9 + fLeftOffset / m_nFftSize) / 10;
	m_fRightOffset = (m_fRightOffset * 9 + fRightOffset / m_nFftSize) / 10;

	const int nLeftOffset = (int)m_fLeftOffset;
	const int nRightOffset = (int)m_fRightOffset;

	for (POSITION pos = m_oFftWindow.GetHeadPosition(); pos != NULL; ) {
		FFTWINDOW *pFftWindow = (FFTWINDOW *)m_oFftWindow.GetNext(pos);

		switch (pFftWindow->m_nMode) {
		case FFT_MODE_CRF:
			if (g_oSetData.Fft.nCorrelation != 0)
				bStereo = TRUE;
			break;
		case FFT_MODE_CRS:
		case FFT_MODE_COH:
		case FFT_MODE_PHS:
			bStereo = TRUE;
			break;
		}
	}

	if (g_oSetData.Fft.nChannel == CH_STEREO || bStereo) {
		for (i = 0, pData2 = pData; i < m_nFftSize; i++) {
			m_pFftBufL[i] = (*pData2++ - nLeftOffset) * m_pWindowFunc[i];
			m_pFftBufR[i] = (*pData2++ - nRightOffset) * m_pWindowFunc[i];
		}
		bLeft = TRUE;
		bRight = TRUE;
	}

	if (g_oSetData.Fft.nChannel == CH_LEFT_ONLY && !bLeft) {
		for (i = 0, pData2 = pData; i < m_nFftSize; i++) {
			m_pFftBufL[i] = (*pData2++ - nLeftOffset) * m_pWindowFunc[i];
			pData2++;
		}
		bLeft = TRUE;
	}

	if (g_oSetData.Fft.nChannel == CH_RIGHT_ONLY && !bRight) {
		for (i = 0, pData2 = pData; i < m_nFftSize; i++) {
			pData2++;
			m_pFftBufR[i] = (*pData2++ - nRightOffset) * m_pWindowFunc[i];
		}
		bRight = TRUE;
	}

	if (g_oSetData.Fft.nChannel == CH_MONO) {
		for (i = 0, pData2 = pData; i < m_nFftSize; i++) {
			fLeftData = *pData2++ - nLeftOffset;
			fRightData = *pData2++ - nRightOffset;
			m_pFftBufM[i] = (fLeftData + fRightData) / 2 * m_pWindowFunc[i];
		}
		bMono = TRUE;
	}

	if (g_oSetData.Fft.nChannel == CH_DIFFERENCE) {
		for (i = 0, pData2 = pData; i < m_nFftSize; i++) {
			fLeftData = *pData2++ - nLeftOffset;
			fRightData = *pData2++ - nRightOffset;
			m_pFftBufM[i] = (fLeftData - fRightData) * m_pWindowFunc[i];
		}
		bMono = TRUE;
	}

	if (bMono) {
		m_oFFT.fft(m_nFftSize, m_pFftBufM);
		pFftBufL = m_pFftBufM;
	}

	if (bLeft) {
		m_oFFT.fft(m_nFftSize, m_pFftBufL);
		pFftBufL = m_pFftBufL;
	}

	if (bRight) {
		m_oFFT.fft(m_nFftSize, m_pFftBufR);
		pFftBufR = m_pFftBufR;
	}

	if (g_oSetData.Fft.nSmoothing == 0 || m_bSmoothingReset) {
		m_fSmoothing1 = 1;
		m_fSmoothing2 = 0;
		m_bSmoothingReset = FALSE;
	} else {
		m_fSmoothing2 = pow(1 / M_E, (m_fTimeStep2 * 1000) / (g_oSetData.Fft.nSmoothing));
		m_fSmoothing1 = 1 - m_fSmoothing2;
	}

	CalcSpectrum(pFftBufL, pFftBufR);

	m_bGetData = TRUE;

	for (POSITION pos = m_oFftWindow.GetHeadPosition(); pos != NULL; )
		DispData((FFTWINDOW *)m_oFftWindow.GetNext(pos));
}

void CFftWnd::DispData(const FFTWINDOW *pFftWindow)
{
	if (!m_bGetData)
		return;

	CBitmap *pBitmapOrg = m_dcMem.SelectObject(pFftWindow->m_pBitmap);
	CBitmap *pBitmapOrg2 = m_dcMem2.SelectObject(pFftWindow->m_pBitmap2);

	SetViewMode(pFftWindow);
	SetMinMax(pFftWindow);

	switch (pFftWindow->m_nMode) {
	case FFT_MODE_PWS:
		GetWaveDataPws(pFftWindow);
		break;
	case FFT_MODE_OCT:
		GetWaveDataOct(pFftWindow);
		break;
	case FFT_MODE_3DD:
		GetWaveData3dd(pFftWindow);
		break;
	case FFT_MODE_CRF:
		GetWaveDataCrf(pFftWindow);
		break;
	case FFT_MODE_PHS:
		GetWaveDataPhs(pFftWindow);
		break;
	case FFT_MODE_SPG:
		GetWaveDataSpg(pFftWindow);
		break;
	case FFT_MODE_CRS:
		GetWaveDataCrs(pFftWindow);
		break;
	case FFT_MODE_COH:
		GetWaveDataCoh(pFftWindow);
		break;
	case FFT_MODE_CEP:
		GetWaveDataCep(pFftWindow);
		break;
	}

	pFftWindow->m_pWnd->InvalidateRect(CRect(0, 0, pFftWindow->m_sizeClient.cx, pFftWindow->m_sizeClient.cy), FALSE);

	m_dcMem.SelectObject(pBitmapOrg);
	m_dcMem2.SelectObject(pBitmapOrg2);
}

void CFftWnd::DispData(CWnd *pWnd)
{
	DispData(GetFftWindow(pWnd));
}

void CFftWnd::CalcSpectrum(const double *pFftBufL, const double *pFftBufR)
{
	if (pFftBufL != NULL)
		CalcPowerSpectrum(&m_aFftData[0], pFftBufL, m_pFilterTblL);

	if (pFftBufR != NULL)
		CalcPowerSpectrum(&m_aFftData[1], pFftBufR, m_pFilterTblR);

	if (pFftBufL != NULL && pFftBufR != NULL)
		CalcCrossSpectrum(pFftBufL, pFftBufR);

	if (++m_nSpectrogramRingBufPtr >= m_nSpectrogramRingBufNum) {
		m_nSpectrogramRingBufPtr = 0;
		m_bSpectrogramRingBufRound = TRUE;
	}
}

void CFftWnd::CalcPowerSpectrum(FFTDATA *pFftData, const double *pBuf, const double *pFilterTbl) const
{
	int i, j;
	double xt, yt;
	double *pPowerSpecBuf1 = pFftData->m_pPowerSpecBuf;
	const int nFftSize2 = m_nFftSize / 2;

	for (i = 1, j = 2; i < nFftSize2; i++) {
		xt = pBuf[j++] * pFilterTbl[i];
		yt = pBuf[j++] * pFilterTbl[i];

		pPowerSpecBuf1[i] = xt * xt + yt * yt;
	}

	pFftData->m_oPowerSpecBuf.Smoothing(pPowerSpecBuf1, m_fSmoothing1, m_fSmoothing2);
	pFftData->m_oPowerSpecBuf.Averaging();

	pFftData->m_fFftAllPower = 0;
	double *pPowerSpecBuf2 = pFftData->m_oPowerSpecBuf.GetBuf();
	for (i = 1; i < nFftSize2; i++)
		pFftData->m_fFftAllPower += pPowerSpecBuf2[i];

	memcpy(pFftData->m_pSpectrogramRingBuf + nFftSize2 * m_nSpectrogramRingBufPtr, pPowerSpecBuf1, nFftSize2 * sizeof(double));
}

void CFftWnd::CalcCrossSpectrum(const double *pBufL, const double *pBufR)
{
	double xt1, yt1, xt2, yt2;
	const int nFftSize2 = m_nFftSize / 2;

	for (int i = 1, j = 2; i < nFftSize2; i++) {
		xt1 = pBufL[j] * m_pFilterTblL[i];
		xt2 = pBufR[j++] * m_pFilterTblR[i];
		yt1 = pBufL[j] * m_pFilterTblL[i];
		yt2 = pBufR[j++] * m_pFilterTblR[i];

		m_pCrossBufRe[i] = xt1 * xt2 + yt1 * yt2;
		m_pCrossBufIm[i] = xt2 * yt1 - xt1 * yt2;
	}

	m_oCrossBufRe.Smoothing(m_pCrossBufRe, m_fSmoothing1, m_fSmoothing2);
	m_oCrossBufRe.Averaging();

	m_oCrossBufIm.Smoothing(m_pCrossBufIm, m_fSmoothing1, m_fSmoothing2);
	m_oCrossBufIm.Averaging();
}

void CFftWnd::FreeBuffer()
{
	if (m_pLogFreqTbl != NULL) {
		delete [] m_pLogFreqTbl;
		m_pLogFreqTbl = NULL;
	}

	if (m_pWindowFunc != NULL) {
		delete [] m_pWindowFunc;
		m_pWindowFunc = NULL;
	}

	if (m_pFftBufL != NULL) {
		delete [] m_pFftBufL;
		m_pFftBufL = NULL;
	}

	if (m_pFftBufR != NULL) {
		delete [] m_pFftBufR;
		m_pFftBufR = NULL;
	}

	if (m_pFftBufM != NULL) {
		delete [] m_pFftBufM;
		m_pFftBufM = NULL;
	}

	if (m_pFilterTblL != NULL) {
		delete [] m_pFilterTblL;
		m_pFilterTblL = NULL;
	}

	if (m_pFilterTblR != NULL) {
		delete [] m_pFilterTblR;
		m_pFilterTblR = NULL;
	}

	if (m_pPhaseBuf != NULL) {
		delete [] m_pPhaseBuf;
		m_pPhaseBuf = NULL;
	}

	if (m_pCrossBufRe != NULL) {
		delete [] m_pCrossBufRe;
		m_pCrossBufRe = NULL;
	}

	if (m_pCrossBufIm != NULL) {
		delete [] m_pCrossBufIm;
		m_pCrossBufIm = NULL;
	}

	if (m_pCrsBuf != NULL) {
		delete [] m_pCrsBuf;
		m_pCrsBuf = NULL;
	}

	if (m_pCrsPeakLevel != NULL) {
		delete [] m_pCrsPeakLevel;
		m_pCrsPeakLevel = NULL;
	}

	if (m_pCoherenceBuf != NULL) {
		delete [] m_pCoherenceBuf;
		m_pCoherenceBuf = NULL;
	}

	for (int i = 0; i < 2; i++) {
		FFTDATA *pFftData = &m_aFftData[i];

		if (pFftData->m_pPowerSpecBuf != NULL) {
			delete [] pFftData->m_pPowerSpecBuf;
			pFftData->m_pPowerSpecBuf = NULL;
		}

		if (pFftData->m_pPwsPeakLevel != NULL) {
			delete [] pFftData->m_pPwsPeakLevel;
			pFftData->m_pPwsPeakLevel = NULL;
		}

		if (pFftData->m_pCrfBuf != NULL) {
			delete [] pFftData->m_pCrfBuf;
			pFftData->m_pCrfBuf = NULL;
		}

		if (pFftData->m_pCepBuf != NULL) {
			delete [] pFftData->m_pCepBuf;
			pFftData->m_pCepBuf = NULL;
		}

		if (pFftData->m_pCepPeakLevel != NULL) {
			delete [] pFftData->m_pCepPeakLevel;
			pFftData->m_pCepPeakLevel = NULL;
		}

		if (pFftData->m_pSpectrogramRingBuf != NULL) {
			delete [] pFftData->m_pSpectrogramRingBuf;
			pFftData->m_pSpectrogramRingBuf = NULL;
		}
	}
}

void CFftWnd::FreeBuffer2(const FFTWINDOW *pFftWindow)
{
	for (int i = 0; i < 2; i++) {
		FFTDATA *pFftData = &m_aFftData[i];

		if (pFftWindow == NULL || pFftWindow->m_nMode == FFT_MODE_PWS) {
			if (pFftData->m_pPwsPoint != NULL) {
				delete [] pFftData->m_pPwsPoint;
				pFftData->m_pPwsPoint = NULL;
			}

			if (pFftData->m_pPwsPeakPoint != NULL) {
				delete [] pFftData->m_pPwsPeakPoint;
				pFftData->m_pPwsPeakPoint = NULL;
			}
		}

		if (pFftWindow == NULL || pFftWindow->m_nMode == FFT_MODE_CRF) {
			if (pFftData->m_pCrfPoint != NULL) {
				delete [] pFftData->m_pCrfPoint;
				pFftData->m_pCrfPoint = NULL;
			}
		}

		if (pFftWindow == NULL || pFftWindow->m_nMode == FFT_MODE_3DD) {
			for (int j = 0; j < MAX_3DD; j++) {
				if (pFftData->m_p3ddPoint[j] != NULL)
					delete [] pFftData->m_p3ddPoint[j];
				pFftData->m_p3ddPoint[j] = NULL;
			}
		}

		if (pFftWindow == NULL || pFftWindow->m_nMode == FFT_MODE_SPG) {
			if (pFftData->m_pSpgBmp != NULL) {
				delete pFftData->m_pSpgBmp;
				pFftData->m_pSpgBmp = NULL;
			}
		}

		if (pFftWindow == NULL || pFftWindow->m_nMode == FFT_MODE_CEP) {
			if (pFftData->m_pCepPoint != NULL) {
				delete [] pFftData->m_pCepPoint;
				pFftData->m_pCepPoint = NULL;
			}

			if (pFftData->m_pCepPeakPoint != NULL) {
				delete [] pFftData->m_pCepPeakPoint;
				pFftData->m_pCepPeakPoint = NULL;
			}
		}
	}

	if (pFftWindow == NULL || pFftWindow->m_nMode == FFT_MODE_3DD) {
		if (m_p3ddCheckTbl != NULL) {
			delete [] m_p3ddCheckTbl;
			m_p3ddCheckTbl = NULL;
		}

		if (m_p3ddWork != NULL) {
			delete [] m_p3ddWork;
			m_p3ddWork = NULL;
		}
	}

	if (pFftWindow == NULL || pFftWindow->m_nMode == FFT_MODE_PHS) {
		if (m_pPhsPoint != NULL) {
			delete [] m_pPhsPoint;
			m_pPhsPoint = NULL;
		}
	}

	if (pFftWindow == NULL || pFftWindow->m_nMode == FFT_MODE_SPG) {
		if (m_pSpgLevel != NULL) {
			delete [] m_pSpgLevel;
			m_pSpgLevel = NULL;
		}
	}

	if (pFftWindow == NULL || pFftWindow->m_nMode == FFT_MODE_CRS) {
		if (m_pCrsPoint != NULL) {
			delete [] m_pCrsPoint;
			m_pCrsPoint = NULL;
		}

		if (m_pCrsPeakPoint != NULL) {
			delete [] m_pCrsPeakPoint;
			m_pCrsPeakPoint = NULL;
		}
	}

	if (pFftWindow == NULL || pFftWindow->m_nMode == FFT_MODE_COH) {
		if (m_pCohPoint != NULL) {
			delete [] m_pCohPoint;
			m_pCohPoint = NULL;
		}
	}
}

void CFftWnd::Redraw()
{
	for (POSITION pos = m_oFftWindow.GetHeadPosition(); pos != NULL; ) {
		FFTWINDOW *pFftWindow = (FFTWINDOW *)m_oFftWindow.GetNext(pos);

		pFftWindow->m_pWnd->Invalidate();
	}
}

void CFftWnd::OnDestroy()
{
	m_bClose = TRUE;

	for (POSITION pos = m_oFftWindow.GetHeadPosition(); pos != NULL; ) {
		FFTWINDOW *pFftWindow = (FFTWINDOW *)m_oFftWindow.GetNext(pos);

		if (pFftWindow->m_pWnd != this)
			pFftWindow->m_pWnd->DestroyWindow();
	}

	CWnd::OnDestroy();
}

void CFftWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	FFTWINDOW *pFftWindow = GetFftWindow(this);

	SetDispFreq(pFftWindow, point);

	CWnd::OnLButtonDown(nFlags, point);
}

void CFftWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
	FFTWINDOW *pFftWindow = GetFftWindow(this);

	ResetDispFreq(pFftWindow);

	CWnd::OnRButtonDown(nFlags, point);
}

void CFftWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	FFTWINDOW *pFftWindow = GetFftWindow(this);

	if (nFlags & MK_LBUTTON)
		SetDispFreq(pFftWindow, point);

	CWnd::OnMouseMove(nFlags, point);
}

void CFftWnd::ResetDispFreq(const FFTWINDOW *pFftWindow)
{
	switch (pFftWindow->m_nMode) {
	case FFT_MODE_PWS:
		ResetDispFreqPws(pFftWindow);
		break;
	case FFT_MODE_OCT:
		ResetDispFreqOct(pFftWindow);
		break;
	case FFT_MODE_CRF:
		ResetDispFreqCrf(pFftWindow);
		break;
	case FFT_MODE_PHS:
		ResetDispFreqPhs(pFftWindow);
		break;
	case FFT_MODE_CRS:
		ResetDispFreqCrs(pFftWindow);
		break;
	case FFT_MODE_COH:
		ResetDispFreqCoh(pFftWindow);
		break;
	case FFT_MODE_CEP:
		ResetDispFreqCep(pFftWindow);
		break;
	}
}

void CFftWnd::ResetDispFreqAll()
{
	for (POSITION pos = m_oFftWindow.GetHeadPosition(); pos != NULL; ) {
		FFTWINDOW *pFftWindow = (FFTWINDOW *)m_oFftWindow.GetNext(pos);

		ResetDispFreq(pFftWindow);
	}
}

void CFftWnd::SetDispFreq(const FFTWINDOW *pFftWindow, CPoint point)
{
	point -= pFftWindow->m_sizeMargin;

	if (point.x < 0 || point.x >= pFftWindow->m_nFrameRight || point.y < 0 || point.y >= pFftWindow->m_nFrameBottom) {
		ResetDispFreq(pFftWindow);
		return;
	}

	switch (pFftWindow->m_nMode) {
	case FFT_MODE_PWS:
		SetDispFreqPws(pFftWindow, point);
		break;
	case FFT_MODE_OCT:
		SetDispFreqOct(pFftWindow, point);
		break;
	case FFT_MODE_CRF:
		SetDispFreqCrf(pFftWindow, point);
		break;
	case FFT_MODE_PHS:
		SetDispFreqPhs(pFftWindow, point);
		break;
	case FFT_MODE_CRS:
		SetDispFreqCrs(pFftWindow, point);
		break;
	case FFT_MODE_COH:
		SetDispFreqCoh(pFftWindow, point);
		break;
	case FFT_MODE_CEP:
		SetDispFreqCep(pFftWindow, point);
		break;
	}
}

void CFftWnd::SetViewMode(const FFTWINDOW *pFftWindow)
{
	if ((pFftWindow->m_nMode == FFT_MODE_CRF && g_oSetData.Fft.nCorrelation != 0) || pFftWindow->m_nMode == FFT_MODE_PHS || pFftWindow->m_nMode == FFT_MODE_CRS || pFftWindow->m_nMode == FFT_MODE_COH) {
		m_nChannel = CH_STEREO;
		m_bLch = TRUE;
		m_bRch = FALSE;
		m_nViewMode = VM_SINGLE;
	} else {
		m_nChannel = g_oSetData.Fft.nChannel;
		m_bLch = (m_nChannel != CH_RIGHT_ONLY);
		m_bRch = (m_nChannel == CH_STEREO || m_nChannel == CH_RIGHT_ONLY);

		if (m_nChannel == CH_STEREO) {
			if (g_oSetData.Fft.bRlSplit)
				m_nViewMode = VM_SPLIT;
			else
				m_nViewMode = VM_OVERLAY;
		} else
			m_nViewMode = VM_SINGLE;
	}
}

void CFftWnd::DrawNote(LPCSTR pText, int nTop, int nRight)
{
	CSize size;
	int x, y;
	LPCSTR pDrawText;

	if (pText == NULL) {
		switch (m_nChannel) {
		case CH_MONO:
			pText = "Mono";
			break;
		case CH_STEREO:
			pText = "";
			break;
		case CH_LEFT_ONLY:
			pText = "Lch";
			break;
		case CH_RIGHT_ONLY:
			pText = "Rch";
			break;
		case CH_DIFFERENCE:
			pText = "Difference";
			break;
		}
	}

	m_dcMem.SelectObject(&m_Font);
	size = m_dcMem.GetOutputTextExtent(pText);
	x = nRight - size.cx - 10;
	y = nTop + 5;

	pDrawText = "Lch";
	if (strstr(pText, pDrawText))
		x = DrawNoteSub(pDrawText, x, y, COLOR_LEFT);

	pDrawText = " / ";
	if (strstr(pText, pDrawText))
		x = DrawNoteSub(pDrawText, x, y, COLOR_GRAY);

	pDrawText = "Rch";
	if (strstr(pText, pDrawText))
		x = DrawNoteSub(pDrawText, x, y, COLOR_RIGHT);

	pDrawText = "Mono";
	if (strstr(pText, pDrawText))
		x = DrawNoteSub(pDrawText, x, y, COLOR_LEFT);

	pDrawText = "Difference";
	if (strstr(pText, pDrawText))
		x = DrawNoteSub(pDrawText, x, y, COLOR_LEFT);
}

int CFftWnd::DrawNoteSub(LPCSTR pText, int x, int y, int nColor)
{
	CSize size;

	m_dcMem.SetTextColor(nColor);
	m_dcMem.TextOut(x, y, pText);
	size = m_dcMem.GetOutputTextExtent(pText);

	return x + size.cx;
}

HBITMAP CFftWnd::GetBitmap()
{
	CDC *pDC = GetDC();
	CDC dc;
	CBitmap bmp;
	CSize sizeMargin(5, 5);

	FFTWINDOW *pFftWindow = GetFftWindow(this);

	dc.CreateCompatibleDC(pDC);
	CSize sizeBmp(pFftWindow->m_nWidth + sizeMargin.cx * 2, pFftWindow->m_nHeight + sizeMargin.cy * 2);
	bmp.CreateCompatibleBitmap(pDC, sizeBmp.cx, sizeBmp.cy);

	CBitmap *pBitmapOrg1 = m_dcMem.SelectObject(pFftWindow->m_pBitmap);
	CBitmap *pBitmapOrg2 = dc.SelectObject(&bmp);

	CRect rectBmp(0, 0, sizeBmp.cx, sizeBmp.cy);
	if (m_hTheme != NULL)
		g_dllUxTheme.DrawThemeBackground(m_hTheme, dc.m_hDC, 10, 0, &rectBmp, NULL);
	else
		dc.FillSolidRect(rectBmp, COLOR_BKGRND);

	dc.SetViewportOrg(sizeMargin.cx, sizeMargin.cy);
	dc.BitBlt(0, 0, pFftWindow->m_nWidth, pFftWindow->m_nHeight, &m_dcMem, 0, 0, SRCCOPY);
	PaintSub(dc, pFftWindow);

	m_dcMem.SelectObject(pBitmapOrg1);
	dc.SelectObject(pBitmapOrg2);

	ReleaseDC(pDC);

	return (HBITMAP)bmp.Detach();
}

FFTWINDOW *CFftWnd::AddFftWindow(int nMode, CWnd *pWnd, BOOL bMasterWindow)
{
	FFTWINDOW *pFftWindow = new FFTWINDOW();
	pFftWindow->m_nMode = nMode;
	pFftWindow->m_pWnd = pWnd;
	pFftWindow->m_pBitmap = new CBitmap;
	pFftWindow->m_pBitmap2 = new CBitmap;
	pFftWindow->m_bMasterWindow = bMasterWindow;

	if (bMasterWindow)
		m_oFftWindow.AddHead(pFftWindow);
	else
		m_oFftWindow.AddTail(pFftWindow);

	return pFftWindow;
}

FFTWINDOW *CFftWnd::GetFftWindow(CWnd *pWnd)
{
	for (POSITION pos = m_oFftWindow.GetHeadPosition(); pos != NULL; ) {
		FFTWINDOW *pFftWindow = (FFTWINDOW *)m_oFftWindow.GetNext(pos);
		if (pFftWindow->m_pWnd == pWnd)
			return pFftWindow;
	}

	return NULL;
}

FFTWINDOW *CFftWnd::GetFftWindow(int nMode)
{
	for (POSITION pos = m_oFftWindow.GetHeadPosition(); pos != NULL; ) {
		FFTWINDOW *pFftWindow = (FFTWINDOW *)m_oFftWindow.GetNext(pos);
		if (pFftWindow->m_nMode == nMode)
			return pFftWindow;
	}

	return NULL;
}

void CFftWnd::RemoveFftWindow(CWnd *pWnd)
{
	for (POSITION pos = m_oFftWindow.GetHeadPosition(); pos != NULL; ) {
		POSITION pos2 = pos;
		FFTWINDOW *pFftWindow = (FFTWINDOW *)m_oFftWindow.GetNext(pos);
		if (pFftWindow->m_pWnd == pWnd) {
			m_oFftWindow.RemoveAt(pos2);
			DeleteFftWindow(pFftWindow);
			break;
		}
	}
}

void CFftWnd::RemoveFftWindowAll()
{
	while (POSITION pos = m_oFftWindow.GetHeadPosition()) {
		FFTWINDOW *pFftWindow = (FFTWINDOW *)m_oFftWindow.GetAt(pos);
		m_oFftWindow.RemoveAt(pos);
		DeleteFftWindow(pFftWindow);
	}
}

void CFftWnd::DeleteFftWindow(FFTWINDOW *pFftWindow)
{
	pFftWindow->m_pBitmap->DeleteObject();
	delete pFftWindow->m_pBitmap;

	pFftWindow->m_pBitmap2->DeleteObject();
	delete pFftWindow->m_pBitmap2;

	delete pFftWindow;
}

void CFftWnd::CreateFftWindow(int nMode, RECT rectWindow)
{
	CRect rect;

	CString sTitle;
	sTitle.LoadString(g_tFftModeId[nMode]);
#ifdef _LANG_JPN
	sTitle = Hankaku2Zenkaku(sTitle);
#endif

	CFftWndEx *pWnd = new CFftWndEx(this, nMode);

	FFTWINDOW *pFftWindow = AddFftWindow(nMode, pWnd);
	pWnd->CreateEx(0, ::AfxRegisterWndClass(0, ::AfxGetApp()->LoadStandardCursor(IDC_ARROW), 0, ::AfxGetApp()->LoadIcon(IDI_FFT_WINDOW)),
				sTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MAXIMIZEBOX ,
				CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
				::AfxGetMainWnd()->m_hWnd, 0);

	if (rectWindow.left == 0 && rectWindow.top == 0) {
		pWnd->GetWindowRect(rect);
		rectWindow.left = rect.left;
		rectWindow.top = rect.top;
	}

	pFftWindow->m_sizeMargin.SetSize(5, 5);

	if (rectWindow.bottom == 0) {
		GetClientRect(rect);
		pWnd->CalcWindowRect(rect, CWnd::adjustBorder);
		rect.InflateRect(pFftWindow->m_sizeMargin);
		rect.MoveToXY(rectWindow.left, rectWindow.top);
	} else
		rect = rectWindow;

	CRect rectStatusBar;
	pWnd->GetDlgItem(AFX_IDW_STATUS_BAR)->GetWindowRect(rectStatusBar);
	pFftWindow->m_nStatusBarHeight = rectStatusBar.Height();
	rect.bottom += pFftWindow->m_nStatusBarHeight;

	pWnd->MoveWindow(rect);
	SetBitmap(pFftWindow);
	pWnd->ShowWindow(SW_SHOW);
}

void CFftWnd::CloseFftWindow(CWnd *pWnd, int nMode)
{
	RemoveFftWindow(pWnd);
	if (!m_bClose)
		m_pFftDlg->RestorePage(nMode);
}

void CFftWnd::CsvOutput(LPCSTR pFileName)
{
	CCsvFile oCsvFile;

	if (oCsvFile.Open(pFileName, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone)) {
		switch (g_oSetData.Fft.nMode) {
		case FFT_MODE_PWS:
			CsvOutputPws(oCsvFile);
			break;
		case FFT_MODE_OCT:
			CsvOutputOct(oCsvFile);
			break;
		case FFT_MODE_CRF:
			CsvOutputCrf(oCsvFile);
			break;
		case FFT_MODE_PHS:
			CsvOutputPhs(oCsvFile);
			break;
		case FFT_MODE_CRS:
			CsvOutputCrs(oCsvFile);
			break;
		case FFT_MODE_COH:
			CsvOutputCoh(oCsvFile);
			break;
		case FFT_MODE_CEP:
			CsvOutputCep(oCsvFile);
			break;
		}
	}
}

void CFftWnd::CsvInput(LPCSTR pFileName)
{
	CCsvFile oCsvFile;

	if (oCsvFile.Open(pFileName, CFile::modeRead | CFile::shareDenyNone)) {
		oCsvFile.Input();

		CString sHeader = oCsvFile.GetString(0);
		if (sHeader != "FFT") {
			::AfxMessageBox(IDS_ERR_CSV_FORMAT, MB_OK | MB_ICONEXCLAMATION);
			return;
		}

		m_bValidFftData = FALSE;

		sHeader = oCsvFile.GetString(1);
		if (sHeader == "Power Spectrum") {
			CsvInputPws(oCsvFile);
		} else if (sHeader == "Octave band") {
			CsvInputOct(oCsvFile);
		} else if (sHeader == "Correlation") {
			CsvInputCrf(oCsvFile);
		} else if (sHeader == "Phase") {
			CsvInputPhs(oCsvFile);
		} else if (sHeader == "Cross Spectrum") {
			CsvInputCrs(oCsvFile);
		} else if (sHeader == "Coherence") {
			CsvInputCoh(oCsvFile);
		} else if (sHeader == "Cepstrum") {
			CsvInputCep(oCsvFile);
		} else {
			::AfxMessageBox(IDS_ERR_CSV_FORMAT, MB_OK | MB_ICONEXCLAMATION);
			return;
		}

		m_bGetData = TRUE;

		Redraw();
	}
}

void CFftWnd::OnLButtonDblClk(UINT /*nFlags*/, CPoint /*point*/)
{
	m_pFftDlg->FullScreen();
}

void CFftWnd::GetThemaHandle()
{
	if (m_hTheme != NULL) {
		g_dllUxTheme.CloseThemeData(m_hTheme);
		m_hTheme = NULL;
	}

	if (g_dllUxTheme.IsThemeActive())
		m_hTheme = g_dllUxTheme.OpenThemeData(m_hWnd, L"TAB");
}

LRESULT CFftWnd::OnThemeChanged()
{
	GetThemaHandle();

	SetBitmapAll();

	return 1;
}
