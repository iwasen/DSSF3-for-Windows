#pragma once

#include "RemarkWnd.h"

class CMtfWnd : public CWnd
{
public:
	CMtfWnd();

	void Initialize(int nFontSize, LPCTSTR pTitle, int nRemark, COLORREF color);
	void DispGraph(const double *pMtfData, const double *pMtfFreq);

protected:
	BOOL m_bInitialize;
	CDC m_dcMem;
	CBitmap m_bitmapMem;
	int m_nWidth, m_nHeight;
	int m_nScaleLeft, m_nScaleTop, m_nScaleRight, m_nScaleBottom;
	int m_nScaleWidth, m_nScaleHeight;
	int m_nScaleCenter;
	CPen m_penMtf;
	CPen m_penBlack, m_penGray;
	CBrush m_brushWhite;
	CBrush m_brushData;
	CFont m_Font, m_Font2, m_Font3;
	CRemarkWnd m_wndRemark;

	void SetBitmap();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
