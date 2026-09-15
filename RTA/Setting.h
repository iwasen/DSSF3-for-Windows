#pragma once

#include "SettingFont.h"
#include "SettingDevice.h"

class CSetting : public CPropertySheet
{
public:
	CSetting(CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

protected:
	CSettingFont m_SettingFont;
	CSettingDevice m_SettingDevice;

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};

