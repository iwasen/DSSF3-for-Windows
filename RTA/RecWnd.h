#pragma once

#include "RemarkWnd.h"

class CRecWnd : public CWnd
{
public:
	CRecWnd();
	virtual ~CRecWnd();

	CRemarkWnd m_wndRemark;

	void Initialize(int nFontSize, PFNGRAPH pCallBackFunc, LPARAM nCallBackData, BOOL bRemark);
	void Resize();
	void DispGraph(double totalTime, double startTime, double dispTime, const double *pData, int nData, int nChannel, int nLR, double fMaxData);
	void DispGraph2(const double *pData, int nData, int nChannel, int nLR, int nOffset, int nLength);
	void SetPenColor(COLORREF color);
	void SetSelectArea(double startTime, double endTime, BOOL bErase = TRUE);
	void GetSelectArea(double *startTime, double *endTime) const;
	void SetReplayPosition(double fReplayTime);

protected:
	BOOL m_bInitialized;
	CDC m_dcMem;
	CBitmap m_bitmapMem;
	LONG m_nWidth, m_nHeight;
	LONG m_nScaleLeft, m_nScaleTop, m_nScaleRight, m_nScaleBottom;
	LONG m_nScaleWidth, m_nScaleHeight, m_nScaleCenter;
	CPen m_penLeft, m_penRight;
	CPen m_penBlack, m_penGray, m_penLightGray;
	CPen m_penSelectBar;
	CPen m_penReplayBar;
	CBrush m_brushWhite;
	CBrush m_brushSelectArea;
	CFont m_Font, m_Font2;
	double m_fStartTime;
	double m_fDispTime;
	int m_nData;
	double m_fStartSelectTime;
	double m_fEndSelectTime;
	int m_nMouseMode;
	int m_nMouseOffset;
	CRgn m_Rgn;
	PFNGRAPH m_pCallBackFunc;
	DWORD_PTR m_dwCallBackData;
	HCURSOR m_hCursorArrow;
	HCURSOR m_hCursorHMove;
	HCURSOR m_hCursorScroll;
	int m_nScrollPos;
	POINT *m_pPointBuf;
	double m_fReplayTime;

	void Initialize2();
	void Uninitialize2();
	void SetBitmap(double startTime, double dispTime);
	void SetMouseCursor(CPoint point) const;

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
};
