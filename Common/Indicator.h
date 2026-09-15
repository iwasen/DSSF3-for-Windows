#pragma once

#include "WaveNotify.h"

class CIndicator : public CWnd
{
public:
	CIndicator();

	void Initialize();
	void Clear();
	void WaveData(LPWAVENOTIFY pWaveNotify);

protected:
	CDC m_dcMem;
	CRect m_Rect;
	CBitmap m_bitmapMono;
	CBitmap m_bitmapLeft;
	CBitmap m_bitmapRight;
	CBrush m_brushGray;
	int m_nLevelLeft;
	int m_nLevelRight;
	int m_nPrevLeft;
	int m_nPrevRight;
	BOOL m_bStereo;
	BOOL m_bOverLeft;
	BOOL m_bOverRight;

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
};
