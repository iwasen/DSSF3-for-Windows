#pragma once

#include "RemarkWnd.h"

class CImpWnd : public CWnd
{
public:
	CImpWnd();
	virtual ~CImpWnd();

	CRemarkWnd m_wndRemark;

	void Initialize(int nFontSize, LPCTSTR pTitle, int nRemark, COLORREF rgbWaveColor, PFNGRAPH pCallBackFunc, LPARAM nCallBackData);
	void Resize();
	void DispImpulse(double totalTime, double startTime, double dispTime, const double *pData, int nData, BOOL bHScroll);
	void SetSelectArea(double startTime, double endTime, BOOL bErase = TRUE);
	void GetSelectArea(double *startTime, double *endTime);
	void SetDeltaT1(double T0, double T1);

protected:
	BOOL m_bInitialized;
	CDC m_dcMem;
	CBitmap m_bitmapMem;
	LONG m_nWidth, m_nHeight;
	LONG m_nScaleLeft, m_nScaleTop, m_nScaleRight, m_nScaleBottom;
	LONG m_nScaleWidth, m_nScaleHeight, m_nScaleCenter;
	CPen m_penWave;
	CPen m_penBlack, m_penGray, m_penLightGray;
	CPen m_penT0, m_penT1;
	CBrush m_brushWhite;
	CBrush m_brushSelectArea;
	CFont m_fontNormal, m_fontRotation;
	CRgn m_rgnScale;
	double m_fStartTime;
	double m_fDispTime;
	int m_nData;
	double m_fT0;
	double m_fT1;
	double m_fStartSelectTime;
	double m_fEndSelectTime;
	int m_nMouseMode;
	int m_nMouseOffset;
	PFNGRAPH m_pCallBackFunc;
	DWORD_PTR m_dwCallBackData;
	HCURSOR m_hCursorArrow;
	HCURSOR m_hCursorHMove;
	HCURSOR m_hCursorHScroll;
	int m_nScrollPos;
	BOOL m_bHScroll;
	POINT *m_pPoint;

	void Initialize2();
	void Uninitialize2();
	void SetPenColor(COLORREF color);
	void SetBitmap(double startTime, double dispTime);
	void SetMouseCursor(CPoint point);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
};
