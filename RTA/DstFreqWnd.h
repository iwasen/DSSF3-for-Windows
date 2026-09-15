#pragma once

#include "CsvFile.h"

class CDstFreqWnd : public CWnd
{
public:
	void Initialize();
	void Resize();
	void SetBitmap(int nFreqStart, int nFreqEnd);
	void DispGraph(const double *pLeftDst, const double *pRightDst, const double *pFreq, int nFreqCount, int nFreqStart, int nFreqEnd, int nFreqPoint, BOOL bSetBitmap = TRUE, COLORREF colorGraph = COLOR_LEFT, COLORREF colorRight = COLOR_RIGHT);
	void CsvOutput(CCsvFile &oCsvFile, const double *pLeftDst, const double *pRightDst, const double *pFreq, int nFreqCount);
	HBITMAP GetBitmap();

protected:
	BOOL m_bInitialized;
	int m_nWidth;
	int m_nHeight;
	int m_nScaleLeft;
	int m_nScaleTop;
	int m_nScaleRight;
	int m_nScaleBottom;
	int m_nScaleWidth;
	int m_nScaleHeight;
	CDC m_dcMem;
	CBitmap m_bitmapMem;
	CPen m_penBlack;
	CPen m_penGray;
	CPen m_penLightGray;
	CFont m_Font;
	CFont m_Font2;
	CBrush m_brushWhite;
	CRgn m_Rgn;

	void Initialize2();
	void Uninitialize2();
	void DrawScaleDst(int nLeft, int nTop, int nRight, int nBottom, int nFreqStart, int nFreqEnd);
	void DrawLine(const double *pData, const double *pFreq, int nFreqCount, int nFreqStart, int nFreqEnd, int nFreqPoint);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
