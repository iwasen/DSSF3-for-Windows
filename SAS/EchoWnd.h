#pragma once

#include "RemarkWnd.h"

class CEchoWnd : public CWnd
{
public:
	CEchoWnd();

	void Initialize(int nFontSize, LPCSTR pTitle, PFNGRAPH pCallBackFunc, LPARAM nCallBackData);
	void DispGraph(double totalTime, double startTime, double dispTime, const double *pData, const double *pData2, int nData, double fRate, double t0, double t1, double dev, double fEndTime, int nMaxLevel, int nMinLevel, BOOL bHScroll, double fT20Reg0, double fT20Reg1, double fT30Reg0, double fT30Reg1, double fEDTReg0, double fEDTReg1);
	void DispEnergy(double totalTime, double startTime, double dispTime, const double *pData, int nData, int nMaxLevel, int nMinLevel, BOOL bHScroll);

protected:
	BOOL m_bInitialize;
	CDC m_dcMem;
	CBitmap m_bitmapMem;
	LONG m_nWidth, m_nHeight;
	LONG m_nScaleLeft, m_nScaleTop, m_nScaleRight, m_nScaleBottom;
	LONG m_nScaleWidth, m_nScaleHeight, m_nScaleZero;
	CPen m_penData, m_penData2;
	CPen m_penTsub;
	CPen m_penDot;
	CPen m_penBlack, m_penGray, m_penLightGray;
	CPen m_penT20, m_penT30, m_penEDT;
	CBrush m_brushWhite;
	CFont m_Font, m_Font2;
	CRgn m_Rgn;
	CRemarkWnd m_wndRemark;
	HCURSOR m_hCursorArrow;
	HCURSOR m_hCursorHScroll;
	int m_nScrollPos;
	PFNGRAPH m_pCallBackFunc;
	DWORD_PTR m_dwCallBackData;
	int m_nMouseMode;
	CString m_sTitle;
	BOOL m_bHScroll;

	void SetBitmap(double startTime, double dispTime, int nMaxLevel, int nMinLevel);
	void SetMouseCursor(CPoint point) const;

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
};
