#pragma once

class CNoiseSourceDlg : public CDialogAF
{
public:
	CNoiseSourceDlg(CWnd* pParent = NULL);   // 標準のコンストラクタ

protected:
	enum { IDD = IDD_NOISE_SOURCE };

	CString	m_sNoiseSource;

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual void OnOK();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};
