#pragma once

class CTimeWnd : public CWnd
{
public:
	CTimeWnd();

protected:
	CDC m_dcMem;
	CFont m_Font;
	CBitmap m_bitmapMem;
	int m_nWidth;
	int m_nHeight;
	BOOL m_bMoving;
	CRect m_Rect;
	CPoint m_Point;
	CString m_sDispFormat;
	BOOL bLayered;
	BOOL m_bDisp;

	void Create();
	void Init(int x = -1, int y = -1);
	void GetDateFormat();
	CString GetTimeStr();
	void GetBitmapSize();
	void CreateBitmap();
	void CreateFont();
	void ChangeFontSize(int nFontSize);
	void ChangeFormat(int nFormat);
	void ChangeTransparency(int nTransparency);

	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnClose();
	afx_msg void OnSize9Point();
	afx_msg void OnSize10Point();
	afx_msg void OnSize11Point();
	afx_msg void OnSize12Point();
	afx_msg void OnFormatDate();
	afx_msg void OnFormatTime();
	afx_msg void OnFormatDateTime();
	afx_msg void OnTransparency0();
	afx_msg void OnTransparency1();
	afx_msg void OnTransparency2();
	afx_msg void OnTransparency3();
	afx_msg void OnSize8Point();
};

