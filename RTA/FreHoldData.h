#pragma once

class CFreHoldData
{
public:
	CFreHoldData();

	COLORREF m_colorLeft;
	COLORREF m_colorRight;
	INT32 m_nFreqPoint;
	INT32 m_nFreqCount;
	INT32 m_nLeftDataSize;
	INT32 m_nRightDataSize;
	INT32 m_nFreqSize;
	double *m_pLeftData;
	double *m_pRightData;
	double *m_pFreq;

	void Set(COLORREF colorLeft, COLORREF colorRight, int nFreqPoint, int nFreqCount,
			int nLeftDataSize, int nRightDataSize, int nFreqSize,
			double *pLeftData, double *pRightData, double *pFreq);
	void FreeBuffers();
	void Save(CFile &oFile);
	void Load(CFile &oFile);
};
