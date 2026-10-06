#include "StdAfx.h"
#include "Sas.h"
#include "ImportExport.h"
#include "FileIO.h"

#define RECOED_TYPE_EOF					0
#define RECOED_TYPE_FOLDER				1
#define RECOED_TYPE_IR					2
#define RECOED_TYPE_IR_WAVEDATA			3
#define RECOED_TYPE_ACPARAM				4
#define RECOED_TYPE_ACPARAM_CONDITION	5
#define RECOED_TYPE_ACPARAM_RESULT		6
#define RECOED_TYPE_ACPARAM_DATA		7
#define RECOED_TYPE_ACF					8
#define RECOED_TYPE_ACF_WAVEDATA		9
#define RECOED_TYPE_ACFFACTOR			10
#define RECOED_TYPE_ACFFACTOR_ACFCOND2	11
#define RECOED_TYPE_ACFFACTOR_ACFFACTOR	12
#define RECOED_TYPE_NMS					13
#define RECOED_TYPE_NMS_WAVEDATA		14
#define RECOED_TYPE_NMS_NMSCOND			15
#define RECOED_TYPE_NMS_NMSFACTOR		16
#define RECOED_TYPE_NMS_NOISESRC		17

#define	OFFSET_MEASTIME	50

CImportExport::CImportExport(void)
{
	m_nImportID = 0;
	m_bReadFlag = FALSE;
	m_bAddIndex = TRUE;
}

void CImportExport::Export(const CString &sFilePath, long *pIDs, int nItem, int nFolderType)
{
	if (!m_oFile.Open(sFilePath, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite)) {
		MessageBoxID(NULL, IDS_ERR_OPEN_EXPORT_FILE, MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	m_oFile.Write("DSSF3", 5);

	for (int i = 0; i < nItem; i++) {
		Write(pIDs[i], nFolderType);
	}

	m_oFile.Close();
}

void CImportExport::Write(long nID, int nFolderType)
{
	switch (nID & ID_KIND) {
	case ID_SYSTEM:
		WriteFolders(0);
		break;
	case ID_DB_IR:
		WriteFolders(FOLDER_TYPE_IR);
		break;
	case ID_DB_ACF:
		WriteFolders(FOLDER_TYPE_ACF);
		break;
	case ID_DB_NMS:
		WriteFolders(FOLDER_TYPE_NMS);
		break;
	case ID_FOLDER:
		WriteFolder(nID & ID_VALUE);
		break;
	case ID_DATA:
		WriteData(nID & ID_VALUE, nFolderType);
		break;
	}
}

void CImportExport::WriteFolders(char nFolderType)
{
	CDbFolder oDbFolder;
	if (oDbFolder.Open()) {
		DbFolderRec oDbFolderRec;
		CString str;
		if (nFolderType != 0)
			str.Format("TYPE=%c", nFolderType);
		oDbFolder.DBSetFilter(str);
		while (oDbFolder.ReadRecNext(&oDbFolderRec)) {
			WriteFolder(oDbFolderRec.nFolderID);
		}
	}
}

void CImportExport::WriteFolder(long nID)
{
	CDbFolder oDbFolder;
	if (oDbFolder.Open()) {
		DbFolderRec oDbFolderRec;
		if (oDbFolder.ReadRecID(nID, &oDbFolderRec)) {
			DbFolderBuf oDbFolderBuf;
			oDbFolder.DBSetFieldBuf(&oDbFolderBuf, &oDbFolderRec);
			WriteRecord(RECOED_TYPE_FOLDER, &oDbFolderBuf, sizeof(oDbFolderBuf));

			switch (oDbFolderRec.sType[0]) {
			case FOLDER_TYPE_IR:
				{
					CDbImpulse oDbImpulse;
					if (oDbImpulse.Open()) {
						CString str;
						long nRec;
						DbImpulseRec oDbImpulseRec;
						oDbImpulse.DBChgIdx(oDbImpulse.m_nIdxImpulseID);
						str.Format("#%d=%ld", oDbImpulse.m_nIdxFolderID, nID);
						oDbImpulse.DBSelect(str, &nRec);
						while (oDbImpulse.ReadRecNext(&oDbImpulseRec)) {
							WriteData(oDbImpulseRec.nImpulseID, oDbFolderRec.sType[0]);
						}
					}
				}
				break;
			case FOLDER_TYPE_ACF:
				{
					CDbAcf oDbAcf;
					if (oDbAcf.Open()) {
						CString str;
						long nRec;
						DbAcfRec oDbAcfRec;
						oDbAcf.DBChgIdx(oDbAcf.m_nIdxAcfID);
						str.Format("#%d=%ld", oDbAcf.m_nIdxFolderID, nID);
						oDbAcf.DBSelect(str, &nRec);
						while (oDbAcf.ReadRecNext(&oDbAcfRec)) {
							WriteData(oDbAcfRec.nAcfID, oDbFolderRec.sType[0]);
						}
					}
				}
				break;
			case FOLDER_TYPE_NMS:
				{
					CDbNms oDbNms;
					if (oDbNms.Open()) {
						CString str;
						long nRec;
						DbNmsRec oDbNmsRec;
						oDbNms.DBChgIdx(oDbNms.m_nIdxNmsID);
						str.Format("#%d=%ld", oDbNms.m_nIdxFolderID, nID);
						oDbNms.DBSelect(str, &nRec);
						while (oDbNms.ReadRecNext(&oDbNmsRec)) {
							WriteData(oDbNmsRec.nNmsID, oDbFolderRec.sType[0]);
						}
					}
				}
				break;
			}
		}
	}
}

void CImportExport::WriteData(long nID, int nFolderType)
{
	switch (nFolderType) {
	case FOLDER_TYPE_IR:
		WriteDataIR(nID);
		break;
	case FOLDER_TYPE_ACF:
		WriteDataACF(nID);
		break;
	case FOLDER_TYPE_NMS:
		WriteDataNMS(nID);
		break;
	}
}

void CImportExport::WriteDataIR(long nID)
{
	CDbImpulse oDbImpulse;

	if (oDbImpulse.Open()) {
		DbImpulseRec oDbImpulseRec;
		if (oDbImpulse.ReadRecID(nID, &oDbImpulseRec)) {
			if (oDbImpulseRec.bImpulseData)
				oDbImpulseRec.nMeasTime += OFFSET_MEASTIME;

			DbImpulseBuf oDbImpulseBuf;
			oDbImpulse.DBSetFieldBuf(&oDbImpulseBuf, &oDbImpulseRec);
			WriteRecord(RECOED_TYPE_IR, &oDbImpulseBuf, sizeof(oDbImpulseBuf));

			WriteBinary(oDbImpulse, "WAVEDATA", RECOED_TYPE_IR_WAVEDATA);

			CDbAcParam oDbAcParam;
			if (oDbAcParam.Open()) {
				DbAcParamRec oDbAcParamRec;
				if (oDbAcParam.ReadRecID(nID, &oDbAcParamRec)) {
					DbAcParamBuf oDbAcParamBuf;
					oDbAcParam.DBSetFieldBuf(&oDbAcParamBuf, &oDbAcParamRec);
					WriteRecord(RECOED_TYPE_ACPARAM, &oDbAcParamBuf, sizeof(oDbAcParamBuf));

					WriteBinary(oDbAcParam, "CONDITION", RECOED_TYPE_ACPARAM_CONDITION);
					WriteBinary(oDbAcParam, "RESULT", RECOED_TYPE_ACPARAM_RESULT);
					WriteBinary(oDbAcParam, "DATA", RECOED_TYPE_ACPARAM_DATA);
				}
			}
		}
	}
}

void CImportExport::WriteDataACF(long nID)
{
	CDbAcf oDbAcf;

	if (oDbAcf.Open()) {
		DbAcfRec oDbAcfRec;
		if (oDbAcf.ReadRecID(nID, &oDbAcfRec)) {
			DbAcfBuf oDbAcfBuf;
			oDbAcf.DBSetFieldBuf(&oDbAcfBuf, &oDbAcfRec);
			WriteRecord(RECOED_TYPE_ACF, &oDbAcfBuf, sizeof(oDbAcfBuf));

			WriteBinary(oDbAcf, "WAVEDATA", RECOED_TYPE_ACF_WAVEDATA);

			CDbAcfFactor oDbAcfFactor;
			if (oDbAcfFactor.Open()) {
				DbAcfFactorRec oDbAcfFactorRec;
				if (oDbAcfFactor.ReadRecID(nID, &oDbAcfFactorRec)) {
					DbAcfFactorBuf oDbAcfFactorBuf;
					oDbAcfFactor.DBSetFieldBuf(&oDbAcfFactorBuf, &oDbAcfFactorRec);
					WriteRecord(RECOED_TYPE_ACFFACTOR, &oDbAcfFactorBuf, sizeof(oDbAcfFactorBuf));

					WriteBinary(oDbAcfFactor, "ACFCOND2", RECOED_TYPE_ACFFACTOR_ACFCOND2);
					WriteBinary(oDbAcfFactor, "ACFFACTOR", RECOED_TYPE_ACFFACTOR_ACFFACTOR);
				}
			}
		}
	}
}

void CImportExport::WriteDataNMS(long nID)
{
	CDbNms oDbNms;

	if (oDbNms.Open()) {
		DbNmsRec oDbNmsRec;
		if (oDbNms.ReadRecID(nID, &oDbNmsRec, NULL, NULL, NULL)) {
			DbNmsBuf oDbNmsBuf;
			oDbNms.DBSetFieldBuf(&oDbNmsBuf, &oDbNmsRec);
			WriteRecord(RECOED_TYPE_NMS, &oDbNmsBuf, sizeof(oDbNmsBuf));

			WriteBinary(oDbNms, "WAVEDATA", RECOED_TYPE_NMS_WAVEDATA);
			WriteBinary(oDbNms, "NMSCOND", RECOED_TYPE_NMS_NMSCOND);
			WriteBinary(oDbNms, "NMSFACTOR", RECOED_TYPE_NMS_NMSFACTOR);
			WriteBinary(oDbNms, "NOISESRC", RECOED_TYPE_NMS_NOISESRC);
		}
	}
}

void CImportExport::WriteRecord(int nRecordType, void *pData, int nSize)
{
	RecordHeader oRecordHeader;
	oRecordHeader.nType = nRecordType;
	oRecordHeader.nSize = nSize;
	m_oFile.Write(&oRecordHeader, sizeof(oRecordHeader));
	m_oFile.Write(pData, oRecordHeader.nSize);
}

void CImportExport::WriteBinary(CPSDB &db, LPCSTR pFieldName, int nRecordType)
{
	long nBinarySize;
	db.DBGetBinarySize(pFieldName, &nBinarySize);
	if (nBinarySize > 0) {
		BYTE *pData = new BYTE[nBinarySize];
		long nReadNum;
		db.DBReadBinary(pFieldName, pData, nBinarySize, &nReadNum);

		WriteRecord(nRecordType, pData, nReadNum);

		delete [] pData;
	}
}

long CImportExport::Import(const CString &sFilePath, long nID)
{
	if (!m_oFile.Open(sFilePath, CFile::modeRead | CFile::shareDenyNone)) {
		MessageBoxID(NULL, IDS_ERR_OPEN_IMPORT_FILE, MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}

	char buf[6];
	m_oFile.Read(buf, 5);
	buf[5] = '\0';
	if (strcmp(buf, "DSSF3") != 0) {
		MessageBoxID(NULL, IDS_ERR_OPEN_IMPORT_FILE, MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}

	ReadHeader();
	if (m_oRecordHeader.nType != RECOED_TYPE_FOLDER && (nID & ID_KIND) != ID_FOLDER) {
		MessageBoxID(NULL, IDS_MSG_SELECT_IMPORT_FOLDER, MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}

	Read(nID & ID_VALUE);

	m_oFile.Close();

	return m_nImportID;
}

BOOL CImportExport::ReadHeader()
{
	if (m_oFile.Read(&m_oRecordHeader, sizeof(m_oRecordHeader)) != sizeof(m_oRecordHeader)) {
		m_oRecordHeader.nType = RECOED_TYPE_EOF;
		m_oRecordHeader.nSize = 0;
		return FALSE;
	}

	m_bReadFlag = TRUE;

	return TRUE;
}

void CImportExport::ReadRecord(void *pData)
{
	m_oFile.Read(pData, m_oRecordHeader.nSize);
	ReadHeader();
}

void CImportExport::Read(long nFolderID)
{
	while (m_oRecordHeader.nType != RECOED_TYPE_EOF && m_bReadFlag) {
		m_bReadFlag = FALSE;

		switch (m_oRecordHeader.nType) {
		case RECOED_TYPE_FOLDER:
			nFolderID = ReadFolder();
			break;
		case RECOED_TYPE_IR:
			ReadIR(nFolderID);
			break;
		case RECOED_TYPE_ACF:
			ReadACF(nFolderID);
			break;
		case RECOED_TYPE_NMS:
			ReadNMS(nFolderID);
			break;
		default:
			return;
		}
	}
}

long CImportExport::ReadFolder()
{
	long nFolderID = 0;
	DbImpulseBuf oDbImpulseBuf;
	ReadRecord(&oDbImpulseBuf);

	CDbFolder oDbFolder;
	DbFolderRec oDbFolderRec;
	if (oDbFolder.Open()) {
		oDbFolder.DBGetFieldBuf(&oDbImpulseBuf, &oDbFolderRec);
		oDbFolder.GetNewID(&oDbFolderRec.nFolderID);

		CString str;
		str.Format("TYPE=%c", oDbFolderRec.sType[0]);
		oDbFolder.DBSetFilter(str);
		DbFolderRec oDbFolderRec2;
		int nIndex = 0;
		while (oDbFolder.ReadRecNext(&oDbFolderRec2)) {
			GetMaxIndex(nIndex, oDbFolderRec.sTitle, oDbFolderRec2.sTitle);
		}
		if (nIndex != 0) {
			str.Format("%s(%d)", (LPCSTR)oDbFolderRec.sTitle, nIndex);
			oDbFolderRec.sTitle = str;
		}

		oDbFolder.StoreRec(&oDbFolderRec);
		nFolderID = oDbFolderRec.nFolderID;

		if (m_nImportID == 0)
			m_nImportID = oDbFolderRec.nFolderID | ID_FOLDER;

		m_bAddIndex = FALSE;
	}

	return nFolderID;
}

void CImportExport::ReadIR(long nFolderID)
{
	CDbFolder oDbFolder;
	DbFolderRec oDbFolderRec;
	if (oDbFolder.Open()) {
		if (!oDbFolder.ReadRecID(nFolderID, &oDbFolderRec))
			return;
	} else
		return;

	if (oDbFolderRec.sType != FOLDER_TYPE_IR)
		return;

	DbImpulseBuf oDbImpulseBuf;
	ReadRecord(&oDbImpulseBuf);

	CDbImpulse oDbImpulse;
	if (oDbImpulse.Open()) {
		DbImpulseRec oDbImpulseRec;
		oDbImpulse.DBGetFieldBuf(&oDbImpulseBuf, &oDbImpulseRec);

		if (oDbImpulseRec.nMeasTime >= OFFSET_MEASTIME) {
			oDbImpulseRec.nMeasTime -= OFFSET_MEASTIME;
			oDbImpulseRec.bImpulseData = TRUE;
		} else
			oDbImpulseRec.bImpulseData = FALSE;

		oDbImpulseRec.nFolderID = oDbFolderRec.nFolderID;
		oDbImpulse.GetNewID(&oDbImpulseRec.nImpulseID);

		if (m_bAddIndex) {
			CString str;
			long nRec;
			str.Format("#%d=%ld", oDbImpulse.m_nIdxFolderID, oDbFolderRec.nFolderID);
			oDbImpulse.DBSelect(str, &nRec);
			DbImpulseRec oDbImpulseRec2;
			int nIndex = 0;
			while (oDbImpulse.ReadRecNext(&oDbImpulseRec2)) {
				GetMaxIndex(nIndex, oDbImpulseRec.sTitle, oDbImpulseRec2.sTitle);
			}
			oDbImpulse.DBSelect(NULL, &nRec);
			if (nIndex != 0) {
				str.Format("%s(%d)", (LPCSTR)oDbImpulseRec.sTitle, nIndex);
				oDbImpulseRec.sTitle = str;
			}
		}

		oDbImpulse.StoreRec(&oDbImpulseRec);

		if (m_oRecordHeader.nType == RECOED_TYPE_IR_WAVEDATA) {
			ReadBinary(oDbImpulse, "WAVEDATA");
		}

		if (m_oRecordHeader.nType == RECOED_TYPE_ACPARAM) {
			DbAcParamBuf oDbAcParamBuf;
			ReadRecord(&oDbAcParamBuf);

			CDbAcParam oDbAcParam;
			if (oDbAcParam.Open()) {
				DbAcParamRec oDbAcParamRec;
				oDbAcParam.DBGetFieldBuf(&oDbAcParamBuf, &oDbAcParamRec);
				oDbAcParamRec.nImpulseID = oDbImpulseRec.nImpulseID;
				oDbAcParam.StoreRec(&oDbAcParamRec);

				if (m_oRecordHeader.nType == RECOED_TYPE_ACPARAM_CONDITION) {
					ReadBinary(oDbAcParam, "CONDITION");
				}

				if (m_oRecordHeader.nType == RECOED_TYPE_ACPARAM_RESULT) {
					ReadBinary(oDbAcParam, "RESULT");
				}

				if (m_oRecordHeader.nType == RECOED_TYPE_ACPARAM_DATA) {
					ReadBinary(oDbAcParam, "DATA");
				}

				if (m_nImportID == 0)
					m_nImportID = oDbImpulseRec.nImpulseID | ID_DATA;
			}
		}
	}
}

void CImportExport::ReadACF(long nFolderID)
{
	CDbFolder oDbFolder;
	DbFolderRec oDbFolderRec;
	if (oDbFolder.Open()) {
		if (!oDbFolder.ReadRecID(nFolderID, &oDbFolderRec))
			return;
	} else
		return;

	if (oDbFolderRec.sType != FOLDER_TYPE_ACF)
		return;

	DbAcfBuf oDbAcfBuf;
	ReadRecord(&oDbAcfBuf);

	CDbAcf oDbAcf;
	if (oDbAcf.Open()) {
		DbAcfRec oDbAcfRec;
		oDbAcf.DBGetFieldBuf(&oDbAcfBuf, &oDbAcfRec);

		oDbAcfRec.nFolderID = oDbFolderRec.nFolderID;
		oDbAcf.GetNewID(&oDbAcfRec.nAcfID);

		if (m_bAddIndex) {
			CString str;
			long nRec;
			str.Format("#%d=%ld", oDbAcf.m_nIdxFolderID, oDbFolderRec.nFolderID);
			oDbAcf.DBSelect(str, &nRec);
			DbAcfRec oDbAcfRec2;
			int nIndex = 0;
			while (oDbAcf.ReadRecNext(&oDbAcfRec2)) {
				GetMaxIndex(nIndex, oDbAcfRec.sTitle, oDbAcfRec2.sTitle);
			}
			oDbAcf.DBSelect(NULL, &nRec);
			if (nIndex != 0) {
				str.Format("%s(%d)", (LPCSTR)oDbAcfRec.sTitle, nIndex);
				oDbAcfRec.sTitle = str;
			}
		}

		oDbAcf.StoreRec(&oDbAcfRec);

		if (m_oRecordHeader.nType == RECOED_TYPE_ACF_WAVEDATA) {
			ReadBinary(oDbAcf, "WAVEDATA");
		}

		if (m_oRecordHeader.nType == RECOED_TYPE_ACFFACTOR) {
			DbAcfFactorBuf oDbAcfFactorBuf;
			ReadRecord(&oDbAcfFactorBuf);

			CDbAcfFactor oDbAcfFactor;
			if (oDbAcfFactor.Open()) {
				DbAcfFactorRec oDbAcfFactorRec;
				oDbAcfFactor.DBGetFieldBuf(&oDbAcfFactorBuf, &oDbAcfFactorRec);
				oDbAcfFactorRec.nAcfID = oDbAcfRec.nAcfID;
				oDbAcfFactor.StoreRec(&oDbAcfFactorRec);

				if (m_oRecordHeader.nType == RECOED_TYPE_ACFFACTOR_ACFCOND2) {
					ReadBinary(oDbAcfFactor, "ACFCOND2");
				}

				if (m_oRecordHeader.nType == RECOED_TYPE_ACFFACTOR_ACFFACTOR) {
					ReadBinary(oDbAcfFactor, "ACFFACTOR");
				}

				if (m_nImportID == 0)
					m_nImportID = oDbAcfRec.nAcfID | ID_DATA;
			}
		}
	}
}

void CImportExport::ReadNMS(long nFolderID)
{
	CDbFolder oDbFolder;
	DbFolderRec oDbFolderRec;
	if (oDbFolder.Open()) {
		if (!oDbFolder.ReadRecID(nFolderID, &oDbFolderRec))
			return;
	} else
		return;

	if (oDbFolderRec.sType != FOLDER_TYPE_NMS)
		return;

	DbNmsBuf oDbNmsBuf;
	ReadRecord(&oDbNmsBuf);

	CDbNms oDbNms;
	if (oDbNms.Open()) {
		DbNmsRec oDbNmsRec;
		oDbNms.DBGetFieldBuf(&oDbNmsBuf, &oDbNmsRec);

		oDbNmsRec.nFolderID = oDbFolderRec.nFolderID;
		oDbNms.GetNewID(&oDbNmsRec.nNmsID);
		oDbNms.StoreRec(&oDbNmsRec);

		if (m_oRecordHeader.nType == RECOED_TYPE_NMS_WAVEDATA) {
			ReadBinary(oDbNms, "WAVEDATA");
		}

		if (m_oRecordHeader.nType == RECOED_TYPE_NMS_NMSCOND) {
			ReadBinary(oDbNms, "NMSCOND");
		}

		if (m_oRecordHeader.nType == RECOED_TYPE_NMS_NMSFACTOR) {
			ReadBinary(oDbNms, "NMSFACTOR");
		}

		if (m_oRecordHeader.nType == RECOED_TYPE_NMS_NOISESRC) {
			ReadBinary(oDbNms, "NOISESRC");
		}
	}
}

void CImportExport::ReadBinary(CPSDB &db, LPCSTR pFieldName)
{
	const int nSize = m_oRecordHeader.nSize;
	BYTE *pBuf = new BYTE[nSize];
	ReadRecord(pBuf);
	db.DBWriteBinary(pFieldName, pBuf, nSize);
	delete [] pBuf;
}

void CImportExport::GetMaxIndex(int &nIndex, const CString &sTitle1, const CString &sTitle2)
{
	int nIndex2 = 0;

	if (sTitle1 == sTitle2) {
		nIndex2 = 1;
	} else {
		const int len = sTitle1.GetLength();
		if (sTitle1 == sTitle2.Left(len)) {
			if (sTitle2.Mid(len, 1) == "(" && sTitle2.Right(1) == ")") {
				const int len2 = sTitle2.GetLength();
				nIndex2 = atoi(sTitle2.Mid(len + 1, len2 - len - 2));
				if (nIndex2 != 0)
					nIndex2++;
			}
		}
	}

	if (nIndex2 > nIndex)
		nIndex = nIndex2;
}
