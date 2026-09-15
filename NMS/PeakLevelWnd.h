#pragma once

#include "Indicator.h"

class CPeakLevelWnd : public CDialogAF
{
public:
	CPeakLevelWnd(CWnd* pParent = NULL);   // 標準のコンストラクタ

	CIndicator	m_cPeakLevel;

protected:
	enum { IDD = IDD_PEAK_LEVEL };

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual void PostNcDestroy();

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};
