#include "StdAfx.h"
#include "Rta.h"
#include "AverageBuf.h"

CAverageBuf::CAverageBuf()
{
	m_pDataBuf = NULL;
	m_pAverageBuf = NULL;
	m_pRingBuf = NULL;
	m_nSize = 0;
	m_nAverageNum = 0;
}

CAverageBuf::~CAverageBuf()
{
	Free();
}

void CAverageBuf::Alloc(int nSize, int nAverageNum)
{
	Free();

	m_pDataBuf = new double[nSize];

	if (nAverageNum != 0)
		m_pAverageBuf = new double[nSize];

	if (nAverageNum > 0)
		m_pRingBuf = new double[nSize * nAverageNum];

	m_nSize = nSize;
	m_nAverageNum = nAverageNum;

	Reset();
}

void CAverageBuf::Smoothing(const double *pBuf, double m1, double m2)
{
	m_pDataBuf[0] = 0;
	for (int i = 1; i < m_nSize; i++)
		m_pDataBuf[i] = pBuf[i] * m1 + m_pDataBuf[i] * m2;
}

void CAverageBuf::Averaging()
{
	int i;

	if (m_nAverageNum != 0) {
		if (m_nAverageNum > 0) {
			double *pRingBuf = &m_pRingBuf[m_nSize * m_nAveragePos];

			if (m_nAverageCnt >= m_nAverageNum) {
				for (i = 0; i < m_nSize; i++) {
					m_pAverageBuf[i] -= pRingBuf[i];
				}
			}

			memcpy(pRingBuf, m_pDataBuf, sizeof(double) * m_nSize);
		}

		if (m_nAverageNum > 0) {
			if (m_nAverageCnt < m_nAverageNum)
				m_nAverageCnt++;

			if (++m_nAveragePos >= m_nAverageNum)
				m_nAveragePos = 0;
		} else if (m_nAverageNum == -1)
			m_nAverageCnt++;

		for (i = 0; i < m_nSize; i++) {
			m_pAverageBuf[i] += m_pDataBuf[i];
			m_pDataBuf[i] = m_pAverageBuf[i] / m_nAverageCnt;
		}
	}
}

void CAverageBuf::Reset()
{
	m_nAveragePos = 0;
	m_nAverageCnt = 0;

	if (m_pDataBuf != NULL)
		MEMCLEAR(m_pDataBuf, m_nSize);

	if (m_pAverageBuf != NULL)
		MEMCLEAR(m_pAverageBuf, m_nSize);
}

void CAverageBuf::Free()
{
	if (m_pDataBuf != NULL) {
		delete [] m_pDataBuf;
		m_pDataBuf = NULL;
	}

	if (m_pAverageBuf != NULL) {
		delete [] m_pAverageBuf;
		m_pAverageBuf = NULL;
	}

	if (m_pRingBuf != NULL) {
		delete [] m_pRingBuf;
		m_pRingBuf = NULL;
	}
}
