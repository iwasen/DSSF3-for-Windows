// Mmlib.cpp : 実装ファイル
//

#include "stdafx.h"
#include "DSS.h"
#include "Mmlib.h"

// CMmlib ダイアログ

IMPLEMENT_DYNAMIC(CMmlib, CPropertyPage)

CMmlib::CMmlib()
	: CPropertyPage(CMmlib::IDD)
	, m_sMmlibPath("")
{
}

void CMmlib::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_MMLIB_PATH, m_sMmlibPath);
}

BEGIN_MESSAGE_MAP(CMmlib, CPropertyPage)
	ON_BN_CLICKED(IDC_REFERENCE_MMLIB, &CMmlib::OnBnClickedReferenceMmlib)
END_MESSAGE_MAP()

// CMmlib メッセージ ハンドラ

BOOL CMmlib::OnInitDialog()
{
	m_sMmlibPath = g_DssData.m_sMmlibPath;

	CPropertyPage::OnInitDialog();

	return TRUE;
}

void CMmlib::OnOK()
{
	strcpy_s(g_DssData.m_sMmlibPath, m_sMmlibPath);

	CPropertyPage::OnOK();
}

void CMmlib::OnBnClickedReferenceMmlib()
{
	CFileDialog fileDlg(TRUE, "exe", "MMLIB.exe",
			OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
			"MMLIB executable file (MMLIB.exe)|MMLIB.exe||", this, 0);
	fileDlg.m_ofn.lpstrInitialDir = "C:\\";

	if (fileDlg.DoModal() == IDOK) {
		m_sMmlibPath = fileDlg.GetPathName();
		UpdateData(FALSE);
	}
}
