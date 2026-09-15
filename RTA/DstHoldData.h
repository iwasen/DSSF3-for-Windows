#pragma once

class CDstHoldData
{
public:
	CDstHoldData();

	COLORREF m_colorLeft;
	COLORREF m_colorRight;
	INT32 m_nPoint;
	INT32 m_nCount;
	INT32 m_bRightData[3];
	double *m_pLeftData[3];
	double *m_pRightData[3];
	double *m_pValue;

	void Set(COLORREF colorLeft, COLORREF colorRight, int nPoint, int nCount,
			double *(pLeftData[3]), double *(pRightData[3]), double *pValue);
	void FreeBuffers();
	void Save(CFile &oFile);
	void Load(CFile &oFile);
};
