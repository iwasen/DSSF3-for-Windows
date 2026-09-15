#pragma once

#include "RemarkWnd.h"
#include "FileIO.h"

class CFactorWnd : public CWnd
{
	struct SMarker {
		int nMarkerID;
		CString sTitle;
		CRect rectMarker;
	};
	typedef CArray <SMarker, SMarker &> CArrayMarker;

public:
	CFactorWnd();

	void Initialize(int nFontSize, LPCTSTR pTitle, PFNGRAPH pCallBackFunc, LPARAM nCallBackData, CWnd *pNotifyWnd = NULL);
	void SetSize(int nFontSize);
	void DispGraph(double fStepTime, double fStartTime, double fDispTime, const double *pData, int nData, double fMaxValue, double fMinValue, LPCTSTR pAxisStr, int nSel, BOOL bHScroll, BOOL bVScroll, DbAcfMarkerRec *pAcfMarkerRec, int nAcfMarkerRec, double fOffsetTime, BOOL bLogScale);
	void DispMarkerCursor(double fStartTime, double fDispTime, double fOffsetTime);
	void HideMarkerCursor();
	void SetZoomRect(double fLeft, double fTop, double fRight, double fBottom);
	void ResetZoomRect();

protected:
	BOOL m_bInitialize;
	CDC m_dcMem, m_dcMem2;
	CBitmap m_bitmapMem, m_bitmapMem2;
	LONG m_nWidth, m_nHeight;
	LONG m_nScaleLeft, m_nScaleTop, m_nScaleRight, m_nScaleBottom;
	LONG m_nScaleWidth, m_nScaleHeight;
	double m_fScaleZero;
	CPen m_penData, m_penSel, m_penSelLine;
	CPen m_penBlack, m_penGray, m_penLightGray;
	CPen m_penMarkerDot, m_penMarkerSolid;
	CPen m_penDrag;
	CPen m_penZoomArea;
	CBrush m_brushWhite;
	CBrush m_brushData, m_brushSel;
	CBrush m_brushMarker;
	CBrush m_brushZoomArea;
	CFont m_Font, m_Font2;
	CRgn m_Rgn;
	CRemarkWnd m_wndRemark;
	CString m_sTitle;
	int m_nMouseMode;
	int m_nScrollPosX, m_nScrollPosY;
	PFNGRAPH m_pCallBackFunc;
	DWORD_PTR m_dwCallBackData;
	HCURSOR m_hCursorArrow, m_hCursorHScroll, m_hCursorVScroll, m_hCursorAScroll;
	BOOL m_bHScroll, m_bVScroll;
	int m_nMarkerCursorX;
	CToolTipCtrl m_tooltip;
	CArrayMarker m_arrayMarker;
	int m_nMarkerIndex;
	CPoint m_pointDragStart;
	CPoint m_pointDragEnd;
	CSize m_sizeCurrent;
	CRect m_rectZoom;
	CPoint m_pointZoomArea;
	HCURSOR m_hCursor;
	CWnd *m_pNotifyWnd;

	void SetBitmap(double fStartTime, double fDispTime, double fMaxValue, double fMinValue, LPCTSTR pAxisStr, BOOL bLogScale);
	void SetMouseCursor();

	DECLARE_MESSAGE_MAP()
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PostNcDestroy();
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
