#include "StdAfx.h"
#include "DstHoldData.h"

CDstHoldData::CDstHoldData()
{
	for (int i = 0; i < 3; i++) {
		m_pLeftData[i] = NULL;
		m_pRightData[i] = NULL;
	}
	m_pValue = NULL;
}

void CDstHoldData::Set(COLORREF colorLeft, COLORREF colorRight, int nPoint, int nCount,
			double *(pLeftData[3]), double *(pRightData[3]), double *pValue)
{
	m_colorLeft = colorLeft;
	m_colorRight = colorRight;
	m_nPoint = nPoint;
	m_nCount = nCount;

	for (int i = 0; i < 3; i++) {
		m_pLeftData[i] = new double[m_nPoint];
		memcpy(m_pLeftData[i], pLeftData[i], sizeof(double) * m_nPoint);

		if (pRightData[i] != NULL) {
			m_pRightData[i] = new double[m_nPoint];
			memcpy(m_pRightData[i], pRightData[i], sizeof(double) * m_nPoint);
			m_bRightData[i] = TRUE;
		} else {
			m_pRightData[i] = NULL;
			m_bRightData[i] = FALSE;
		}
	}

	if (m_pValue != NULL) {
		m_pValue = new double[m_nPoint];
		memcpy(m_pValue, pValue, sizeof(double) * m_nPoint);
	}
}

void CDstHoldData::FreeBuffers()
{
	for (int i = 0; i < 3; i++) {
		if (m_pLeftData[i] != NULL) {
			delete [] m_pLeftData[i];
			m_pLeftData[i] = NULL;
		}

		if (m_pRightData[i] != NULL) {
			delete [] m_pRightData[i];
			m_pRightData[i] = NULL;
		}
	}

	if (m_pValue != NULL) {
		delete [] m_pValue;
		m_pValue = NULL;
	}
}

void CDstHoldData::Save(CFile &oFile) const
{
	oFile.Write(&m_colorLeft, sizeof(m_colorLeft));
	oFile.Write(&m_colorRight, sizeof(m_colorRight));
	oFile.Write(&m_nPoint, sizeof(m_nPoint));
	oFile.Write(&m_nCount, sizeof(m_nCount));
	oFile.Write(&m_bRightData, sizeof(m_bRightData));

	for (int i = 0; i < 3; i++) {
		oFile.Write(m_pLeftData[i], sizeof(double) * m_nPoint);
	}

	for (int i = 0; i < 3; i++) {
		if (m_bRightData[i]) {
			oFile.Write(m_pRightData[i], sizeof(double) * m_nPoint);
		}
	}

	oFile.Write(m_pValue, sizeof(double) * m_nPoint);
}

void CDstHoldData::Load(CFile &oFile)
{
	oFile.Read(&m_colorLeft, sizeof(m_colorLeft));
	oFile.Read(&m_colorRight, sizeof(m_colorRight));
	oFile.Read(&m_nPoint, sizeof(m_nPoint));
	oFile.Read(&m_nCount, sizeof(m_nCount));
	oFile.Read(&m_bRightData, sizeof(m_bRightData));

	for (int i = 0; i < 3; i++) {
		if (m_nPoint != 0) {
			m_pLeftData[i] = new double[m_nPoint];
			oFile.Read(m_pLeftData[i], sizeof(double) * m_nPoint);
		} else {
			m_pLeftData[i] = NULL;
		}
	}

	for (int i = 0; i < 3; i++) {
		if (m_nPoint != 0 && m_bRightData[i]) {
			m_pRightData[i] = new double[m_nPoint];
			oFile.Read(m_pRightData[i], sizeof(double) * m_nPoint);
		} else {
			m_pRightData[i] = NULL;
		}
	}

	if (m_nPoint != 0) {
		m_pValue = new double[m_nPoint];
		oFile.Read(m_pValue, sizeof(double) * m_nPoint);
	} else {
		m_pValue = NULL;
	}
}
