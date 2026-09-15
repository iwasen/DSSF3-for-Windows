#pragma once

class CAcf3dd : public CWnd
{
public:
	CAcf3dd();
	virtual ~CAcf3dd();

	void Initialize(int nFontSize);
	void Resize();
	void DispGraph(const double *pLeftData, const double *pRightData, int nDataSize, double fSamplingRate);
	void ClearGraph();

protected:
	struct DISPDATA {
		POINT **m_pPoint;
		int *m_pPointCount;
		int m_nAllocCount;
		int m_nCurrentPtr;
	};

	BOOL m_bInitialized;
	int m_nWidth, m_nHeight;
	int m_nScaleLeft, m_nScaleTop, m_nScaleRight, m_nScaleBottom;
	int m_nScaleWidth, m_nScaleHeight;
	int m_nScaleLeft2, m_nScaleTop2, m_nScaleRight2, m_nScaleBottom2;
	int m_nScaleWidth2, m_nScaleHeight2;
	double m_dx, m_dy;
	CDC m_dcMem, m_dcMem2;
	CBitmap m_bitmapMem, m_bitmapMem2;
	CFont m_Font, m_Font2;
	CPen m_penLeft, m_penRight;
	DISPDATA m_aDispData[2];
	POINT *m_pWork;
	int *m_pCheckTbl;
	int m_nTimeDataNum;

	void Initialize2();
	void Uninitialize2();
	void SetBitmap();
	void FreeBuffers();
	void Calc3dd(const double *pData, DISPDATA *pDispData, int nDataSize, double fSamplingRate);
	void PaintSub(const DISPDATA *pDispData);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
