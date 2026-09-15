#pragma once

#include "MyCtrl.h"

// CDataBackupDlg ダイアログ

class CDataBackupDlg : public CDialogAF
{
public:
	CDataBackupDlg(CWnd* pParent = NULL);   // 標準コンストラクタ

// ダイアログ データ
	enum { IDD = IDD_DATA_BACKUP };

protected:
	CMyEdit m_cEditComment;
	CListCtrl m_cListBackup;

	CStringArray m_oBackupList;

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBnClickedDeleteBackup();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};
