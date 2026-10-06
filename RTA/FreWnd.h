#pragma once

#include "CsvFile.h"

class CFreWnd : public CWnd
{
public:
	void Initialize(int nMode, int nFreqStart, int nFreqEnd);
	void Resize();
	void SetBitmap(int nFreqStart, int nFreqEnd);
	void DispGraph(const double *pLeftFre, const double *pRightFre, const double *pFreq, int nFreqCount, int nFreqStart, int nFreqEnd, int nFreqPoint, BOOL bSetBitmap = TRUE, COLORREF colorGraph = COLOR_LEFT, COLORREF colorRight = COLOR_RIGHT);
	void CsvOutput(CCsvFile &oCsvFile, const double *pLeftData, const double *pRightData, const double *pFreq, int nFreqCount, LPCSTR pTitle);
	HBITMAP GetBitmap();
	void AddDataHold(COLORREF color);
	void DelDataHold();

protected:
	BOOL m_bInitialized;
	int m_nMode;
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
	CBrush m_brushWhite;
	CFont m_Font;
	CFont m_Font2;
	CRgn m_Rgn;

	void Initialize2();
	void Uninitialize2();
	void DrawScale(int nLeft, int nTop, int nRight, int nBottom, int nFreqStart, int nFreqEnd);
	void DrawLine(const double *pData, const double *pFreq, int nFreqCount, int nFreqStart, int nFreqEnd, int nFreqPoint);
	void DrawLine2(const double *pData, const double *pFreq, int nFreqCount, int nFreqStart, int nFreqEnd);

	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
};
