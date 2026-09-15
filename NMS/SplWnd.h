#pragma once

#include "MyCtrl.h"

class CSplWnd : public CDialogAF
{
public:
	CSplWnd(CWnd* pParent = NULL);   // 標準のコンストラクタ

	void ShowSpl(double fSpl);
	void Clear();

protected:
	enum { IDD = IDD_SPL };

	CStatic	m_cDB;
	CMyEdit	m_cSpl;
	CFont m_Font;

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual void PostNcDestroy();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnDestroy();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};
