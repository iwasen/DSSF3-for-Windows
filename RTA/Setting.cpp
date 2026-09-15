// Setting.cpp : 実装ファイル
//

#include "stdafx.h"
#include "Rta.h"
#include "Setting.h"
#include "Help\ContextHelp.h"

// CSetting

CSetting::CSetting(CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(IDS_SETTING, pParentWnd, iSelectPage)
{
	AddPage(&m_SettingFont);
	AddPage(&m_SettingDevice);

	m_psh.dwFlags |= PSH_NOAPPLYNOW;
	m_psh.dwFlags &= ~PSH_HASHELP;
}

BEGIN_MESSAGE_MAP(CSetting, CPropertySheet)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

// CSetting メッセージ ハンドラ

BOOL CSetting::OnInitDialog()
{
	BOOL bResult = CPropertySheet::OnInitDialog();

	ModifyStyleEx(0, WS_EX_CONTEXTHELP);

	CenterWindow(GetDesktopWindow());

	return bResult;
}

BOOL CSetting::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static const UINT aIDs[] = {
		IDOK, IDH_SETTING_OK,
		IDCANCEL, IDH_SETTING_CANCEL,
		0
	};

	DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}
