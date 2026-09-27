// Filter.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "MakeFilter.h"
#include "Common.h"
#include "Spline.h"

static const FilterData AFilter[] = {
	{pow(10.0, 1.0), -70.4}, {pow(10.0, 1.1), -63.4}, {pow(10.0, 1.2), -56.7}, {pow(10.0, 1.3), -50.5}, {pow(10.0, 1.4), -44.7},
	{pow(10.0, 1.5), -39.4}, {pow(10.0, 1.6), -34.6}, {pow(10.0, 1.7), -30.2}, {pow(10.0, 1.8), -26.2}, {pow(10.0, 1.9), -22.5},
	{pow(10.0, 2.0), -19.1}, {pow(10.0, 2.1), -16.1}, {pow(10.0, 2.2), -13.4}, {pow(10.0, 2.3), -10.9}, {pow(10.0, 2.4),  -8.6},
	{pow(10.0, 2.5),  -6.6}, {pow(10.0, 2.6),  -4.8}, {pow(10.0, 2.7),  -3.2}, {pow(10.0, 2.8),  -1.9}, {pow(10.0, 2.9),  -0.8},
	{pow(10.0, 3.0),   0.0}, {pow(10.0, 3.1),   0.6}, {pow(10.0, 3.2),   1.0}, {pow(10.0, 3.3),   1.2}, {pow(10.0, 3.4),   1.3},
	{pow(10.0, 3.5),   1.2}, {pow(10.0, 3.6),   1.0}, {pow(10.0, 3.7),   0.5}, {pow(10.0, 3.8),  -0.1}, {pow(10.0, 3.9),  -1.1},
	{pow(10.0, 4.0),  -2.5}, {pow(10.0, 4.1),  -4.3}, {pow(10.0, 4.2),  -6.6}, {pow(10.0, 4.3),  -9.3}
};

static const FilterData BFilter[] = {
	{pow(10.0, 1.0), -38.2}, {pow(10.0, 1.1), -33.2}, {pow(10.0, 1.2), -28.5}, {pow(10.0, 1.3), -24.2}, {pow(10.0, 1.4), -20.4},
	{pow(10.0, 1.5), -17.1}, {pow(10.0, 1.6), -14.2}, {pow(10.0, 1.7), -11.6}, {pow(10.0, 1.8),  -9.3}, {pow(10.0, 1.9),  -7.4},
	{pow(10.0, 2.0),  -5.6}, {pow(10.0, 2.1),  -4.2}, {pow(10.0, 2.2),  -3.0}, {pow(10.0, 2.3),  -2.0}, {pow(10.0, 2.4),  -1.3},
	{pow(10.0, 2.5),  -0.8}, {pow(10.0, 2.6),  -0.5}, {pow(10.0, 2.7),  -0.3}, {pow(10.0, 2.8),  -0.1}, {pow(10.0, 2.9),   0.0},
	{pow(10.0, 3.0),   0.0}, {pow(10.0, 3.1),   0.0}, {pow(10.0, 3.2),   0.0}, {pow(10.0, 3.3),  -0.1}, {pow(10.0, 3.4),  -0.2},
	{pow(10.0, 3.5),  -0.4}, {pow(10.0, 3.6),  -0.7}, {pow(10.0, 3.7),  -1.2}, {pow(10.0, 3.8),  -1.9}, {pow(10.0, 3.9),  -2.9},
	{pow(10.0, 4.0),  -4.3}, {pow(10.0, 4.1),  -6.1}, {pow(10.0, 4.2),  -8.4}, {pow(10.0, 4.3), -11.1}
};

static const FilterData CFilter[] = {
	{pow(10.0, 1.0), -14.3}, {pow(10.0, 1.1), -11.2}, {pow(10.0, 1.2),  -8.5}, {pow(10.0, 1.3),  -6.2}, {pow(10.0, 1.4),  -4.4},
	{pow(10.0, 1.5),  -3.0}, {pow(10.0, 1.6),  -2.0}, {pow(10.0, 1.7),  -1.3}, {pow(10.0, 1.8),  -0.8}, {pow(10.0, 1.9),  -0.5},
	{pow(10.0, 2.0),  -0.3}, {pow(10.0, 2.1),  -0.2}, {pow(10.0, 2.2),  -0.1}, {pow(10.0, 2.3),   0.0}, {pow(10.0, 2.4),   0.0},
	{pow(10.0, 2.5),   0.0}, {pow(10.0, 2.6),   0.0}, {pow(10.0, 2.7),   0.0}, {pow(10.0, 2.8),   0.0}, {pow(10.0, 2.9),   0.0},
	{pow(10.0, 3.0),   0.0}, {pow(10.0, 3.1),   0.0}, {pow(10.0, 3.2),  -0.1}, {pow(10.0, 3.3),  -0.2}, {pow(10.0, 3.4),  -0.3},
	{pow(10.0, 3.5),  -0.5}, {pow(10.0, 3.6),  -0.8}, {pow(10.0, 3.7),  -1.3}, {pow(10.0, 3.8),  -2.0}, {pow(10.0, 3.9),  -3.0},
	{pow(10.0, 4.0),  -4.4}, {pow(10.0, 4.1),  -6.2}, {pow(10.0, 4.2),  -8.5}, {pow(10.0, 4.3), -11.2}
};

BOOL MakeFilterTbl2(double *pFilterTbl, int nData, double fRate, int nFilterType, int nLog)
{
	const FilterData *pFilterData;
	int nFilterData;

	switch (nFilterType) {
	case FILTER_F:
		pFilterData = NULL;
		nFilterData = 0;
		break;
	case FILTER_A:
		pFilterData = AFilter;
		nFilterData = ARRAY_SIZE(AFilter);
		break;
	case FILTER_B:
		pFilterData = BFilter;
		nFilterData = ARRAY_SIZE(BFilter);
		break;
	case FILTER_C:
		pFilterData = CFilter;
		nFilterData = ARRAY_SIZE(CFilter);
		break;
	default:
		return FALSE;
	}

	MakeFilterTbl3(pFilterTbl, nData, fRate, pFilterData, nFilterData, nLog);

	return TRUE;
}

void MakeFilterTbl3(double *pFilterTbl, int nData, double fRate, const FilterData *pFilterData, int nFilterData, int nLog)
{
	int i;

	if (nFilterData < 3) {
		pFilterTbl[0] = 0;
		for (i = 1; i < nData; i++)
			pFilterTbl[i] = 1.0;
		return;
	}

	CSpline spline;
	double *pFreq = new double[nFilterData];
	double *pLevel = new double[nFilterData];

	for (i = 0; i < nFilterData; i++) {
		pFreq[i] = log(pFilterData->fFreq);
		pLevel[i] = pFilterData->fLevel;
		pFilterData++;
	}

	spline.MakeTable(pFreq, pLevel, nFilterData);

	pFilterTbl[0] = 0;
	for (i = 1; i <= nData / 2; i++)
		pFilterTbl[i] = pFilterTbl[nData - i] = pow(10.0, spline.Spline(log((double)i / nData * fRate)) / nLog);

	delete [] pFreq;
	delete [] pLevel;
}
