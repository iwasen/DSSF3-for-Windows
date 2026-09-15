#pragma once

#include "RemarkWnd.h"

class CFreqWnd : public CWnd
{
public:
	CFreqWnd();

	void Initialize(int nFontSize, LPCTSTR pTitle, int nRemark, COLORREF freqColor, COLORREF phaseColor);
	void DispGraph(const double *pFreqData, int nData, double fRate, int nMinFreq, int nMaxFreq, int nMinLevel, int nMaxLevel, const double *pPhaseData, BOOL bMean);

protected:
	BOOL m_bInitialized;
	CDC m_dcMem;
	CBitmap m_bitmapMem;
	int m_nWidth, m_nHeight;
	int m_nScaleLeft, m_nScaleTop, m_nScaleRight, m_nScaleBottom;
	int m_nScaleWidth, m_nScaleHeight;
	int m_nScaleCenter;
	CPen m_penFreq;
	CPen m_penPhase;
	CPen m_penBlack, m_penGray;
	CBrush m_brushWhite;
	CFont m_Font, m_Font2, m_Font3;
	CRemarkWnd m_wndRemark;

	void SetBitmap(int nScale, int nMinFreq, int nMaxFreq, int nMinLevel, int nMaxLevel, BOOL bPhase);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
