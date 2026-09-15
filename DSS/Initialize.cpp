// Initialize.cpp : 実装ファイル
//

#include "stdafx.h"
#include "Dss.h"
#include "Initialize.h"
#include "Directory.h"

// CInitialize ダイアログ

CInitialize::CInitialize()
	: CPropertyPage(CInitialize::IDD)
	, m_bCheckInitRta(FALSE)
	, m_bCheckInitSas(FALSE)
	, m_bCheckInitNms(FALSE)
{
}

void CInitialize::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_INIT_RTA, m_bCheckInitRta);
	DDX_Check(pDX, IDC_CHECK_INIT_SAS, m_bCheckInitSas);
	DDX_Check(pDX, IDC_CHECK_INIT_NMS, m_bCheckInitNms);
}

BEGIN_MESSAGE_MAP(CInitialize, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_INIT_SETTINGS, &CInitialize::OnBnClickedInitSettings)
	ON_BN_CLICKED(IDC_BUTTON_INIT_DATABASE, &CInitialize::OnBnClickedInitDatabase)
END_MESSAGE_MAP()

// CInitialize メッセージ ハンドラ

void CInitialize::OnBnClickedInitSettings()
{
	UpdateData(TRUE);

	if (m_bCheckInitRta || m_bCheckInitSas || m_bCheckInitNms) {
		if (::AfxMessageBox(IDS_INIT_SETTINGS, MB_OKCANCEL | MB_ICONQUESTION) == IDOK) {
			CString sDataDirectory;
			CString sPathName;

			GetDataDirectory(sDataDirectory);

			if (m_bCheckInitRta) {
				sPathName.Format("%s\\%s", (LPCSTR)sDataDirectory, "RTA.DAT");
				::DeleteFile(sPathName);

				sPathName.Format("%s\\%s", (LPCSTR)sDataDirectory, "RTA.INI");
				::DeleteFile(sPathName);
			}

			if (m_bCheckInitSas) {
				sPathName.Format("%s\\%s", (LPCSTR)sDataDirectory, "SAS.DAT");
				::DeleteFile(sPathName);
			}

			if (m_bCheckInitNms) {
				sPathName.Format("%s\\%s", (LPCSTR)sDataDirectory, "NMS.DAT");
				::DeleteFile(sPathName);
			}
		}
	}
}

void CInitialize::OnBnClickedInitDatabase()
{
	if (::AfxMessageBox(IDS_INIT_DATABASE, MB_OKCANCEL | MB_ICONQUESTION) == IDOK) {
		if (g_DssData.m_sDatabaseFolder[0]) {
			CString sPathName;
			sPathName.Format("%s\\*\t", g_DssData.m_sDatabaseFolder);
			sPathName.Replace('\t', '\0');

			SHFILEOPSTRUCT sfo = { 0 };
			sfo.wFunc = FO_DELETE;
			sfo.pFrom = sPathName;
			sfo.fFlags = FOF_NOCONFIRMATION;

			::SHFileOperation(&sfo);
		}
	}
}
