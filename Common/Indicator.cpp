// Indicator.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Indicator.h"
#include "common.h"
#include "comres.h"

#define MIN_LEVEL	80

/////////////////////////////////////////////////////////////////////////////
// CIndicator

CIndicator::CIndicator()
{
	m_bOverLeft = FALSE;
	m_bOverRight = FALSE;
}

BEGIN_MESSAGE_MAP(CIndicator, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIndicator メッセージ ハンドラ

void CIndicator::OnPaint()
{
	CPaintDC dc(this); // 描画用のデバイス コンテキスト
	CRect rect;

	if (m_bStereo) {
		int nHeight = m_Rect.bottom / 2;

		m_dcMem.SelectObject(&m_bitmapLeft);
		dc.BitBlt(0, 0, m_nLevelLeft, nHeight, &m_dcMem, 0, 0, SRCCOPY);

		rect.SetRect(m_nLevelLeft, 0, m_Rect.right, nHeight);
		dc.FillRect(rect, &m_brushGray);

		m_dcMem.SelectObject(&m_bitmapRight);
		dc.BitBlt(0, nHeight, m_nLevelRight, nHeight, &m_dcMem, 0, 0, SRCCOPY);

		rect.SetRect(m_nLevelRight, nHeight, m_Rect.right, m_Rect.bottom);
		dc.FillRect(rect, &m_brushGray);
	} else {
		m_dcMem.SelectObject(&m_bitmapMono);
		dc.BitBlt(0, 0, m_nLevelLeft, m_Rect.bottom, &m_dcMem, 0, 0, SRCCOPY);

		rect.SetRect(m_nLevelLeft, 0, m_Rect.right, m_Rect.bottom);
		dc.FillRect(rect, &m_brushGray);
	}
}

#define COLOR_RIGHT			RGB(255, 64, 192)
void CIndicator::Initialize()
{
	GetClientRect(m_Rect);
	m_Rect.bottom &= ~1;

	m_nLevelLeft = 0;
	m_nLevelRight = 0;
	m_nPrevLeft = 0;
	m_nPrevRight = 0;

	CDC *pDC = GetDC();
	m_dcMem.CreateCompatibleDC(pDC);
	ReleaseDC(pDC);

	LoadBimapStretch(m_bitmapMono, IDB_INDICATOR_LEFT, m_Rect.Width(), m_Rect.Height());
	LoadBimapStretch(m_bitmapLeft, IDB_INDICATOR_LEFT, m_Rect.Width(), m_Rect.Height() / 2);
	LoadBimapStretch(m_bitmapRight, IDB_INDICATOR_RIGHT, m_Rect.Width(), m_Rect.Height() / 2);

	m_brushGray.CreateSolidBrush(RGB(128, 128, 128));
}

void CIndicator::Clear()
{
	m_nLevelLeft = 0;
	m_nLevelRight = 0;
	m_nPrevLeft = 0;
	m_nPrevRight = 0;

	m_bStereo = FALSE;
	m_bOverLeft = FALSE;
	m_bOverRight = FALSE;

	Invalidate(FALSE);
}

void CIndicator::WaveData(LPWAVENOTIFY pWaveNotify)
{
	RECT rect;
	BOOL bLeftColorChange = FALSE;
	BOOL bRightColorChange = FALSE;

	if (!(pWaveNotify->nFlags & WHDR_DONE))
		return;

	double fMaxData = GetMaxData(pWaveNotify->pSamplesData, pWaveNotify->nSamplesNum, pWaveNotify->nChannels);
	if (fMaxData != 0)
		m_nLevelLeft = m_Rect.right + (int)(dB20((double)fMaxData) / MIN_LEVEL * m_Rect.right);
	else
		m_nLevelLeft = 0;

	if (m_nLevelLeft < 0)
		m_nLevelLeft = 0;
	if (pWaveNotify->nChannels == 2) {
		fMaxData = GetMaxData(pWaveNotify->pSamplesData + 1, pWaveNotify->nSamplesNum, pWaveNotify->nChannels);
		if (fMaxData != 0)
			m_nLevelRight = m_Rect.right + (int)(dB20((double)fMaxData) / MIN_LEVEL * m_Rect.right);
		else
			m_nLevelRight = 0;

		if (m_nLevelRight < 0)
			m_nLevelRight = 0;
		if (bLeftColorChange) {
			rect.left = 0;
			rect.right = m_Rect.right;
		} else {
			rect.left = min(m_nPrevLeft, m_nLevelLeft);
			rect.right = max(m_nPrevLeft, m_nLevelLeft);
		}
		rect.top = 0;
		rect.bottom = m_Rect.bottom / 2;
		InvalidateRect(&rect, FALSE);

		if (bRightColorChange) {
			rect.left = 0;
			rect.right = m_Rect.right;
		} else {
			rect.left = min(m_nPrevRight, m_nLevelRight);
			rect.right = max(m_nPrevRight, m_nLevelRight);
		}
		rect.top = m_Rect.bottom / 2;
		rect.bottom = m_Rect.bottom;
		InvalidateRect(&rect, FALSE);

		m_nPrevLeft = m_nLevelLeft;
		m_nPrevRight = m_nLevelRight;

		m_bStereo = TRUE;
	} else {
		m_nLevelRight = 0;

		if (bLeftColorChange) {
			rect.left = 0;
			rect.right = max(m_nPrevLeft, m_nLevelLeft);
		} else {
			rect.left = min(m_nPrevLeft, m_nLevelLeft);
			rect.right = m_Rect.right;
		}
		rect.top = 0;
		rect.bottom = m_Rect.bottom;
		InvalidateRect(&rect, FALSE);

		m_nPrevLeft = m_nLevelLeft;

		m_bStereo = FALSE;
	}
}
