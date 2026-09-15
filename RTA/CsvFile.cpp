#include "stdafx.h"
#include "csvfile.h"

CCsvFile::CCsvFile()
{
	m_sSeparator = ",";
}

void CCsvFile::SetData(LPCTSTR pData)
{
	m_aCsvData.Add(pData);
}

void CCsvFile::SetData(double fData, LPCTSTR pFormat)
{
	CString sData;
	sData.Format(pFormat, fData);
	m_aCsvData.Add(sData);
}

void CCsvFile::SetData(int nData, LPCTSTR pFormat)
{
	CString sData;
	sData.Format(pFormat, nData);
	m_aCsvData.Add(sData);
}

void CCsvFile::Output()
{
	int nSize = (int)m_aCsvData.GetSize();
	for (int i = 0; i < nSize; i++) {
		if (i != 0)
			WriteString(m_sSeparator);
		WriteString(m_aCsvData[i]);
	}
	WriteString("\n");

	m_aCsvData.RemoveAll();
}

BOOL CCsvFile::Input()
{
	CString sBuf;

	if (!ReadString(sBuf)) {
		return false;
	}

	sBuf.ReleaseBuffer();
	sBuf.TrimRight();

	CString sToken;
	int nCurPos = 0;
	m_aCsvData.RemoveAll();
	while (true) {
		sToken = sBuf.Tokenize(m_sSeparator, nCurPos);
		if (sToken == "") {
			break;
		}
		m_aCsvData.Add(sToken);
	}

	return true;
}

CString CCsvFile::GetString(int nColumn)
{
	if (nColumn >= m_aCsvData.GetCount())
		return "";

	return m_aCsvData[nColumn];
}

double CCsvFile::GetDouble(int nColumn)
{
	if (nColumn >= m_aCsvData.GetCount())
		return 0;

	CString str = m_aCsvData[nColumn];
	double val = atof(str);
	if (str.Right(1) == "k") {
		val *= 1000;
	}

	return val;
}

int CCsvFile::GetInt(int nColumn)
{
	if (nColumn >= m_aCsvData.GetCount())
		return 0;

	CString str = m_aCsvData[nColumn];
	int val = atoi(str);
	if (str.Right(1) == "k") {
		val *= 1000;
	}

	return val;
}
