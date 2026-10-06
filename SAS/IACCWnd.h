#pragma once

#include "RemarkWnd.h"

class CIACCWnd : public CWnd
{
public:
	CIACCWnd();

	void Initialize(int nFontSize, LPCSTR pTitle, int nRemark);
	void DispGraph(const double *pData, int nData, double fRate, double IACC, double TIACC, double WIACC1, double WIACC2);

protected:
	BOOL m_bInitialize;
	CDC m_dcMem;
	CBitmap m_bitmapMem;
	int m_nWidth, m_nHeight;
	int m_nScaleLeft, m_nScaleTop, m_nScaleRight, m_nScaleBottom;
	int m_nScaleWidth, m_nScaleHeight, m_nScaleVCenter, m_nScaleHCenter;
	CPen m_penData;
	CPen m_penBlack, m_penGray, m_penLightGray;
	CPen m_penIACC, m_penTIACC, m_penWIACC;
	CBrush m_brushWhite;
	CFont m_Font, m_Font2;
	CRemarkWnd m_wndRemark;
	CRgn m_Rgn;

	void SetBitmap();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
