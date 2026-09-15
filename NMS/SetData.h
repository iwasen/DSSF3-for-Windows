#pragma once

#include "FileIO.h"

class CSetData
{
public:
	CSetData();
	virtual ~CSetData();

	struct {
		int left;
		int top;
		int width;
		int height;
	} MainWindow;

	struct {
		int left;
		int top;
		BOOL bShow;
	} SplWindow;

	struct {
		int left;
		int top;
		BOOL bShow;
	} PeakLevelWindow;

	struct {
		int left;
		int top;
	} FactorWindow;

	struct SMeasurement {
		int nSamplingRate;
		double fSamplingLevel;
		BOOL bRelativeLevel;
		double fTimeConstant;
		int nSamplingTiming;
		double fSamplingTime1;
		double fSamplingTime2;
		AcfCondition AcfCondition;
		IAcfCondition IAcfCondition;
		IdentCondition IdentCondition;
		long nMicCalID;
		BOOL bAutoLearning;
		BOOL bStartDateTime;
		CTime tStartDateTime;
		BOOL bEndDateTime;
		CTime tEndDateTime;
	} Measurement;

	struct {
		int nInputDevice;
		int nInputSelect;
	} InputDevice;

	struct {
		int left;
		int top;
		double fSamplingTime;
	} ManualWindow;

	struct {
		BOOL bAutoCalc;
	} Calc;
};
