#pragma once

#include "FileIO.h"

#define STATUS_INIT			0
#define STATUS_WAIT_CALC	1
#define STATUS_IN_CALC		2
#define STATUS_FINISH_CALC	3

class CNmsItem : public CObject
{
public:
	CNmsItem();
	virtual ~CNmsItem();

	int m_nIndex;
	BOOL m_bSaved;
	CTime m_cTime;
	int m_nStatus;
	int m_nDecision;
	double m_fDataTime;
	int m_nNmsFactorData;
	NmsConditionData m_NmsConditionData;
	NmsFactorData *m_pNmsFactorData;
	NoiseSrcData m_NoiseSrcData;
	int m_nNoiseTmpID;
	BOOL m_bLearning;

	void LearningNoiseSrc();
	void UnLearningNoiseSrc();
};
