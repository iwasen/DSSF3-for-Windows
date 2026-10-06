#pragma once

#include "RemarkWnd.h"

class CAcfWnd : public CWnd
{
public:
	CAcfWnd();

	void Initialize(int nFontSize, LPCSTR pTitle, PFNGRAPH pCallBackFunc, LPARAM nCallBackData);
	void DispGraph1(double totalTime, double startTime, double dispTime, const double *pData, int nData, double fTau1, double fPhi1, BOOL bHScroll);
	void DispGraph2(double totalTime, double startTime, double dispTime, const double *pData, int nData, double fTauE, double fTauE0, int nMaxLevel, int nMinLevel, BOOL bHScroll);

protected:
	BOOL m_bInitialize;
	CDC m_dcMem;
	CBitmap m_bitmapMem;
	LONG m_nWidth, m_nHeight;
	LONG m_nScaleLeft, m_nScaleTop, m_nScaleRight, m_nScaleBottom;
	LONG m_nScaleWidth, m_nScaleHeight, m_nScaleZero;
	CPen m_penData;
	CPen m_penLine1;
	CPen m_penLine2;
	CPen m_penBlack, m_penGray, m_penLightGray;
	CBrush m_brushWhite;
	CFont m_Font, m_Font2;
	CRgn m_Rgn;
	CRemarkWnd m_wndRemark;
	int m_nScrollPos;
	CString m_sTitle;
	int m_nMouseMode;
	PFNGRAPH m_pCallBackFunc;
	DWORD_PTR m_dwCallBackData;
	HCURSOR m_hCursorArrow, m_hCursorHScroll;
	BOOL m_bHScroll;

	void SetBitmap(double startTime, double dispTime, int nMaxLevel, int nMinLevel, int nStep, int nDiv, LPCSTR pStr);
	void SetMouseCursor() const;

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
};
