#pragma once

#include "RemarkWnd.h"
#include "FileIO.h"

class CParamWnd : public CWnd
{
public:
	CParamWnd();

	void Initialize(int nFontSize);
	void DispGraph(const int *pDataFreq, int nFreqBand, const double *pDataAll, const double *pDataLeft, const double *pDataRight, int nData, double scaleMin, double scaleMax, double scaleStep, LPCSTR vAxis);
	void DispRemark(LPCSTR pTitle, int nChannel);

protected:
	BOOL m_bInitialize;
	CDC m_dcMem;
	CBitmap m_bitmapMem;
	int m_nWidth, m_nHeight;
	int m_nScaleLeft, m_nScaleTop, m_nScaleRight, m_nScaleBottom;
	int m_nScaleWidth, m_nScaleHeight;
	CPen m_penAll, m_penLeft, m_penRight;
	CPen m_penBlack, m_penGray;
	CBrush m_brushAll, m_brushLeft, m_brushRight;
	CBrush m_brushWhite;
	CFont m_Font, m_Font2;
	CRemarkWnd m_wndRemark;

	void SetBitmap(const int *pDataFreq, int nFreqBand, int nData, double scaleMin, double scaleMax, double scaleStep, LPCSTR vAxis);
	void DispGraphSub(const int *pDataFreq, const double *pData, int nData, double scaleMin, double scaleMax, int channel);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
