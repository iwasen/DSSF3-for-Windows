// FilterWnd.cpp : 実装ファイル
//

#include "stdafx.h"
#include "RTA.h"
#include "FilterWnd.h"

// CFilterWnd

BEGIN_MESSAGE_MAP(CFilterWnd, CFreqWnd)
END_MESSAGE_MAP()

// CFilterWnd メッセージ ハンドラ

void CFilterWnd::DispGraph(const CDigitalFilter &cDigitalFilter, int minFreq, int maxFreq, int minLevel, int maxLevel)
{
	const double fMaxFreqLog = log((double)maxFreq);
	const double fMinFreqLog = log((double)minFreq);
	CRgn rgn;
	double fLevel;
	int x, y;

	SetBitmap(0, minFreq, maxFreq, minLevel, maxLevel, FALSE);

	rgn.CreateRectRgn(m_nScaleLeft, m_nScaleTop, m_nScaleRight - 1, m_nScaleBottom);
	m_dcMem.SelectClipRgn(&rgn);

	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
	CPen *pPenOrg = m_dcMem.SelectObject(&m_penFreq);

	const double fStepY = (double)m_nScaleHeight / (minLevel - maxLevel);

	for (int i = 0; i < m_nScaleWidth; i++) {
		x = m_nScaleLeft + i;
		fLevel = cDigitalFilter.GetLevel(exp(fMinFreqLog + (fMaxFreqLog - fMinFreqLog) * i / m_nScaleWidth));
		y = m_nScaleTop + (int)((dB20(fLevel) - maxLevel) * fStepY + 0.5);
		if (i == 0)
			m_dcMem.MoveTo(x, y);
		else
			m_dcMem.LineTo(x, y);
	}

	m_dcMem.SelectClipRgn(NULL);

	m_dcMem.SelectObject(pBitmapOrg);
	m_dcMem.SelectObject(pPenOrg);

	Invalidate(FALSE);
	UpdateWindow();
}
