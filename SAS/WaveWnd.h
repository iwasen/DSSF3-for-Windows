#pragma once

#include "RemarkWnd.h"
#include "FileIO.h"

class CWaveWnd : public CWnd
{
	struct SMarker {
		int nMarkerID;
		CString sTitle;
		CRect rectMarker;
	};
	typedef CArray <SMarker, SMarker &> CArrayMarker;

public:
	CWaveWnd();
	virtual ~CWaveWnd();

	void Initialize(int nChannel, LPCTSTR pTitle, PFNGRAPH pCallBackFunc, LPARAM nCallBackData, CWnd *pNotifyWnd = NULL);
	void SetSize(int nFontSize);
	void DispGraph(double fTotalTime, double fStartTime, double fDispTime, const double *pData, int nData, int nChannel, double fMaxValue, double fMinValue, BOOL bHScroll, BOOL bVScroll, double fSelStart, double fSelEnd, DbAcfMarkerRec *pAcfMarkerRec, int nAcfMarkerRec);
	void DispMarkerCursor(double fStartTime, double fDispTime, double fOffsetTime);
	void HideMarkerCursor();
	void SetZoomRect(double fLeft, double fTop, double fRight, double fBottom);
	void ResetZoomRect();
	void ChangeChannel(int nChannel);

protected:
	void SetBitmap(double fStartTime, double fDispTime, double fMaxValue, double fMinValue);
	void SetMouseCursor();

	BOOL m_bInitialize;
	CDC m_dcMem, m_dcMem2;
	CBitmap m_bitmapMem, m_bitmapMem2;
	int m_nWidth, m_nHeight;
	int m_nScaleLeft, m_nScaleTop, m_nScaleRight, m_nScaleBottom;
	int m_nScaleWidth, m_nScaleHeight;
	int m_nScaleCenter;
	double m_nScaleZero;
	CPen m_penData, m_penSel;
	CPen m_penBlack, m_penGray, m_penLightGray;
	CPen m_penMarkerDot, m_penMarkerSolid;
	CPen m_penDrag;
	CPen m_penZoomArea;
	CBrush m_brushWhite, m_brushSel;
	CBrush m_brushMarker;
	CBrush m_brushZoomArea;
	CFont m_Font, m_Font2;
	CRgn m_Rgn;
	CRemarkWnd m_wndRemark;
	int m_nMouseMode;
	int m_nScrollPosX, m_nScrollPosY;
	PFNGRAPH m_pCallBackFunc;
	DWORD_PTR m_dwCallBackData;
	HCURSOR m_hCursorArrow, m_hCursorHScroll, m_hCursorVScroll, m_hCursorAScroll;
	BOOL m_bHScroll, m_bVScroll;
	POINT *m_pPointBuf;
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
	int m_nChannel;

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void PostNcDestroy();
};
