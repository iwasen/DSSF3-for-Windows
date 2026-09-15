#include "stdafx.h"
#include "DssData.h"
#include "Directory.h"

#define FILENAME "DSS.dat"

CDssData g_DssData;

CDssData::CDssData()
{
	BOOL bRead = FALSE;
	CString sDataDirectory;
	CString sPathName;

	GetDataDirectory(sDataDirectory);
	sPathName.Format("%s\\%s", (LPCSTR)sDataDirectory, FILENAME);

	CFile file;
	if (file.Open(sPathName, CFile::modeRead | CFile::shareDenyNone)) {
		file.Read(this, sizeof(CDssData));
		bRead = TRUE;
	}

	if (!bRead) {
		memset(this, 0, sizeof(CDssData));

		CString sDatabaseDirectory;
		GetDefaultDatabaseDirectory(sDatabaseDirectory);
		strcpy_s(m_sDatabaseFolder, sDatabaseDirectory);

		CString sBackupDirectory;
		GetDefaultBackupDirectory(sBackupDirectory);
		strcpy_s(m_sBackupFolder, sBackupDirectory);

		SaveData();
	}
}

void CDssData::SaveData()
{
	CString sDataDirectory;
	CString sPathName;

	GetDataDirectory(sDataDirectory);
	sPathName.Format("%s\\%s", (LPCSTR)sDataDirectory, FILENAME);

	CFile file;
	if (file.Open(sPathName, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone))
		file.Write((char *)this, sizeof(CDssData));
}
