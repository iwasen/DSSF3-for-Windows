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
	SetBitmap(0, minFreq, maxFreq, minLevel, maxLevel, FALSE);

	CRgn rgn;
	rgn.CreateRectRgn(m_nScaleLeft, m_nScaleTop, m_nScaleRight - 1, m_nScaleBottom);
	m_dcMem.SelectClipRgn(&rgn);

	CBitmap *pBitmapOrg = m_dcMem.SelectObject(&m_bitmapMem);
	CPen *pPenOrg = m_dcMem.SelectObject(&m_penFreq);

	const double fStepY = (double)m_nScaleHeight / (minLevel - maxLevel);
	const double fMaxFreqLog = log((double)maxFreq);
	const double fMinFreqLog = log((double)minFreq);
	for (int i = 0; i < m_nScaleWidth; i++) {
		int x = m_nScaleLeft + i;
		double fLevel = cDigitalFilter.GetLevel(exp(fMinFreqLog + (fMaxFreqLog - fMinFreqLog) * i / m_nScaleWidth));
		int y = m_nScaleTop + (int)((dB20(fLevel) - maxLevel) * fStepY + 0.5);
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
