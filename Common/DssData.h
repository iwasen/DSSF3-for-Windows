#pragma once

class CDssData {
public:
	CDssData();

	void SaveData() const;

	char m_sDatabaseFolder[MAX_PATH];
	char m_sBackupFolder[MAX_PATH];
	char m_sMmlibPath[MAX_PATH];
};

extern CDssData g_DssData;
