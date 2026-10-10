// Filter.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "MakeFilter.h"
#include "Common.h"
#include "Spline.h"

#define FREQ_10	10.0000000000000
#define FREQ_11	12.5892541179417
#define FREQ_12	15.8489319246111
#define FREQ_13	19.9526231496888
#define FREQ_14	25.1188643150958
#define FREQ_15	31.6227766016838
#define FREQ_16	39.8107170553497
#define FREQ_17	50.1187233627272
#define FREQ_18	63.0957344480193
#define FREQ_19	79.4328234724281
#define FREQ_20	100.000000000000
#define FREQ_21	125.892541179416
#define FREQ_22	158.489319246111
#define FREQ_23	199.526231496887
#define FREQ_24	251.188643150958
#define FREQ_25	316.227766016838
#define FREQ_26	398.107170553497
#define FREQ_27	501.187233627272
#define FREQ_28	630.957344480193
#define FREQ_29	794.328234724281
#define FREQ_30	1000.00000000000
#define FREQ_31	1258.92541179416
#define FREQ_32	1584.89319246111
#define FREQ_33	1995.26231496887
#define FREQ_34	2511.88643150957
#define FREQ_35	3162.27766016837
#define FREQ_36	3981.07170553497
#define FREQ_37	5011.87233627272
#define FREQ_38	6309.57344480193
#define FREQ_39	7943.28234724281
#define FREQ_40	10000.0000000000
#define FREQ_41	12589.2541179416
#define FREQ_42	15848.9319246111
#define FREQ_43	19952.6231496887

static constexpr FilterData aAFilter[] = {
	{FREQ_10, -70.4}, {FREQ_11, -63.4}, {FREQ_12, -56.7}, {FREQ_13, -50.5}, {FREQ_14, -44.7},
	{FREQ_15, -39.4}, {FREQ_16, -34.6}, {FREQ_17, -30.2}, {FREQ_18, -26.2}, {FREQ_19, -22.5},
	{FREQ_20, -19.1}, {FREQ_21, -16.1}, {FREQ_22, -13.4}, {FREQ_23, -10.9}, {FREQ_24,  -8.6},
	{FREQ_25,  -6.6}, {FREQ_26,  -4.8}, {FREQ_27,  -3.2}, {FREQ_28,  -1.9}, {FREQ_29,  -0.8},
	{FREQ_30,   0.0}, {FREQ_31,   0.6}, {FREQ_32,   1.0}, {FREQ_33,   1.2}, {FREQ_34,   1.3},
	{FREQ_35,   1.2}, {FREQ_36,   1.0}, {FREQ_37,   0.5}, {FREQ_38,  -0.1}, {FREQ_39,  -1.1},
	{FREQ_40,  -2.5}, {FREQ_41,  -4.3}, {FREQ_42,  -6.6}, {FREQ_43,  -9.3}
};

static constexpr FilterData aBFilter[] = {
	{FREQ_10, -38.2}, {FREQ_11, -33.2}, {FREQ_12, -28.5}, {FREQ_13, -24.2}, {FREQ_14, -20.4},
	{FREQ_15, -17.1}, {FREQ_16, -14.2}, {FREQ_17, -11.6}, {FREQ_18,  -9.3}, {FREQ_19,  -7.4},
	{FREQ_20,  -5.6}, {FREQ_21,  -4.2}, {FREQ_22,  -3.0}, {FREQ_23,  -2.0}, {FREQ_24,  -1.3},
	{FREQ_25,  -0.8}, {FREQ_26,  -0.5}, {FREQ_27,  -0.3}, {FREQ_28,  -0.1}, {FREQ_29,   0.0},
	{FREQ_30,   0.0}, {FREQ_31,   0.0}, {FREQ_32,   0.0}, {FREQ_33,  -0.1}, {FREQ_34,  -0.2},
	{FREQ_35,  -0.4}, {FREQ_36,  -0.7}, {FREQ_37,  -1.2}, {FREQ_38,  -1.9}, {FREQ_39,  -2.9},
	{FREQ_40,  -4.3}, {FREQ_41,  -6.1}, {FREQ_42,  -8.4}, {FREQ_43, -11.1}
};

static constexpr FilterData aCFilter[] = {
	{FREQ_10, -14.3}, {FREQ_11, -11.2}, {FREQ_12,  -8.5}, {FREQ_13,  -6.2}, {FREQ_14,  -4.4},
	{FREQ_15,  -3.0}, {FREQ_16,  -2.0}, {FREQ_17,  -1.3}, {FREQ_18,  -0.8}, {FREQ_19,  -0.5},
	{FREQ_20,  -0.3}, {FREQ_21,  -0.2}, {FREQ_22,  -0.1}, {FREQ_23,   0.0}, {FREQ_24,   0.0},
	{FREQ_25,   0.0}, {FREQ_26,   0.0}, {FREQ_27,   0.0}, {FREQ_28,   0.0}, {FREQ_29,   0.0},
	{FREQ_30,   0.0}, {FREQ_31,   0.0}, {FREQ_32,  -0.1}, {FREQ_33,  -0.2}, {FREQ_34,  -0.3},
	{FREQ_35,  -0.5}, {FREQ_36,  -0.8}, {FREQ_37,  -1.3}, {FREQ_38,  -2.0}, {FREQ_39,  -3.0},
	{FREQ_40,  -4.4}, {FREQ_41,  -6.2}, {FREQ_42,  -8.5}, {FREQ_43, -11.2}
};

/*
static constexpr FilterData aAFilter[] = {
	{pow(10.0, 1.0), -70.4}, {pow(10.0, 1.1), -63.4}, {pow(10.0, 1.2), -56.7}, {pow(10.0, 1.3), -50.5}, {pow(10.0, 1.4), -44.7},
	{pow(10.0, 1.5), -39.4}, {pow(10.0, 1.6), -34.6}, {pow(10.0, 1.7), -30.2}, {pow(10.0, 1.8), -26.2}, {pow(10.0, 1.9), -22.5},
	{pow(10.0, 2.0), -19.1}, {pow(10.0, 2.1), -16.1}, {pow(10.0, 2.2), -13.4}, {pow(10.0, 2.3), -10.9}, {pow(10.0, 2.4),  -8.6},
	{pow(10.0, 2.5),  -6.6}, {pow(10.0, 2.6),  -4.8}, {pow(10.0, 2.7),  -3.2}, {pow(10.0, 2.8),  -1.9}, {pow(10.0, 2.9),  -0.8},
	{pow(10.0, 3.0),   0.0}, {pow(10.0, 3.1),   0.6}, {pow(10.0, 3.2),   1.0}, {pow(10.0, 3.3),   1.2}, {pow(10.0, 3.4),   1.3},
	{pow(10.0, 3.5),   1.2}, {pow(10.0, 3.6),   1.0}, {pow(10.0, 3.7),   0.5}, {pow(10.0, 3.8),  -0.1}, {pow(10.0, 3.9),  -1.1},
	{pow(10.0, 4.0),  -2.5}, {pow(10.0, 4.1),  -4.3}, {pow(10.0, 4.2),  -6.6}, {pow(10.0, 4.3),  -9.3}
};

static constexpr FilterData aBFilter[] = {
	{pow(10.0, 1.0), -38.2}, {pow(10.0, 1.1), -33.2}, {pow(10.0, 1.2), -28.5}, {pow(10.0, 1.3), -24.2}, {pow(10.0, 1.4), -20.4},
	{pow(10.0, 1.5), -17.1}, {pow(10.0, 1.6), -14.2}, {pow(10.0, 1.7), -11.6}, {pow(10.0, 1.8),  -9.3}, {pow(10.0, 1.9),  -7.4},
	{pow(10.0, 2.0),  -5.6}, {pow(10.0, 2.1),  -4.2}, {pow(10.0, 2.2),  -3.0}, {pow(10.0, 2.3),  -2.0}, {pow(10.0, 2.4),  -1.3},
	{pow(10.0, 2.5),  -0.8}, {pow(10.0, 2.6),  -0.5}, {pow(10.0, 2.7),  -0.3}, {pow(10.0, 2.8),  -0.1}, {pow(10.0, 2.9),   0.0},
	{pow(10.0, 3.0),   0.0}, {pow(10.0, 3.1),   0.0}, {pow(10.0, 3.2),   0.0}, {pow(10.0, 3.3),  -0.1}, {pow(10.0, 3.4),  -0.2},
	{pow(10.0, 3.5),  -0.4}, {pow(10.0, 3.6),  -0.7}, {pow(10.0, 3.7),  -1.2}, {pow(10.0, 3.8),  -1.9}, {pow(10.0, 3.9),  -2.9},
	{pow(10.0, 4.0),  -4.3}, {pow(10.0, 4.1),  -6.1}, {pow(10.0, 4.2),  -8.4}, {pow(10.0, 4.3), -11.1}
};

static constexpr FilterData aCFilter[] = {
	{pow(10.0, 1.0), -14.3}, {pow(10.0, 1.1), -11.2}, {pow(10.0, 1.2),  -8.5}, {pow(10.0, 1.3),  -6.2}, {pow(10.0, 1.4),  -4.4},
	{pow(10.0, 1.5),  -3.0}, {pow(10.0, 1.6),  -2.0}, {pow(10.0, 1.7),  -1.3}, {pow(10.0, 1.8),  -0.8}, {pow(10.0, 1.9),  -0.5},
	{pow(10.0, 2.0),  -0.3}, {pow(10.0, 2.1),  -0.2}, {pow(10.0, 2.2),  -0.1}, {pow(10.0, 2.3),   0.0}, {pow(10.0, 2.4),   0.0},
	{pow(10.0, 2.5),   0.0}, {pow(10.0, 2.6),   0.0}, {pow(10.0, 2.7),   0.0}, {pow(10.0, 2.8),   0.0}, {pow(10.0, 2.9),   0.0},
	{pow(10.0, 3.0),   0.0}, {pow(10.0, 3.1),   0.0}, {pow(10.0, 3.2),  -0.1}, {pow(10.0, 3.3),  -0.2}, {pow(10.0, 3.4),  -0.3},
	{pow(10.0, 3.5),  -0.5}, {pow(10.0, 3.6),  -0.8}, {pow(10.0, 3.7),  -1.3}, {pow(10.0, 3.8),  -2.0}, {pow(10.0, 3.9),  -3.0},
	{pow(10.0, 4.0),  -4.4}, {pow(10.0, 4.1),  -6.2}, {pow(10.0, 4.2),  -8.5}, {pow(10.0, 4.3), -11.2}
};
*/

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
		pFilterData = aAFilter;
		nFilterData = ARRAY_SIZE(aAFilter);
		break;
	case FILTER_B:
		pFilterData = aBFilter;
		nFilterData = ARRAY_SIZE(aBFilter);
		break;
	case FILTER_C:
		pFilterData = aCFilter;
		nFilterData = ARRAY_SIZE(aCFilter);
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

	double *pFreq = new double[nFilterData];
	double *pLevel = new double[nFilterData];

	for (i = 0; i < nFilterData; i++) {
		pFreq[i] = log(pFilterData->fFreq);
		pLevel[i] = pFilterData->fLevel;
		pFilterData++;
	}

	CSpline spline;
	spline.MakeTable(pFreq, pLevel, nFilterData);

	pFilterTbl[0] = 0;
	for (i = 1; i <= nData / 2; i++)
		pFilterTbl[i] = pFilterTbl[nData - i] = pow(10.0, spline.Spline(log((double)i / nData * fRate)) / nLog);

	delete [] pFreq;
	delete [] pLevel;
}
