#pragma once

class CIRMethod
{
public:
	virtual ~CIRMethod();

	virtual void InitMethod(int nStage) = 0;
	virtual void GenerateSequence(double *pData, int nAtt) = 0;
	virtual void CalcImpulse(double *pData) = 0;

protected:
	void PolarityCheck(int nData, double *pData);
};
