// WaveViewWnd.cpp : 実装ファイル
//

#include "stdafx.h"
#include "RTA.h"
#include "WaveViewWnd.h"
#include "Spline.h"

#define COLOR_DATA	RGB(0, 0, 255)

// CWaveViewWnd

CWaveViewWnd::CWaveViewWnd()
{
	m_bInitialized = FALSE;
}

BEGIN_MESSAGE_MAP(CWaveViewWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CWaveViewWnd メッセージ ハンドラ

void CWaveViewWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
	dc.BitBlt(0, 0, m_nWidth, m_nHeight, &m_dcMem, 0, 0, SRCCOPY);
	m_dcMem.SelectObject(pBitmapOrg);
}

BOOL CWaveViewWnd::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void CWaveViewWnd::Initialize()
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

	m_penData.CreatePen(PS_SOLID, 0, COLOR_LEFT);
	m_penBlack.CreatePen(PS_SOLID, 0, RGB(0, 0, 0));
	m_penGray.CreatePen(PS_SOLID, 0, RGB(128, 128, 128));
	m_penLightGray.CreatePen(PS_SOLID, 0, RGB(192, 192, 192));

	m_brushWhite.CreateSolidBrush(RGB(255, 255, 255));
	m_brushData.CreateSolidBrush(COLOR_DATA);

	m_dcMem.SetBkMode(TRANSPARENT);
	m_dcMem.SetTextColor(RGB(64, 64, 64));

	m_nScaleLeft = 0;
	m_nScaleTop = 0;
	m_nScaleRight = m_nWidth;
	m_nScaleBottom = m_nHeight;
	m_nScaleWidth = m_nScaleRight - m_nScaleLeft;
	m_nScaleHeight = m_nScaleBottom - m_nScaleTop;
	m_nScaleCenter = m_nScaleTop + m_nScaleHeight / 2;
}

void CWaveViewWnd::SetBitmap()
{
	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
	CPen *pPenOrg = m_dcMem.SelectObject(&m_penGray);

	CRect rect;
	rect.SetRect(0, 0, m_nWidth, m_nHeight);
	m_dcMem.FillRect(rect, &m_brushWhite);

	m_dcMem.MoveTo(m_nScaleLeft, m_nScaleCenter);
	m_dcMem.LineTo(m_nScaleRight, m_nScaleCenter);

	m_dcMem.SelectObject(pBitmapOrg);
	m_dcMem.SelectObject(pPenOrg);
}

void CWaveViewWnd::DispGraph(const WaveForm *pWaveForm)
{
	switch (pWaveForm->nDataType) {
	case WAVEFORM_FM:
		DispGraphFM((WaveFormFM *)pWaveForm);
		break;
	default:
		DispGraphManual((WaveFormManual *)pWaveForm);
		break;
	}
}

void CWaveViewWnd::DispGraphManual(const WaveFormManual *pWaveForm)
{
	int i;
	double fStartX = 0;
	double fDispX = 1;
	double fMaxY = 1;
	double fMinY = -1;
	double fLevelRange = (fMaxY - fMinY);

	SetBitmap();

	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
	CPen *pPenOrg = m_dcMem.SelectObject(&m_penData);
	CBrush *pBrushOrg = m_dcMem.SelectObject(&m_brushData);

	POINT *pPoint = new POINT[pWaveForm->nDataNum];

	for (i = 0; i < pWaveForm->nDataNum; i++) {
		pPoint[i].x = (int)((pWaveForm->vertex[i].x - fStartX) * m_nScaleWidth / fDispX + 0.5) + m_nScaleLeft;
		pPoint[i].y = (int)(m_nScaleCenter - pWaveForm->vertex[i].y * m_nScaleHeight / fLevelRange + 0.5);
	}

	if (pWaveForm->bSmoothing) {
		int nPoint = pWaveForm->nDataNum;
		CSpline spl;
		double *xp = new double[nPoint];
		double *yp = new double[nPoint];

		for (i = 0; i < nPoint; i++) {
			xp[i] = pPoint[i].x;
			yp[i] = pPoint[i].y;
		}

		spl.MakeTable(xp, yp, nPoint);

		delete [] yp;
		delete [] xp;

		POINT *pSpline = new POINT[m_nScaleWidth];
		for (i = 0; i < m_nScaleWidth; i++) {
			pSpline[i].x = m_nScaleLeft + i;
			pSpline[i].y = (LONG)(spl.Spline(pSpline[i].x) + 0.5);
			if (pSpline[i].y < m_nScaleTop)
				pSpline[i].y = m_nScaleTop;
			else if (pSpline[i].y > m_nScaleBottom)
				pSpline[i].y = m_nScaleBottom;
		}

		m_dcMem.Polyline(pSpline, m_nScaleWidth);

		delete [] pSpline;
	} else {
		if (pWaveForm->nDataNum >= 2)
			m_dcMem.Polyline(pPoint, pWaveForm->nDataNum);
	}

	delete [] pPoint;

	m_dcMem.SelectObject(pBitmapOrg);
	m_dcMem.SelectObject(pPenOrg);
	m_dcMem.SelectObject(pBrushOrg);

	Invalidate(FALSE);
}

void CWaveViewWnd::DispGraphFM(const WaveFormFM *pWaveForm)
{
	SetBitmap();

	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
	CPen *pPenOrg = m_dcMem.SelectObject(&m_penData);

	POINT *pPoint = new POINT[m_nScaleWidth];

	for (int i = 0; i < m_nScaleWidth; i++) {
		double fAngle = 8 * M_PI * i / m_nScaleWidth;
		double fAmp = sin(fAngle + pWaveForm->fModAmp * sin(pWaveForm->fModFreq * fAngle));

		pPoint[i].x = m_nScaleLeft + i;
		pPoint[i].y = (int)(m_nScaleCenter - fAmp * (m_nScaleHeight - 10) / 2 + 0.5);
	}

	m_dcMem.Polyline(pPoint, m_nScaleWidth);

	delete [] pPoint;

	m_dcMem.SelectObject(pBitmapOrg);
	m_dcMem.SelectObject(pPenOrg);

	Invalidate(FALSE);
}
