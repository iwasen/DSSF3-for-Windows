#pragma once
//*****************************************************************************************************
//  1. ファイル名
//		DigitalFilter.h
//----------------------------------------------------------------------------------------------------
//  2. 機能
//		デジタルフィルタクラス
//----------------------------------------------------------------------------------------------------
//  3. 備考
//----------------------------------------------------------------------------------------------------
//  4. 履歴
//		2007.12.17 S.Aizawa 新規作成
//*****************************************************************************************************

#include <complex>
using namespace std;

const int MAX_ORDER = 20;				// 次数の最大値
const int MAX_COEF = MAX_ORDER + 1;		// 係数の最大数
const int A_SIZE = MAX_ORDER * 2 + 2;	// 極と零点の最大数

// フィルタ特性
enum EDFShape {
	EDF_Butterworth,
	EDF_Chebyshev,
	EDF_Bessel
};

// フィルタ種類
enum EDFBand {
	EDF_LPF,
	EDF_HPF,
	EDF_BPF,
	EDF_BSF
};

class CDigitalFilter {
public:
	void InitIIR(EDFShape eShape, EDFBand eBand, int nOrder, double fSamplingFreq, double fFreq1, double fFreq2, double fRipple);
	void ExecIIR(const double *pDataIn, double *pDataOut, int nSize);
	double ExecIIR(double fData);
	double GetLevel(double fFreq) const;

protected:
	static const complex<double> m_aBesselTable[];
	complex<double> s_pz[A_SIZE];
	complex<double> z_pz[A_SIZE];
	double ak[MAX_COEF];
	double bk[MAX_COEF];
	double akm[2][MAX_COEF / 2];
	double bkm[3][MAX_COEF / 2];
	double m_fGain;
	int m_nZOrder;
	EDFBand m_eBand;
	EDFShape m_eShape;
	double m_fSamplingFreq;
	double m_aDelay[3][MAX_COEF / 2];

	void ButChe(int order, int np, double a, double b);
	void Butterworth(int order, int np);
	void Chebyshev(int order, double phi, int np);
	void Bessel(int order, int np);
	void Plane(int order, double phi, int &np, int &nz);
	int Bilinear(int np, int nz, double cgam, double c, double cbp, double wc);
	void AddZero(int jt);
	void ToCascade(double cgam, double fScale);
};
