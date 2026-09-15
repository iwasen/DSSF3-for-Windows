#pragma once

#include "MyCtrl.h"

// CAutoRecDlg ダイアログ

class CAutoRecDlg : public CDialogAF
{
	DECLARE_DYNAMIC(CAutoRecDlg)

public:
	CAutoRecDlg(CWnd* pParent = NULL);   // 標準コンストラクタ

// ダイアログ データ
	enum { IDD = IDD_AUTO_REC };

protected:
	CMyEdit m_cEditSaveFolder;
	CMyButton m_cCheckInputData;
	CMyButton m_cCheckOutputData;

	static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBnClickedButtonReference();
};
