#pragma once

class COsWin : public CWnd
{
public:
	COsWin();

	int m_nHDiv;
	int m_nVDiv;

	void Initialize();
	void Resize();
	void DispScreen(const double *pLeftData, const double *pRightData, double fTotal, int nStart, int nData,
					int nChannel, double fLevelL, double fLevelR, int nPosL, int nPosR, double fOffset);
	void DispXY(const double *pData, int nData, double fLevelL, double fLevelR, int nPosL, int nPosR);
	void ClearScreen();
	void DispTriggerLevel(int nTrigLevel, int nChannel, BOOL bDraw);
	void DispText(LPCTSTR text, int nLine);
	HBITMAP GetBitmap();

protected:
	BOOL m_bInitialized;
	CDC m_dcMem;
	CDC m_dcMem2;
	CBitmap m_bitmapMem;
	HBITMAP m_hBitmapOrg;
	CBitmap m_bitmapMem2;
	HBITMAP m_hBitmapOrg2;
	LONG m_nWidth, m_nHeight;
	LONG m_nScaleDiv;
	LONG m_nScaleLeft, m_nScaleTop, m_nScaleRight, m_nScaleBottom;
	LONG m_nScaleWidth, m_nScaleHeight;
	LONG m_nScaleHCenter, m_nScaleVCenter;
	HPEN m_hPenOrg;
	HPEN m_hPenOrg2;
	CPen m_penBlack, m_penGray, m_penLightGray;
	CPen m_penLeft, m_penRight;
	CPen m_penLeftZero, m_penRightZero;
	CBrush m_brushWhite;
	CBrush m_brushGreen;
	HBRUSH m_hBrushOrg2;
	HFONT m_hFontOrg;
	CFont m_Font;
	int m_nTrigLevel;
	BOOL m_bTrigLevel;
	CRgn m_Rgn;
	POINT *m_pOverlayBufL, *m_pOverlayBufR, *m_pOverlayBufXY;
	int *m_pOverlayCntL, *m_pOverlayCntR, *m_pOverlayCntXY;
	CPen *m_pPenLeft, *m_pPenRight, *m_pPenXY;
	int m_nOverlaySize;
	int m_nOverlayNum;
	int m_nOverlayPos;
	int m_nOverlayWave;
	int m_nOverlayXY;
	struct POSTBL {
		int yMin;
		int yMax;
	} *m_pPosTbl;
	int m_nChannel;
	BOOL m_bLch;
	BOOL m_bRch;

	void Initialize2();
	void Uninitialize();
	void Uninitialize2();
	void ResetOverlay();
	void SetBitmap();
	void AllocOverlayWave();
	void AllocOverlayXY(int nData);
	void FreeOverlayBuf();
	void DispCenterLine(CDC &dc);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
};
