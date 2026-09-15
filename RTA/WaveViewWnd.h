#pragma once

class CWaveViewWnd : public CWnd
{
public:
	CWaveViewWnd();

	void Initialize();
	void DispGraph(const WaveForm *pWaveForm);

protected:
	BOOL m_bInitialized;
	CDC m_dcMem;
	CBitmap m_bitmapMem;
	int m_nWidth, m_nHeight;
	int m_nScaleLeft, m_nScaleTop, m_nScaleRight, m_nScaleBottom;
	int m_nScaleWidth, m_nScaleHeight;
	int m_nScaleCenter;
	CPen m_penData;
	CPen m_penBlack, m_penGray, m_penLightGray;
	CBrush m_brushWhite, m_brushData;

	void SetBitmap();
	void DispGraphManual(const WaveFormManual *pWaveForm);
	void DispGraphFM(const WaveFormFM *pWaveForm);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

