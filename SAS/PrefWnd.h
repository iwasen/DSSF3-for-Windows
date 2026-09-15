#pragma once

#include "RemarkWnd.h"

class CPrefWnd : public CWnd
{
public:
	CPrefWnd();

	void Initialize(int nFontSize);
	void DispGraph(const double *pData, int nData, double maxTauE, double maxS);

protected:
	void SetBitmap(int minS, int maxTe);

	BOOL m_bInitialize;
	CDC m_dcMem;
	CBitmap m_bitmapMem;
	int m_nWidth, m_nHeight;
	int m_nScaleLeft, m_nScaleTop, m_nScaleRight, m_nScaleBottom;
	int m_nScaleWidth, m_nScaleHeight;
	CPen m_penData, m_penTauE, m_penS;
	CPen m_penBlack, m_penGray, m_penLightGray;
	CBrush m_brushWhite;
	CFont m_Font, m_Font2;
	CRemarkWnd m_wndRemark;

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
