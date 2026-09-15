#pragma once

#include "NmsItem.h"

class CChangeNoiseSrcDlg : public CDialogAF
{
public:
	CChangeNoiseSrcDlg(CWnd* pParent = NULL);   // 標準のコンストラクタ

	CNmsItem *m_pNmsItem;

protected:
	enum { IDD = IDD_CHANGE_NOISE_SRC };

	void SetNsTmpList(long nID);

	CListBox	m_cNoiseSrcList;

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnAddNew();
	afx_msg void OnDblclkNoiseSrcList();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};
