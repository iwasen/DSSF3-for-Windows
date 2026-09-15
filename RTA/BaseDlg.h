#pragma once

#include "DialogEx.h"

class CBaseDlg : public CDialogExt
{
public:
	CBaseDlg(UINT nID, CWnd* pParent);   // 標準コンストラクタ

	BOOL m_bStart;

	virtual void ChangeWaveDevice(int /*nInOut*/) {}
	BOOL Create(UINT nIDTemplate, int nLeft, int nTop, int nFontSize, CWnd* pParentWnd = NULL);

protected:
	int m_nWindowKind;
	HACCEL m_hAccel;

	int GetFontIndex();

	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnInitMenu(CMenu* pMenu);
};
