#pragma once

#include "MyCtrl.h"

class CMainFrame : public CFrameWnd
{
protected: // シリアライズ機能のみから作成します。
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	CStatusBar	m_wndStatusBar;
	CMyToolBar	m_wndToolBar;
	ULONG m_nCheckData;

	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg LRESULT OnGetCheckLicense1(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetCheckLicense2(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRegistLicense(WPARAM wParam, LPARAM lParam);
};
