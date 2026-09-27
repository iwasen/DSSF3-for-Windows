// Folder.cpp : 実装ファイル
//

#include "stdafx.h"
#include "Dss.h"
#include "Folder.h"
#include "Directory.h"

// CFolder ダイアログ

CFolder::CFolder()
	: CPropertyPage(CFolder::IDD)
{
}

void CFolder::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_DATABASE_FOLDER, m_sDatabaseFolder);
	DDX_Text(pDX, IDC_BACKUP_FOLDER, m_sBackupFolder);
}

BEGIN_MESSAGE_MAP(CFolder, CPropertyPage)
	ON_BN_CLICKED(IDC_REFERENCE_DATABASE, &CFolder::OnBnClickedReferenceDatabase)
	ON_BN_CLICKED(IDC_DEFAULT_DATABASE, &CFolder::OnBnClickedDefaultDatabase)
	ON_BN_CLICKED(IDC_REFERENCE_BACKUP, &CFolder::OnBnClickedReferenceBackup)
	ON_BN_CLICKED(IDC_DEFAULT_BACKUP, &CFolder::OnBnClickedDefaultBackup)
END_MESSAGE_MAP()

// CFolder メッセージ ハンドラ

BOOL CFolder::OnInitDialog()
{
	m_sDatabaseFolder = g_DssData.m_sDatabaseFolder;
	m_sBackupFolder = g_DssData.m_sBackupFolder;

	CPropertyPage::OnInitDialog();

	return TRUE;
}

void CFolder::OnOK()
{
	strcpy_s(g_DssData.m_sDatabaseFolder, m_sDatabaseFolder);
	strcpy_s(g_DssData.m_sBackupFolder, m_sBackupFolder);

	CPropertyPage::OnOK();
}

void CFolder::OnBnClickedReferenceDatabase()
{
	SelectFolder(m_sDatabaseFolder, IDS_DATABASE_FOLDER);
}

void CFolder::OnBnClickedDefaultDatabase()
{
	GetDefaultDatabaseDirectory(m_sDatabaseFolder);
	UpdateData(FALSE);
}

void CFolder::OnBnClickedReferenceBackup()
{
	SelectFolder(m_sBackupFolder, IDS_BACKUP_FOLDER);
}

void CFolder::OnBnClickedDefaultBackup()
{
	GetDefaultBackupDirectory(m_sBackupFolder);
	UpdateData(FALSE);
}

void CFolder::SelectFolder(CString &sFolder, int nTitleId)
{
	char dst_file[MAX_PATH];
	LPITEMIDLIST idlist;
	CString sTitle;

	sTitle.LoadString(nTitleId);
	strcpy_s(dst_file, sFolder);
	BROWSEINFO binfo{
		.hwndOwner = m_hWnd,
		.pidlRoot = NULL,
		.pszDisplayName = dst_file,
		.lpszTitle = sTitle,
		.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI,
		.lpfn = &BrowseCallbackProc,
		.lParam = (LPARAM)dst_file,
		.iImage = (int)NULL
	};
	idlist = ::SHBrowseForFolder(&binfo);
	if (idlist) {
		::SHGetPathFromIDList(idlist, dst_file);
		::CoTaskMemFree(idlist);
		sFolder = dst_file;
		UpdateData(FALSE);
	}
}

int CALLBACK CFolder::BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM /*lParam*/, LPARAM lpData)
{
	if (uMsg == BFFM_INITIALIZED)
		::SendMessage(hwnd, BFFM_SETSELECTION, (WPARAM)TRUE, lpData);

	return 0;
}
