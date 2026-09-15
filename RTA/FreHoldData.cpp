#include "StdAfx.h"
#include "FreHoldData.h"

CFreHoldData::CFreHoldData()
{
	m_pLeftData = NULL;
	m_pRightData = NULL;
	m_pFreq = NULL;
}

void CFreHoldData::Set(COLORREF colorLeft, COLORREF colorRight, int nFreqPoint, int nFreqCount,
		int nLeftDataSize, int nRightDataSize, int nFreqSize,
		double *pLeftData, double *pRightData, double *pFreq)
{
	m_colorLeft = colorLeft;
	m_colorRight = colorRight;
	m_nFreqPoint = nFreqPoint;
	m_nFreqCount = nFreqCount;
	m_nLeftDataSize = pLeftData != NULL ? nLeftDataSize : 0;
	m_nRightDataSize = pRightData != NULL ? nRightDataSize : 0;
	m_nFreqSize = pFreq != NULL ? nFreqSize : 0;
	m_pLeftData = pLeftData;
	m_pRightData = pRightData;
	m_pFreq = pFreq;

	if (m_nLeftDataSize != 0) {
		m_pLeftData = new double[m_nLeftDataSize];
		memcpy(m_pLeftData, pLeftData, sizeof(double) * m_nLeftDataSize);
	} else {
		m_pLeftData = NULL;
	}

	if (m_nRightDataSize != 0) {
		m_pRightData = new double[m_nRightDataSize];
		memcpy(m_pRightData, pRightData, sizeof(double) * m_nRightDataSize);
	} else {
		m_pRightData = NULL;
	}

	if (m_nFreqSize != 0) {
		m_pFreq = new double[m_nFreqSize];
		memcpy(m_pFreq, pFreq, sizeof(double) * m_nFreqSize);
	} else {
		m_pFreq = NULL;
	}
}

void CFreHoldData::FreeBuffers()
{
	if (m_pLeftData != NULL) {
		delete [] m_pLeftData;
		m_pLeftData = NULL;
	}

	if (m_pRightData != NULL) {
		delete [] m_pRightData;
		m_pLeftData = NULL;
	}

	if (m_pFreq != NULL) {
		delete [] m_pFreq;
		m_pLeftData = NULL;
	}
}

void CFreHoldData::Save(CFile &oFile)
{
	oFile.Write(&m_colorLeft, sizeof(m_colorLeft));
	oFile.Write(&m_colorRight, sizeof(m_colorRight));
	oFile.Write(&m_nFreqPoint, sizeof(m_nFreqPoint));
	oFile.Write(&m_nFreqCount, sizeof(m_nFreqCount));
	oFile.Write(&m_nLeftDataSize, sizeof(m_nLeftDataSize));
	oFile.Write(&m_nRightDataSize, sizeof(m_nRightDataSize));
	oFile.Write(&m_nFreqSize, sizeof(m_nFreqSize));
	oFile.Write(m_pLeftData, sizeof(double) * m_nLeftDataSize);
	oFile.Write(m_pRightData, sizeof(double) * m_nRightDataSize);
	oFile.Write(m_pFreq, sizeof(double) * m_nFreqSize);
}

void CFreHoldData::Load(CFile &oFile)
{
	oFile.Read(&m_colorLeft, sizeof(m_colorLeft));
	oFile.Read(&m_colorRight, sizeof(m_colorRight));
	oFile.Read(&m_nFreqPoint, sizeof(m_nFreqPoint));
	oFile.Read(&m_nFreqCount, sizeof(m_nFreqCount));
	oFile.Read(&m_nLeftDataSize, sizeof(m_nLeftDataSize));
	oFile.Read(&m_nRightDataSize, sizeof(m_nRightDataSize));
	oFile.Read(&m_nFreqSize, sizeof(m_nFreqSize));

	if (m_nLeftDataSize != 0) {
		m_pLeftData = new double[m_nLeftDataSize];
		oFile.Read(m_pLeftData, sizeof(double) * m_nLeftDataSize);
	} else {
		m_pLeftData = NULL;
	}

	if (m_nRightDataSize != 0) {
		m_pRightData = new double[m_nRightDataSize];
		oFile.Read(m_pRightData, sizeof(double) * m_nRightDataSize);
	} else {
		m_pRightData = NULL;
	}

	if (m_nFreqSize != 0) {
		m_pFreq = new double[m_nFreqSize];
		oFile.Read(m_pFreq, sizeof(double) * m_nFreqSize);
	} else {
		m_pFreq = NULL;
	}
}
