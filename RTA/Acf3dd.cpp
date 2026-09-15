// Acf3dd.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Rta.h"
#include "Acf3dd.h"

/////////////////////////////////////////////////////////////////////////////
// CAcf3dd

CAcf3dd::CAcf3dd()
{
	m_bInitialized = FALSE;
	memset(m_aDispData, 0, sizeof(m_aDispData));
	m_pWork = NULL;
	m_pCheckTbl = NULL;
	m_nTimeDataNum = 0;
}

CAcf3dd::~CAcf3dd()
{
	FreeBuffers();
}

BEGIN_MESSAGE_MAP(CAcf3dd, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAcf3dd メッセージ ハンドラ

void CAcf3dd::Initialize(int nFontSize)
{
	if (m_bInitialized)
		return;

	CDC *pDC = GetDC();
	m_dcMem.CreateCompatibleDC(pDC);
	m_dcMem2.CreateCompatibleDC(pDC);
	ReleaseDC(pDC);

	CString str;
	LOGFONT font;
	memset(&font, 0, sizeof(font));
	font.lfHeight = CDpi::OriginalDpi(nFontSize, 88);
	font.lfCharSet = DEFAULT_CHARSET;
	str.LoadString(IDS_DEFAULTFONT);
	strcpy_s(font.lfFaceName, str);
	m_Font.CreatePointFontIndirect(&font, &m_dcMem);
	font.lfEscapement = 900;
	m_Font2.CreatePointFontIndirect(&font, &m_dcMem);

	m_penLeft.CreatePen(PS_SOLID, 0, COLOR_LEFT);
	m_penRight.CreatePen(PS_SOLID, 0, COLOR_RIGHT);

	Initialize2();

	SetBitmap();

	m_bInitialized = TRUE;
}

void CAcf3dd::Initialize2()
{
	RECT rect;
	GetClientRect(&rect);

	m_nWidth = rect.right;
	m_nHeight = rect.bottom;

	CDC *pDC = GetDC();
	m_bitmapMem.CreateCompatibleBitmap(pDC, m_nWidth, m_nHeight);
	ReleaseDC(pDC);
}

void CAcf3dd::Uninitialize2()
{
	m_bitmapMem.DeleteObject();
}

void CAcf3dd::Resize()
{
	Uninitialize2();
	Initialize2();
	SetBitmap();
}

void CAcf3dd::SetBitmap()
{
	int	nLevel;
	int x, y, y1, y2;
	int dx, dy;
	int i, j;
	CString sText;
	CSize size;
	CBitmap *pBitmapOrg;
	CPen *pPenOrg;
	CFont *pFontOrg;
	CRect rect;
	double t;
	int ns;
	CBrush brushBkGrnd(COLOR_BKGRND);
	CPen penBlack(PS_SOLID, 0, COLOR_BLACK);
	CPen penGray(PS_SOLID, 0, COLOR_GRAY);

	TEXTMETRIC tm;
	m_dcMem.GetTextMetrics(&tm);

	m_nScaleLeft2 = tm.tmAveCharWidth * 4 + tm.tmHeight + 5;
	m_nScaleTop = tm.tmHeight / 2 + 1;
	m_nScaleRight = m_nWidth - (tm.tmAveCharWidth * 5 + 5);
	m_nScaleBottom2 = m_nHeight - ((tm.tmHeight + 1) * 2 + 1);
	m_nScaleWidth = m_nScaleRight - m_nScaleLeft2;
	m_nScaleHeight = m_nScaleBottom2 - m_nScaleTop;

	m_nTimeDataNum = g_oSetData.Acf.nTimeDataNum;
	m_dx = (m_nScaleWidth / 4.5) / max(m_nTimeDataNum, 1);
	if (m_dx >= 1) {
		m_dx = (int)m_dx;
		m_dy = (int)(m_dx * 5 / 3);
	} else {
		m_dy = m_dx * 5 / 3;
	}
	dx = (int)(m_dx * m_nTimeDataNum);
	dy = (int)(m_dy * m_nTimeDataNum);

	m_nScaleLeft = m_nScaleLeft2 + dx;
	m_nScaleTop2 = m_nScaleTop + dy;
	m_nScaleRight2 = m_nScaleRight - dx;
	m_nScaleBottom = m_nScaleBottom2 - dy;
	m_nScaleWidth2 = m_nScaleRight2 - m_nScaleLeft2;
	m_nScaleHeight2 = m_nScaleBottom2 - m_nScaleTop2;

	pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
	pFontOrg = m_dcMem.SelectObject(&m_Font);
	pPenOrg = m_dcMem.SelectObject(&penBlack);

	rect.SetRect(0, 0, m_nWidth, m_nHeight);
	m_dcMem.FillRect(rect, &brushBkGrnd);

	POINT point[6];
	point[0].x = m_nScaleRight2;
	point[0].y = m_nScaleBottom2;
	point[1].x = m_nScaleRight;
	point[1].y = m_nScaleBottom;
	point[2].x = m_nScaleRight;
	point[2].y = m_nScaleTop;
	point[3].x = m_nScaleLeft;
	point[3].y = m_nScaleTop;
	point[4].x = m_nScaleLeft2;
	point[4].y = m_nScaleTop2;
	point[5].x = m_nScaleLeft2;
	point[5].y = m_nScaleBottom2;
	m_dcMem.Polygon(point, 6);

	m_dcMem.SelectObject(&penGray);

	m_dcMem.MoveTo(m_nScaleLeft, m_nScaleBottom);
	m_dcMem.LineTo(m_nScaleRight, m_nScaleBottom);
	m_dcMem.MoveTo(m_nScaleLeft, m_nScaleBottom);
	m_dcMem.LineTo(m_nScaleLeft, m_nScaleTop);
	m_dcMem.MoveTo(m_nScaleLeft, m_nScaleBottom);
	m_dcMem.LineTo(m_nScaleLeft2, m_nScaleBottom2);

	m_dcMem.SetBkMode(TRANSPARENT);
	m_dcMem.SetTextColor(COLOR_TEXT);

	double t1 = g_oSetData.Acf.fMaxTau / 6;
	double step = pow(10.0, floor(log10(t1)));
	double t2 = t1 / step;
	if (t2 < 2) {
		step *= 0.5;
		ns = 2;
	} else if (t2 < 5)
		ns = 2;
	else
		ns = 5;

	for (t = 0; t <= g_oSetData.Acf.fMaxTau + step * 0.5; t += step) {
		x =  m_nScaleLeft2 + (int)(t * m_nScaleWidth2 / g_oSetData.Acf.fMaxTau);

		if (int(t / step + 0.5) % ns == 0) {
			if (t < 1000)
				sText.Format("%g", t);
			else
				sText.Format("%gk", t / 1000);
			size = m_dcMem.GetOutputTextExtent(sText);
			m_dcMem.TextOut(x - size.cx / 2, m_nScaleBottom2 + 2, sText);

			m_dcMem.MoveTo(x, m_nScaleBottom2 - 6);
			m_dcMem.LineTo(x, m_nScaleBottom2);
		} else {
			m_dcMem.MoveTo(x, m_nScaleBottom2 - 4);
			m_dcMem.LineTo(x, m_nScaleBottom2);
		}
	}

	if (g_oSetData.Acf.nScaleType == 0) {
		if (g_oSetData.Acf.bScaleAbs) {
			for (nLevel = 10; nLevel >= 0; nLevel--) {
				sText.Format("%g", nLevel / 10.0);
				size = m_dcMem.GetOutputTextExtent(sText);
				y = m_nScaleTop2 - (nLevel - 10) * m_nScaleHeight2 / 10;

				m_dcMem.MoveTo(m_nScaleLeft2, y);
				m_dcMem.LineTo(m_nScaleLeft2 + 4, y);

				m_dcMem.TextOut(m_nScaleLeft2 - size.cx - 2, y - size.cy / 2, sText);

				y = m_nScaleTop - (nLevel - 10) * m_nScaleHeight2 / 10;

				m_dcMem.MoveTo(m_nScaleRight, y);
				m_dcMem.LineTo(m_nScaleRight - 4, y);

				m_dcMem.TextOut(m_nScaleRight + 2, y - size.cy / 2, sText);
			}
		} else {
			for (nLevel = 10; nLevel >= -10; nLevel -= 2) {
				sText.Format("%g", nLevel / 10.0);
				size = m_dcMem.GetOutputTextExtent(sText);
				y = m_nScaleTop2 - (nLevel - 10) * m_nScaleHeight2 / 20;

				m_dcMem.MoveTo(m_nScaleLeft2, y);
				m_dcMem.LineTo(m_nScaleLeft2 + 4, y);

				m_dcMem.TextOut(m_nScaleLeft2 - size.cx - 2, y - size.cy / 2, sText);

				y = m_nScaleTop - (nLevel - 10) * m_nScaleHeight2 / 20;

				m_dcMem.MoveTo(m_nScaleRight, y);
				m_dcMem.LineTo(m_nScaleRight - 4, y);

				m_dcMem.TextOut(m_nScaleRight + 2, y - size.cy / 2, sText);
			}
		}
	} else {
		t1 = (double)-g_oSetData.Acf.nScaleLevel / 6;
		step = pow(10.0, floor(log10(t1)));
		t2 = t1 / step;
		if (t2 < 2) {
			step *= 0.5;
			ns = 2;
		} else if (t2 < 5)
			ns = 2;
		else
			ns = 5;

		for (t = 0; t >= g_oSetData.Acf.nScaleLevel; t -= step) {
			y1 = m_nScaleTop2 + (int)(t * m_nScaleHeight2 / g_oSetData.Acf.nScaleLevel);
			y2 = m_nScaleTop + (int)(t * m_nScaleHeight2 / g_oSetData.Acf.nScaleLevel);

			if (int(-t / step + 0.5) % ns == 0) {
				sText.Format("%g", t);
				size = m_dcMem.GetOutputTextExtent(sText);

				m_dcMem.MoveTo(m_nScaleLeft2, y1);
				m_dcMem.LineTo(m_nScaleLeft2 + 6, y1);
				m_dcMem.TextOut(m_nScaleLeft2 - size.cx - 2, y1 - size.cy / 2, sText);

				m_dcMem.MoveTo(m_nScaleRight, y2);
				m_dcMem.LineTo(m_nScaleRight - 6, y2);
				m_dcMem.TextOut(m_nScaleRight + 2, y2 - size.cy / 2, sText);
			} else {
				m_dcMem.MoveTo(m_nScaleLeft2, y1);
				m_dcMem.LineTo(m_nScaleLeft2 + 4, y1);

				m_dcMem.MoveTo(m_nScaleRight, y2);
				m_dcMem.LineTo(m_nScaleRight - 4, y2);
			}
		}
	}

	for (i = 0; i < m_nTimeDataNum; i++) {
		x = m_nScaleRight2 + (int)(m_dx * i);
		y = m_nScaleBottom2 - (int)(m_dy * i);
		m_dcMem.MoveTo(x, y);
		m_dcMem.LineTo(x - 4, y);
	}

	sText.LoadString(IDS_TAU);
	sText += " [ms]";
	size = m_dcMem.GetOutputTextExtent(sText);
	m_dcMem.TextOut(m_nScaleLeft2 + (m_nScaleWidth2 - size.cx) / 2, m_nHeight - size.cy - 2, sText);

	m_dcMem.SelectObject(&m_Font2);
	sText.LoadString(IDS_ACF);
	if (g_oSetData.Acf.nScaleType != 0)
		sText += " [dB]";
	size = m_dcMem.GetOutputTextExtent(sText);
	m_dcMem.TextOut(2, m_nScaleTop2 + (m_nScaleHeight2 + size.cx) / 2, sText);

	if ((HBITMAP)m_bitmapMem2 != NULL)
		m_bitmapMem2.DeleteObject();

	CDC *pDC = GetDC();
	m_bitmapMem2.CreateCompatibleBitmap(pDC, m_nScaleWidth, m_nScaleHeight);
	ReleaseDC(pDC);

	CBitmap *pBitmap2Org = m_dcMem2.SelectObject(&m_bitmapMem2);
	m_dcMem2.BitBlt(0, 0, m_nScaleWidth, m_nScaleHeight, &m_dcMem, m_nScaleLeft2, m_nScaleTop, SRCCOPY);
	m_dcMem2.SelectObject(pBitmap2Org);

	m_dcMem.SelectObject(pBitmapOrg);
	m_dcMem.SelectObject(pFontOrg);
	m_dcMem.SelectObject(pPenOrg);

	FreeBuffers();
	for (i = 0; i < 2; i++) {
		m_aDispData[i].m_pPoint = new POINT *[m_nTimeDataNum];
		m_aDispData[i].m_pPointCount = new int[m_nTimeDataNum];
		for (j = 0; j < m_nTimeDataNum; j++) {
			m_aDispData[i].m_pPoint[j] = new POINT[m_nScaleWidth2 + 2];
			m_aDispData[i].m_pPointCount[j] = 0;
		}
		m_aDispData[i].m_nAllocCount = m_nTimeDataNum;
		m_aDispData[i].m_nCurrentPtr = 0;
	}

	if (m_pWork == NULL)
		m_pWork = new POINT[m_nScaleWidth2 + 2];

	if (m_pCheckTbl == NULL)
		m_pCheckTbl = new int[m_nScaleWidth];
}

void CAcf3dd::DispGraph(const double *pLeftData, const double *pRightData, int nDataSize, double fSamplingRate)
{
	if (pLeftData != NULL)
		Calc3dd(pLeftData, &m_aDispData[0], nDataSize, fSamplingRate);

	if (pRightData != NULL)
		Calc3dd(pRightData, &m_aDispData[1], nDataSize, fSamplingRate);

	Invalidate(FALSE);
}

void CAcf3dd::ClearGraph()
{
	SetBitmap();
	Invalidate();
}

void CAcf3dd::Calc3dd(const double *pData, DISPDATA *pDispData, int nDataSize, double fSamplingRate)
{
	int i, j;
	int ptr = pDispData->m_nCurrentPtr;
	POINT *pPoint = pDispData->m_pPoint[ptr];
	int x, y;
	int x2 = 0;
	double fStepY;
	double fTmp2 = double(m_nScaleWidth2) / g_oSetData.Acf.fMaxTau;
	int nDispSize = nDataSize / 2;
	int nPointCount = 0;
	int xOffset = 0;
	int yOffset = m_nScaleBottom2 - m_nScaleBottom;
	int dx, xj;
	double dl;
	double fTimeStep = 1 / fSamplingRate * 1000;
	double fLevel, level2;
	int nMaxLevel;

	if (g_oSetData.Acf.nScaleType == 0) {
		level2 = (double)1;
		nMaxLevel = 1;
		if (g_oSetData.Acf.bScaleAbs)
			fStepY = -m_nScaleHeight2;
		else {
			fStepY = (double)-m_nScaleHeight2 / 2;
		}
	} else {
		level2 = dB10(fabs(pData[0]));
		fStepY = (double)m_nScaleHeight2 / g_oSetData.Acf.nScaleLevel;
		nMaxLevel = 0;
	}

	for (i = 0; i < nDispSize; i++) {
		x = int((i * fTimeStep) * fTmp2);

		if (g_oSetData.Acf.nScaleType == 0) {
			if (g_oSetData.Acf.bScaleAbs)
				fLevel = fabs(pData[i]);
			else
				fLevel = pData[i];
		} else
			fLevel = dB10(fabs(pData[i]));

		if (x != x2) {
			dx = x - x2;
			dl = fLevel - level2;
			for (j = 0; j < dx; j++) {
				xj = x2 + j;
				if (xj >= m_nScaleWidth2)
					goto big_break;
				if (xj >= 0) {
					pPoint[nPointCount].x = xj + xOffset;
					y = int(((level2 + dl * j / dx) - nMaxLevel) * fStepY) + yOffset;
					if (y > m_nScaleHeight)
						y = m_nScaleHeight;
					pPoint[nPointCount].y = y;
					nPointCount++;
				}
			}

			x2 = x;
			level2 = fLevel;
			fLevel = 0;
		}
	}

big_break:
	pDispData->m_pPointCount[ptr] = nPointCount;

	if (++(pDispData->m_nCurrentPtr) >= m_nTimeDataNum)
		pDispData->m_nCurrentPtr = 0;
}

void CAcf3dd::FreeBuffers()
{
	int i, j;

	for (i = 0; i < 2; i++) {
		if (m_aDispData[i].m_pPoint != NULL) {
			for (j = 0; j < m_aDispData[i].m_nAllocCount; j++) {
				if (m_aDispData[i].m_pPoint[j] != NULL) {
					delete [] m_aDispData[i].m_pPoint[j];
					m_aDispData[i].m_pPoint[j] = NULL;
				}
			}
			delete [] m_aDispData[i].m_pPoint;
			m_aDispData[i].m_pPoint = NULL;
		}

		if (m_aDispData[i].m_pPointCount != NULL) {
			delete [] m_aDispData[i].m_pPointCount;
			m_aDispData[i].m_pPointCount = NULL;
		}
	}

	if (m_pWork != NULL) {
		delete [] m_pWork;
		m_pWork = NULL;
	}

	if (m_pCheckTbl != NULL) {
		delete [] m_pCheckTbl;
		m_pCheckTbl = NULL;
	}
}

void CAcf3dd::OnPaint()
{
	CPaintDC dc(this); // 描画用のデバイス コンテキスト

	if (m_bInitialized) {
		CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
		CBitmap *pBitmap2Org = m_dcMem2.SelectObject(&m_bitmapMem2);

		m_dcMem2.BitBlt(0, 0, m_nScaleWidth, m_nScaleHeight, &m_dcMem, m_nScaleLeft2, m_nScaleTop, SRCCOPY);

		CPen *pPenOrg = m_dcMem2.SelectObject(&m_penLeft);
		PaintSub(&m_aDispData[0]);

		m_dcMem2.SelectObject(&m_penRight);
		PaintSub(&m_aDispData[1]);

		dc.BitBlt(m_nScaleLeft2, m_nScaleTop, m_nScaleWidth, m_nScaleHeight, &m_dcMem2, 0, 0, SRCCOPY);

		m_dcMem.SelectObject(pBitmapOrg);
		m_dcMem2.SelectObject(pBitmap2Org);
		m_dcMem2.SelectObject(pPenOrg);
	}
}

void CAcf3dd::PaintSub(const DISPDATA *pDispData)
{
	int i, j, ptr, nPointCount;
	int x, y;
	POINT *pPoint;

	if (m_pCheckTbl == NULL)
		return;

	for (i = 0; i < m_nScaleWidth; i++)
		m_pCheckTbl[i] = m_nScaleHeight;

	ptr = pDispData->m_nCurrentPtr;
	for (i = 0; i < m_nTimeDataNum; i++) {
		if (g_oSetData.Acf.nTimeDir != 0) {
			if (--ptr < 0)
				ptr = m_nTimeDataNum - 1;
		}

		nPointCount = pDispData->m_pPointCount[ptr];
		pPoint = pDispData->m_pPoint[ptr] + nPointCount - 1;
		for (j = 0; j < nPointCount - 1; j++) {
			x = pPoint->x + (int)(m_dx * i);
			y = pPoint->y - (int)(m_dy * i);
			if (m_pCheckTbl[x] > y)
				m_pCheckTbl[x] = y;

			m_pWork[j].x = x;
			m_pWork[j].y = m_pCheckTbl[x];

			pPoint--;
		}
		m_pWork[j].x = pPoint->x + (int)(m_dx * i);
		m_pWork[j].y = pPoint->y - (int)(m_dy * i);

		if (nPointCount != 0)
			m_dcMem2.Polyline(m_pWork, nPointCount);

//		if (g_oSetData.Acf.nScaleType == 0 && !g_oSetData.Acf.bScaleAbs && pDispData->m_nPointCount[ptr] != 0) {
//			m_dcMem2.MoveTo(m_dx * i, (m_nScaleTop2 - m_nScaleTop) + (m_nScaleHeight2 / 2) - m_dy * i);
//			m_dcMem2.LineTo(m_nScaleWidth2 + m_dx * i, (m_nScaleTop2 - m_nScaleTop) + (m_nScaleHeight2 / 2) - m_dy * i);
//		}

		if (g_oSetData.Acf.nTimeDir == 0) {
			if (++ptr >= m_nTimeDataNum)
				ptr = 0;
		}
	}
}

BOOL CAcf3dd::OnEraseBkgnd(CDC* pDC)
{
	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
	pDC->BitBlt(0, 0, m_nWidth, m_nHeight, &m_dcMem, 0, 0, SRCCOPY);
	m_dcMem.SelectObject(pBitmapOrg);

	return TRUE;
}
