#pragma once

class CAverageBuf
{
public:
	CAverageBuf();
	~CAverageBuf();

	void Alloc(int nSize, int nAverageNum);
	void Smoothing(const double *pBuf, double m1, double m2);
	void Averaging();
	void Reset();
	double *GetBuf() { return m_pDataBuf; }
	int GetBufSize() { return m_nSize; }

protected:
	double *m_pDataBuf;
	double *m_pAverageBuf;
	double *m_pRingBuf;
	int m_nSize;
	int m_nAverageNum;
	int m_nAveragePos;
	int m_nAverageCnt;

	void Free();
};
