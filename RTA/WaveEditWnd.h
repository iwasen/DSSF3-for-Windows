#pragma once

#include "WaveForm.h"

class CWaveEditWnd : public CWnd
{
public:
	CWaveEditWnd();

	void Initialize(class CWaveEditManual *pWaveEditManual);
	void DispGraph(double fStartX, double fDispX, const WaveFormManual *pWaveForm, double fMaxY, double fMinY, int nSelPoint);

protected:
	BOOL m_bInitialized;
	CDC m_dcMem;
	CBitmap m_bitmapMem;
	int m_nWidth, m_nHeight;
	int m_nScaleLeft, m_nScaleTop, m_nScaleRight, m_nScaleBottom;
	int m_nScaleWidth, m_nScaleHeight;
	int m_nScaleCenter;
	double m_nScaleZero;
	CPen m_penData, m_penSel;
	CPen m_penBlack, m_penGray, m_penLightGray;
	CBrush m_brushWhite, m_brushData, m_brushSel;
	CFont m_Font, m_Font2;
	CRgn m_Rgn;
	class CWaveEditManual *m_pWaveEditManual;
	int m_nMouseDown;
	double m_fStartX;
	double m_fDispX;
	double m_fMaxY;
	double m_fMinY;
	CPoint m_pointMenu;

	void SetBitmap(double fMaxX, double fMinX, double fMaxY, double fMinY);
	float GetVertexX(int x);
	float GetVertexY(int y);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnAdd();
	afx_msg void OnDelete();
};

