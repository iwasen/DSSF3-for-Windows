#pragma once

class CNoise
{
public:
	CNoise();

	void GenerateWhiteNoise(double *pData, int nStep, int nSize, double fLevel);
	void GeneratePinkNoise(double *pData, int nStep, int nSize, double fLevel);
	void GenerateBrownNoise(double *pData, int nStep, int nSize, double fLevel);

protected:
	double m_aPinkBuf[7];
	double m_fBrownBuf;

	double GeneratePinkNoiseSub();
};
