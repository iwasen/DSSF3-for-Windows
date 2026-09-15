#pragma once

class CCsvFile : public CStdioFile
{
public:
	CString m_sSeparator;

	CCsvFile();
	void SetData(LPCTSTR pData);
	void SetData(int nData, LPCTSTR pFormat = "%d");
	void SetData(double fData, LPCTSTR pFormat = "%.5g");
	void Output();
	BOOL Input();
	CString GetString(int nColumn);
	double GetDouble(int nColumn);
	int GetInt(int nColumn);

protected:
	CStringArray m_aCsvData;
};
