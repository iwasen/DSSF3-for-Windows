#pragma once

extern BOOL CreateDirectoryAll(LPCTSTR pDirPath);
extern void DeleteDirectoryAll(LPCTSTR pDirName);
extern void GetDataDirectory(CString &sDataDirectory);
extern void GetDefaultDatabaseDirectory(CString &sDatabaseDirectory);
extern void GetDefaultBackupDirectory(CString &sBackupDirectory);
