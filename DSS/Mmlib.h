#pragma once

// CMmlib ダイアログ

class CMmlib : public CPropertyPage
{
	DECLARE_DYNAMIC(CMmlib)

public:
	CMmlib();

// ダイアログ データ
	enum { IDD = IDD_MMLIB };

protected:
	CString m_sMmlibPath;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBnClickedReferenceMmlib();
};
