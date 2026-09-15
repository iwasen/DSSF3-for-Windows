#pragma once
#include "afxwin.h"

/////////////////////////////////////////////////////////////////////////////
// CPresetDlg ダイアログ

class CPresetDlg : public CDialogAF
{
public:
	CPresetDlg(CWnd* pParent = NULL);   // 標準のコンストラクタ

	int m_nPresetID;

protected:
	enum { IDD = IDD_PRESET };

	CListBox	m_cPresetList;
	CButton m_cButtonOverwrite;
	CButton m_cButtonDelete;

	void SetPresetList();

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSave();
	afx_msg void OnOverwrite();
	afx_msg void OnDelete();
	afx_msg void OnDblclkPresetList();
	afx_msg void OnBnClickedMakeShortcut();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnLbnSelchangePresetList();
};

/////////////////////////////////////////////////////////////////////////////
// CPresetSaveDlg ダイアログ

class CPresetSaveDlg : public CDialogAF
{
public:
	CPresetSaveDlg(CWnd* pParent = NULL);   // 標準のコンストラクタ

	CString	m_sTitle;

protected:
	enum { IDD = IDD_PRESET_SAVE };

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual void OnOK();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};
