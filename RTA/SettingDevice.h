#pragma once

// CSettingDevice ダイアログ

class CSettingDevice : public CPropertyPage
{
public:
	CSettingDevice();

// ダイアログ データ
	enum { IDD = IDD_SETTING_DEVICE };

protected:
	int m_iInputBit;
	int m_iOutputBit;

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};
