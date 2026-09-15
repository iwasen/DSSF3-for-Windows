#pragma once

#include "FftWnd.h"

class CFftWndEx : public CWnd
{
public:
	CFftWndEx(CFftWnd *pFftWnd, int nMode);

protected:
	CFftWnd *m_pFftWnd;
	int m_nMode;
	CStatusBarCtrl m_wndStatusBar;
	BOOL m_bLButtonDown;
	int m_nBorderType;

	void InitStatusPane();
	void SetStatusPane();
	void SaveWindowPos();

	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnFreqLevelLeft(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnFreqLevelRight(WPARAM wParam, LPARAM lParam);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnMove(int x, int y);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg LRESULT OnThemeChanged();
};
