// FileIO.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "FileIO.h"
#include "comres.h"
#include "mmdb.h"
#include "DssData.h"
#include "shellapi.h"
#include "shlwapi.h"

#define	OFFSET_MEASTIME	50

char g_szDefaultDir[_MAX_DIR];
CMyPcserve g_oPcserve;

static void ErrorMessage(UINT msgID, int err);
static void MakeStrID(long id, CString &strID);

void CMyPcserve::Initialize()
{
	strcpy_s(g_szDefaultDir, g_DssData.m_sDatabaseFolder);

	if (!::PathFileExists(g_szDefaultDir)) {
		char szSrcDir[MAX_PATH];
		::GetCurrentDirectory(MAX_PATH, szSrcDir);
		::PathAppend(szSrcDir, "DataBase");
		szSrcDir[strlen(szSrcDir) + 1] = '\0';

		SHFILEOPSTRUCT shfs;
		memset(&shfs, 0, sizeof(shfs));
		shfs.wFunc = FO_COPY;
		shfs.pFrom = szSrcDir;
		shfs.pTo = g_szDefaultDir;
		shfs.fFlags = FOF_NOCONFIRMMKDIR;
		SHFileOperation(&shfs);
	}

	Connect(0, NULL, "");
}

BOOL CDbFolder::Open(LPCTSTR dir)
{
	static DBF_I fieldInf[] = {
		{"FOLDERID", 'I', LEN_ID,      0},
		{"TYPE",     'C', LEN_TYPE,    0},
		{"NAME",     'C', LEN_NAME,    0},
		{"PLACE",    'C', LEN_PLACE,   0},
		{"DATE",     'C', LEN_DATE,    0},
		{"TITLE",    'C', LEN_TITLE,   0},
		{"COMMENT",  'C', LEN_COMMENT, 0},
		{"SCALE",    'N', LEN_FLOAT,   2}
	};
	BOOL bCreateIndex = FALSE;
	CString pathName;
	int nRecSize;
	int idx;
	int err;

Reopen:
	pathName.Format("%s\\%s", dir, FN_FOLDER_DBF);
	if ((err = DBOpen(g_oPcserve, pathName)) != 0) {
		if (err == ERROR_DB_NOFILE) {
			if ((err = DBCreate(g_oPcserve, pathName, fieldInf, sizeof(fieldInf) / sizeof(DBF_I))) != 0) {
				ErrorMessage(IDS_ERR_CREATEFOLDER, err);
				return FALSE;
			}
			bCreateIndex = TRUE;
		} else {
			ErrorMessage(IDS_ERR_OPENFOLDER, err);
			return FALSE;
		}
	}

	DBRecSize(&nRecSize);
	if (nRecSize - 1 != sizeof(DbFolderBuf)) {
		if (::AfxMessageBox(IDS_MSG_CONVERTFOLDER, MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL) {
			DBClose();
			return FALSE;
		}

		CPSDB dbTemp;
		CString pathNameTmp;
		pathNameTmp.Format("%s\\%s", dir, FN_TEMP_DBF);
		if ((err = dbTemp.DBCreate(g_oPcserve, pathNameTmp, fieldInf, sizeof(fieldInf) / sizeof(DBF_I))) != 0) {
			ErrorMessage(IDS_ERR_CREATEFOLDER, err);
			return FALSE;
		}

		if ((err = dbTemp.DBCopy(*this)) != 0) {
			ErrorMessage(IDS_ERR_CONVERFOLDER, err);
			return FALSE;
		}

		dbTemp.DBClose();
		DBClose();

		g_oPcserve.CFDelete(pathName);
		g_oPcserve.CFRename(pathNameTmp, pathName);
		goto Reopen;
	}

	pathName.Format("%s\\%s", dir, FN_FOLDER1_EDX);
	if (bCreateIndex || DBIndex(pathName, &idx) != 0) {
		if ((err = DBIdxCreate(pathName, "FOLDERID", 1, &idx)) != 0) {
			ErrorMessage(IDS_ERR_CREATEFOLDERINDEX, err);
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CDbFolder::GetNewID(long *pID)
{
	DbFolderBuf dbBuf;
	char szID[LEN_ID + 1];
	int err;

	DBBottom();
	if ((err = DBRead(&dbBuf)) == 0) {
		memcpy(szID, dbBuf.aFolderID, LEN_ID);
		szID[LEN_ID] = '\0';
		*pID = atol(szID) + 1;
	} else if (err == ERROR_DB_EOF)
		*pID = 0;
	else {
		ErrorMessage(IDS_ERR_READFOLDER, err);
		return FALSE;
	}

	return TRUE;
}

BOOL CDbFolder::ReadRecID(long id, DbFolderRec *pDbRec)
{
	DbFolderBuf dbBuf;
	CString strID;
	int find;
	int err;

	MakeStrID(id, strID);
	if ((err = DBReadKey(strID, LEN_ID, &dbBuf, 0, &find)) != 0) {
		ErrorMessage(IDS_ERR_READFOLDER, err);
		return FALSE;
	}

	if (find == 0)
		return FALSE;

	DBGetFieldBuf(&dbBuf, pDbRec);

	return TRUE;
}

BOOL CDbFolder::ReadRecNext(DbFolderRec *pDbRec)
{
	DbFolderBuf dbBuf;
	int nRead;
	int err;

	if ((err = DBReadNext(1, &dbBuf, &nRead)) != 0) {
		ErrorMessage(IDS_ERR_READFOLDER, err);
		return FALSE;
	}

	if (nRead != 1)
		return FALSE;

	DBGetFieldBuf(&dbBuf, pDbRec);

	return TRUE;
}

BOOL CDbFolder::StoreRec(DbFolderRec *pDbRec)
{
	DbFolderBuf dbBuf;
	int err;

	DBSetFieldBuf(&dbBuf, pDbRec);
	if ((err = DBStore(&dbBuf)) != 0) {
		ErrorMessage(IDS_ERR_WRITEFOLDER, err);
		return FALSE;
	}

	return TRUE;
}

BOOL CDbFolder::UpdateRec(DbFolderRec *pDbRec)
{
	DbFolderBuf dbBuf;
	int err;

	DBSetFieldBuf(&dbBuf, pDbRec);
	if ((err = DBUpdateKey(dbBuf.aFolderID, LEN_ID, &dbBuf)) != 0) {
		ErrorMessage(IDS_ERR_WRITEFOLDER, err);
		return FALSE;
	}

	return TRUE;
}

BOOL CDbImpulse::Open(LPCTSTR dir)
{
	static DBF_I fieldInf[] = {
		{"FOLDERID",   'I', LEN_ID,         0},
		{"IMPULSEID",  'I', LEN_ID,         0},
		{"SAMPLING",   'I', LEN_SAMPLING,   0},
		{"MESTIME",    'I', LEN_MESTIME,    0},
		{"MESNUM",     'I', LEN_MESNUM,     0},
		{"CHANNEL",    'I', LEN_CHANNEL,    0},
		{"BIT",        'I', LEN_BIT,        0},
		{"TIME",       'C', LEN_TIME,       0},
		{"TITLE",      'C', LEN_TITLE,      0},
		{"COMMENT",    'C', LEN_COMMENT,    0},
		{"USEFILTER",  'I', LEN_BOOL,       0},
		{"IFILTERID",  'I', LEN_ID,         0},
		{"TEMPERATUR", 'N', LEN_FLOAT,      2},
		{"HUMIDITY",   'N', LEN_FLOAT,      2},
		{"MULTIPLIER", 'I', LEN_MULTIPLIER, 0},
		{"WAVEDATA",   'B', 0,              0}
	};
	BOOL bCreateIndex = FALSE;
	CString pathName;
	int nRecSize;
	int err;

Reopen:
	pathName.Format("%s\\%s", dir, FN_IMPULSE_DBF);
	if ((err = DBOpen(g_oPcserve, pathName)) != 0) {
		if (err == ERROR_DB_NOFILE) {
			if ((err = DBCreate(g_oPcserve, pathName, fieldInf, sizeof(fieldInf) / sizeof(DBF_I))) != 0) {
				ErrorMessage(IDS_ERR_CREATEIRFILE, err);
				return FALSE;
			}
			bCreateIndex = TRUE;
		} else {
			ErrorMessage(IDS_ERR_OPENIRFILE, err);
			return FALSE;
		}
	}

	DBRecSize(&nRecSize);
	if (nRecSize - 1 != sizeof(DbImpulseBuf)) {
		if (::AfxMessageBox(IDS_MSG_CONVERTIRFILE, MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL) {
			DBClose();
			return FALSE;
		}

		CPSDB dbTemp;
		CString pathNameTmp;
		pathNameTmp.Format("%s\\%s", dir, FN_TEMP_DBF);
		if ((err = dbTemp.DBCreate(g_oPcserve, pathNameTmp, fieldInf, sizeof(fieldInf) / sizeof(DBF_I))) != 0) {
			ErrorMessage(IDS_ERR_CREATEIRFILE, err);
			return FALSE;
		}

		if ((err = dbTemp.DBCopy(*this)) != 0) {
			ErrorMessage(IDS_ERR_CONVERTIRFILE, err);
			return FALSE;
		}

		dbTemp.DBClose();
		DBClose();

		g_oPcserve.CFDelete(pathName);
		g_oPcserve.CFRename(pathNameTmp, pathName);

		pathName.Format("%s\\%s", dir, FN_IMPULSE_BIN);
		pathNameTmp.Format("%s\\%s", dir, FN_TEMP_BIN);
		g_oPcserve.CFDelete(pathName);
		g_oPcserve.CFRename(pathNameTmp, pathName);
		goto Reopen;
	}

	pathName.Format("%s\\%s", dir, FN_IMPULSE1_EDX);
	if (bCreateIndex || DBIndex(pathName, &m_nIdxFolderID) != 0) {
		if ((err = DBIdxCreate(pathName, "FOLDERID", 0, &m_nIdxFolderID)) != 0) {
			ErrorMessage(IDS_ERR_CREATEIRINDEX1, err);
			return FALSE;
		}
	}

	pathName.Format("%s\\%s", dir, FN_IMPULSE2_EDX);
	if (bCreateIndex || DBIndex(pathName, &m_nIdxImpulseID) != 0) {
		if ((err = DBIdxCreate(pathName, "IMPULSEID", 1, &m_nIdxImpulseID)) != 0) {
			ErrorMessage(IDS_ERR_CREATEIRINDEX2, err);
			return FALSE;
		}
	}

	return TRUE;
}

int CDbImpulse::GetNewID(long *pID)
{
	DbImpulseBuf dbBuf;
	char szID[LEN_ID + 1];
	int err;

	DBChgIdx(m_nIdxImpulseID);
	DBBottom();
	if ((err = DBRead(&dbBuf)) == 0) {
		memcpy(szID, dbBuf.aImpulseID, LEN_ID);
		szID[LEN_ID] = '\0';
		*pID = atoi(szID) + 1;
	} else if (err == ERROR_DB_EOF)
		*pID = 0;
	else {
		ErrorMessage(IDS_ERR_READIRFILE, err);
		return FALSE;
	}

	return TRUE;
}

BOOL CDbImpulse::ReadRecID(long id, DbImpulseRec *pDbRec)
{
	DbImpulseBuf dbBuf;
	CString strID;
	int find;
	int err;

	if (id != -1) {
		MakeStrID(id, strID);
		DBChgIdx(m_nIdxImpulseID);
		if ((err = DBReadKey(strID, LEN_ID, &dbBuf, 0, &find)) != 0) {
			ErrorMessage(IDS_ERR_READIRFILE, err);
			return FALSE;
		}

		if (find == 0)
			return FALSE;
	} else {
		if ((err = DBRead(&dbBuf)) != 0) {
			ErrorMessage(IDS_ERR_READIRFILE, err);
			return FALSE;
		}
	}

	DBGetFieldBuf(&dbBuf, pDbRec);

	if (pDbRec->nMeasTime >= OFFSET_MEASTIME) {
		pDbRec->nMeasTime -= OFFSET_MEASTIME;
		pDbRec->bImpulseData = TRUE;
	} else
		pDbRec->bImpulseData = FALSE;

	return TRUE;
}

BOOL CDbImpulse::ReadRecNext(DbImpulseRec *pDbRec)
{
	DbImpulseBuf dbBuf;
	int nRead;
	int err;

	if ((err = DBReadNext(1, &dbBuf, &nRead)) != 0) {
		ErrorMessage(IDS_ERR_READIRFILE, err);
		return FALSE;
	}

	if (nRead != 1)
		return FALSE;

	DBGetFieldBuf(&dbBuf, pDbRec);

	if (pDbRec->nMeasTime >= OFFSET_MEASTIME) {
		pDbRec->nMeasTime -= OFFSET_MEASTIME;
		pDbRec->bImpulseData = TRUE;
	} else
		pDbRec->bImpulseData = FALSE;

	return TRUE;
}

BOOL CDbImpulse::StoreRec(DbImpulseRec *pDbRec)
{
	DbImpulseBuf dbBuf;
	int err;

	if (pDbRec->bImpulseData)
		pDbRec->nMeasTime += OFFSET_MEASTIME;

	DBSetFieldBuf(&dbBuf, pDbRec);
	if ((err = DBStore(&dbBuf)) != 0) {
		ErrorMessage(IDS_ERR_WRITEIRFILE, err);
		return FALSE;
	}

	return TRUE;
}

BOOL CDbImpulse::UpdateRec(DbImpulseRec *pDbRec)
{
	DbImpulseBuf dbBuf;
	int err;

	DBSetFieldBuf(&dbBuf, pDbRec);
	DBChgIdx(m_nIdxImpulseID);
	if ((err = DBUpdateKey(dbBuf.aImpulseID, LEN_ID, &dbBuf)) != 0) {
		ErrorMessage(IDS_ERR_WRITEIRFILE, err);
		return FALSE;
	}

	return TRUE;
}

BOOL CDbAcf::Open(LPCTSTR dir)
{
	static DBF_I fieldInf[] = {
		{"FOLDERID",   'I', LEN_ID,         0},
		{"ACFID",      'I', LEN_ID,         0},
		{"SAMPLING",   'I', LEN_SAMPLING,   0},
		{"CHANNEL",    'I', LEN_CHANNEL,    0},
		{"SAMPLENUM",  'I', LEN_SAMPLENUM,  0},
		{"DATATIME",   'N', LEN_FLOAT,      2},
		{"TIME",       'C', LEN_TIME,       0},
		{"TITLE",      'C', LEN_TITLE,      0},
		{"COMMENT",    'C', LEN_COMMENT,    0},
		{"WAVEDATA",   'B', 0,              0}
	};
	BOOL bCreateIndex = FALSE;
	CString pathName;
	int nRecSize;
	int err;

Reopen:
	pathName.Format("%s\\%s", dir, FN_ACF_DBF);
	if ((err = DBOpen(g_oPcserve, pathName)) != 0) {
		if (err == ERROR_DB_NOFILE) {
			if ((err = DBCreate(g_oPcserve, pathName, fieldInf, sizeof(fieldInf) / sizeof(DBF_I))) != 0) {
				ErrorMessage(IDS_ERR_CREATEACFFILE, err);
				return FALSE;
			}
			bCreateIndex = TRUE;
		} else {
			ErrorMessage(IDS_ERR_OPENACFFILE, err);
			return FALSE;
		}
	}

	DBRecSize(&nRecSize);
	if (nRecSize - 1 != sizeof(DbAcfBuf)) {
		if (::AfxMessageBox(IDS_MSG_CONVERTACFFILE, MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL) {
			DBClose();
			return FALSE;
		}

		CPSDB dbTemp;
		CString pathNameTmp;
		pathNameTmp.Format("%s\\%s", dir, FN_TEMP_DBF);
		if ((err = dbTemp.DBCreate(g_oPcserve, pathNameTmp, fieldInf, sizeof(fieldInf) / sizeof(DBF_I))) != 0) {
			ErrorMessage(IDS_ERR_CREATEACFFILE, err);
			return FALSE;
		}

		if ((err = dbTemp.DBCopy(*this)) != 0) {
			ErrorMessage(IDS_ERR_CONVERTACFFILE, err);
			return FALSE;
		}

		dbTemp.DBClose();
		DBClose();

		g_oPcserve.CFDelete(pathName);
		g_oPcserve.CFRename(pathNameTmp, pathName);

		pathName.Format("%s\\%s", dir, FN_ACF_BIN);
		pathNameTmp.Format("%s\\%s", dir, FN_TEMP_BIN);
		g_oPcserve.CFDelete(pathName);
		g_oPcserve.CFRename(pathNameTmp, pathName);
		goto Reopen;
	}

	pathName.Format("%s\\%s", dir, FN_ACF1_EDX);
	if (bCreateIndex || DBIndex(pathName, &m_nIdxFolderID) != 0) {
		if ((err = DBIdxCreate(pathName, "FOLDERID", 0, &m_nIdxFolderID)) != 0) {
			ErrorMessage(IDS_ERR_CREATEACFINDEX1, err);
			return FALSE;
		}
	}

	pathName.Format("%s\\%s", dir, FN_ACF2_EDX);
	if (bCreateIndex || DBIndex(pathName, &m_nIdxAcfID) != 0) {
		if ((err = DBIdxCreate(pathName, "ACFID", 1, &m_nIdxAcfID)) != 0) {
			ErrorMessage(IDS_ERR_CREATEACFINDEX2, err);
			return FALSE;
		}
	}

	return TRUE;
}

int CDbAcf::GetNewID(long *pID)
{
	DbAcfBuf dbBuf;
	char szID[LEN_ID + 1];
	int err;

	DBChgIdx(m_nIdxAcfID);
	DBSetFilter("ACFID>=0");
	DBBottom();
	if ((err = DBRead(&dbBuf)) == 0) {
		memcpy(szID, dbBuf.aAcfID, LEN_ID);
		szID[LEN_ID] = '\0';
		*pID = atoi(szID) + 1;
	} else if (err == ERROR_DB_EOF)
		*pID = 0;
	else {
		DBSetFilter(NULL);
		ErrorMessage(IDS_ERR_READACFFILE, err);
		return FALSE;
	}

	DBSetFilter(NULL);

	return TRUE;
}

BOOL CDbAcf::ReadRecID(long id, DbAcfRec *pDbRec)
{
	DbAcfBuf dbBuf;
	CString strID;
	int find;
	int err;

	MakeStrID(id, strID);
	DBChgIdx(m_nIdxAcfID);
	if ((err = DBReadKey(strID, LEN_ID, &dbBuf, 0, &find)) != 0) {
		ErrorMessage(IDS_ERR_READACFFILE, err);
		return FALSE;
	}

	if (find == 0)
		return FALSE;

	DBGetFieldBuf(&dbBuf, pDbRec);

	return TRUE;
}

BOOL CDbAcf::ReadRecNext(DbAcfRec *pDbRec)
{
	DbAcfBuf dbBuf;
	int nRead;
	int err;

	if ((err = DBReadNext(1, &dbBuf, &nRead)) != 0) {
		ErrorMessage(IDS_ERR_READACFFILE, err);
		return FALSE;
	}

	if (nRead != 1)
		return FALSE;

	DBGetFieldBuf(&dbBuf, pDbRec);

	return TRUE;
}

BOOL CDbAcf::StoreRec(DbAcfRec *pDbRec)
{
	DbAcfBuf dbBuf;
	int err;

	DBSetFieldBuf(&dbBuf, pDbRec);
	if ((err = DBStore(&dbBuf)) != 0) {
		ErrorMessage(IDS_ERR_WRITEACFFILE, err);
		return FALSE;
	}

	return TRUE;
}

BOOL CDbAcf::UpdateRec(DbAcfRec *pDbRec)
{
	DbAcfBuf dbBuf;
	int err;

	DBSetFieldBuf(&dbBuf, pDbRec);
	DBChgIdx(m_nIdxAcfID);
	if ((err = DBUpdateKey(dbBuf.aAcfID, LEN_ID, &dbBuf)) != 0) {
		ErrorMessage(IDS_ERR_WRITEACFFILE, err);
		return FALSE;
	}

	return TRUE;
}

BOOL CDbNms::Open(LPCTSTR dir)
{
	static DBF_I fieldInf[] = {
		{"FOLDERID",  'I', LEN_ID,        0},
		{"NMSID",     'I', LEN_ID,        0},
		{"SAMPLING",  'I', LEN_SAMPLING,  0},
		{"CHANNEL",   'I', LEN_CHANNEL,   0},
		{"SAMPLENUM", 'I', LEN_SAMPLENUM, 0},
		{"NUMBER",    'I', LEN_SHORT,     0},
		{"DATATIME",  'N', LEN_FLOAT,     2},
		{"TIME",      'C', LEN_TIME,      0},
		{"CONDSIZE",  'I', LEN_SIZE,      0},
		{"FACTSIZE",  'I', LEN_SIZE,      0},
		{"NNMSFACT",  'I', LEN_NNMSFACT,  0},
		{"WAVEDATA",  'B', 0, 0},
		{"NMSCOND",   'B', 0, 0},
		{"NMSFACTOR", 'B', 0, 0},
		{"NOISESRC",  'B', 0, 0}
	};
	BOOL bCreateIndex = FALSE;
	CString pathName;
	int nRecSize;
	int nField;
	int err;

Reopen:
	pathName.Format("%s\\%s", dir, FN_NMS_DBF);
	if ((err = DBOpen(g_oPcserve, pathName)) != 0) {
		if (err == ERROR_DB_NOFILE) {
			if ((err = DBCreate(g_oPcserve, pathName, fieldInf, sizeof(fieldInf) / sizeof(DBF_I))) != 0) {
				ErrorMessage(IDS_ERR_CREATENMSFILE, err);
				return FALSE;
			}
			bCreateIndex = TRUE;
		} else {
			ErrorMessage(IDS_ERR_OPENNMSFILE, err);
			return FALSE;
		}
	}

	DBRecSize(&nRecSize);
	DBNField(&nField);
	if (nRecSize - 1 != sizeof(DbNmsBuf) || nField != sizeof(fieldInf) / sizeof(DBF_I)) {
		if (::AfxMessageBox(IDS_MSG_CONVERTNMSFILE, MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL) {
			DBClose();
			return FALSE;
		}

		CPSDB dbTemp;
		CString pathNameTmp;
		pathNameTmp.Format("%s\\%s", dir, FN_TEMP_DBF);
		if ((err = dbTemp.DBCreate(g_oPcserve, pathNameTmp, fieldInf, sizeof(fieldInf) / sizeof(DBF_I))) != 0) {
			ErrorMessage(IDS_ERR_CREATENMSFILE, err);
			return FALSE;
		}

		if ((err = dbTemp.DBCopy(*this)) != 0) {
			ErrorMessage(IDS_ERR_CONVERTNMSFILE, err);
			return FALSE;
		}

		dbTemp.DBClose();
		DBClose();

		g_oPcserve.CFDelete(pathName);
		g_oPcserve.CFRename(pathNameTmp, pathName);

		pathName.Format("%s\\%s", dir, FN_NMS_BIN);
		pathNameTmp.Format("%s\\%s", dir, FN_TEMP_BIN);
		g_oPcserve.CFDelete(pathName);
		g_oPcserve.CFRename(pathNameTmp, pathName);
		goto Reopen;
	}

	pathName.Format("%s\\%s", dir, FN_NMS1_EDX);
	if (bCreateIndex || DBIndex(pathName, &m_nIdxFolderID) != 0) {
		if ((err = DBIdxCreate(pathName, "FOLDERID", 0, &m_nIdxFolderID)) != 0) {
			ErrorMessage(IDS_ERR_CREATENMSINDEX1, err);
			return FALSE;
		}
	}

	pathName.Format("%s\\%s", dir, FN_NMS2_EDX);
	if (bCreateIndex || DBIndex(pathName, &m_nIdxNmsID) != 0) {
		if ((err = DBIdxCreate(pathName, "NMSID", 1, &m_nIdxNmsID)) != 0) {
			ErrorMessage(IDS_ERR_CREATENMSINDEX2, err);
			return FALSE;
		}
	}

	return TRUE;
}

int CDbNms::GetNewID(long *pID)
{
	DbNmsBuf dbBuf;
	char szID[LEN_ID + 1];
	int err;

	DBChgIdx(m_nIdxNmsID);
	DBBottom();
	if ((err = DBRead(&dbBuf)) == 0) {
		memcpy(szID, dbBuf.aNmsID, LEN_ID);
		szID[LEN_ID] = '\0';
		*pID = atoi(szID) + 1;
	} else if (err == ERROR_DB_EOF)
		*pID = 0;
	else {
		ErrorMessage(IDS_ERR_READNMSFILE, err);
		return FALSE;
	}

	return TRUE;
}

BOOL CDbNms::ReadRecID(long id, DbNmsRec *pDbRec, NmsConditionData *pNmsConditionData, NmsFactorData **pNmsFactorData, NoiseSrcData *pNoiseSrcData)
{
#pragma pack(4)
	struct OldAcfCondition {
		double fIntegrationTime;
		double fRunningStep;
		int nFreqWeighting;
		double fMaxDelayTime;
		long nPhi0RefData;
		double fPhi0RefLevel;
		double fTaueInterval;
		double fTaueEndLevel;
		double fTaueEndTime;
	};
	struct OldNmsConditionData {
		long	nStartPos;
		long	nDataNum;
		OldAcfCondition AcfCondition;
		IAcfCondition IAcfCondition;
		NmsMicCal NmsMicCal;
	};
	struct OldAcfFactorData {
		float	fPhi0;
		float	fTauE;
		float	fTau1;
		float	fPhi1;
	};
	struct OldIAcfFactorData {
		float	fSPL;
		float	fIACC;
		float	fTauIACC;
		float	fWIACC;
	};
	struct OldNmsFactorData1 {
		OldAcfFactorData	Acf;
		OldIAcfFactorData	IAcf;
	};
	struct OldNmsFactorData2 {
		OldAcfFactorData	Acf;
		IAcfFactor	IAcf;
	};
#pragma pack()
	DbNmsBuf dbBuf;
	CString strID;
	int find;
	long nRead;
	int err;

	if (id != -1) {
		MakeStrID(id, strID);
		DBChgIdx(m_nIdxNmsID);
		if ((err = DBReadKey(strID, LEN_ID, &dbBuf, 0, &find)) != 0) {
			ErrorMessage(IDS_ERR_READNMSFILE, err);
			return FALSE;
		}

		if (find == 0)
			return FALSE;
	} else {
		if ((err = DBRead(&dbBuf)) != 0) {
			ErrorMessage(IDS_ERR_READNMSFILE, err);
			return FALSE;
		}
	}

	DBGetFieldBuf(&dbBuf, pDbRec);

	if (pNmsConditionData != NULL) {
		memset(pNmsConditionData, 0, sizeof(NmsConditionData));
		if ((err = DBReadBinary("NMSCOND", pNmsConditionData, sizeof(NmsConditionData), &nRead)) != 0) {
			ErrorMessage(IDS_ERR_READCALCFILE, err);
			return FALSE;
		}

		if (pDbRec->nConditionSize == sizeof(OldNmsConditionData)) {
			memmove(&pNmsConditionData->IAcfCondition,
					&((OldNmsConditionData *)pNmsConditionData)->IAcfCondition,
					sizeof(IAcfCondition) + sizeof(NmsMicCal));
			pNmsConditionData->AcfCondition.bTaueOrigin = FALSE;
		}
	}

	if (pNmsFactorData != NULL) {
		*pNmsFactorData = new NmsFactorData[pDbRec->nNmsFactor];
		memset(*pNmsFactorData, 0, sizeof(NmsFactorData) * pDbRec->nNmsFactor);
		if ((err = DBReadBinary("NMSFACTOR", *pNmsFactorData, sizeof(NmsFactorData) * pDbRec->nNmsFactor, &nRead)) != 0) {
			ErrorMessage(IDS_ERR_READCALCFILE, err);
			return FALSE;
		}

		if (pDbRec->nFactorSize == sizeof(OldNmsFactorData1)) {
			for (int i = pDbRec->nNmsFactor - 1; i >= 0; i--) {
				memmove(&((OldNmsFactorData2 *)(*pNmsFactorData))[i], &((OldNmsFactorData1 *)(*pNmsFactorData))[i], sizeof(OldNmsFactorData1));
				((OldNmsFactorData2 *)(*pNmsFactorData))[i].IAcf.fWIACC1 = -1;
			}
			pDbRec->nFactorSize = sizeof(OldNmsFactorData2);
		}

		if (pDbRec->nFactorSize == sizeof(OldNmsFactorData2)) {
			for (int i = pDbRec->nNmsFactor - 1; i >= 0; i--) {
				memmove(&(*pNmsFactorData)[i].IAcf, &((OldNmsFactorData2 *)(*pNmsFactorData))[i].IAcf, sizeof(IAcfFactor));
				memmove(&(*pNmsFactorData)[i].Acf, &((OldNmsFactorData2 *)(*pNmsFactorData))[i].Acf, sizeof(OldAcfFactorData));
				(*pNmsFactorData)[i].Acf.fTauE0 = 0;
			}
		}
	}

	if (pNoiseSrcData != NULL) {
		memset(pNoiseSrcData, 0, sizeof(NoiseSrcData));
		if ((err = DBReadBinary("NOISESRC", pNoiseSrcData, sizeof(NoiseSrcData), &nRead)) != 0) {
			ErrorMessage(IDS_ERR_READCALCFILE, err);
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CDbNms::ReadRecNext(DbNmsRec *pDbRec)
{
	DbNmsBuf dbBuf;
	int nRead;
	int err;

	if ((err = DBReadNext(1, &dbBuf, &nRead)) != 0) {
		ErrorMessage(IDS_ERR_READNMSFILE, err);
		return FALSE;
	}

	if (nRead != 1)
		return FALSE;

	DBGetFieldBuf(&dbBuf, pDbRec);

	return TRUE;
}

BOOL CDbNms::StoreRec(DbNmsRec *pDbRec)
{
	DbNmsBuf dbBuf;
	int err;

	DBSetFieldBuf(&dbBuf, pDbRec);
	if ((err = DBStore(&dbBuf)) != 0) {
		ErrorMessage(IDS_ERR_WRITENMSFILE, err);
		return FALSE;
	}

	return TRUE;
}

BOOL CDbNms::UpdateRec(DbNmsRec *pDbRec)
{
	DbNmsBuf dbBuf;
	int err;

	DBSetFieldBuf(&dbBuf, pDbRec);
	DBChgIdx(m_nIdxNmsID);
	if ((err = DBUpdateKey(dbBuf.aNmsID, LEN_ID, &dbBuf)) != 0) {
		ErrorMessage(IDS_ERR_WRITENMSFILE, err);
		return FALSE;
	}

	return TRUE;
}

BOOL CDbNsTmp::Open(LPCTSTR dir)
{
	static DBF_I fieldInf[] = {
		{"NSTMPID",   'I', LEN_ID,        0},
		{"NSTMPNAME", 'C', LEN_NSTMPNAME, 0},
		{"NSTMPDATA", 'B', 0, 0}
	};
	BOOL bCreateIndex = FALSE;
	CString pathName;
	int nRecSize;
	int err;

Reopen:
	pathName.Format("%s\\%s", dir, FN_NSTMP_DBF);
	if ((err = DBOpen(g_oPcserve, pathName)) != 0) {
		if (err == ERROR_DB_NOFILE) {
			if ((err = DBCreate(g_oPcserve, pathName, fieldInf, sizeof(fieldInf) / sizeof(DBF_I))) != 0) {
				ErrorMessage(IDS_ERR_CREATENSTMPFILE, err);
				return FALSE;
			}
			bCreateIndex = TRUE;
		} else {
			ErrorMessage(IDS_ERR_OPENNSTMPFILE, err);
			return FALSE;
		}
	}

	DBRecSize(&nRecSize);
	if (nRecSize - 1 != sizeof(DbNsTmpBuf)) {
		if (::AfxMessageBox(IDS_MSG_CONVERTNSTMPFILE, MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL) {
			DBClose();
			return FALSE;
		}

		CPSDB dbTemp;
		CString pathNameTmp;
		pathNameTmp.Format("%s\\%s", dir, FN_TEMP_DBF);
		if ((err = dbTemp.DBCreate(g_oPcserve, pathNameTmp, fieldInf, sizeof(fieldInf) / sizeof(DBF_I))) != 0) {
			ErrorMessage(IDS_ERR_CREATENSTMPFILE, err);
			return FALSE;
		}

		if ((err = dbTemp.DBCopy(*this)) != 0) {
			ErrorMessage(IDS_ERR_CONVERTNSTMPFILE, err);
			return FALSE;
		}

		dbTemp.DBClose();
		DBClose();

		g_oPcserve.CFDelete(pathName);
		g_oPcserve.CFRename(pathNameTmp, pathName);

		pathName.Format("%s\\%s", dir, FN_NSTMP_BIN);
		pathNameTmp.Format("%s\\%s", dir, FN_TEMP_BIN);
		g_oPcserve.CFDelete(pathName);
		g_oPcserve.CFRename(pathNameTmp, pathName);
		goto Reopen;
	}

	pathName.Format("%s\\%s", dir, FN_NSTMP1_EDX);
	if (bCreateIndex || DBIndex(pathName, &m_nIdxNsTmpID) != 0) {
		if ((err = DBIdxCreate(pathName, "NSTMPID", 1, &m_nIdxNsTmpID)) != 0) {
			ErrorMessage(IDS_ERR_CREATENSTMPINDEX, err);
			return FALSE;
		}
	}

	pathName.Format("%s\\%s", dir, FN_NSTMP2_EDX);
	if (bCreateIndex || DBIndex(pathName, &m_nIdxNsTmpName) != 0) {
		if ((err = DBIdxCreate(pathName, "NSTMPNAME", 0, &m_nIdxNsTmpName)) != 0) {
			ErrorMessage(IDS_ERR_CREATENSTMPINDEX, err);
			return FALSE;
		}
	}

	return TRUE;
}

int CDbNsTmp::GetNewID(long *pID)
{
	DbNsTmpBuf dbBuf;
	char szID[LEN_ID + 1];
	int err;

	DBChgIdx(m_nIdxNsTmpID);
	DBBottom();
	if ((err = DBRead(&dbBuf)) == 0) {
		memcpy(szID, dbBuf.aNsTmpID, LEN_ID);
		szID[LEN_ID] = '\0';
		*pID = atoi(szID) + 1;
	} else if (err == ERROR_DB_EOF)
		*pID = 0;
	else {
		ErrorMessage(IDS_ERR_READNSTMPFILE, err);
		return FALSE;
	}

	return TRUE;
}

BOOL CDbNsTmp::ReadRecID(long id, DbNsTmpRec *pDbRec, NsTmpData *pNsTmpData)
{
	DbNsTmpBuf dbBuf;
	CString strID;
	int find;
	long nRead;
	int err;

	if (id != -1) {
		MakeStrID(id, strID);
		DBChgIdx(m_nIdxNsTmpID);
		if ((err = DBReadKey(strID, LEN_ID, &dbBuf, 0, &find)) != 0) {
			ErrorMessage(IDS_ERR_READNSTMPFILE, err);
			return FALSE;
		}

		if (find == 0)
			return FALSE;
	} else {
		if ((err = DBRead(&dbBuf)) != 0) {
			ErrorMessage(IDS_ERR_READNSTMPFILE, err);
			return FALSE;
		}
	}

	DBGetFieldBuf(&dbBuf, pDbRec);

	if (pNsTmpData != NULL) {
		memset(pNsTmpData, 0, sizeof(NsTmpData));
		if ((err = DBReadBinary("NSTMPDATA", pNsTmpData, sizeof(NsTmpData), &nRead)) != 0) {
			ErrorMessage(IDS_ERR_READCALCFILE, err);
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CDbNsTmp::ReadRecNext(DbNsTmpRec *pDbRec, NsTmpData *pNsTmpData)
{
	DbNsTmpBuf dbBuf;
	long nReadBin;
	int nRead;
	int err;

	if (pNsTmpData != NULL) {
		memset(pNsTmpData, 0, sizeof(NsTmpData));
		if ((err = DBReadBinary("NSTMPDATA", pNsTmpData, sizeof(NsTmpData), &nReadBin)) != 0) {
			if (err != ERROR_DB_EOF)
				ErrorMessage(IDS_ERR_READNSTMPFILE, err);
			return FALSE;
		}
	}

	if ((err = DBReadNext(1, &dbBuf, &nRead)) != 0) {
		ErrorMessage(IDS_ERR_READNSTMPFILE, err);
		return FALSE;
	}

	if (nRead != 1)
		return FALSE;

	DBGetFieldBuf(&dbBuf, pDbRec);

	return TRUE;
}

BOOL CDbNsTmp::StoreRec(DbNsTmpRec *pDbRec, NsTmpData *pNsTmpData)
{
	DbNsTmpBuf dbBuf;
	int err;

	DBSetFieldBuf(&dbBuf, pDbRec);
	if ((err = DBStore(&dbBuf)) != 0) {
		ErrorMessage(IDS_ERR_WRITENSTMPFILE, err);
		return FALSE;
	}

	if (pNsTmpData != NULL) {
		if ((err = DBWriteBinary("NSTMPDATA", pNsTmpData, sizeof(NsTmpData))) != 0) {
			ErrorMessage(IDS_ERR_WRITENSTMPFILE, err);
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CDbNsTmp::UpdateRec(DbNsTmpRec *pDbRec, NsTmpData *pNsTmpData)
{
	DbNsTmpBuf dbBuf;
	int err;

	DBSetFieldBuf(&dbBuf, pDbRec);
	DBChgIdx(m_nIdxNsTmpID);
	if ((err = DBUpdateKey(dbBuf.aNsTmpID, LEN_ID, &dbBuf)) != 0) {
		ErrorMessage(IDS_ERR_WRITENSTMPFILE, err);
		return FALSE;
	}

	if (pNsTmpData != NULL) {
		if ((err = DBWriteBinary("NSTMPDATA", pNsTmpData, sizeof(NsTmpData))) != 0) {
			ErrorMessage(IDS_ERR_WRITENSTMPFILE, err);
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CDbNsTmp::DeleteRec(long id)
{
	CString strID;
	int err;

	MakeStrID(id, strID);
	if ((err = DBDeleteKey(strID, LEN_ID, 1)) != 0) {
		ErrorMessage(IDS_ERR_WRITENSTMPFILE, err);
		return FALSE;
	}

	return TRUE;
}

BOOL CDbAcParam::Open(LPCTSTR dir)
{
	static DBF_I fieldInf[] = {
		{"IMPULSEID", 'I', LEN_ID,    0},
		{"CONDSIZE",  'I', LEN_SIZE,  0},
		{"DATASIZE",  'I', LEN_SIZE,  0},
		{"DATANUM",   'I', LEN_NDATA, 0},
		{"CONDITION", 'B', 0},
		{"RESULT",    'B', 0},
		{"DATA",      'B', 0}
	};
	BOOL bCreateIndex = FALSE;
	CString pathName;
	int nRecSize;
	int idx;
	int err;

	pathName.Format("%s\\%s", dir, FN_ACPARAM_DBF);
Reopen:
	if ((err = DBOpen(g_oPcserve, pathName)) != 0) {
		if (err == ERROR_DB_NOFILE) {
			if ((err = DBCreate(g_oPcserve, pathName, fieldInf, sizeof(fieldInf) / sizeof(DBF_I))) != 0) {
				ErrorMessage(IDS_ERR_CREATECALCFILE, err);
				return FALSE;
			}
			bCreateIndex = TRUE;
		} else {
			ErrorMessage(IDS_ERR_OPENCALCFILE, err);
			return FALSE;
		}
	}

	DBRecSize(&nRecSize);
	if (nRecSize - 1 != sizeof(DbAcParamBuf)) {
		DBClose();
		if (::AfxMessageBox(IDS_MSG_DELETECALCFILE, MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL)
			return FALSE;
		g_oPcserve.CFDelete(pathName);
		goto Reopen;
	}

	pathName.Format("%s\\%s", dir, FN_ACPARAM1_EDX);
	if (bCreateIndex || DBIndex(pathName, &idx) != 0) {
		if ((err = DBIdxCreate(pathName, "IMPULSEID", 1, &idx)) != 0) {
			ErrorMessage(IDS_ERR_CREATECALCINDEX, err);
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CDbAcParam::ReadRecID(long id, DbAcParamRec *pDbRec)
{
	DbAcParamBuf dbBuf;
	CString strID;
	int find;
	long nRead;
	int err;

	MakeStrID(id, strID);
	if ((err = DBReadKey(strID, LEN_ID, &dbBuf, 0, &find)) != 0) {
		if (err != ERROR_DB_NOKEY)
			ErrorMessage(IDS_ERR_READCALCFILE, err);
		return FALSE;
	}

	if (find == 0)
		return FALSE;

	memset(pDbRec, 0, sizeof(DbAcParamRec));

	DBGetFieldBuf(&dbBuf, pDbRec);

	if ((err = DBReadBinary("CONDITION", &pDbRec->dbAcParamCond, sizeof(DbAcParamCond), &nRead)) != 0) {
		ErrorMessage(IDS_ERR_READCALCFILE, err);
		return FALSE;
	}

	if ((err = DBReadBinary("RESULT", &pDbRec->dbAcParamResult, sizeof(DbAcParamResult), &nRead)) != 0) {
		ErrorMessage(IDS_ERR_READCALCFILE, err);
		return FALSE;
	}

	if ((err = DBReadBinary("DATA", &pDbRec->dbAcParamData, sizeof(DbAcParamData) * pDbRec->nDataNum, &nRead)) != 0) {
		ErrorMessage(IDS_ERR_READCALCFILE, err);
		return FALSE;
	}

	return TRUE;
}

BOOL CDbAcParam::SearchID(long id, BOOL bDispErr)
{
	CString strID;
	int find;
	int err;

	MakeStrID(id, strID);
	if ((err = DBSearch2(strID, LEN_ID, &find)) != 0) {
		if (bDispErr)
			ErrorMessage(IDS_ERR_FINDCALCFILE, err);
		return FALSE;
	}

	if (find != 1)
		return FALSE;

	return TRUE;
}

BOOL CDbAcParam::StoreRec(DbAcParamRec *pDbRec)
{
	DbAcParamBuf dbBuf;
	int err;

	pDbRec->nConditionSize = sizeof(DbAcParamCond);
	pDbRec->nDataSize = sizeof(DbAcParamData);

	DBSetFieldBuf(&dbBuf, pDbRec);
	if ((err = DBStore(&dbBuf)) != 0) {
		ErrorMessage(IDS_ERR_WRITECALCFILE, err);
		return FALSE;
	}

	if ((err = DBWriteBinary("CONDITION", &pDbRec->dbAcParamCond, sizeof(DbAcParamCond))) != 0) {
		ErrorMessage(IDS_ERR_WRITECALCFILE, err);
		return FALSE;
	}

	if ((err = DBWriteBinary("RESULT", &pDbRec->dbAcParamResult, sizeof(DbAcParamResult))) != 0) {
		ErrorMessage(IDS_ERR_WRITECALCFILE, err);
		return FALSE;
	}

	if ((err = DBWriteBinary("DATA", &pDbRec->dbAcParamData, sizeof(DbAcParamData) * pDbRec->nDataNum)) != 0) {
		ErrorMessage(IDS_ERR_WRITECALCFILE, err);
		return FALSE;
	}

	return TRUE;
}

BOOL CDbAcParam::UpdateRec(DbAcParamRec *pDbRec)
{
	DbAcParamBuf dbBuf;
	int err;

	pDbRec->nConditionSize = sizeof(DbAcParamCond);
	pDbRec->nDataSize = sizeof(DbAcParamData);

	DBSetFieldBuf(&dbBuf, pDbRec);
	if ((err = DBUpdateKey(dbBuf.aImpulseID, LEN_ID, &dbBuf)) != 0) {
		ErrorMessage(IDS_ERR_WRITECALCFILE, err);
		return FALSE;
	}

	if ((err = DBWriteBinary("CONDITION", &pDbRec->dbAcParamCond, sizeof(DbAcParamCond))) != 0) {
		ErrorMessage(IDS_ERR_WRITECALCFILE, err);
		return FALSE;
	}

	if ((err = DBWriteBinary("RESULT", &pDbRec->dbAcParamResult, sizeof(DbAcParamResult))) != 0) {
		ErrorMessage(IDS_ERR_WRITECALCFILE, err);
		return FALSE;
	}

	if ((err = DBWriteBinary("DATA", &pDbRec->dbAcParamData, sizeof(DbAcParamData) * pDbRec->nDataNum)) != 0) {
		ErrorMessage(IDS_ERR_WRITECALCFILE, err);
		return FALSE;
	}

	return TRUE;
}

BOOL CDbAcfFactor::Open(LPCTSTR dir)
{
	static DBF_I fieldInf[] = {
		{"ACFID",     'I', LEN_ID,       0},
		{"CONDSIZE",  'I', LEN_SIZE,     0},
		{"FACTSIZE",  'I', LEN_SIZE,     0},
		{"NACFFACT",  'I', LEN_NACFFACT, 0},
		{"ACFCOND2",  'B', 0},
		{"ACFFACTOR", 'B', 0}
	};
	BOOL bCreateIndex = FALSE;
	CString pathName;
	int nRecSize;
	DBF_I dbfi;
	int idx;
	int err;

Reopen:
	pathName.Format("%s\\%s", dir, FN_ACFFACT_DBF);
	if ((err = DBOpen(g_oPcserve, pathName)) != 0) {
		if (err == ERROR_DB_NOFILE) {
			if ((err = DBCreate(g_oPcserve, pathName, fieldInf, sizeof(fieldInf) / sizeof(DBF_I))) != 0) {
				ErrorMessage(IDS_ERR_CREATECALCFILE, err);
				return FALSE;
			}
			bCreateIndex = TRUE;
		} else {
			ErrorMessage(IDS_ERR_OPENCALCFILE, err);
			return FALSE;
		}
	}

	DBRecSize(&nRecSize);
	DBField(5, &dbfi);
	if (nRecSize - 1 != sizeof(DbAcfFactorBuf) || strcmp(dbfi.name, "ACFCOND2") != 0) {
		if (::AfxMessageBox(IDS_MSG_CONVERTCALCFILE, MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL) {
			DBClose();
			return FALSE;
		}

		CPSDB dbTemp;
		CString pathNameTmp;
		pathNameTmp.Format("%s\\%s", dir, FN_TEMP_DBF);
		if ((err = dbTemp.DBCreate(g_oPcserve, pathNameTmp, fieldInf, sizeof(fieldInf) / sizeof(DBF_I))) != 0) {
			ErrorMessage(IDS_ERR_CREATECALCFILE, err);
			return FALSE;
		}

		if ((err = dbTemp.DBCopy(*this)) != 0) {
			ErrorMessage(IDS_ERR_CONVERTCALCFILE, err);
			return FALSE;
		}

		dbTemp.DBClose();
		DBClose();

		g_oPcserve.CFDelete(pathName);
		g_oPcserve.CFRename(pathNameTmp, pathName);

		pathName.Format("%s\\%s", dir, FN_ACFFACT_BIN);
		pathNameTmp.Format("%s\\%s", dir, FN_TEMP_BIN);
		g_oPcserve.CFDelete(pathName);
		g_oPcserve.CFRename(pathNameTmp, pathName);

		goto Reopen;
	}

	pathName.Format("%s\\%s", dir, FN_ACFFACT1_EDX);
	if (bCreateIndex || DBIndex(pathName, &idx) != 0) {
		if ((err = DBIdxCreate(pathName, "ACFID", 1, &idx)) != 0) {
			ErrorMessage(IDS_ERR_CREATECALCINDEX, err);
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CDbAcfFactor::ReadRecID(long id, DbAcfFactorRec *pDbRec, AcfConditionData *pAcfConditionData, AcfFactorData **pAcfFactorData, TaunPhinData **pTaunPhinData)
{
	DbAcfFactorBuf dbBuf;
	CString strID;
	int find;
	long nRead;
	int err;

	MakeStrID(id, strID);
	if ((err = DBReadKey(strID, LEN_ID, &dbBuf, 0, &find)) != 0) {
		if (err != ERROR_DB_NOKEY)
			ErrorMessage(IDS_ERR_READCALCFILE, err);
		return FALSE;
	}

	if (find == 0)
		return FALSE;

	DBGetFieldBuf(&dbBuf, pDbRec);

	if (pAcfConditionData != NULL) {
		memset(pAcfConditionData, 0, sizeof(AcfConditionData));
		if ((err = DBReadBinary("ACFCOND2", pAcfConditionData, sizeof(AcfConditionData), &nRead)) != 0) {
			ErrorMessage(IDS_ERR_READCALCFILE, err);
			return FALSE;
		}
	}

	if (pAcfFactorData != NULL) {
		*pAcfFactorData = new AcfFactorData[pDbRec->nAcfFactor];
		memset(*pAcfFactorData, 0, sizeof(AcfFactorData) * pDbRec->nAcfFactor);
		if ((err = DBReadBinary("ACFFACTOR", *pAcfFactorData, sizeof(AcfFactorData) * pDbRec->nAcfFactor, &nRead)) != 0) {
			ErrorMessage(IDS_ERR_READCALCFILE, err);
			return FALSE;
		}
		if (pTaunPhinData != NULL) {
			*pTaunPhinData = new TaunPhinData[pDbRec->nAcfFactor];
			memset(*pTaunPhinData, 0, sizeof(TaunPhinData) * pDbRec->nAcfFactor);
			if ((err = DBReadBinary(NULL, *pTaunPhinData, sizeof(TaunPhinData) * pDbRec->nAcfFactor, &nRead)) != 0) {
				ErrorMessage(IDS_ERR_READCALCFILE, err);
				return FALSE;
			}
		}
	}

	return TRUE;
}

BOOL CDbAcfFactor::SearchID(long id)
{
	CString strID;
	int find;
	int err;

	MakeStrID(id, strID);
	if ((err = DBSearch2(strID, LEN_ID, &find)) != 0) {
		ErrorMessage(IDS_ERR_FINDCALCFILE, err);
		return FALSE;
	}

	if (find != 1)
		return FALSE;

	return TRUE;
}

BOOL CDbAcfFactor::StoreRec(DbAcfFactorRec *pDbRec)
{
	DbAcfFactorBuf dbBuf;
	int err;

	DBSetFieldBuf(&dbBuf, pDbRec);
	if ((err = DBStore(&dbBuf)) != 0) {
		ErrorMessage(IDS_ERR_WRITECALCFILE, err);
		return FALSE;
	}

	return TRUE;
}

BOOL CDbAcfFactor::UpdateRec(DbAcfFactorRec *pDbRec)
{
	DbAcfFactorBuf dbBuf;
	int err;

	DBSetFieldBuf(&dbBuf, pDbRec);
	if ((err = DBUpdateKey(dbBuf.aAcfID, LEN_ID, &dbBuf)) != 0) {
		ErrorMessage(IDS_ERR_WRITECALCFILE, err);
		return FALSE;
	}

	return TRUE;
}

BOOL CDbAcfMarker::Open(LPCTSTR dir)
{
	static DBF_I fieldInf[] = {
		{"MARKERID",   'I', LEN_ID,         0},
		{"ACFID",      'I', LEN_ID,         0},
		{"TIME",	   'N', LEN_FLOAT,      5},
		{"TITLE",      'C', LEN_TITLE,      0},
		{"COMMENT",    'C', LEN_COMMENT,    0}
	};
	BOOL bCreateIndex = FALSE;
	CString pathName;
	int err;

	pathName.Format("%s\\%s", dir, FN_ACFMARKER_DBF);
	if ((err = DBOpen(g_oPcserve, pathName)) != 0) {
		if (err == ERROR_DB_NOFILE) {
			if ((err = DBCreate(g_oPcserve, pathName, fieldInf, sizeof(fieldInf) / sizeof(DBF_I))) != 0) {
//				ErrorMessage(IDS_ERR_CREATEACFFILE, err);
				return FALSE;
			}
			bCreateIndex = TRUE;
		} else {
//			ErrorMessage(IDS_ERR_OPENACFFILE, err);
			return FALSE;
		}
	}

	pathName.Format("%s\\%s", dir, FN_ACFMARKER1_EDX);
	if (bCreateIndex || DBIndex(pathName, &m_nIdxMarkerID) != 0) {
		if ((err = DBIdxCreate(pathName, "MARKERID", 0, &m_nIdxMarkerID)) != 0) {
//			ErrorMessage(IDS_ERR_CREATEACFINDEX1, err);
			return FALSE;
		}
	}

	pathName.Format("%s\\%s", dir, FN_ACFMARKER2_EDX);
	if (bCreateIndex || DBIndex(pathName, &m_nIdxAcfID) != 0) {
		if ((err = DBIdxCreate(pathName, "ACFID", 1, &m_nIdxAcfID)) != 0) {
//			ErrorMessage(IDS_ERR_CREATEACFINDEX2, err);
			return FALSE;
		}
	}

	return TRUE;
}

int CDbAcfMarker::GetNewID(long *pID)
{
	DbAcfMarkerBuf dbBuf;
	char szID[LEN_ID + 1];
	int err;

	DBChgIdx(m_nIdxMarkerID);
	DBBottom();
	if ((err = DBRead(&dbBuf)) == 0) {
		memcpy(szID, dbBuf.aMarkerID, LEN_ID);
		szID[LEN_ID] = '\0';
		*pID = atoi(szID) + 1;
	} else if (err == ERROR_DB_EOF)
		*pID = 1;
	else {
		DBSetFilter(NULL);
//		ErrorMessage(IDS_ERR_READACFFILE, err);
		return FALSE;
	}

	DBSetFilter(NULL);

	return TRUE;
}

BOOL CDbAcfMarker::ReadRecID(long id, DbAcfMarkerRec *pDbRec)
{
	DbAcfMarkerBuf dbBuf;
	CString strID;
	int find;
	int err;

	MakeStrID(id, strID);
	DBChgIdx(m_nIdxMarkerID);
	if ((err = DBReadKey(strID, LEN_ID, &dbBuf, 0, &find)) != 0) {
//		ErrorMessage(IDS_ERR_READACFFILE, err);
		return FALSE;
	}

	if (find == 0)
		return FALSE;

	DBGetFieldBuf(&dbBuf, pDbRec);

	return TRUE;
}

BOOL CDbAcfMarker::ReadRecNext(DbAcfMarkerRec *pDbRec)
{
	DbAcfMarkerBuf dbBuf;
	int nRead;
	int err;

	if ((err = DBReadNext(1, &dbBuf, &nRead)) != 0) {
//		ErrorMessage(IDS_ERR_READACFFILE, err);
		return FALSE;
	}

	if (nRead != 1)
		return FALSE;

	DBGetFieldBuf(&dbBuf, pDbRec);

	return TRUE;
}

BOOL CDbAcfMarker::StoreRec(DbAcfMarkerRec *pDbRec)
{
	DbAcfMarkerBuf dbBuf;
	int err;

	DBSetFieldBuf(&dbBuf, pDbRec);
	if ((err = DBStore(&dbBuf)) != 0) {
//		ErrorMessage(IDS_ERR_WRITEACFFILE, err);
		return FALSE;
	}

	return TRUE;
}

BOOL CDbAcfMarker::UpdateRec(DbAcfMarkerRec *pDbRec)
{
	DbAcfMarkerBuf dbBuf;
	int err;

	DBSetFieldBuf(&dbBuf, pDbRec);
	DBChgIdx(m_nIdxMarkerID);
	if ((err = DBUpdateKey(dbBuf.aMarkerID, LEN_ID, &dbBuf)) != 0) {
//		ErrorMessage(IDS_ERR_WRITEACFFILE, err);
		return FALSE;
	}

	return TRUE;
}

BOOL CDbAcfMarker::DeleteRec(long id)
{
	CString strID;
	int err;

	MakeStrID(id, strID);
	DBChgIdx(m_nIdxMarkerID);
	if ((err = DBDeleteKey(strID, LEN_ID, 1)) != 0) {
//		ErrorMessage(IDS_ERR_WRITENSTMPFILE, err);
		return FALSE;
	}

	return TRUE;
}

BOOL CDbAcfMarker::ReadRecAcfID(long id, DbAcfMarkerRec **pDbRec, int &nDataNum)
{
	CString strID;

	MakeStrID(id, strID);
	DBChgIdx(m_nIdxMarkerID);

	CString sFilter;
	sFilter.Format("ACFID=%d", id);
	DBSetFilter(sFilter);

	long nRec = 0;
	DBLRecCount(&nRec);
	DbAcfMarkerRec *pRec = new DbAcfMarkerRec[nRec];
	*pDbRec = pRec;

	DBTop();
	for (nDataNum = 0; nDataNum < nRec; nDataNum++) {
		if (!ReadRecNext(pRec))
			break;

		pRec++;
	}

	return TRUE;
}

BOOL CDbIFilter::Open(LPCTSTR dir)
{
	static DBF_I fieldInf[] = {
		{"IFILTERID", 'I', LEN_ID,       0},
		{"TITLE",     'C', LEN_TITLE,    0},
		{"COMMENT",   'C', LEN_COMMENT,  0},
		{"SAMPLING",  'I', LEN_SAMPLING, 0},
		{"STARTPOS",  'N', LEN_FLOAT,    3},
		{"ENDPOS",    'N', LEN_FLOAT,    3},
		{"MAXLEVEL",  'I', LEN_MAXLEVEL, 0},
		{"PHASEADJ",  'I', LEN_BOOL,     0},
		{"IRDATA",    'B', 0,            0},
	};
	BOOL bCreateIndex = FALSE;
	CString pathName;
	int idx;
	int err;

	pathName.Format("%s\\%s", dir, FN_IFILTER_DBF);
	if ((err = DBOpen(g_oPcserve, pathName)) != 0) {
		if (err == ERROR_DB_NOFILE) {
			if ((err = DBCreate(g_oPcserve, pathName, fieldInf, sizeof(fieldInf) / sizeof(DBF_I))) != 0) {
				ErrorMessage(IDS_ERR_CREATEIFFILE, err);
				return FALSE;
			}
			bCreateIndex = TRUE;
		} else {
			ErrorMessage(IDS_ERR_OPENIFFILE, err);
			return FALSE;
		}
	}

	pathName.Format("%s\\%s", dir, FN_IFILTER1_EDX);
	if (bCreateIndex || DBIndex(pathName, &idx) != 0) {
		if ((err = DBIdxCreate(pathName, "IFILTERID", 1, &idx)) != 0) {
			ErrorMessage(IDS_ERR_CREATEIFINDEX, err);
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CDbIFilter::GetNewID(long *pID)
{
	DbIFilterBuf dbBuf;
	char szID[LEN_ID + 1];
	int err;

	DBBottom();
	if ((err = DBRead(&dbBuf)) == 0) {
		memcpy(szID, dbBuf.aIFilterID, LEN_ID);
		szID[LEN_ID] = '\0';
		*pID = atol(szID) + 1;
	} else if (err == ERROR_DB_EOF)
		*pID = 0;
	else {
		ErrorMessage(IDS_ERR_READIFFILE, err);
		return FALSE;
	}

	return TRUE;
}

BOOL CDbIFilter::ReadRecID(long id, DbIFilterRec *pDbRec)
{
	DbIFilterBuf dbBuf;
	CString strID;
	int find;
	int err;

	MakeStrID(id, strID);
	if ((err = DBReadKey(strID, LEN_ID, &dbBuf, 0, &find)) != 0) {
		ErrorMessage(IDS_ERR_READIFFILE, err);
		return FALSE;
	}

	if (find == 0)
		return FALSE;

	DBGetFieldBuf(&dbBuf, pDbRec);

	return TRUE;
}

BOOL CDbIFilter::ReadRecNext(DbIFilterRec *pDbRec)
{
	DbIFilterBuf dbBuf;
	int nRead;
	int err;

	if ((err = DBReadNext(1, &dbBuf, &nRead)) != 0) {
		ErrorMessage(IDS_ERR_READIFFILE, err);
		return FALSE;
	}

	if (nRead != 1)
		return FALSE;

	DBGetFieldBuf(&dbBuf, pDbRec);

	return TRUE;
}

BOOL CDbIFilter::StoreRec(DbIFilterRec *pDbRec)
{
	DbIFilterBuf dbBuf;
	int err;

	DBSetFieldBuf(&dbBuf, pDbRec);
	if ((err = DBStore(&dbBuf)) != 0) {
		ErrorMessage(IDS_ERR_WRITEIFFILE, err);
		return FALSE;
	}

	return TRUE;
}

BOOL CDbIFilter::UpdateRec(DbIFilterRec *pDbRec)
{
	DbIFilterBuf dbBuf;
	int err;

	DBSetFieldBuf(&dbBuf, pDbRec);
	if ((err = DBUpdateKey(dbBuf.aIFilterID, LEN_ID, &dbBuf)) != 0) {
		ErrorMessage(IDS_ERR_WRITEIFFILE, err);
		return FALSE;
	}

	return TRUE;
}

BOOL CDbIFilter::DeleteID(long id)
{
	CString strID;
	int err;

	MakeStrID(id, strID);
	if ((err = DBDeleteKey(strID, LEN_ID, 1)) != 0) {
		ErrorMessage(IDS_ERR_DELETEIFFILE, err);
		return FALSE;
	}

	return TRUE;
}

BOOL CDbMicCal::Open(LPCTSTR dir)
{
	static DBF_I fieldInf[] = {
		{"MICCALID",   'I', LEN_ID,      0},
		{"MICINFO",    'C', LEN_MICINFO, 0},
		{"MICAMPINFO", 'C', LEN_MICAMPINFO, 0},
		{"COMMENT",    'C', LEN_COMMENT, 0},
		{"INPUTSENS",  'N', LEN_FLOAT,   2},
		{"INPUTSEL",   'I', LEN_SHORT,   0},
		{"INPUTVOL",   'I', LEN_SHORT,   0},
		{"NFREQDATA",  'I', LEN_SHORT,   0},
		{"FREQ01",     'N', LEN_FLOAT,   2},
		{"LEVEL01",    'N', LEN_FLOAT,   2},
		{"FREQ02",     'N', LEN_FLOAT,   2},
		{"LEVEL02",    'N', LEN_FLOAT,   2},
		{"FREQ03",     'N', LEN_FLOAT,   2},
		{"LEVEL03",    'N', LEN_FLOAT,   2},
		{"FREQ04",     'N', LEN_FLOAT,   2},
		{"LEVEL04",    'N', LEN_FLOAT,   2},
		{"FREQ05",     'N', LEN_FLOAT,   2},
		{"LEVEL05",    'N', LEN_FLOAT,   2},
		{"FREQ06",     'N', LEN_FLOAT,   2},
		{"LEVEL06",    'N', LEN_FLOAT,   2},
		{"FREQ07",     'N', LEN_FLOAT,   2},
		{"LEVEL07",    'N', LEN_FLOAT,   2},
		{"FREQ08",     'N', LEN_FLOAT,   2},
		{"LEVEL08",    'N', LEN_FLOAT,   2},
		{"FREQ09",     'N', LEN_FLOAT,   2},
		{"LEVEL09",    'N', LEN_FLOAT,   2},
		{"FREQ10",     'N', LEN_FLOAT,   2},
		{"LEVEL10",    'N', LEN_FLOAT,   2},
		{"FREQ11",     'N', LEN_FLOAT,   2},
		{"LEVEL11",    'N', LEN_FLOAT,   2},
		{"FREQ12",     'N', LEN_FLOAT,   2},
		{"LEVEL12",    'N', LEN_FLOAT,   2},
		{"FREQ13",     'N', LEN_FLOAT,   2},
		{"LEVEL13",    'N', LEN_FLOAT,   2},
		{"FREQ14",     'N', LEN_FLOAT,   2},
		{"LEVEL14",    'N', LEN_FLOAT,   2},
		{"FREQ15",     'N', LEN_FLOAT,   2},
		{"LEVEL15",    'N', LEN_FLOAT,   2},
		{"FREQ16",     'N', LEN_FLOAT,   2},
		{"LEVEL16",    'N', LEN_FLOAT,   2},
		{"FREQ17",     'N', LEN_FLOAT,   2},
		{"LEVEL17",    'N', LEN_FLOAT,   2},
		{"FREQ18",     'N', LEN_FLOAT,   2},
		{"LEVEL18",    'N', LEN_FLOAT,   2},
		{"FREQ19",     'N', LEN_FLOAT,   2},
		{"LEVEL19",    'N', LEN_FLOAT,   2},
		{"FREQ20",     'N', LEN_FLOAT,   2},
		{"LEVEL20",    'N', LEN_FLOAT,   2}
	};
	BOOL bCreateIndex = FALSE;
	CString pathName;
	int nRecSize;
	int idx;
	int err;

Reopen:
	pathName.Format("%s\\%s", dir, FN_MICCAL_DBF);
	if ((err = DBOpen(g_oPcserve, pathName)) != 0) {
		if (err == ERROR_DB_NOFILE) {
			if ((err = DBCreate(g_oPcserve, pathName, fieldInf, sizeof(fieldInf) / sizeof(DBF_I))) != 0) {
				ErrorMessage(IDS_ERR_CREATEMICCAL, err);
				return FALSE;
			}
			bCreateIndex = TRUE;
		} else {
			ErrorMessage(IDS_ERR_OPENMICCAL, err);
			return FALSE;
		}
	}

	DBRecSize(&nRecSize);
	if (nRecSize - 1 != sizeof(DbMicCalBuf)) {
		if (::AfxMessageBox(IDS_MSG_CONVERTMICCAL, MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL) {
			DBClose();
			return FALSE;
		}

		CPSDB dbTemp;
		CString pathNameTmp;
		pathNameTmp.Format("%s\\%s", dir, FN_TEMP_DBF);
		if ((err = dbTemp.DBCreate(g_oPcserve, pathNameTmp, fieldInf, sizeof(fieldInf) / sizeof(DBF_I))) != 0) {
			ErrorMessage(IDS_ERR_CREATEMICCAL, err);
			return FALSE;
		}

		if ((err = dbTemp.DBCopy(*this)) != 0) {
			ErrorMessage(IDS_ERR_CONVERTMICCAL, err);
			return FALSE;
		}

		dbTemp.DBClose();
		DBClose();

		g_oPcserve.CFDelete(pathName);
		g_oPcserve.CFRename(pathNameTmp, pathName);
		goto Reopen;
	}

	pathName.Format("%s\\%s", dir, FN_MICCAL1_EDX);
	if (bCreateIndex || DBIndex(pathName, &idx) != 0) {
		if ((err = DBIdxCreate(pathName, "MICCALID", 1, &idx)) != 0) {
			ErrorMessage(IDS_ERR_CREATEMICCALINDEX, err);
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CDbMicCal::GetNewID(long *pID)
{
	DbMicCalBuf dbBuf;
	char szID[LEN_ID + 1];
	int err;

	DBBottom();
	if ((err = DBRead(&dbBuf)) == 0) {
		memcpy(szID, dbBuf.aMicCalID, LEN_ID);
		szID[LEN_ID] = '\0';
		*pID = atol(szID) + 1;
	} else if (err == ERROR_DB_EOF)
		*pID = 0;
	else {
		ErrorMessage(IDS_ERR_READMICCAL, err);
		return FALSE;
	}

	return TRUE;
}

BOOL CDbMicCal::ReadRecID(long id, DbMicCalRec *pDbRec)
{
	DbMicCalBuf dbBuf;
	CString strID;
	int find;
	int err;

	MakeStrID(id, strID);
	if ((err = DBReadKey(strID, LEN_ID, &dbBuf, 0, &find)) != 0) {
		ErrorMessage(IDS_ERR_READMICCAL, err);
		return FALSE;
	}

	if (find == 0)
		return FALSE;

	DBGetFieldBuf(&dbBuf, pDbRec);

	return TRUE;
}

BOOL CDbMicCal::ReadRecNext(DbMicCalRec *pDbRec)
{
	DbMicCalBuf dbBuf;
	int nRead;
	int err;

	if ((err = DBReadNext(1, &dbBuf, &nRead)) != 0) {
		ErrorMessage(IDS_ERR_READMICCAL, err);
		return FALSE;
	}

	if (nRead != 1)
		return FALSE;

	DBGetFieldBuf(&dbBuf, pDbRec);

	return TRUE;
}

BOOL CDbMicCal::StoreRec(DbMicCalRec *pDbRec)
{
	DbMicCalBuf dbBuf;
	int err;

	DBSetFieldBuf(&dbBuf, pDbRec);
	if ((err = DBStore(&dbBuf)) != 0) {
		ErrorMessage(IDS_ERR_WRITEMICCAL, err);
		return FALSE;
	}

	return TRUE;
}

BOOL CDbMicCal::UpdateRec(DbMicCalRec *pDbRec)
{
	DbMicCalBuf dbBuf;
	int err;

	DBSetFieldBuf(&dbBuf, pDbRec);
	if ((err = DBUpdateKey(dbBuf.aMicCalID, LEN_ID, &dbBuf)) != 0) {
		if (err != 3006 || (err = DBStore(&dbBuf)) != 0) {
			ErrorMessage(IDS_ERR_WRITEMICCAL, err);
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CDbMicCal::DeleteID(long id)
{
	CString strID;
	int err;

	MakeStrID(id, strID);
	if ((err = DBDeleteKey(strID, LEN_ID, 1)) != 0) {
		ErrorMessage(IDS_ERR_DELETEMICCAL, err);
		return FALSE;
	}

	return TRUE;
}

BOOL CDbMicCal::SearchRecID(long id)
{
	CString strID;
	int find;
	int err;

	MakeStrID(id, strID);
	if ((err = DBSearch2(strID, LEN_ID, &find)) != 0)
		return FALSE;

	if (find != 1)
		return FALSE;

	return TRUE;
}

BOOL CDbPreset::Open(LPCTSTR dir)
{
	static DBF_I fieldInf[] = {
		{"PRESETID",   'I', LEN_ID,    0},
		{"TITLE",      'C', LEN_TITLE, 0},
		{"OPENWINDOW", 'I', LEN_SHORT, 0},
		{"SG",         'B', 0, 0},
		{"FFT",        'B', 0, 0},
		{"OS",         'B', 0, 0},
		{"FRE",        'B', 0, 0},
		{"DST",        'B', 0, 0},
		{"IMP",        'B', 0, 0},
		{"ACF",        'B', 0, 0}
	};
	BOOL bCreateIndex = FALSE;
	CString pathName;
	BOOL bReopen = FALSE;
	int nRecSize;
	int nField;
	int idx;
	int err;

Reopen:
	pathName.Format("%s\\%s", dir, FN_PRESET_DBF);
	if ((err = DBOpen(g_oPcserve, pathName)) != 0) {
		if (err == ERROR_DB_NOFILE) {
			if ((err = DBCreate(g_oPcserve, pathName, fieldInf, sizeof(fieldInf) / sizeof(DBF_I))) != 0) {
				ErrorMessage(IDS_ERR_CREATEPRESET, err);
				return FALSE;
			}
			bCreateIndex = TRUE;
		} else {
			ErrorMessage(IDS_ERR_OPENPRESET, err);
			return FALSE;
		}
	}

	DBNField(&nField);
	DBRecSize(&nRecSize);
	if (nField != sizeof(fieldInf) / sizeof(DBF_I) || nRecSize - 1 != sizeof(DbPresetBuf)) {
//		if (::AfxMessageBox(IDS_MSG_CONVERTPRESET, MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL) {
		if (bReopen) {
			DBClose();
			return FALSE;
		}

		CPSDB dbTemp;
		CString pathNameTmp;
		pathNameTmp.Format("%s\\%s", dir, FN_TEMP_DBF);
		if ((err = dbTemp.DBCreate(g_oPcserve, pathNameTmp, fieldInf, sizeof(fieldInf) / sizeof(DBF_I))) != 0) {
			ErrorMessage(IDS_ERR_CREATEPRESET, err);
			return FALSE;
		}

		if ((err = dbTemp.DBCopy(*this)) != 0) {
			ErrorMessage(IDS_ERR_CONVERTPRESET, err);
			return FALSE;
		}

		dbTemp.DBClose();
		DBClose();

		g_oPcserve.CFDelete(pathName);
		g_oPcserve.CFRename(pathNameTmp, pathName);

		pathName.Format("%s\\%s", dir, FN_PRESET_BIN);
		pathNameTmp.Format("%s\\%s", dir, FN_TEMP_BIN);
		g_oPcserve.CFDelete(pathName);
		g_oPcserve.CFRename(pathNameTmp, pathName);

		bReopen = TRUE;
		goto Reopen;
	}

	pathName.Format("%s\\%s", dir, FN_PRESET1_EDX);
	if (bCreateIndex || DBIndex(pathName, &idx) != 0) {
		if ((err = DBIdxCreate(pathName, "PRESETID", 1, &idx)) != 0) {
			ErrorMessage(IDS_ERR_CREATEPRESETINDEX, err);
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CDbPreset::GetNewID(long *pID)
{
	DbPresetBuf dbBuf;
	char szID[LEN_ID + 1];
	int err;

	DBBottom();
	if ((err = DBRead(&dbBuf)) == 0) {
		memcpy(szID, dbBuf.aPresetID, LEN_ID);
		szID[LEN_ID] = '\0';
		*pID = atol(szID) + 1;
	} else if (err == ERROR_DB_EOF)
		*pID = 0;
	else {
		ErrorMessage(IDS_ERR_READPRESET, err);
		return FALSE;
	}

	return TRUE;
}

BOOL CDbPreset::ReadRecID(long id, DbPresetRec *pDbRec)
{
	DbPresetBuf dbBuf;
	CString strID;
	int find;
	int err;

	MakeStrID(id, strID);
	if ((err = DBReadKey(strID, LEN_ID, &dbBuf, 0, &find)) != 0) {
		ErrorMessage(IDS_ERR_READPRESET, err);
		return FALSE;
	}

	if (find == 0)
		return FALSE;

	DBGetFieldBuf(&dbBuf, pDbRec);

	return TRUE;
}

BOOL CDbPreset::ReadRecNext(DbPresetRec *pDbRec)
{
	DbPresetBuf dbBuf;
	int nRead;
	int err;

	if ((err = DBReadNext(1, &dbBuf, &nRead)) != 0) {
		ErrorMessage(IDS_ERR_READPRESET, err);
		return FALSE;
	}

	if (nRead != 1)
		return FALSE;

	DBGetFieldBuf(&dbBuf, pDbRec);

	return TRUE;
}

BOOL CDbPreset::StoreRec(DbPresetRec *pDbRec)
{
	DbPresetBuf dbBuf;
	int err;

	DBSetFieldBuf(&dbBuf, pDbRec);
	if ((err = DBStore(&dbBuf)) != 0) {
		ErrorMessage(IDS_ERR_WRITEPRESET, err);
		return FALSE;
	}

	return TRUE;
}

BOOL CDbPreset::UpdateRec(DbPresetRec *pDbRec)
{
	DbPresetBuf dbBuf;
	int err;

	DBSetFieldBuf(&dbBuf, pDbRec);
	if ((err = DBUpdateKey(dbBuf.aPresetID, LEN_ID, &dbBuf)) != 0) {
		ErrorMessage(IDS_ERR_WRITEPRESET, err);
		return FALSE;
	}

	return TRUE;
}

BOOL CDbPreset::DeleteID(long id)
{
	CString strID;
	int err;

	MakeStrID(id, strID);
	if ((err = DBDeleteKey(strID, LEN_ID, 1)) != 0) {
		ErrorMessage(IDS_ERR_DELETEPRESET, err);
		return FALSE;
	}

	return TRUE;
}

BOOL CDbWaveForm::Open(LPCTSTR dir)
{
	static DBF_I fieldInf[] = {
		{"WAVEFORMID", 'I', LEN_ID,    0},
		{"SMOOTHFLAG", 'I', LEN_BOOL,  0},
		{"DATANUM",    'I', LEN_WAVEFORMNUM, 0},
		{"DATA",       'B', 0, 0}
	};
	BOOL bCreateIndex = FALSE;
	CString pathName;
	int idx;
	int err;

	pathName.Format("%s\\%s", dir, FN_WAVEFORM_DBF);
	if ((err = DBOpen(g_oPcserve, pathName)) != 0) {
		if (err == ERROR_DB_NOFILE) {
			if ((err = DBCreate(g_oPcserve, pathName, fieldInf, sizeof(fieldInf) / sizeof(DBF_I))) != 0) {
				ErrorMessage(IDS_ERR_CREATEWAVEFORM, err);
				return FALSE;
			}
			bCreateIndex = TRUE;
		} else {
			ErrorMessage(IDS_ERR_OPENWAVEFORM, err);
			return FALSE;
		}
	}

	pathName.Format("%s\\%s", dir, FN_WAVEFORM1_EDX);
	if (bCreateIndex || DBIndex(pathName, &idx) != 0) {
		if ((err = DBIdxCreate(pathName, "WAVEFORMID", 1, &idx)) != 0) {
			ErrorMessage(IDS_ERR_CREATEWAVEFORMINDEX, err);
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CDbWaveForm::ReadRecID(long id, DbWaveFormRec *pDbRec, WaveFormData *pWaveFormData)
{
	DbWaveFormBuf dbBuf;
	CString strID;
	int find;
	long nRead;
	int err;

	MakeStrID(id, strID);
	if ((err = DBReadKey(strID, LEN_ID, &dbBuf, 0, &find)) != 0) {
		if (err != 3006)
			ErrorMessage(IDS_ERR_READWAVEFORM, err);
		return FALSE;
	}

	if (find == 0)
		return FALSE;

	DBGetFieldBuf(&dbBuf, pDbRec);

	int nDataSize;
	switch (pDbRec->nDataNum) {
	case 1000:	// FM音源
		nDataSize = sizeof(float) * 2;
		break;
	default:
		nDataSize = sizeof(WaveFormData) * pDbRec->nDataNum;
		break;
	}

	if ((err = DBReadBinary("DATA", pWaveFormData, nDataSize, &nRead)) != 0) {
		ErrorMessage(IDS_ERR_READWAVEFORM, err);
		return FALSE;
	}

	return TRUE;
}

BOOL CDbWaveForm::WriteRec(DbWaveFormRec *pDbRec, WaveFormData *pWaveFormData)
{
	DbWaveFormBuf dbBuf;
	int find;
	int err;

	DBSetFieldBuf(&dbBuf, pDbRec);

	DBSearch2(dbBuf.aWaveFormID, LEN_ID, &find);
	if (find != 1) {
		if ((err = DBStore(&dbBuf)) != 0) {
			ErrorMessage(IDS_ERR_WRITEWAVEFORM, err);
			return FALSE;
		}
	} else {
		if ((err = DBUpdateKey(dbBuf.aWaveFormID, LEN_ID, &dbBuf)) != 0) {
			ErrorMessage(IDS_ERR_WRITEWAVEFORM, err);
			return FALSE;
		}
	}

	int nDataSize;
	switch (pDbRec->nDataNum) {
	case 1000:	// FM音源
		nDataSize = sizeof(float) * 2;
		break;
	default:
		nDataSize = sizeof(WaveFormData) * pDbRec->nDataNum;
		break;
	}

	if ((err = DBWriteBinary("DATA", pWaveFormData, nDataSize)) != 0) {
		ErrorMessage(IDS_ERR_WRITEWAVEFORM, err);
		return FALSE;
	}

	return TRUE;
}

void DeleteFolder(long nFolderID)
{
	CDbFolder dbFolder;
	DbFolderBuf dbFolderBuf;
	CDbImpulse dbImpulse;
	DbImpulseRec dbImpulseRec;
	CDbAcParam dbAcParam;
	CDbAcf dbAcf;
	DbAcfRec dbAcfRec;
	CDbAcfFactor dbAcfFactor;
	CDbNms dbNms;
	CString strFolderID;
	CString strDataID;
	CString select;
	int find;
	int i;
	long nRec;

	if (!dbFolder.Open())
		return;

	MakeStrID(nFolderID, strFolderID);
	if (dbFolder.DBReadKey(strFolderID, LEN_ID, &dbFolderBuf, 0, &find) != 0 || find == 0) {
		::AfxMessageBox(IDS_ERR_FINDFOLDER);
		return;
	}

	if (dbFolder.DBDeleteKey(strFolderID, LEN_ID, 0) != 0) {
		::AfxMessageBox(IDS_ERR_DELETEFOLDER);
		return;
	}

	switch (dbFolderBuf.aType[0]) {
	case FOLDER_TYPE_IR:
		if (!dbImpulse.Open())
			return;

		if (!dbAcParam.Open())
			return;

		select.Format("#%d=%ld", dbImpulse.m_nIdxFolderID, nFolderID);
		if (dbImpulse.DBSelect(select, &nRec) == 0) {
			for (i = 0; i < nRec; i++) {
				if (!dbImpulse.ReadRecNext(&dbImpulseRec))
					break;
				MakeStrID(dbImpulseRec.nImpulseID, strDataID);
				dbAcParam.DBDeleteKey(strDataID, LEN_ID, 1);
			}
		}
		dbImpulse.DBSelect(NULL, &nRec);

		if (dbImpulse.DBChgIdx(dbImpulse.m_nIdxFolderID) != 0) {
			::AfxMessageBox(IDS_ERR_DELETEIRDATA);
			return;
		}

		if (dbImpulse.DBDeleteKey(strFolderID, LEN_ID, 1) != 0) {
			::AfxMessageBox(IDS_ERR_DELETEIRDATA);
			return;
		}
		break;
	case FOLDER_TYPE_ACF:
		if (!dbAcf.Open())
			return;

		if (!dbAcfFactor.Open())
			return;

		select.Format("#%d=%ld", dbAcf.m_nIdxFolderID, nFolderID);
		if (dbAcf.DBSelect(select, &nRec) == 0) {
			for (i = 0; i < nRec; i++) {
				if (!dbAcf.ReadRecNext(&dbAcfRec))
					break;
				MakeStrID(dbAcfRec.nAcfID, strDataID);
				dbAcfFactor.DBDeleteKey(strDataID, LEN_ID, 1);
			}
		}
		dbAcf.DBSelect(NULL, &nRec);

		if (dbAcf.DBChgIdx(dbAcf.m_nIdxFolderID) != 0) {
			::AfxMessageBox(IDS_ERR_DELETEIRDATA);
			return;
		}

		if (dbAcf.DBDeleteKey(strFolderID, LEN_ID, 1) != 0) {
			::AfxMessageBox(IDS_ERR_DELETEIRDATA);
			return;
		}
		break;
	case FOLDER_TYPE_NMS:
		if (!dbNms.Open())
			return;

		if (dbNms.DBChgIdx(dbNms.m_nIdxFolderID) != 0) {
			::AfxMessageBox(IDS_ERR_DELETEIRDATA);
			return;
		}

		if (dbNms.DBDeleteKey(strFolderID, LEN_ID, 1) != 0) {
			::AfxMessageBox(IDS_ERR_DELETEIRDATA);
			return;
		}
		break;
	default:
		return;
	}
}

void DeleteImpulse(long nImpulseID)
{
	CDbImpulse dbImpulse;
	CDbAcParam dbAcParam;
	CString strImpulseID;

	if (!dbImpulse.Open())
		return;

	MakeStrID(nImpulseID, strImpulseID);
	if (dbImpulse.DBChgIdx(dbImpulse.m_nIdxImpulseID) != 0) {
		::AfxMessageBox(IDS_ERR_DELETEIRDATA);
		return;
	}

	if (dbImpulse.DBDeleteKey(strImpulseID, LEN_ID, 1) != 0) {
		::AfxMessageBox(IDS_ERR_DELETEIRDATA);
		return;
	}

	if (!dbAcParam.Open())
		return;

	dbAcParam.DBDeleteKey(strImpulseID, LEN_ID, 1);
}

void DeleteAcf(long nAcfID)
{
	CDbAcf dbAcf;
	CDbAcfFactor dbAcfFactor;
	CDbAcfMarker dbAcfMarker;
	CString strAcfID;

	if (!dbAcf.Open())
		return;

	MakeStrID(nAcfID, strAcfID);
	if (dbAcf.DBChgIdx(dbAcf.m_nIdxAcfID) != 0) {
		::AfxMessageBox(IDS_ERR_DELETEIRDATA);
		return;
	}

	if (dbAcf.DBDeleteKey(strAcfID, LEN_ID, 1) != 0) {
		::AfxMessageBox(IDS_ERR_DELETEIRDATA);
		return;
	}

	if (dbAcfFactor.Open())
		dbAcfFactor.DBDeleteKey(strAcfID, LEN_ID, 1);

	if (dbAcfMarker.Open()) {
		dbAcfMarker.DBChgIdx(dbAcfMarker.m_nIdxAcfID);
		dbAcfMarker.DBDeleteKey(strAcfID, LEN_ID, 1);
	}
}

void DeleteNms(long nNmsID)
{
	CDbNms dbNms;
	CString strNmsID;

	if (!dbNms.Open())
		return;

	MakeStrID(nNmsID, strNmsID);
	if (dbNms.DBChgIdx(dbNms.m_nIdxNmsID) != 0) {
		::AfxMessageBox(IDS_ERR_DELETEIRDATA);
		return;
	}

	if (dbNms.DBDeleteKey(strNmsID, LEN_ID, 1) != 0) {
		::AfxMessageBox(IDS_ERR_DELETEIRDATA);
		return;
	}
}

CString FormatTime(const CString &src)
{
	CString dst;

	switch (src.GetLength()) {
	case 8:		// 日付のみ
		dst = src.Left(4) + "/" + src.Mid(4, 2) + "/" + src.Mid(6, 2);
		break;
	case 14:	// 日付＋時刻
		dst = src.Left(4) + "/" + src.Mid(4, 2) + "/" + src.Mid(6, 2) + " " +
				src.Mid(8, 2) + ":" + src.Mid(10, 2) + ":" + src.Mid(12, 2);
	}

	return dst;
}

CString UnformatTime(const CString &src)
{
	CString dst;

	switch (src.GetLength()) {
	case 10:		// 日付のみ
		dst = src.Left(4) + src.Mid(5, 2) + src.Mid(8, 2);
		break;
	case 19:		// 日付＋時刻
		dst = src.Left(4) + src.Mid(5, 2) + src.Mid(8, 2) +
				src.Mid(11, 2) + src.Mid(14, 2) + src.Mid(17, 2);
		break;
	}

	return dst;
}

static void ErrorMessage(UINT msgID, int err)
{
	CString str1, str2;

	str1.LoadString(msgID);
	str2.Format("%s (Error=%d)", (LPCSTR)str1, err);
	AfxMessageBox(str2);
}

static void MakeStrID(long id, CString &strID)
{
	strID.Format("%8ld", id);
}

void ReadNsWeightData(NsWeightData *pNsWeightData)
{
	CString path;
	CFile file;

	path.Format("%s\\%s", g_szDefaultDir, FN_NSWEIGHT_DAT);
	if (file.Open(path, CFile::modeRead | CFile::shareDenyNone)) {
		memset(pNsWeightData, 0, sizeof(NsWeightData));
		file.Read(pNsWeightData, sizeof(NsWeightData));
	} else {
		pNsWeightData->fPhi0 = 0;
		pNsWeightData->fTaue = 1;
		pNsWeightData->fTau1 = 1;
		pNsWeightData->fPhi1 = 1;
		pNsWeightData->fIACC = 0;
		pNsWeightData->fTIACC = 0;
		pNsWeightData->fWIACC = 0;
	}
}

void WriteNsWeightData(const NsWeightData *pNsWeightData)
{
	CString path;
	CFile file;

	path.Format("%s\\%s", g_szDefaultDir, FN_NSWEIGHT_DAT);
	if (file.Open(path, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone))
		file.Write(pNsWeightData, sizeof(NsWeightData));
}

void ImportIR(LPCTSTR pDir)
{
	CDbFolder dbSrcFolder, dbDstFolder;
	CDbImpulse dbSrcImpulse, dbDstImpulse;
	DbFolderRec dbFolderRec;
	DbImpulseRec dbImpulseRec;
	long nFolderID;
	CString str;
	long nRec;
	int i;
	HANDLE hWaveData;

	if (_stricmp(g_szDefaultDir, pDir) == 0)
		return;

	if (!dbSrcFolder.Open(pDir))
		return;

	if (!dbSrcImpulse.Open(pDir))
		return;

	if (!dbDstFolder.Open())
		return;

	if (!dbDstImpulse.Open())
		return;

	dbSrcFolder.DBSetFilter("TYPE=I");
	while (dbSrcFolder.ReadRecNext(&dbFolderRec)) {
		nFolderID = dbFolderRec.nFolderID;
		dbDstFolder.GetNewID(&dbFolderRec.nFolderID);
		dbDstFolder.StoreRec(&dbFolderRec);

		dbSrcImpulse.DBChgIdx(dbSrcImpulse.m_nIdxImpulseID);
		str.Format("#%d=%ld", dbSrcImpulse.m_nIdxFolderID, nFolderID);
		dbSrcImpulse.DBSelect(str, &nRec);
		for (i = 0; i < nRec; i++) {
			hWaveData = NULL;
			MMWaveReadDB(&hWaveData, dbSrcImpulse.GetFCB(), "WAVEDATA");

			if (!dbSrcImpulse.ReadRecNext(&dbImpulseRec))
				break;

			dbImpulseRec.nFolderID = dbFolderRec.nFolderID;
			dbDstImpulse.GetNewID(&dbImpulseRec.nImpulseID);
			dbDstImpulse.StoreRec(&dbImpulseRec);

			if (hWaveData != NULL) {
				MMWaveWriteDB(hWaveData, dbDstImpulse.GetFCB(), "WAVEDATA", MMTYPE_WAV);
				::GlobalFree(hWaveData);
			}
		}
	}
}

void ImportACF(LPCTSTR pDir)
{
	CDbFolder dbSrcFolder, dbDstFolder;
	CDbAcf dbSrcAcf, dbDstAcf;
	DbFolderRec dbFolderRec;
	DbAcfRec dbAcfRec;
	long nFolderID;
	CString str;
	long nRec;
	int i;
	HANDLE hWaveData;

	if (_stricmp(g_szDefaultDir, pDir) == 0)
		return;

	if (!dbSrcFolder.Open(pDir))
		return;

	if (!dbSrcAcf.Open(pDir))
		return;

	if (!dbDstFolder.Open())
		return;

	if (!dbDstAcf.Open())
		return;

	dbSrcFolder.DBSetFilter("TYPE=A");
	while (dbSrcFolder.ReadRecNext(&dbFolderRec)) {
		nFolderID = dbFolderRec.nFolderID;
		dbDstFolder.GetNewID(&dbFolderRec.nFolderID);
		dbDstFolder.StoreRec(&dbFolderRec);

		dbSrcAcf.DBChgIdx(dbSrcAcf.m_nIdxAcfID);
		str.Format("#%d=%ld", dbSrcAcf.m_nIdxFolderID, nFolderID);
		dbSrcAcf.DBSelect(str, &nRec);
		for (i = 0; i < nRec; i++) {
			hWaveData = NULL;
			MMWaveReadDB(&hWaveData, dbSrcAcf.GetFCB(), "WAVEDATA");

			if (!dbSrcAcf.ReadRecNext(&dbAcfRec))
				break;

			dbAcfRec.nFolderID = dbFolderRec.nFolderID;
			dbDstAcf.GetNewID(&dbAcfRec.nAcfID);
			dbDstAcf.StoreRec(&dbAcfRec);

			if (hWaveData != NULL) {
				MMWaveWriteDB(hWaveData, dbDstAcf.GetFCB(), "WAVEDATA", MMTYPE_WAV);
				::GlobalFree(hWaveData);
			}
		}
	}
}
