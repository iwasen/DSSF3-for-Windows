#pragma once

// CInitialize ダイアログ

class CInitialize : public CPropertyPage
{
public:
	CInitialize();

protected:
	enum { IDD = IDD_INITIALIZE };

	CString m_sInitialize;
	BOOL m_bCheckInitRta;
	BOOL m_bCheckInitSas;
	BOOL m_bCheckInitNms;

	static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	afx_msg void OnBnClickedInitSettings();
	afx_msg void OnBnClickedInitDatabase();
};
