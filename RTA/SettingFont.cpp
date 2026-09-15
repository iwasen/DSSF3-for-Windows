// SettingFont.cpp : 実装ファイル
//

#include "stdafx.h"
#include "Rta.h"
#include "SettingFont.h"
#include "Help\ContextHelp.h"

// CSettingFont ダイアログ

CSettingFont::CSettingFont()
	: CPropertyPage(CSettingFont::IDD)
{
}

void CSettingFont::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FONT_SIZE_SG, m_cFontSizeSg);
	DDX_Control(pDX, IDC_FONT_SIZE_FFT, m_cFontSizeFft);
	DDX_Control(pDX, IDC_FONT_SIZE_OS, m_cFontSizeOs);
	DDX_Control(pDX, IDC_FONT_SIZE_FRE, m_cFontSizeFre);
	DDX_Control(pDX, IDC_FONT_SIZE_DST, m_cFontSizeDst);
	DDX_Control(pDX, IDC_FONT_SIZE_IMP, m_cFontSizeImp);
	DDX_Control(pDX, IDC_FONT_SIZE_ACF, m_cFontSizeAcf);
}

BEGIN_MESSAGE_MAP(CSettingFont, CPropertyPage)
	ON_BN_CLICKED(IDC_DEFAULT, OnBnClickedDefault)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

// CSettingFont メッセージ ハンドラ

BOOL CSettingFont::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	SetFontSizeList(m_cFontSizeSg, g_oSetData2.nFontSizeSg);
	SetFontSizeList(m_cFontSizeFft, g_oSetData2.nFontSizeFft);
	SetFontSizeList(m_cFontSizeOs, g_oSetData2.nFontSizeOs);
	SetFontSizeList(m_cFontSizeFre, g_oSetData2.nFontSizeFre);
	SetFontSizeList(m_cFontSizeDst, g_oSetData2.nFontSizeDst);
	SetFontSizeList(m_cFontSizeImp, g_oSetData2.nFontSizeImp);
	SetFontSizeList(m_cFontSizeAcf, g_oSetData2.nFontSizeAcf);

	return TRUE;
}

void CSettingFont::OnBnClickedDefault()
{
	SetDefault(m_cFontSizeSg);
	SetDefault(m_cFontSizeFft);
	SetDefault(m_cFontSizeOs);
	SetDefault(m_cFontSizeFre);
	SetDefault(m_cFontSizeDst);
	SetDefault(m_cFontSizeImp);
	SetDefault(m_cFontSizeAcf);
}

void CSettingFont::OnOK()
{
	g_oSetData2.nFontSizeSg = GetFontSize(m_cFontSizeSg);
	g_oSetData2.nFontSizeFft = GetFontSize(m_cFontSizeFft);
	g_oSetData2.nFontSizeOs = GetFontSize(m_cFontSizeOs);
	g_oSetData2.nFontSizeFre = GetFontSize(m_cFontSizeFre);
	g_oSetData2.nFontSizeDst = GetFontSize(m_cFontSizeDst);
	g_oSetData2.nFontSizeImp = GetFontSize(m_cFontSizeImp);
	g_oSetData2.nFontSizeAcf = GetFontSize(m_cFontSizeAcf);

	CPropertyPage::OnOK();
}

void CSettingFont::SetFontSizeList(CComboBox &cmb, int nFontSize)
{
	int index;
	CString str;

	for (int i = 0; i < g_tFontSize[i] != 0; i++) {
		str.Format("%d", g_tFontSize[i]);
		index = cmb.AddString(str);
		cmb.SetItemData(index, g_tFontSize[i]);
	}

	if (nFontSize == 0)
		nFontSize = g_tFontSize[0];
	str.Format("%d", nFontSize);
	cmb.SetWindowText(str);
}

void CSettingFont::SetDefault(CComboBox &cmb)
{
	CString str;

	cmb.GetLBText(0, str);
	cmb.SetWindowText(str);
}

int CSettingFont::GetFontSize(CComboBox &cmb)
{
	CString str;

	cmb.GetWindowText(str);
	return atoi(str);
}

BOOL CSettingFont::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static const UINT aIDs[] = {
		IDC_FONT_SIZE_SG, IDH_SETTING_FONT_SIZE_SG,
		IDC_FONT_SIZE_FFT, IDH_SETTING_FONT_SIZE_FFT,
		IDC_FONT_SIZE_OS, IDH_SETTING_FONT_SIZE_OS,
		IDC_FONT_SIZE_FRE, IDH_SETTING_FONT_SIZE_FRE,
		IDC_FONT_SIZE_DST, IDH_SETTING_FONT_SIZE_DST,
		IDC_FONT_SIZE_IMP, IDH_SETTING_FONT_SIZE_IMP,
		IDC_FONT_SIZE_ACF, IDH_SETTING_FONT_SIZE_ACF,
		IDC_DEFAULT, IDH_SETTING_FONT_SIZE_DEFAULT,
		0
	};

	DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}
