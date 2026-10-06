#pragma once

extern BOOL CreateDirectoryAll(LPCSTR pDirPath);
extern void DeleteDirectoryAll(LPCSTR pDirName);
extern void GetDataDirectory(CString &sDataDirectory);
extern void GetDefaultDatabaseDirectory(CString &sDatabaseDirectory);
extern void GetDefaultBackupDirectory(CString &sBackupDirectory);
