#pragma once

class CImportExport
{
public:
	CImportExport(void);

	void Export(const CString &sFilePath, long *pIDs, int nItem, int nFolderType);
	long Import(const CString &sFilePath, long nID);

protected:
#pragma pack(push, 1)
	struct RecordHeader {
		int nType;
		int nSize;
	};
#pragma pack(pop)
	CFile m_oFile;
	RecordHeader m_oRecordHeader;
	long m_nImportID;
	BOOL m_bReadFlag;
	BOOL m_bAddIndex;

	void Write(long nID, int nFolderType);
	void WriteFolders(char nFolderType);
	void WriteFolder(long nID);
	void WriteData(long nID, int nFolderType);
	void WriteDataIR(long nID);
	void WriteDataACF(long nID);
	void WriteDataNMS(long nID);
	void WriteRecord(int nRecordType, void *pData, int nSize);
	void WriteBinary(CPSDB &db, LPCSTR pFieldName, int nRecordType);

	BOOL ReadHeader();
	void ReadRecord(void *pData);
	void Read(long nFolderID);
	long ReadFolder();
	void ReadIR(long nFolderID);
	void ReadACF(long nFolderID);
	void ReadNMS(long nFolderID);
	void ReadBinary(CPSDB &db, LPCSTR pFieldName);
	void GetMaxIndex(int &nIndex, const CString &sTitle1, const CString &sTitle2);
};
