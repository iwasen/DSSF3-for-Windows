#pragma once

#define FILTER_F	0
#define FILTER_A	1
#define FILTER_B	2
#define FILTER_C	3

struct FilterData {
	double fFreq;
	double fLevel;
};

extern BOOL MakeFilterTbl2(double *pFilterTbl, int nData, double fRate, int nFilterType, int nLog);
extern void MakeFilterTbl3(double *pFilterTbl, int nData, double fRate, const FilterData *pFilterData, int nFilterData, int nLog);
