#pragma once

class CSettingFont : public CPropertyPage
{
public:
	CSettingFont();

protected:
	enum { IDD = IDD_SETTING_FONT };

	CComboBox m_cFontSizeSg;
	CComboBox m_cFontSizeFft;
	CComboBox m_cFontSizeOs;
	CComboBox m_cFontSizeFre;
	CComboBox m_cFontSizeDst;
	CComboBox m_cFontSizeImp;
	CComboBox m_cFontSizeAcf;

	void SetFontSizeList(CComboBox &cmb, int nFontSize);
	void SetDefault(CComboBox &cmb);
	int GetFontSize(CComboBox &cmb);

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBnClickedDefault();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};
