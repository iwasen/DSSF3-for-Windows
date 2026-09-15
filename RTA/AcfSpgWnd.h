#pragma once

// CAcfSpgWnd

class CAcfSpgWnd : public CWnd
{
public:
	CAcfSpgWnd();
	virtual ~CAcfSpgWnd();

	void Initialize();
	void DispGraph(const double *pData, int nFreqData, int nTimeData, int nTimeCount, int nSampleRate, double fTimeMin, double fTimeMax, double fTimeOffset, int nFreqMin, int nFreqMax, BOOL bFreqLog, int nLevelMin, int nLevelMax);

protected:
	CDC m_dcGraph;
	CDC m_dcGraph2;
	CBitmap m_bmpGraph;
	CBitmap m_bmpGraph2;
	CPen m_penScale;
	CPen m_penGray;
	CPen m_penLightGray;
	CPen m_penMidGray;
	CFont m_fontGraph, m_fontGraph2;
	BOOL m_bDispGraph;
	CRect m_rectView;
	CRect m_rectScale;
	double *m_pSpgLevel;

	void DrawScale(CDC &dc, double fTimeMin, double fTimeMax, int nFreqMin, int nFreqMax, BOOL bFreqLog, int nLevelMin, int nLevelMax);
	COLORREF GetLevelColor(double fLevel, double fLevelMin, double fLevelMax);
	void DrawData(CDC &dc, const double *pData, int nFreqData, int nTimeData, int nTimeCount, int nSampleRate, int nFreqMin, int nFreqMax, BOOL bFreqLog, int nLevelMin, int nLevelMax);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

