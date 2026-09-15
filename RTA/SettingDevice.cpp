// SettingDevice.cpp : 実装ファイル
//

#include "stdafx.h"
#include "RTA.h"
#include "SettingDevice.h"
#include "Help\ContextHelp.h"

// CSettingDevice ダイアログ

CSettingDevice::CSettingDevice()
	: CPropertyPage(CSettingDevice::IDD)
{
}

void CSettingDevice::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_INPUT_24BIT, m_iInputBit);
	DDX_Radio(pDX, IDC_OUTPUT_24BIT, m_iOutputBit);
}

BEGIN_MESSAGE_MAP(CSettingDevice, CPropertyPage)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

// CSettingDevice メッセージ ハンドラ

BOOL CSettingDevice::OnInitDialog()
{
	m_iInputBit = g_oSetData2.bInput16BitOnly;
	m_iOutputBit = g_oSetData2.bOutput16BitOnly;

	CPropertyPage::OnInitDialog();

	return TRUE;
}

void CSettingDevice::OnOK()
{
	UpdateData(TRUE);

	g_oSetData2.bInput16BitOnly = m_iInputBit;
	g_oSetData2.bOutput16BitOnly = m_iOutputBit;

	CPropertyPage::OnOK();
}

BOOL CSettingDevice::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static const UINT aIDs[] = {
		IDC_INPUT_24BIT, IDH_SETTING_INPUT_24BIT,
		IDC_INPUT_16BIT, IDH_SETTING_INPUT_16BIT,
		IDC_OUTPUT_24BIT, IDH_SETTING_OUTPUT_24BIT,
		IDC_OUTPUT_16BIT, IDH_SETTING_OUTPUT_16BIT,
		0
	};

	DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}
