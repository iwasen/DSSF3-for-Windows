#pragma once
#include "afxcmn.h"

// CDataRestoreDlg ダイアログ

class CDataRestoreDlg : public CDialogAF
{
public:
	CDataRestoreDlg(CWnd* pParent = NULL);   // 標準コンストラクタ

// ダイアログ データ
	enum { IDD = IDD_DATA_RESTORE };

protected:
	CListCtrl m_cListBackup;

	CStringArray m_oBackupList;

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};
